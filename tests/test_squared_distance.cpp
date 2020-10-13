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
// Created by Mitra, Aniket on 2019-02-20.
//

#include <array>
#include "catch2/catch.hpp"
#if CGAL_BACKEND_ENABLED
#include "movetk/geom/CGALTraits.h"
#else

#include "movetk/geom/BoostGeometryTraits.h"
#endif

#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/DistanceInterface.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "movetk/metric/Norm.h"

using namespace std;

#if CGAL_BACKEND_ENABLED
//==============================
// Define the Number Type
// For the CGAL backend,
// One can choose from the
// following number types
typedef long double NT;
//typedef CGAL::Mpzf NT;
//typedef CGAL::Gmpfr NT;
//typedef CGAL::Gmpq NT;
//==============================

//==============================
// Define the Dimensions
const size_t dimensions = 2;
//==============================

//==============================
// Define the Geometry Backend
typedef movetk_support::CGALTraits<NT, dimensions> CGAL_GeometryBackend;
//Using the Geometry Backend define the Movetk Geometry Kernel
typedef movetk_core::MovetkGeometryKernel<
    typename CGAL_GeometryBackend::Wrapper_CGAL_Geometry>
    MovetkGeometryKernel;
typedef movetk_support::FiniteNorm<MovetkGeometryKernel, 2> Norm;
//==============================
#else
//==============================
// Define the Number Type
// For the Boost Backend
typedef long double NT;
//==============================

//==============================
// Define the Dimensions
// It is possible to choose
// a higher dimension
// Note: Boost Geometry Adapter supports geometry in two
// dimensions only
const static size_t dimensions = 2;
//==============================

//==============================
// Define the Geometry Backend
typedef movetk_support::BoostGeometryTraits<long double, dimensions> Boost_Geometry_Backend;
//Using the Geometry Backend define the Movetk Geometry Kernel
typedef movetk_core::MovetkGeometryKernel<typename Boost_Geometry_Backend::Wrapper_Boost_Geometry> MovetkGeometryKernel;
typedef void Norm;
//==============================
#endif

TEST_CASE("Check Distance between point and segment", "[is_valid_dist_point_seg]")
{
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    MovetkGeometryKernel::MovetkPoint p1 = make_point({5.5, 3.1});
    MovetkGeometryKernel::MovetkPoint p2 = make_point({3.22, 1.3});
    MovetkGeometryKernel::MovetkPoint p3 = make_point({5, 5});
    std::cout << "Segment : {";
    std::cout << p1;
    std::cout << "},{";
    std::cout << p2;
    std::cout << "}";
    std::cout << "\nPoint: ";
    std::cout << p3;
    std::cout << "\n";
    movetk_core::MakeSegment<MovetkGeometryKernel> make_segment;
    MovetkGeometryKernel::MovetkSegment seg = make_segment(p1, p2);
    movetk_core::ComputeSquaredDistance<MovetkGeometryKernel, Norm> squared_dist;
    MovetkGeometryKernel::NT result = squared_dist(p3, seg);
    std::cout << "Squared Distance from Point to Segment: " << result << std::endl;
    REQUIRE(abs(result - 3.86) < MOVETK_EPS);

    p3 = make_point({3.85, 4.12});
    std::cout << "\nPoint: ";
    std::cout << p3;
    std::cout << "\n";
    result = squared_dist(p3, seg);
    std::cout << "Squared Distance from Point to Segment: " << result << std::endl;
    REQUIRE(abs(result - 3.3233) < MOVETK_EPS);

    p3 = make_point({1.53, 2.6});
    std::cout << "\nPoint: ";
    std::cout << p3;
    std::cout << "\n";
    result = squared_dist(p3, seg);
    std::cout << "Squared Distance from Point to Segment: " << result << std::endl;
    REQUIRE(abs(result - 4.5461) < MOVETK_EPS);
}
