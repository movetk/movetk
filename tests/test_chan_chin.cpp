/*
 * Copyright (C) 2018-2020 HERE Europe B.V.
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
 * SPDX-License-Identifier: Apache-2.0
 * License-Filename: LICENSE
 */

 //
 // Created by Mitra, Aniket on 2019-04-04.
 //

#include <array>
#include <catch2/catch.hpp>
#include <map>

#include "helpers/CustomCatchTemplate.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/Norm.h"
#include "movetk/simplification/ChanChin.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"

template <typename Backend>
struct ChanChinTests {
    using MovetkGeometryKernel = typename Backend::MovetkGeometryKernel;
    using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;

    // Point creator
    movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
    using PolyLine = std::vector<typename MovetkGeometryKernel::MovetkPoint>;
    using Wedge = movetk::geom::Wedge<MovetkGeometryKernel, Norm>;

    using Edge = std::pair<std::size_t, std::size_t>;
    using EdgeList = std::vector<Edge>;

    struct ShortcutTestcase {
        PolyLine polyline;
        EdgeList expectedEdges;
    };
    std::map<std::string, ShortcutTestcase> test_cases{
        {"Case 1",
         {{make_point({-8, 4}), make_point({-2, 4}), make_point({4, 4})},
          {std::make_pair(0, 1), std::make_pair(0, 2), std::make_pair(1, 2)}}},
        {"Case 2",
         {{make_point({-9, 8}), make_point({-2, 4}), make_point({4, 4})}, {std::make_pair(0, 1), std::make_pair(1, 2)}}},
        {"Case 3",
         {{make_point({-9, 8}), make_point({-2, 4}), make_point({3.503, 3.456})},
          {std::make_pair(0, 1), std::make_pair(1, 2)}}} };
};


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(ChanChinTests, "Check Chan Chin Shortcuts", "[is_valid_shortcut_1]") {
    using Fixture = ChanChinTests<TestType>;
    for (const auto& [test_case_name, test_data] : Fixture::test_cases) {
        SECTION(test_case_name) {
            movetk::simplification::ChanChin<typename Fixture::MovetkGeometryKernel, typename Fixture::Wedge> ChanChin(
                1);
            typename Fixture::EdgeList edges;
            ChanChin(std::begin(test_data.polyline),
                std::end(test_data.polyline),
                std::back_inserter(edges));
            REQUIRE(edges.size() == test_data.expectedEdges.size());
            auto eit = std::begin(test_data.expectedEdges);
            for (auto& edge : edges) {
                REQUIRE(edge == *eit);
                eit++;
            }
        }
    }
}


/*
TEST_CASE("Check Chan Chin Shortcuts 4", "[is_valid_shortcut_4]") {
    movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
    typedef std::vector<MovetkGeometryKernel::MovetkPoint> PolyLine;
    PolyLine polyline({
        make_point({1, -6}), make_point({4, -4}),
        make_point({5, -2}), make_point({6, -5}),
        make_point({7, -2}), make_point({8, -5}),
        make_point({9, -2}), make_point({10, -5}),
        make_point({11, -2}), make_point({13, -4})
    });
    std::cout << "Polyline to be simplified: ";
    std::cout << "{";
    for (auto &vertex: polyline) {
        cout << vertex;
        std::cout << ";";
    }
    std::cout << "}\n";
    typedef movetk::utils::Wedge<MovetkGeometryKernel, Norm> Wedge;
    movetk::ChanChin<MovetkGeometryKernel, Wedge> ChanChin(2);
    std::vector<std::pair<std::size_t, std::size_t> > edges;
    std::vector<std::pair<std::size_t, std::size_t> > ExpectedEdges({
        std::make_pair(0, 1), std::make_pair(1, 2),
        std::make_pair(2, 3), std::make_pair(3, 4),
        std::make_pair(2, 4), std::make_pair(4, 5),
        std::make_pair(5, 6), std::make_pair(6, 7),
        std::make_pair(5, 7), std::make_pair(7, 8),
        std::make_pair(1, 9), std::make_pair(7, 9),
        std::make_pair(8, 9)
    });
    ChanChin(std::begin(polyline), std::end(polyline), std::back_inserter(edges));
    REQUIRE(edges.size() == ExpectedEdges.size());
    std::cout << "Valid Shortcuts: ";
    std::cout << "{";
    auto eit = std::begin(ExpectedEdges);
    for (auto &edge: edges) {
        std::cout << edge.first << "," << edge.second << ";";
        REQUIRE(edge == *eit);
        eit++;
    }
    std::cout << "}\n";
}*/
