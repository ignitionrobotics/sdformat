# Copyright (C) 2022 Open Source Robotics Foundation

# Licensed under the Apache License, version 2.0 (the "License")
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#       http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import copy
from ignition.math import Vector3d, Pose3d
from sdformat import Error, Light, Root, SDF_VERSION, SDF_PROTOCOL_VERSION, World
import unittest


class RootTEST(unittest.TestCase):

    def test_default_construction(self):
        root = Root()
        self.assertEqual(SDF_VERSION, root.version())
        self.assertFalse(root.world_name_exists("default"))
        self.assertFalse(root.world_name_exists(""))
        self.assertEqual(0, root.world_count())
        self.assertTrue(root.world_by_index(0) == None)
        self.assertTrue(root.world_by_index(1) == None)

        self.assertEqual(None, root.model())
        self.assertEqual(None, root.light())
        # self.assertEqual(None, root.Actor())


    def test_string_model_sdf_parse(self):
        sdf = """<?xml version="1.0"?>
             <sdf version="1.8">
               <model name='shapes'>
                 <link name='link'>
                   <collision name='box_col'>
                     <geometry>
                       <box>
                         <size>3 4 5</size>
                       </box>
                     </geometry>
                   </collision>
                 </link>
               </model>
             </sdf>"""

        root = Root()
        errors = root.load_sdf_string(sdf)
        self.assertEqual(0, len(errors))

        model = root.model()
        self.assertNotEqual(None, model)

        self.assertEqual("shapes", model.name())
        self.assertEqual(1, model.link_count())

        link = model.link_by_index(0)
        self.assertNotEqual(None, link)
        self.assertEqual("link", link.name())
        self.assertEqual(1, link.collision_count())

        collision = link.collision_by_index(0)
        self.assertNotEqual(None, collision)
        self.assertEqual("box_col", collision.name())

        self.assertEqual(None, root.light())
        # self.assertEqual(None, root.Actor())
        self.assertEqual(0, root.world_count())

        # Test cloning
        root2 = copy.deepcopy(root)

        model2 = root2.model()
        self.assertNotEqual(None, model2)

        self.assertEqual("shapes", model2.name())
        self.assertEqual(1, model2.link_count())

        link2 = model2.link_by_index(0)
        self.assertNotEqual(None, link2)
        self.assertEqual("link", link2.name())
        self.assertEqual(1, link2.collision_count())

        collision2 = link2.collision_by_index(0)
        self.assertNotEqual(None, collision2)
        self.assertEqual("box_col", collision2.name())

        self.assertEqual(None, root2.light())
        # self.assertEqual(None, root2.Actor())
        self.assertEqual(0, root2.world_count())


    def test_string_light_sdf_parse(self):
        sdf = """<?xml version="1.0"?>"
                   <sdf version="1.8">"
                     <light type='directional' name='sun'>"
                       <direction>-0.5 0.1 -0.9</direction>"
                     </light>"
                   </sdf>"""

        root = Root()
        errors = root.load_sdf_string(sdf)
        self.assertEqual(0, len(errors))

        light = root.light()
        self.assertNotEqual(None, light)
        self.assertEqual("sun", light.name())

        self.assertEqual(None, root.model())
        # self.assertEqual(None, root.Actor())
        self.assertEqual(0, root.world_count())

    # /////////////////////////////////////////////////
    # TEST(DOMRoot, StringActorSdfParse)
    # {
    #   std::string sdf = "<?xml version=\"1.0\"?>"
    #     " <sdf version=\"1.8\">"
    #     "   <actor name='actor_test'>"
    #     "     <pose>0 0 1.0 0 0 0</pose>"
    #     "     <skin>"
    #     "       <filename>/fake/path/to/mesh.dae</filename>"
    #     "       <scale>1.0</scale>"
    #     "     </skin>"
    #     "     <animation name='run'>"
    #     "       <filename>/fake/path/to/mesh.dae</filename>"
    #     "       <scale>0.055</scale>"
    #     "       <interpolate_x>true</interpolate_x>"
    #     "     </animation>"
    #     "     <script>"
    #     "       <loop>true</loop>"
    #     "       <delay_start>5.0</delay_start>"
    #     "       <auto_start>true</auto_start>"
    #     "     </script>"
    #     "   </actor>"
    #     " </sdf>"
    #
    #   root = Root()
    #   errors = root.load_sdf_string(sdf)
    #   self.assertTrue(errors.empty())
    #
    #   const sdf::Actor *actor = root.Actor()
    #   self.assertNotEqual(None, actor)
    #   self.assertEqual("actor_test", actor.name())
    #   self.assertNotEqual(None, actor.Element())
    #
    #   self.assertEqual(None, root.model())
    #   self.assertEqual(None, root.Light())
    #   self.assertEqual(0, root.world_count())
    # }

    def test_set(self):
        root = Root()
        self.assertEqual(SDF_VERSION, root.version())
        root.set_version(SDF_PROTOCOL_VERSION)
        self.assertEqual(SDF_PROTOCOL_VERSION, root.version())

    def test_frame_semantics_on_move(self):
        sdfString1 = """
            <sdf version="1.8">
              <world name="default">
                <frame name="frame1">
                  <pose>0 1 0 0 0 0</pose>
                </frame>
              </world>
            </sdf>"""

        sdfString2 = """
            <sdf version="1.8">
              <world name="default">
                <frame name="frame2">
                  <pose>1 1 0 0 0 0</pose>
                </frame>
              </world>
            </sdf>"""

        def testFrame1(_root):
            world = _root.world_by_index(0)
            self.assertNotEqual(None, world)
            frame = world.frame_by_index(0)
            self.assertNotEqual(None, frame)
            self.assertEqual("frame1", frame.name())
            pose = Pose3d()
            errors = frame.semantic_pose().resolve(pose)
            self.assertEqual(0, len(errors))
            self.assertEqual(Pose3d(0, 1, 0, 0, 0, 0), pose)

        def testFrame2(_root):
            world = _root.world_by_index(0)
            self.assertNotEqual(None, world)
            frame = world.frame_by_index(0)
            self.assertNotEqual(None, frame)
            self.assertEqual("frame2", frame.name())
            pose = Pose3d()
            errors = frame.semantic_pose().resolve(pose)
            self.assertEqual(0, len(errors))
            self.assertEqual(Pose3d(1, 1, 0, 0, 0, 0), pose)

        root1 = Root()
        errors = root1.load_sdf_string(sdfString1)
        self.assertEqual(0, len(errors))
        testFrame1(root1)

        root2 = Root()
        errors = root2.load_sdf_string(sdfString2)
        self.assertEqual(0, len(errors))
        testFrame2(root2)


    def test_add_world(self):
        root = Root()
        self.assertEqual(0, root.world_count())

        world = World()
        world.set_name("world1")
        errors = root.add_world(world)
        self.assertEqual(0, len(errors))
        self.assertEqual(1, root.world_count())
        self.assertEqual(1, len(root.add_world(world)))
        self.assertEqual(Error.ErrorCode.DUPLICATE_NAME, root.add_world(world)[0].code())
        self.assertEqual(1, root.world_count())

        root.clear_worlds()
        self.assertEqual(0, root.world_count())

        self.assertEqual(0, len(root.add_world(world)))
        self.assertEqual(1, root.world_count())
        worldFromRoot = root.world_by_index(0)
        self.assertNotEqual(None, worldFromRoot)
        self.assertEqual(worldFromRoot.name(), world.name())


    def test_mutable_by_index(self):
        root = Root()
        self.assertEqual(0, root.world_count())

        world = World()
        world.set_name("world1")
        self.assertEqual(0, len(root.add_world(world)))
        self.assertEqual(1, root.world_count())

        # Modify the world
        w = root.world_by_index(0)
        self.assertNotEqual(None, w)
        self.assertEqual("world1", w.name())
        w.set_name("world2")
        self.assertEqual("world2", root.world_by_index(0).name())


if __name__ == '__main__':
    unittest.main()
