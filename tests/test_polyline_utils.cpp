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

#include "catch2/catch.hpp"

#include <iostream>
#include <vector>
#include <array>
#include <numeric>
#include "movetk/TuplePrinter.h"
#include "movetk/algo/PolylineUtils.h"
#if CGAL_BACKEND_ENABLED
#include "movetk/geom/CGALTraits.h"
#else

#include "movetk/geom/BoostGeometryTraits.h"
#endif

#include "movetk/utils/Iterators.h"


using namespace movetk_algorithms;


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
            typename CGAL_GeometryBackend::Wrapper_CGAL_Geometry> MovetkGeometryKernel;
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
//==============================
#endif


TEST_CASE("trajectory length", "[trajectory_length]") {

    using Point = std::array<double, 2>;
    std::vector<Point> pts = {{0., 0.}, {2., 0.}, {2., 2.}, {0., 2.}, {0., 0.}};

    auto length = polyline_length_m(std::begin(pts), std::end(pts));
    std::cout << "length_m of segment: " << length << '\n';
    REQUIRE( length == Approx(8.0) );
}

TEST_CASE("trajectory length 2pts", "[trajectory_length_2pts]") {

    using Point = std::array<double, 2>;
    std::vector<Point> pts = {{2., 0.}, {2., 2.}};

    auto length = polyline_length_m(std::begin(pts), std::end(pts));
    std::cout << "length_m of segment: " << length << '\n';
    REQUIRE( length == Approx(2.0) );
}

TEST_CASE("trajectory length 3d 2pts", "[trajectory_length_3d_2pts]") {

    using Point = std::array<double, 3>;
    std::vector<Point> pts = {{2., 0., 1.}, {2., 3., 5.}};

    auto length = polyline_length_m(std::begin(pts), std::end(pts));
    std::cout << "length_m of segment: " << length << '\n';
    REQUIRE( length == Approx(5.0) );
}

TEST_CASE("trajectory length 3d 2pts 2", "[trajectory_length_3d_2pts_2]") {

    using Point = std::array<float, 3>;
    std::vector<Point> pts = {{186393.015625, -4740913.500000, 4248293.000000}, {186378.343750, -4740926.000000, 4248280.000000}};
    for (auto xyz: pts)
        std::cout << "xyz: " << xyz[0] << ", " << xyz[1] << ", " << xyz[2] << '\n';

    auto length = polyline_length_m(std::begin(pts), std::end(pts));
    std::cout << "length_m of segment: " << length << '\n';
    REQUIRE( length == Approx(23.249) );
}

TEST_CASE("trajectory length with movetk points", "[trajectory_length_movetk_pts]") {

    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    typedef std::vector<MovetkGeometryKernel::MovetkPoint> PolyLine;
    PolyLine polyline({make_point({0, 0}), make_point({1, 0}),
                       make_point({1, 1}), make_point({0, 1}),
                       make_point({0, 0})
                      });
    double length = polyline_length_m(std::begin(polyline), std::end(polyline));
    REQUIRE( length == Approx(4.0).epsilon(0.00001) );
}

TEST_CASE("trajectory monotonicity","[trajectory_monotonicity]"){
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    typedef std::vector<MovetkGeometryKernel::MovetkPoint> PolyLine;
    PolyLine polyline({make_point({4, 2}), make_point({14, 10}),
                       make_point({18.812, 4.0566}), make_point({20, 12}),
                       make_point({28, 14}), make_point({30, 25}),
                       make_point({31.9003, 8.43149})
                      });
    for (auto vertex: polyline) {
        std::cout << vertex;
        std::cout << ";";
    }
    std::cout<<"\n";
    std::vector<bool> result,ExpectedResult({true, true, false, true, true, false});
    movetk_algorithms::is_monotone<MovetkGeometryKernel> is_monotone;
    is_monotone(std::begin(polyline), std::end(polyline),
                movetk_core::movetk_back_insert_iterator(result));

    for (auto flag: result) {
        std::cout << flag;
        std::cout << ";";
    }
    std::cout<<"\n";
    REQUIRE(std::accumulate(std::begin(result), std::end(result),0)
    == std::accumulate(std::begin(ExpectedResult), std::end(ExpectedResult),0));
}
