#
# Copyright (C) 2018-2020 HERE Europe B.V.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0
# License-Filename: LICENSE
#
import unittest

from TestHelpers import parseIpePath, points_approximately_equal

from PyMoveTk.simplification import agarwal

class AgarwalTest:
    def __init__(self, polyline, epsilonLine, expectedComplexity=None, expectedIndices=None):
        self.polyline = polyline
        self.epsilonLine = epsilonLine
        self.expectedComplexity = expectedComplexity
        self.expectedIndices = expectedIndices
    def test(self, testCaseCls):
        # Requested epsilon
        epsilon = (self.epsilonLine[0] - self.epsilonLine[1]).norm()
        simplification = agarwal(self.polyline, epsilon)
        # Assert
        testCaseCls.assertEqual(len(simplification),self.expectedComplexity)
        # Verify the points at the indices are selected
        for i in range(len(simplification)):
            # Soft compare
            testCaseCls.assertTrue(points_approximately_equal(simplification[i], self.polyline[self.expectedIndices[i]]))

class TestAgarwal(unittest.TestCase):
    def test_spike_simplification_correct(self):
        polyline = parseIpePath("""<ipeselection pos="224 480">
            <path stroke="0">
            96 448 m
            128 448 l
            144 448 l
            192 448 l
            208 512 l
            224 448 l
            336 448 l
            352 448 l
            384 448 l
            </path>
            </ipeselection>""")
        epsilonPath = parseIpePath("""<ipeselection pos = "160 448">
            < path stroke = "black">
            160 448 m
            160 464 l
            </path>
            </ipeselection>
        )""")
        tester = AgarwalTest(polyline, epsilonPath, expectedComplexity=5, expectedIndices=[0,3,4,5,8])
        tester.test(self)
    
    def test_spike_simplification_larger_eps_correct(self):
        polyline = parseIpePath("""<ipeselection pos="224 480">
            <path stroke="0">
            96 448 m
            128 448 l
            144 448 l
            192 448 l
            208 512 l
            224 448 l
            336 448 l
            352 448 l
            384 448 l
            </path>
            </ipeselection>""")
        epsilonPath = parseIpePath("""<ipeselection pos = "160 448">
            < path stroke = "black">
            192 448 m
            168.371 489.355 l
            </path>
            </ipeselection>
        )""")
        tester = AgarwalTest(polyline, epsilonPath, expectedComplexity=4, expectedIndices=[0,4,5,8])
        tester.test(self)
    def test_spike_simplification_largest_eps_correct(self):
        polyline = parseIpePath("""<ipeselection pos="224 480">
            <path stroke="0">
            96 448 m
            128 448 l
            144 448 l
            192 448 l
            208 512 l
            224 448 l
            336 448 l
            352 448 l
            384 448 l
            </path>
            </ipeselection>""")
        epsilonPath = parseIpePath("""<ipeselection pos = "160 448">
            < path stroke = "black">
            208 448 m
            208 512 l
            </path>
            </ipeselection>
        )""")
        tester = AgarwalTest(polyline, epsilonPath, expectedComplexity=2, expectedIndices=[0,8])
        tester.test(self)

#     SECTION("Single segment")
#     {
#         std::vector<MovetkGeometryKernel::MovetkPoint> points;

#         test_helpers::parseIpePath(R"IPE(<ipeselection pos="224 480">
#             <path stroke="0">
#             96 448 m
#             128 448 l
#             </path>
#             </ipeselection>
#         )IPE", points);
#         std::vector<MovetkGeometryKernel::MovetkPoint> epsilonPath;
#         test_helpers::parseIpePath(R"IPE(
#             <ipeselection pos="208 464">
#             <path stroke="black">
#             0 0 m
#             0 5 l
#             </path>
#             </ipeselection>
#         )IPE", epsilonPath);

#         // Requested epsilon
#         const auto epsilon = std::sqrt(SqDistance()(epsilonPath[0], epsilonPath[1]));
#         simplifier.setEpsilon(epsilon);

#         std::vector<decltype(points.begin())> output;

#         simplifier(points.begin(), points.end(), std::back_inserter(output));
#         REQUIRE(output.size() == 2);
#         // Compute indices of iteratorss
#         std::vector<std::size_t> inds;
#         for (const auto& el : output)
#         {
#             inds.push_back(std::distance(points.begin(), el));
#         }
#         std::vector<std::size_t> expectedInds = { 0, 1 };
#         REQUIRE(inds == expectedInds);
#     }
#     SECTION("Single point and empty polyline")
#     {
#         std::vector<MovetkGeometryKernel::MovetkPoint> points;
#         points.push_back(movetk::geom::MakePoint<MovetkGeometryKernel>()({ (NT)2.0, (NT)5000. }));

#         simplifier.setEpsilon(1.0);

#         std::vector<decltype(points.begin())> output;

#         simplifier(points.begin(), points.end(), std::back_inserter(output));

#         REQUIRE(output.size() == 1);
#         REQUIRE(output[0] == points.begin());

#         // Now with empty
#         output.clear();
#         points.clear();

#         simplifier(points.begin(), points.end(), std::back_inserter(output));
#         REQUIRE(output.size() == 0);
#     }
# }