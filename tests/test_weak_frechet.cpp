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
// Created by Custers, Bram on 2020-10-10.
//



#include <array>
#include "catch2/catch.hpp"
#if CGAL_BACKEND_ENABLED
#include "movetk/geom/CGALTraits.h"
#else

#include "movetk/geom/BoostGeometryTraits.h"
#endif

#include "movetk/geom/GeometryInterface.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "movetk/metric/Norm.h"
#include <movetk/algo/Similarity.h>

// Helpers for testing
#include "test_includes.h"

using namespace std;

#if CGAL_BACKEND_ENABLED
    typedef movetk_support::FiniteNorm<MovetkGeometryKernel, 2> Norm;
#else
    typedef void Norm;
#endif


TEST_CASE("Check Weak frechet distance between polylines","[weak_frechet]"){
    movetk_algorithms::WeakFrechet<MovetkGeometryKernel, movetk_support::squared_distance_d<MovetkGeometryKernel, Norm>> wfr{};
    movetk_support::squared_distance_d<MovetkGeometryKernel, Norm> sqDist;
    SECTION("Simple spike example")
    {
        std::vector<MovetkGeometryKernel::MovetkPoint> polyA;
        test_helpers::parseIpePath(R"IPE(<path>
        96 448 m
        192 448 l
        256 448 l
        320 448 l
        384 448 l
        </path>
        )IPE", polyA);
        std::ostream_iterator<MovetkGeometryKernel::MovetkPoint> out(std::cout, ",");
        std::copy(polyA.begin(), polyA.end(), out);
        std::vector<MovetkGeometryKernel::MovetkPoint> polyB;
        test_helpers::parseIpePath(R"IPE(<path>
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
        )IPE", polyB);
        std::vector<MovetkGeometryKernel::MovetkPoint> expectedDistLine;
        test_helpers::parseIpePath(R"IPE(
        <path>
        208 448 m
        208 512 l
        </path>
        )IPE", expectedDistLine);


        auto dist = wfr(polyA.begin(), polyA.end(), polyB.begin(), polyB.end());

        auto expectedDist = std::sqrt(sqDist(expectedDistLine[0], expectedDistLine[1]));

        REQUIRE(abs(dist - expectedDist) < MOVETK_EPS);
    }
    SECTION("Interweaved grid example")
    {
        std::vector<MovetkGeometryKernel::MovetkPoint> polyA;
        test_helpers::parseIpePath(R"IPE(<ipeselection pos="160 384">
        <path stroke="darkred" pen="heavier" arrow="normal/normal">
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
        </ipeselection>)IPE", polyA);

        std::vector<MovetkGeometryKernel::MovetkPoint> polyB;
        test_helpers::parseIpePath(R"IPE( <ipeselection>
        <path>
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
        )IPE", polyB);

        std::vector<MovetkGeometryKernel::MovetkPoint> expectedDistLine;
        test_helpers::parseIpePath(R"IPE( <ipeselection>
        <path>
        352 384 m
        352 192 l
        </path>
        </ipeselection>
        )IPE", expectedDistLine);

        auto dist = wfr(polyA.begin(), polyA.end(), polyB.begin(), polyB.end());

        auto expectedDist = std::sqrt(sqDist(expectedDistLine[0], expectedDistLine[1]));

        REQUIRE(abs(dist - expectedDist) < MOVETK_EPS);
    }
}
