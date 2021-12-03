/*
 * Copyright (C) 2021 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include "sdf_usd_parser/model.hh"

#include <iostream>
#include <string>

#include <ignition/math/Pose3.hh>
#include <ignition/math/Vector3.hh>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdPhysics/rigidBodyAPI.h>

#include "sdf/Model.hh"
#include "sdf_usd_parser/link.hh"
#include "sdf_usd_parser/utils.hh"

namespace usd
{
  bool ParseSdfModel(const sdf::Model &_model, pxr::UsdStageRefPtr &_stage,
      const std::string &_path)
  {
    auto usdModelXform = pxr::UsdGeomXform::Define(_stage, pxr::SdfPath(_path));
    // since USD does not have a plane yet, planes are being represented as a
    // wide, thin box. The plane/box pose needs to be offset according to the
    // thickness to ensure that the top of the plane is at the correct height.
    // This pose offset workaround will no longer be needed when a pxr::USDGeomPlane
    // class is created (see the notes in the usd::ParseSdfPlaneGeometry method in
    // the geometry.cc file for more information)
    if (usd::IsPlane(_model))
    {
      ignition::math::Vector3d planePosition(
          _model.RawPose().X(),
          _model.RawPose().Y(),
          _model.RawPose().Z() - (0.5 * usd::kPlaneThickness));
      usd::SetPose(ignition::math::Pose3d(planePosition, _model.RawPose().Rot()),
          usdModelXform);
    }
    else
    {
      usd::SetPose(_model.RawPose(), usdModelXform);
    }

    if (!_model.Static())
    {
      auto modelPrim = _stage->GetPrimAtPath(pxr::SdfPath(_path));
      if (!modelPrim)
      {
        std::cerr << "Internal error: unable to get prim at path ["
                  << _path << "], but a model prim should exist at this path\n";
        return false;
      }

      if (!pxr::UsdPhysicsRigidBodyAPI::Apply(modelPrim))
      {
        std::cerr << "Internal error: unable to mark model at path ["
                  << _path << "] as a rigid body\n";
        return false;
      }
    }

    // TODO(adlarkin) finish parsing model. It will look something like this
    // (this does not cover parsing all elements of a model):
    //  * ParseSdfLink
    //  * ParseSdfJoint

    // parse all of the model's links and convert them to USD
    for (uint64_t i = 0; i < _model.LinkCount(); ++i)
    {
      const auto link = *(_model.LinkByIndex(i));
      const auto linkPath = std::string(_path + "/" + link.Name());
      if (!ParseSdfLink(link, _stage, linkPath))
      {
        std::cerr << "Error parsing link [" << link.Name() << "]\n";
        return false;
      }
    }

    return true;
  }
}
