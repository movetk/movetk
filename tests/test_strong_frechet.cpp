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

// Add a message to a Catch2 require
#define REQUIRE_MESSAGE(cond, msg) do { INFO(msg); REQUIRE(cond); } while((void)0, 0)

// The norm to be used in weak Frechet distance computations.
typedef movetk_support::FiniteNorm<MovetkGeometryKernel, 2> Norm;

struct StrongFrechetTestCase
{
    // Two input lines, given as ipe paths, followed by the expected distance line.
    // Only the length of segment of expectedLine will be used.
    std::string polyA, polyB, expectedLine;
};

std::map<std::string, StrongFrechetTestCase> testCases
{
    {
        "Simple spike example", 
        StrongFrechetTestCase
        {
            R"IPE(<path>
        96 448 m
        192 448 l
        256 448 l
        320 448 l
        384 448 l
        </path>
        )IPE",
            R"IPE(<path>
        96 448 m
        128 448 l
        144 448 l
        192 448 l
        208 512 l
        224 448 l
        256 448 l
        304 448 l
        384 448 l
        </path>
        )IPE",
        R"IPE(
        <path>
        208 448 m
        208 512 l
        </path>
        )IPE"
        }
    },
    {
        "Weaved grid example",
        StrongFrechetTestCase
        {
            R"IPE(<ipeselection pos="160 384">
        <path stroke="darkorange" pen="heavier" arrow="normal/normal">
        128 384 m
        320 384 l
        320 352 l
        128 352 l
        128 320 l
        320 320 l
        320 288 l
        128 288 l
        128 256 l
        320 256 l
        320 224 l
        128 224 l
        128 192 l
        320 192 l
        </path>
        </ipeselection>
        )IPE",
        R"IPE(<ipeselection pos="192 368">
        <path stroke="darkred" pen="heavier" arrow="normal/normal">
        128 384 m
        128 192 l
        160 192 l
        160 384 l
        192 384 l
        192 192 l
        224 192 l
        224 384 l
        256 384 l
        256 192 l
        288 192 l
        288 384 l
        320 384 l
        320 192 l
        </path>
        </ipeselection>
        )IPE",
        R"IPE(<ipeselection pos="144 384">
        <path matrix="1 0 0 1 0 -32" stroke="black">
        128 416 m
        320 416 l
        </path>
        </ipeselection>
        )IPE"
        }
    },
    {
        "Single segment and spike example",
        StrongFrechetTestCase
        {
            R"IPE(
            <ipeselection pos="-80 352">
            <path stroke="black">
            128 320 m
            144 352 l
            160 368 l
            176 416 l
            192 368 l
            208 352 l
            224 320 l
            </path>
            </ipeselection>
            )IPE",
            R"IPE(
            <ipeselection pos="-32 368">
            <path stroke="black">
            128 320 m
            224 320 l
            </path>
            </ipeselection>
            )IPE", 
            R"IPE(
            <ipeselection pos="176 384">
            <path matrix="1 0 0 1 32 32" stroke="black">
            176 320 m
            176 416 l
            </path>
            </ipeselection>s
            )IPE"
        }
    },
    {
        "Two segments example",
        StrongFrechetTestCase
        {
            R"IPE(
            <ipeselection pos="176 736">
            <path stroke="black" arrow="normal/normal">
            128 736 m
            192 736 l
            </path>
            </ipeselection>
            )IPE", 
            R"IPE(
            <ipeselection pos="0 768">
            <path stroke="black" arrow="normal/normal">
            98.5874 755.395 m
            157.413 780.605 l
            </path>
            </ipeselection>
            )IPE",
            R"IPE(
            <ipeselection pos="174.706 758.303">
            <path stroke="black" arrow="normal/normal">
            157.413 780.605 m
            192 736 l
            </path>
            </ipeselection>
            )IPE"
        }
    },
    {
        "Segment-point example",
        StrongFrechetTestCase
        {
            R"IPE(
            <ipeselection pos="176 736">
            <path stroke="black" arrow="normal/normal">
            128 736 m
            192 736 l
            </path>
            </ipeselection>
            )IPE",
            R"IPE(
            <ipeselection pos="0 768">
            <path stroke="black" arrow="normal/normal">
            98.5874 755.395 m
            </path>
            </ipeselection>
            )IPE",
            R"IPE(
            <ipeselection pos="139.422 746.917">
            <path stroke="black">
            98.5874 755.395 m
            192 736 l
            </path>
            </ipeselection>
            )IPE"
        }
    },
    {
        "Point-point example",
        StrongFrechetTestCase
        {
            R"IPE(
            <ipeselection pos="176 736">
            <path stroke="black" arrow="normal/normal">
            128 736 m
            </path>
            </ipeselection>
            )IPE", 
            R"IPE(
            <ipeselection pos="0 768">
            <path stroke="black" arrow="normal/normal">
            98.5874 755.395 m
            </path>
            </ipeselection>
            )IPE", 
            R"IPE(
            <ipeselection pos="96 736">
            <path stroke="black" arrow="normal/normal">
            98.5874 755.395 m
            128 736 l
            </path>
            </ipeselection>
            )IPE"
        }
    }
};


TEST_CASE( "Check if polyline strong Frechet distance is correct", "[strong_frechet]" ) 
{
    using SFR = movetk_support::StrongFrechet<MovetkGeometryKernel, movetk_support::squared_distance_d<MovetkGeometryKernel, Norm>>;
    using SqDistance = movetk_support::squared_distance_d<MovetkGeometryKernel, Norm>;
    // Initialize algorithm.
    movetk_support::StrongFrechet<MovetkGeometryKernel, SqDistance> sfr;
    sfr.setMode(SFR::Mode::DoubleAndSearch);
    sfr.setTolerance(0.0001);

    // Distance computer for expected distance
    SqDistance sqDist;

    for(const auto& pair: testCases)
    {
        StrongFrechetTestCase tc = pair.second;
        SECTION(pair.first)
        {
            // Read input
            std::vector<MovetkGeometryKernel::MovetkPoint> polyA;
            test_helpers::parseIpePath(tc.polyA, polyA);
            std::vector<MovetkGeometryKernel::MovetkPoint> polyB;
            test_helpers::parseIpePath(tc.polyB, polyB);
            // Expected distance element
            std::vector<MovetkGeometryKernel::MovetkPoint> expectedDistLine;
            test_helpers::parseIpePath(tc.expectedLine, expectedDistLine);
            // Compute expected distance
            auto expectedDist = std::sqrt(sqDist(expectedDistLine[0], expectedDistLine[1]));

            // Try strong frechet in both orders
            {
                auto dist = sfr(polyA.begin(), polyA.end(), polyB.begin(), polyB.end());
                REQUIRE(dist == Approx(expectedDist).margin(sfr.tolerance()));
            }
            {
                auto dist = sfr(polyB.begin(), polyB.end(), polyA.begin(), polyA.end());
                REQUIRE(dist == Approx(expectedDist).margin(sfr.tolerance()));
            }
        }
    }
}

TEST_CASE("Check if polyline strong Frechet distance is correct with upperbounded search", "[strong_frechet]")
{
    using SFR = movetk_support::StrongFrechet<MovetkGeometryKernel, movetk_support::squared_distance_d<MovetkGeometryKernel, Norm>>;
    using SqDistance = movetk_support::squared_distance_d<MovetkGeometryKernel, Norm>;
    // Initialize algorithm.
    movetk_support::StrongFrechet<MovetkGeometryKernel, SqDistance> sfr;
    sfr.setMode(SFR::Mode::BisectionSearch);
    sfr.setTolerance(0.0001);

    // Distance computer for expected distance
    SqDistance sqDist;

    // Test fractions of known distance
    std::vector<MovetkGeometryKernel::NT> fractions = { 0.1, 0.4, 0.55, 0.8, 1.0, 1.2, 5.0, 1000 };
    std::vector<bool> expectSuccess = { false, false, false, false, true, true, true, true };

    for (const auto& pair : testCases)
    {
        std::string testCaseName = pair.first;
        StrongFrechetTestCase tc = pair.second;
        SECTION(testCaseName)
        {
            // Read input
            std::vector<MovetkGeometryKernel::MovetkPoint> polyA;
            test_helpers::parseIpePath(tc.polyA, polyA);
            std::vector<MovetkGeometryKernel::MovetkPoint> polyB;
            test_helpers::parseIpePath(tc.polyB, polyB);
            // Expected distance element
            std::vector<MovetkGeometryKernel::MovetkPoint> expectedDistLine;
            test_helpers::parseIpePath(tc.expectedLine, expectedDistLine);
            // Compute expected distance
            auto expectedDist = std::sqrt(sqDist(expectedDistLine[0], expectedDistLine[1]));

            for(std::size_t i = 0; i < fractions.size(); ++i)
            {
                sfr.setUpperbound(expectedDist*fractions[i]);
                NT epsilon = -1;
                // Try algorithm with both polyA and polyB as first polyline
                {
                    bool success = sfr(polyA.begin(), polyA.end(), polyB.begin(), polyB.end(), epsilon);
                    // Check whether we expect success or not.
                    REQUIRE(success == expectSuccess[i]);
                    if(success)
                    {
                        REQUIRE_MESSAGE(epsilon == Approx(expectedDist).margin(sfr.tolerance()), (std::string("Failed at fraction ") + std::to_string(fractions[i])));
                    }
                }
                {
                    bool success = sfr(polyB.begin(), polyB.end(), polyA.begin(), polyA.end(), epsilon);
                    REQUIRE(success == expectSuccess[i]);
                    if (success)
                    {
                        REQUIRE(epsilon == Approx(expectedDist).margin(sfr.tolerance()));
                    }
                }
            }
        }
    }
}

TEST_CASE("Check if decision strong Frechet distance is correct", "[strong_frechet]")
{
    using SFR = movetk_support::StrongFrechet<MovetkGeometryKernel, movetk_support::squared_distance_d<MovetkGeometryKernel, Norm>>;
    using SqDistance = movetk_support::squared_distance_d<MovetkGeometryKernel, Norm>;
    // Initialize algorithm.
    movetk_support::StrongFrechet<MovetkGeometryKernel, SqDistance> sfr;
    sfr.setTolerance(0.0001);

    // Distance computer for expected distance
    SqDistance sqDist;

    // Test fractions of known distance
    std::vector<MovetkGeometryKernel::NT> fractions = { 0.1, 0.4, 0.55, 0.8, 1.0, 1.2, 5.0, 1000 };
    std::vector<bool> expectSuccess = { false, false, false, false, true, true, true, true };

    for (const auto& pair : testCases)
    {
        std::string testCaseName = pair.first;
        StrongFrechetTestCase tc = pair.second;
        SECTION(testCaseName)
        {
            // Read input
            std::vector<MovetkGeometryKernel::MovetkPoint> polyA;
            test_helpers::parseIpePath(tc.polyA, polyA);
            std::vector<MovetkGeometryKernel::MovetkPoint> polyB;
            test_helpers::parseIpePath(tc.polyB, polyB);
            // Expected distance element
            std::vector<MovetkGeometryKernel::MovetkPoint> expectedDistLine;
            test_helpers::parseIpePath(tc.expectedLine, expectedDistLine);
            // Compute expected distance
            auto expectedDist = std::sqrt(sqDist(expectedDistLine[0], expectedDistLine[1]));

            for (std::size_t i = 0; i < fractions.size(); ++i)
            {
                NT epsilon = -1;
                // Try algorithm with both polyA and polyB as first polyline
                {
                    bool success = sfr.decide(polyA.begin(), polyA.end(), polyB.begin(), polyB.end(), expectedDist*fractions[i]);
                    // Check whether we expect success or not.
                    REQUIRE(success == expectSuccess[i]);
                }
                {
                    bool success = sfr.decide(polyB.begin(), polyB.end(), polyA.begin(), polyA.end(), expectedDist*fractions[i]);
                    REQUIRE(success == expectSuccess[i]);
                }
            }
        }
    }
}