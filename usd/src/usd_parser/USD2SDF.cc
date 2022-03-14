/*
 * Copyright (C) 2022 Open Source Robotics Foundation
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

#include "USD2SDF.hh"

#include "sdf/Console.hh"
#include "sdf/Types.hh"
#include "usd_model/WorldInterface.hh"
#include "USDWorld.hh"

#include "sdf/Root.hh"
#include "sdf/World.hh"

namespace sdf {
inline namespace SDF_VERSION_NAMESPACE {
namespace usd {
////////////////////////////////////////////////
UsdErrors USD2SDF::Read(const std::string &_fileName,
  sdf::Root &_root)
{
  UsdErrors errors;

  WorldInterface worldInterface;

  const auto errorsParseUSD = parseUSDWorld(_fileName, worldInterface);
  if (!errorsParseUSD.empty())
  {
    errors.emplace_back(UsdError(
      UsdErrorCode::SDF_TO_USD_PARSING_ERROR,
      "Error parsing usd file [" + _fileName + "]"));
    return errors;
  }

  sdf::World world;

  std::string worldName = worldInterface.worldName;
  if (worldName.empty())
  {
    world.SetName("world_name");
  }
  else
  {
    world.SetName(worldName + "_world");
  }
  // Add lights
  for (auto & light : worldInterface.lights)
  {
    if (!world.AddLight(*light.second.get()))
    {
      errors.emplace_back(UsdError(
        UsdErrorCode::SDF_TO_USD_PARSING_ERROR,
        "Error parsing usd file [" + _fileName + "]"));
    }
  }

  auto addWorldErrors = _root.AddWorld(world);
  if (!addWorldErrors.empty())
  {
    errors.emplace_back(UsdError(
      UsdErrorCode::SDF_ERROR,
      "Error adding the world [" + worldName + "]"));
    return errors;
  }
  world.SetGravity(worldInterface.gravity * worldInterface.magnitude);

  return errors;
}
}
}
}
