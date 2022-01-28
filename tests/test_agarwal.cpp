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


#include <array>
#include <map>
#include <catch2/catch.hpp>

 // Defines the geometry kernel
#include "test_includes.h" 

#include "movetk/metric/Distances.h"
#include "movetk/metric/Norm.h"
#include "movetk/algo/Simplification.h"

// Add a message to a Catch2 require
#define REQUIRE_MESSAGE(cond, msg) do { INFO(msg); REQUIRE(cond); } while((void)0, 0)

struct AgarwalTestcase {
    std::string polyline_input;
    // Ipe input for single edge that will define the epsilon for the testcase
    std::string epsilon_line_input;
    std::vector<std::size_t> expectedInds;
};

std::map<std::string, AgarwalTestcase> test_cases{
    {
        "Spike example", {
            R"IPE(
            <ipeselection pos="224 480">
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
            </ipeselection>
            )IPE",
            R"IPE(
            <ipeselection pos = "160 448">
                <path stroke = "black">
                160 448 m
                160 464 l
                </path>
            </ipeselection>
            )IPE",
            {0,3,4,5,8}
        }
    },
    {
        "Spike example larger epsilon", {
            R"IPE(
            <ipeselection pos="224 480">
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
            </ipeselection>
            )IPE",
            R"IPE(
            <ipeselection pos = "160 448">
            <path stroke="black">
            192 448 m
            168.371 489.355 l
            </path>
            </ipeselection>
            )IPE",
            { 0, 4,5, 8 }
        }
    },
    {
        "Spike example largest epsilon", {
            R"IPE(
            <ipeselection pos="224 480">
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
            </ipeselection>
            )IPE",
            R"IPE(
            <ipeselection pos="208 464">
            <path stroke="black">
            208 448 m
            208 512 l
            </path>
            </ipeselection>
            )IPE",
            { 0, 8 }
        }
    },
    {
        "Single segment", {
            R"IPE(
            <ipeselection pos="224 480">
            <path stroke="0">
            96 448 m
            128 448 l
            </path>
            </ipeselection>
            )IPE",
            R"IPE(
            <ipeselection pos="208 464">
            <path stroke="black">
            0 0 m
            0 5 l
            </path>
            </ipeselection>
            )IPE",
           { 0, 1 }
        }
    }
};

TEMPLATE_LIST_TEST_CASE("Check that the simplifications are correct", "[agarwal_simplification][simplification]", movetk::test::AvailableBackends)
{
    using MovetkGeometryKernel = typename TestType::MovetkGeometryKernel;
    // The norm to be used in weak Frechet distance computations.
    using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;
    using NT = typename MovetkGeometryKernel::NT;
    using SFR = movetk::metric::StrongFrechet<MovetkGeometryKernel, movetk::metric::squared_distance_d<MovetkGeometryKernel, Norm>>;
    using SqDistance = movetk::metric::squared_distance_d<MovetkGeometryKernel, Norm>;
    movetk::algo::Agarwal<MovetkGeometryKernel, SqDistance> simplifier;
    simplifier.setTolerance(0.0001);
    using PointList = std::vector<typename MovetkGeometryKernel::MovetkPoint>;
    const auto parseIpe = [](const std::string& string_data, PointList& output) {
        test_helpers::parseIpePath<MovetkGeometryKernel>(string_data, output);
    };
    for (const auto& [test_case_name,test_case] : test_cases) {
        SECTION(test_case_name) {
            PointList points, epsilonPath;

            parseIpe(test_case.polyline_input, points);
            parseIpe(test_case.epsilon_line_input, epsilonPath);

            // Requested epsilon
            const auto epsilon = std::sqrt(SqDistance()(epsilonPath[0], epsilonPath[1]));
            simplifier.setEpsilon(epsilon);

            std::vector<decltype(points.begin())> output;

            simplifier(points.begin(), points.end(), movetk::utils::movetk_back_insert_iterator(output));
            REQUIRE(output.size() == test_case.expectedInds.size());
            // Compute indices of iterators
            std::vector<std::size_t> inds;
            for (const auto& el : output)
            {
                inds.push_back(std::distance(points.begin(), el));
            }
            REQUIRE(inds == test_case.expectedInds);
        }
    }
    SECTION("Single point and empty polyline")
    {
        std::vector<typename MovetkGeometryKernel::MovetkPoint> points;
        points.push_back(movetk::geom::MakePoint<MovetkGeometryKernel>()({ (NT)2.0, (NT)5000.}));

        simplifier.setEpsilon(1.0);

        std::vector<decltype(points.begin())> output;

        simplifier(points.begin(), points.end(), movetk::utils::movetk_back_insert_iterator(output));

        REQUIRE(output.size() == 1);
        REQUIRE(output[0] == points.begin());

        // Now with empty
        output.clear();
        points.clear();

        simplifier(points.begin(), points.end(), movetk::utils::movetk_back_insert_iterator(output));
        REQUIRE(output.size() == 0);
    }
}