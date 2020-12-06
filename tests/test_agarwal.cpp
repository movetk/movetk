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
#include "catch2/catch.hpp"

// Defines the geometry kernel
#include "test_includes.h"

#include <movetk/metric/Distances.h>
#include <movetk/metric/Norm.h>
#include "movetk/algo/Simplification.h"

// Add a message to a Catch2 require
#define REQUIRE_MESSAGE(cond, msg) do { INFO(msg); REQUIRE(cond); } while((void)0, 0)

// The norm to be used in weak Frechet distance computations.
typedef movetk_support::FiniteNorm<MovetkGeometryKernel, 2> Norm;

TEST_CASE( "Check that the simplifications are correct", "[agarwal_simplification][simplification]" ) 
{
    using SFR = movetk_support::StrongFrechet<MovetkGeometryKernel, movetk_support::squared_distance_d<MovetkGeometryKernel, Norm>>;
    using SqDistance = movetk_support::squared_distance_d<MovetkGeometryKernel, Norm>;
    movetk_algorithms::Agarwal<MovetkGeometryKernel, SqDistance> simplifier;
    simplifier.setTolerance(0.0001);

    SECTION("Spike example")
    {
        std::vector<MovetkGeometryKernel::MovetkPoint> points;
        
        test_helpers::parseIpePath(R"IPE(<ipeselection pos="224 480">
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
        )IPE", points);
        std::vector<MovetkGeometryKernel::MovetkPoint> epsilonPath;
        test_helpers::parseIpePath(R"IPE(
            <ipeselection pos = "160 448">
            < path stroke = "black">
            160 448 m
            160 464 l
            </path>
            </ipeselection>
        )IPE", epsilonPath);

        // Requested epsilon
        const auto epsilon = std::sqrt(SqDistance()(epsilonPath[0], epsilonPath[1]));
        simplifier.setEpsilon(epsilon);

        std::vector<decltype(points.begin())> output;

        simplifier(points.begin(), points.end(),movetk_core::movetk_back_insert_iterator(output));
        REQUIRE(output.size() == 5);
        // Compute indices of iterators
        std::vector<std::size_t> inds;
        for(const auto& el: output)
        {
            inds.push_back(std::distance(points.begin(), el));
        }
        std::vector<std::size_t> expectedInds = { 0, 3,4,5, 8 };
        REQUIRE(inds == expectedInds);
    }
    SECTION("Spike example larger epsilon")
    {
        std::vector<MovetkGeometryKernel::MovetkPoint> points;

        test_helpers::parseIpePath(R"IPE(<ipeselection pos="224 480">
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
        )IPE", points);
        std::vector<MovetkGeometryKernel::MovetkPoint> epsilonPath;
        test_helpers::parseIpePath(R"IPE(
            <ipeselection pos = "160 448">
            <path stroke="black">
            192 448 m
            168.371 489.355 l
            </path>
            </ipeselection>
        )IPE", epsilonPath);

        // Requested epsilon
        const auto epsilon = std::sqrt(SqDistance()(epsilonPath[0], epsilonPath[1]));
        simplifier.setEpsilon(epsilon);

        std::vector<decltype(points.begin())> output;

        simplifier(points.begin(), points.end(), movetk_core::movetk_back_insert_iterator(output));
        REQUIRE(output.size() == 4);
        // Compute indices of iteratorss
        std::vector<std::size_t> inds;
        for (const auto& el : output)
        {
            inds.push_back(std::distance(points.begin(), el));
        }
        std::vector<std::size_t> expectedInds = { 0, 4,5, 8 };
        REQUIRE(inds == expectedInds);
    }
    SECTION("Spike example largest epsilon")
    {
        std::vector<MovetkGeometryKernel::MovetkPoint> points;

        test_helpers::parseIpePath(R"IPE(<ipeselection pos="224 480">
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
        )IPE", points);
        std::vector<MovetkGeometryKernel::MovetkPoint> epsilonPath;
        test_helpers::parseIpePath(R"IPE(
            <ipeselection pos="208 464">
            <path stroke="black">
            208 448 m
            208 512 l
            </path>
            </ipeselection>
        )IPE", epsilonPath);

        // Requested epsilon
        const auto epsilon = std::sqrt(SqDistance()(epsilonPath[0], epsilonPath[1]));
        simplifier.setEpsilon(epsilon);

        std::vector<decltype(points.begin())> output;

        simplifier(points.begin(), points.end(), movetk_core::movetk_back_insert_iterator(output));
        REQUIRE(output.size() == 2);
        // Compute indices of iteratorss
        std::vector<std::size_t> inds;
        for (const auto& el : output)
        {
            inds.push_back(std::distance(points.begin(), el));
        }
        std::vector<std::size_t> expectedInds = { 0, 8 };
        REQUIRE(inds == expectedInds);
    }
    SECTION("Single segment")
    {
        std::vector<MovetkGeometryKernel::MovetkPoint> points;

        test_helpers::parseIpePath(R"IPE(<ipeselection pos="224 480">
            <path stroke="0">
            96 448 m
            128 448 l
            </path>
            </ipeselection>
        )IPE", points);
        std::vector<MovetkGeometryKernel::MovetkPoint> epsilonPath;
        test_helpers::parseIpePath(R"IPE(
            <ipeselection pos="208 464">
            <path stroke="black">
            0 0 m
            0 5 l
            </path>
            </ipeselection>
        )IPE", epsilonPath);

        // Requested epsilon
        const auto epsilon = std::sqrt(SqDistance()(epsilonPath[0], epsilonPath[1]));
        simplifier.setEpsilon(epsilon);

        std::vector<decltype(points.begin())> output;

        simplifier(points.begin(), points.end(), movetk_core::movetk_back_insert_iterator(output));
        REQUIRE(output.size() == 2);
        // Compute indices of iteratorss
        std::vector<std::size_t> inds;
        for (const auto& el : output)
        {
            inds.push_back(std::distance(points.begin(), el));
        }
        std::vector<std::size_t> expectedInds = { 0, 1 };
        REQUIRE(inds == expectedInds);
    }
    SECTION("Single point and empty polyline")
    {
        std::vector<MovetkGeometryKernel::MovetkPoint> points;
        points.push_back(movetk_core::MakePoint<MovetkGeometryKernel>()({ (NT)2.0, (NT)5000. }));

        simplifier.setEpsilon(1.0);

        std::vector<decltype(points.begin())> output;

        simplifier(points.begin(), points.end(), movetk_core::movetk_back_insert_iterator(output));

        REQUIRE(output.size() == 1);
        REQUIRE(output[0] == points.begin());

        // Now with empty
        output.clear();
        points.clear();

        simplifier(points.begin(), points.end(), movetk_core::movetk_back_insert_iterator(output));
        REQUIRE(output.size() == 0);
    }
}