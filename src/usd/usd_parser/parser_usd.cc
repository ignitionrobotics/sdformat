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

#include <pxr/usd/sdf/reference.h>
#include <pxr/usd/usd/attribute.h>
#include <pxr/base/tf/stringUtils.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/primRange.h>
#include "pxr/usd/usdGeom/gprim.h"

#include "pxr/base/gf/vec3d.h"
#include "pxr/base/gf/matrix4d.h"
#include "pxr/base/gf/rotation.h"

#include "pxr/usd/usdPhysics/scene.h"
#include "pxr/usd/usdPhysics/joint.h"
#include "pxr/usd/usdPhysics/fixedJoint.h"

#include "pxr/usd/usdLux/light.h"
#include "pxr/usd/usdLux/sphereLight.h"

// #include "pxr/base/tf/staticTokens.h"
// #include "pxr/usd/usdRi/tokens.h"

#include "usd_parser/parser_usd.hh"
#include "physics.hh"
#include "joints.hh"
#include "links.hh"
#include "sdf/Console.hh"

#include <fstream>

namespace usd {

ModelInterfaceSharedPtr parseUSD(const std::string &xml_string)
{
  ModelInterfaceSharedPtr model(new ModelInterface);
  model->clear();

  auto referencee = pxr::UsdStage::CreateInMemory();
  if (!referencee->GetRootLayer()->ImportFromString(xml_string))
  {
    return nullptr;
  }

  // Get robot name
  const char *name = "test";
  if (!name)
  {
    model.reset();
    return model;
  }
  model->name_ = std::string(name);

  // TODO(ahcorde): Get all Material elements

  auto range = pxr::UsdPrimRange::Stage(referencee);


  // Get all Link elements
  for (auto const &prim : range ) {
    sdferr << "------------------------------------------------------\n";
    sdferr << pxr::TfStringify(prim.GetPath()) << "\n";
    sdferr << pxr::TfEnum(prim.GetPrim().GetSpecifier()) << "\n";
    sdferr << prim.GetName().GetText() << "\n";

    if (prim.IsA<pxr::UsdPhysicsScene>())
    {
      usd::ParsePhysicsScene(prim);
    }

    if (prim.IsA<pxr::UsdPhysicsJoint>())
    {
      sdferr << "UsdPhysicsJoint" << "\n";

      JointSharedPtr joint = usd::ParseJoints(prim);
      if (joint != nullptr)
      {
        model->joints_.insert(make_pair(joint->name, joint));
      }

      continue;
    }

    if (prim.IsA<pxr::UsdLuxLight>())
    {
      sdferr << "Light" << "\n";
      if (prim.IsA<pxr::UsdLuxSphereLight>())
      {
        sdferr << "Sphere light" << "\n";
      }
      continue;
    }

    if (!prim.IsA<pxr::UsdGeomGprim>())
    {
      sdferr << "Not a geometry" << "\n";
      continue;
    }

    LinkSharedPtr link = usd::ParseLinks(prim);
    if (link)
    {
      model->links_.insert(make_pair(link->name, link));
    }

    if (model->links_.empty()){
      model.reset();
      return model;
    }
  }

  // every link has children links and joints, but no parents, so we create a
  // local convenience data structure for keeping child->parent relations
  std::map<std::string, std::string> parent_link_tree;
  parent_link_tree.clear();

  // building tree: name mapping
  try
  {
    model->initTree(parent_link_tree);
  }
  catch(ParseError &e)
  {
    model.reset();
    sdferr << "error initTree " << e.what()  << "\n";
    return model;
  }

  // find the root link
  try
  {
    model->initRoot(parent_link_tree);
  }
  catch(ParseError &e)
  {
    model.reset();
    sdferr << "error initRoot " << e.what() << "\n";
    return model;
  }

  if (referencee)
    return model;
  return nullptr;
}


ModelInterfaceSharedPtr parseUSDFile(const std::string &filename)
{
  auto referencee = pxr::UsdStage::Open(filename);
  std::ifstream stream( filename.c_str() );
  if (!stream)
  {
    return ModelInterfaceSharedPtr();
  }
  std::string xml_str((std::istreambuf_iterator<char>(stream)),
                     std::istreambuf_iterator<char>());
  return usd::parseUSD(xml_str);
}

void exportUSD()
{

}
}
