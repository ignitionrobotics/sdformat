/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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

#include <gtest/gtest.h>
#include "sdf/Model.hh"

/////////////////////////////////////////////////
TEST(DOMModel, Construction)
{
  sdf::Model model;
  EXPECT_TRUE(model.Name().empty());

  model.SetName("test_model");
  EXPECT_EQ(model.Name(), "test_model");

  EXPECT_EQ(0u, model.LinkCount());
  EXPECT_EQ(nullptr, model.LinkByIndex(0));
  EXPECT_EQ(nullptr, model.LinkByIndex(1));
  EXPECT_FALSE(model.LinkNameExists(""));
  EXPECT_FALSE(model.LinkNameExists("default"));

  EXPECT_EQ(0u, model.JointCount());
  EXPECT_EQ(nullptr, model.JointByIndex(0));
  EXPECT_EQ(nullptr, model.JointByIndex(1));
  EXPECT_FALSE(model.JointNameExists(""));
  EXPECT_FALSE(model.JointNameExists("default"));
}
