# Copyright (C) 2022 Open Source Robotics Foundation

# Licensed under the Apache License, Version 2.0 (the "License")
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#       http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import copy

import math

from gz_test_deps.sdformat import Cylinder

import unittest


class CylinderTEST(unittest.TestCase):

  def test_default_construction(self):
    cylinder = Cylinder()

    self.assertEqual(math.pi * math.pow(0.5, 2) * 1.0,
                     cylinder.shape().volume())

    self.assertEqual(0.5, cylinder.radius())
    self.assertEqual(1.0, cylinder.length())

    cylinder.set_radius(0.5)
    cylinder.set_length(2.3)

    self.assertEqual(0.5, cylinder.radius())
    self.assertEqual(2.3, cylinder.length())

  def test_assignment(self):
    cylinder = Cylinder()
    cylinder.set_radius(0.2)
    cylinder.set_length(3.0)
    self.assertEqual(math.pi * math.pow(0.2, 2) * 3.0,
                   cylinder.shape().volume())

    cylinder2 = cylinder
    self.assertEqual(0.2, cylinder2.radius())
    self.assertEqual(3.0, cylinder2.length())

    self.assertEqual(math.pi * math.pow(0.2, 2) * 3.0,
                   cylinder2.shape().volume())
    self.assertEqual(0.2, cylinder2.shape().radius())
    self.assertEqual(3.0, cylinder2.shape().length())

    cylinder.set_radius(2.0)
    cylinder.set_length(0.3)

    self.assertEqual(2.0, cylinder.radius())
    self.assertEqual(0.3, cylinder.length())
    self.assertEqual(2.0, cylinder2.radius())
    self.assertEqual(0.3, cylinder2.length())


  def test_copy_construction(self):
    cylinder = Cylinder();
    cylinder.set_radius(0.2)
    cylinder.set_length(3.0)

    cylinder2 = Cylinder(cylinder)
    self.assertEqual(0.2, cylinder2.radius())
    self.assertEqual(3.0, cylinder2.length())

    cylinder.set_radius(2.)
    cylinder.set_length(0.3)

    self.assertEqual(2, cylinder.radius())
    self.assertEqual(0.3, cylinder.length())
    self.assertEqual(0.2, cylinder2.radius())
    self.assertEqual(3.0, cylinder2.length())

  def test_deepcopy(self):
    cylinder = Cylinder();
    cylinder.set_radius(0.2)
    cylinder.set_length(3.0)

    cylinder2 = copy.deepcopy(cylinder);
    self.assertEqual(0.2, cylinder2.radius())
    self.assertEqual(3.0, cylinder2.length())

    cylinder.set_radius(2.)
    cylinder.set_length(0.3)

    self.assertEqual(2, cylinder.radius())
    self.assertEqual(0.3, cylinder.length())
    self.assertEqual(0.2, cylinder2.radius())
    self.assertEqual(3.0, cylinder2.length())

  def test_shape(self):
    cylinder = Cylinder();
    self.assertEqual(0.5, cylinder.radius())
    self.assertEqual(1.0, cylinder.length())

    cylinder.shape().set_radius(0.123)
    cylinder.shape().set_length(0.456)
    self.assertEqual(0.123, cylinder.radius())
    self.assertEqual(0.456, cylinder.length())


if __name__ == '__main__':
    unittest.main()
