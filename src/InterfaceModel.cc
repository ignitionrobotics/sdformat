/*
 * Copyright 2021 Open Source Robotics Foundation
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

#include "sdf/InterfaceModel.hh"

namespace sdf
{
inline namespace SDF_VERSION_NAMESPACE
{
class InterfaceModel::Implementation
{
  public: std::string name;
  public: std::string canonicalLinkName;
  public: ignition::math::Pose3d poseInCanonicalLinkFrame;
  public: ignition::math::Pose3d poseInParentFrame;
};

InterfaceModel::InterfaceModel(const std::string &_name,
    const std::string &_canonicalLinkName,
    const ignition::math::Pose3d &_poseInCanonicalLinkFrame,
    const ignition::math::Pose3d &_poseInParentFrame)
    : dataPtr(ignition::utils::MakeImpl<Implementation>())
{
  this->dataPtr->name = _name;
  this->dataPtr->canonicalLinkName = _canonicalLinkName;
  this->dataPtr->poseInCanonicalLinkFrame = _poseInCanonicalLinkFrame;
  this->dataPtr->poseInParentFrame = _poseInParentFrame;
}

/////////////////////////////////////////////////
const std::string &InterfaceModel::Name() const
{
  return this->dataPtr->name;
}

/////////////////////////////////////////////////
const std::string &InterfaceModel::CanonicalLinkName() const
{
  return this->dataPtr->canonicalLinkName;
}

/////////////////////////////////////////////////
const ignition::math::Pose3d &
InterfaceModel::ModelFramePoseInCanonicalLinkFrame() const
{
  return this->dataPtr->poseInCanonicalLinkFrame;
}

/////////////////////////////////////////////////
const ignition::math::Pose3d &
InterfaceModel::ModelFramePoseInParentFrame() const
{
  return this->dataPtr->poseInParentFrame;
}
}
}
