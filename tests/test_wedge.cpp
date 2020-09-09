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
// Created by Mitra, Aniket on 2019-03-11.
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

typedef movetk_support::FiniteNorm<MovetkGeometryKernel, 2> Norm;

TEST_CASE("Check wedge construction 1","[is_valid_wedge_1]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    MovetkGeometryKernel::MovetkPoint ORIGIN = make_point({0, 0});
    MovetkGeometryKernel::MovetkVector e1 = make_point({1, 0}) - ORIGIN;
    MovetkGeometryKernel::MovetkVector e2 = make_point({0, 1}) - ORIGIN;
    MovetkGeometryKernel::MovetkPoint p = make_point({-4, 3});
    MovetkGeometryKernel::MovetkPoint center = make_point({3, 3});
    MovetkGeometryKernel::NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge(p, center, radius);
    MovetkGeometryKernel::MovetkVector slope = wedge.slope();
    MovetkGeometryKernel::MovetkVector intercept = wedge.intercept();
    MovetkGeometryKernel::NT result_m1 = slope * e1;
    MovetkGeometryKernel::NT result_m2 = slope * e2;
    MovetkGeometryKernel::NT result_c1 = intercept * e1;
    MovetkGeometryKernel::NT result_c2 = intercept * e2;
    std::cout<<"m1:"<<result_m1<<"\n";
    std::cout<<"c1:"<<result_c1<<"\n";
    std::cout<<"m2:"<<result_m2<<"\n";
    std::cout<<"c2:"<<result_c2<<"\n";
    REQUIRE(abs(result_m1 - 0.206284249) < MOVETK_EPS);
    REQUIRE(abs(result_c1 - 3.825136997) < MOVETK_EPS);
    REQUIRE(abs(result_m2 + 0.206284249) < MOVETK_EPS);
    REQUIRE(abs(result_c2 - 2.17486300) < MOVETK_EPS);
}


TEST_CASE("Check wedge construction 2", "[is_valid_wedge_2]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    MovetkGeometryKernel::MovetkPoint ORIGIN = make_point({0, 0});
    MovetkGeometryKernel::MovetkVector e1 = make_point({1, 0}) - ORIGIN;
    MovetkGeometryKernel::MovetkVector e2 = make_point({0, 1}) - ORIGIN;
    MovetkGeometryKernel::MovetkPoint p = make_point({1, 2});
    MovetkGeometryKernel::MovetkPoint center = make_point({3, 3});
    MovetkGeometryKernel::NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge(p, center, radius);
    MovetkGeometryKernel::MovetkVector slope = wedge.slope();
    MovetkGeometryKernel::MovetkVector intercept = wedge.intercept();
    MovetkGeometryKernel::NT result_m1 = slope * e1;
    MovetkGeometryKernel::NT result_m2 = slope * e2;
    MovetkGeometryKernel::NT result_c1 = intercept * e1;
    MovetkGeometryKernel::NT result_c2 = intercept * e2;
    std::cout << "m1:" << result_m1 << "\n";
    std::cout << "c1:" << result_c1 << "\n";
    std::cout << "m2:" << result_m2 << "\n";
    std::cout << "c2:" << result_c2 << "\n";
    REQUIRE(abs(result_m1 - 2.2247448) < MOVETK_EPS);
    REQUIRE(abs(result_c1 + 0.2247448) < MOVETK_EPS);
    REQUIRE(abs(result_m2 + 0.2247448) < MOVETK_EPS);
    REQUIRE(abs(result_c2 - 2.2247448) < MOVETK_EPS);
}


TEST_CASE("Check wedge construction 3", "[is_valid_wedge_3]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    MovetkGeometryKernel::MovetkPoint ORIGIN = make_point({0, 0});
    MovetkGeometryKernel::MovetkVector e1 = make_point({1, 0}) - ORIGIN;
    MovetkGeometryKernel::MovetkVector e2 = make_point({0, 1}) - ORIGIN;
    MovetkGeometryKernel::MovetkPoint p = make_point({-1, -2});
    MovetkGeometryKernel::MovetkPoint center = make_point({3, 3});
    MovetkGeometryKernel::NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge(p, center, radius);
    MovetkGeometryKernel::MovetkVector slope = wedge.slope();
    MovetkGeometryKernel::MovetkVector intercept = wedge.intercept();
    MovetkGeometryKernel::NT result_m1 = slope * e1;
    MovetkGeometryKernel::NT result_m2 = slope * e2;
    MovetkGeometryKernel::NT result_c1 = intercept * e1;
    MovetkGeometryKernel::NT result_c2 = intercept * e2;
    std::cout << "m1:" << result_m1 << "\n";
    std::cout << "c1:" << result_c1 << "\n";
    std::cout << "m2:" << result_m2 << "\n";
    std::cout << "c2:" << result_c2 << "\n";
    REQUIRE(abs(result_m1 - 2.059411490) < MOVETK_EPS);
    REQUIRE(abs(result_c1 - 0.059411490) < MOVETK_EPS);
    REQUIRE(abs(result_m2 - 0.797731366) < MOVETK_EPS);
    REQUIRE(abs(result_c2 + 1.202268633) < MOVETK_EPS);
}


TEST_CASE("Check wedge construction 4", "[is_valid_wedge_4]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    MovetkGeometryKernel::MovetkPoint ORIGIN = make_point({0, 0});
    MovetkGeometryKernel::MovetkVector e1 = make_point({1, 0}) - ORIGIN;
    MovetkGeometryKernel::MovetkVector e2 = make_point({0, 1}) - ORIGIN;
    MovetkGeometryKernel::MovetkPoint p = make_point({3, -2});
    MovetkGeometryKernel::MovetkPoint center = make_point({3, 3});
    MovetkGeometryKernel::NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge(p, center, radius);
    MovetkGeometryKernel::MovetkVector slope = wedge.slope();
    MovetkGeometryKernel::MovetkVector intercept = wedge.intercept();
    MovetkGeometryKernel::NT result_m1 = slope * e1;
    MovetkGeometryKernel::NT result_m2 = slope * e2;
    MovetkGeometryKernel::NT result_c1 = intercept * e1;
    MovetkGeometryKernel::NT result_c2 = intercept * e2;
    std::cout << "m1:" << result_m1 << "\n";
    std::cout << "c1:" << result_c1 << "\n";
    std::cout << "m2:" << result_m2 << "\n";
    std::cout << "c2:" << result_c2 << "\n";
    REQUIRE(abs(result_m1 + 3.39116499) < MOVETK_EPS);
    REQUIRE(abs(result_c1 - 8.173494) < MOVETK_EPS);
    REQUIRE(abs(result_m2 - 3.39116499) < MOVETK_EPS);
    REQUIRE(abs(result_c2 + 12.173494) < MOVETK_EPS);
}


TEST_CASE("Check wedge construction 5", "[is_valid_wedge_5]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    MovetkGeometryKernel::MovetkPoint ORIGIN = make_point({0, 0});
    MovetkGeometryKernel::MovetkVector e1 = make_point({1, 0}) - ORIGIN;
    MovetkGeometryKernel::MovetkVector e2 = make_point({0, 1}) - ORIGIN;
    MovetkGeometryKernel::MovetkPoint p = make_point({5, 3});
    MovetkGeometryKernel::MovetkPoint center = make_point({3, 3});
    MovetkGeometryKernel::NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge(p, center, radius);
    MovetkGeometryKernel::MovetkVector slope = wedge.slope();
    MovetkGeometryKernel::MovetkVector intercept = wedge.intercept();
    MovetkGeometryKernel::NT result_m1 = slope * e1;
    MovetkGeometryKernel::NT result_m2 = slope * e2;
    MovetkGeometryKernel::NT result_c1 = intercept * e1;
    MovetkGeometryKernel::NT result_c2 = intercept * e2;
    std::cout << "m1:" << result_m1 << "\n";
    std::cout << "c1:" << result_c1 << "\n";
    std::cout << "m2:" << result_m2 << "\n";
    std::cout << "c2:" << result_c2 << "\n";
    REQUIRE(abs(result_m1 + 1) < MOVETK_EPS);
    REQUIRE(abs(result_c1 - 8) < MOVETK_EPS);
    REQUIRE(abs(result_m2 - 1) < MOVETK_EPS);
    REQUIRE(abs(result_c2 + 2) < MOVETK_EPS);
}



TEST_CASE("Check wedge construction 6","[is_valid_wedge_6]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    MovetkGeometryKernel::MovetkPoint ORIGIN = make_point({0, 0});
    MovetkGeometryKernel::MovetkVector e1 = make_point({1, 0}) - ORIGIN;
    MovetkGeometryKernel::MovetkVector e2 = make_point({0, 1}) - ORIGIN;
    MovetkGeometryKernel::MovetkPoint p = make_point({-2, 7});
    MovetkGeometryKernel::MovetkPoint center = make_point({3, 3});
    MovetkGeometryKernel::NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge(p, center, radius);
    MovetkGeometryKernel::MovetkVector slope = wedge.slope();
    MovetkGeometryKernel::MovetkVector intercept = wedge.intercept();
    MovetkGeometryKernel::NT result_m1 = slope * e1;
    MovetkGeometryKernel::NT result_m2 = slope * e2;
    MovetkGeometryKernel::NT result_c1 = intercept * e1;
    MovetkGeometryKernel::NT result_c2 = intercept * e2;
    std::cout<<"m1:"<<result_m1<<"\n";
    std::cout<<"c1:"<<result_c1<<"\n";
    std::cout<<"m2:"<<result_m2<<"\n";
    std::cout<<"c2:"<<result_c2<<"\n";
    REQUIRE(abs(result_m1 + 0.4855756145) < MOVETK_EPS);
    REQUIRE(abs(result_c1 - 6.0288487709) < MOVETK_EPS);
    REQUIRE(abs(result_m2 + 1.2535548202) < MOVETK_EPS);
    REQUIRE(abs(result_c2 - 4.4928903594) < MOVETK_EPS);
}


TEST_CASE("Check wedge construction 7","[is_valid_wedge_7]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    MovetkGeometryKernel::MovetkPoint ORIGIN = make_point({0, 0});
    MovetkGeometryKernel::MovetkVector e1 = make_point({1, 0}) - ORIGIN;
    MovetkGeometryKernel::MovetkVector e2 = make_point({0, 1}) - ORIGIN;
    MovetkGeometryKernel::MovetkPoint p = make_point({6, -2});
    MovetkGeometryKernel::MovetkPoint center = make_point({3, 3});
    MovetkGeometryKernel::NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge(p, center, radius);
    MovetkGeometryKernel::MovetkVector slope = wedge.slope();
    MovetkGeometryKernel::MovetkVector intercept = wedge.intercept();
    MovetkGeometryKernel::NT result_m1 = slope * e1;
    MovetkGeometryKernel::NT result_m2 = slope * e2;
    MovetkGeometryKernel::NT result_c1 = intercept * e1;
    MovetkGeometryKernel::NT result_c2 = intercept * e2;
    std::cout<<"m1:"<<result_m1<<"\n";
    std::cout<<"c1:"<<result_c1<<"\n";
    std::cout<<"m2:"<<result_m2<<"\n";
    std::cout<<"c2:"<<result_c2<<"\n";
    REQUIRE(abs(result_m2 + 1) < MOVETK_EPS);
    REQUIRE(abs(result_c2 - 4) < MOVETK_EPS);
    REQUIRE(abs(result_m1 + 3.28571428) < MOVETK_EPS);
    REQUIRE(abs(result_c1 - 17.71428571) < MOVETK_EPS);
}


TEST_CASE("Check point in wedge 1","[is_point_in_wedge_1]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    MovetkGeometryKernel::MovetkPoint ORIGIN = make_point({0, 0});
    MovetkGeometryKernel::MovetkVector e1 = make_point({1, 0}) - ORIGIN;
    MovetkGeometryKernel::MovetkVector e2 = make_point({0, 1}) - ORIGIN;
    MovetkGeometryKernel::MovetkPoint p = make_point({6, -2});
    MovetkGeometryKernel::MovetkPoint center = make_point({-3, -3});
    MovetkGeometryKernel::NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge(p, center, radius);
    MovetkGeometryKernel::MovetkPoint p1 = make_point({-7, -5});
    MovetkGeometryKernel::MovetkPoint p2 = make_point({-7, -6});
    MovetkGeometryKernel::MovetkVector slope = wedge.slope();
    MovetkGeometryKernel::MovetkVector intercept = wedge.intercept();
    MovetkGeometryKernel::NT result_m1 = slope * e1;
    MovetkGeometryKernel::NT result_m2 = slope * e2;
    MovetkGeometryKernel::NT result_c1 = intercept * e1;
    MovetkGeometryKernel::NT result_c2 = intercept * e2;
    std::cout<<"m1:"<<result_m1<<"\n";
    std::cout<<"c1:"<<result_c1<<"\n";
    std::cout<<"m2:"<<result_m2<<"\n";
    std::cout<<"c2:"<<result_c2<<"\n";
    REQUIRE(wedge * p1 == 1);
    REQUIRE(wedge * p2 == 0);
}



TEST_CASE("Check point in wedge 2","[is_point_in_wedge_2]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    MovetkGeometryKernel::MovetkPoint ORIGIN = make_point({0, 0});
    MovetkGeometryKernel::MovetkVector e1 = make_point({1, 0}) - ORIGIN;
    MovetkGeometryKernel::MovetkVector e2 = make_point({0, 1}) - ORIGIN;
    MovetkGeometryKernel::MovetkPoint p = make_point({-4, 3});
    MovetkGeometryKernel::MovetkPoint center = make_point({3, 3});
    MovetkGeometryKernel::NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge(p, center, radius);
    MovetkGeometryKernel::MovetkPoint p1 = make_point({-2, 3});
    MovetkGeometryKernel::MovetkPoint p2 = make_point({-3, 5});
    MovetkGeometryKernel::MovetkVector slope = wedge.slope();
    MovetkGeometryKernel::MovetkVector intercept = wedge.intercept();
    MovetkGeometryKernel::NT result_m1 = slope * e1;
    MovetkGeometryKernel::NT result_m2 = slope * e2;
    MovetkGeometryKernel::NT result_c1 = intercept * e1;
    MovetkGeometryKernel::NT result_c2 = intercept * e2;
    std::cout<<"m1:"<<result_m1<<"\n";
    std::cout<<"c1:"<<result_c1<<"\n";
    std::cout<<"m2:"<<result_m2<<"\n";
    std::cout<<"c2:"<<result_c2<<"\n";
    REQUIRE(wedge * p1 == 1);
    REQUIRE(wedge * p2 == 0);
}



TEST_CASE("Check point in wedge 3","[is_point_in_wedge_3]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    MovetkGeometryKernel::MovetkPoint ORIGIN = make_point({0, 0});
    MovetkGeometryKernel::MovetkVector e1 = make_point({1, 0}) - ORIGIN;
    MovetkGeometryKernel::MovetkVector e2 = make_point({0, 1}) - ORIGIN;
    MovetkGeometryKernel::MovetkPoint p = make_point({-14, -10});
    MovetkGeometryKernel::MovetkPoint center = make_point({-3, -3});
    MovetkGeometryKernel::NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge(p, center, radius);
    MovetkGeometryKernel::MovetkPoint p1 = make_point({-7, -5});
    MovetkGeometryKernel::MovetkPoint p2 = make_point({-7, -6});
    MovetkGeometryKernel::MovetkVector slope = wedge.slope();
    MovetkGeometryKernel::MovetkVector intercept = wedge.intercept();
    MovetkGeometryKernel::NT result_m1 = slope * e1;
    MovetkGeometryKernel::NT result_m2 = slope * e2;
    MovetkGeometryKernel::NT result_c1 = intercept * e1;
    MovetkGeometryKernel::NT result_c2 = intercept * e2;
    std::cout<<"m1:"<<result_m1<<"\n";
    std::cout<<"c1:"<<result_c1<<"\n";
    std::cout<<"m2:"<<result_m2<<"\n";
    std::cout<<"c2:"<<result_c2<<"\n";
    REQUIRE(wedge * p1 == 1);
    REQUIRE(wedge * p2 == 1);
}



TEST_CASE("Check point in wedge 4","[is_point_in_wedge_4]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    MovetkGeometryKernel::MovetkPoint ORIGIN = make_point({0, 0});
    MovetkGeometryKernel::MovetkVector e1 = make_point({1, 0}) - ORIGIN;
    MovetkGeometryKernel::MovetkVector e2 = make_point({0, 1}) - ORIGIN;
    MovetkGeometryKernel::MovetkPoint p = make_point({-14, 10});
    MovetkGeometryKernel::MovetkPoint center = make_point({-3, -3});
    MovetkGeometryKernel::NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge(p, center, radius);
    MovetkGeometryKernel::MovetkPoint p1 = make_point({0, -6});
    MovetkGeometryKernel::MovetkPoint p2 = make_point({-7, -6});
    MovetkGeometryKernel::MovetkVector slope = wedge.slope();
    MovetkGeometryKernel::MovetkVector intercept = wedge.intercept();
    MovetkGeometryKernel::NT result_m1 = slope * e1;
    MovetkGeometryKernel::NT result_m2 = slope * e2;
    MovetkGeometryKernel::NT result_c1 = intercept * e1;
    MovetkGeometryKernel::NT result_c2 = intercept * e2;
    std::cout<<"m1:"<<result_m1<<"\n";
    std::cout<<"c1:"<<result_c1<<"\n";
    std::cout<<"m2:"<<result_m2<<"\n";
    std::cout<<"c2:"<<result_c2<<"\n";
    REQUIRE(wedge * p1 == 1);
    REQUIRE(wedge * p2 == 0);
}


TEST_CASE("Check point in wedge 5","[is_point_in_wedge_5]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    MovetkGeometryKernel::MovetkPoint ORIGIN = make_point({0, 0});
    MovetkGeometryKernel::MovetkVector e1 = make_point({1, 0}) - ORIGIN;
    MovetkGeometryKernel::MovetkVector e2 = make_point({0, 1}) - ORIGIN;
    MovetkGeometryKernel::MovetkPoint p = make_point({12, -6});
    MovetkGeometryKernel::MovetkPoint center = make_point({3, -3});
    MovetkGeometryKernel::NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge(p, center, radius);
    MovetkGeometryKernel::MovetkPoint p1 = make_point({-7, -6});
    MovetkGeometryKernel::MovetkPoint p2 = make_point({-3.001944, 1.64825});
    MovetkGeometryKernel::MovetkVector slope = wedge.slope();
    MovetkGeometryKernel::MovetkVector intercept = wedge.intercept();
    MovetkGeometryKernel::NT result_m1 = slope * e1;
    MovetkGeometryKernel::NT result_m2 = slope * e2;
    MovetkGeometryKernel::NT result_c1 = intercept * e1;
    MovetkGeometryKernel::NT result_c2 = intercept * e2;
    std::cout<<"m1:"<<result_m1<<"\n";
    std::cout<<"c1:"<<result_c1<<"\n";
    std::cout<<"m2:"<<result_m2<<"\n";
    std::cout<<"c2:"<<result_c2<<"\n";
    REQUIRE(wedge * p1 == 0);
    REQUIRE(wedge * p2 == 0);
}

TEST_CASE("Check wedge intersection 1", "[wedge_intersection_1]"){
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    MovetkGeometryKernel::MovetkPoint ORIGIN = make_point({0, 0});
    MovetkGeometryKernel::MovetkVector e1 = make_point({1, 0}) - ORIGIN;
    MovetkGeometryKernel::MovetkVector e2 = make_point({0, 1}) - ORIGIN;
    MovetkGeometryKernel::MovetkPoint p1 = make_point({-4, 3});
    MovetkGeometryKernel::MovetkPoint center1 = make_point({3, 3});
    MovetkGeometryKernel::MovetkPoint center2 = make_point({-2, 3});
    MovetkGeometryKernel::NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge1(p1, center1, radius);
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge2(p1, center2, radius);
    movetk_core::Wedge<MovetkGeometryKernel, Norm> intersection = wedge1 * wedge2;
    MovetkGeometryKernel::MovetkVector slope = intersection.slope();
    MovetkGeometryKernel::MovetkVector intercept = intersection.intercept();
    MovetkGeometryKernel::NT result_m1 = slope * e1;
    MovetkGeometryKernel::NT result_m2 = slope * e2;
    MovetkGeometryKernel::NT result_c1 = intercept * e1;
    MovetkGeometryKernel::NT result_c2 = intercept * e2;
    std::cout<<"m1:"<<result_m1<<"\n";
    std::cout<<"c1:"<<result_c1<<"\n";
    std::cout<<"m2:"<<result_m2<<"\n";
    std::cout<<"c2:"<<result_c2<<"\n";
    REQUIRE(abs(result_m1 - 0.206284249) < MOVETK_EPS);
    REQUIRE(abs(result_c1 - 3.825136997) < MOVETK_EPS);
    REQUIRE(abs(result_m2 + 0.206284249) < MOVETK_EPS);
    REQUIRE(abs(result_c2 - 2.174863002) < MOVETK_EPS);
}



TEST_CASE("Check wedge intersection 2", "[wedge_intersection_2]"){
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    MovetkGeometryKernel::MovetkPoint ORIGIN = make_point({0, 0});
    MovetkGeometryKernel::MovetkVector e1 = make_point({1, 0}) - ORIGIN;
    MovetkGeometryKernel::MovetkVector e2 = make_point({0, 1}) - ORIGIN;
    MovetkGeometryKernel::MovetkPoint p1 = make_point({-4, 3});
    MovetkGeometryKernel::MovetkPoint center1 = make_point({3, 3});
    MovetkGeometryKernel::MovetkPoint center2 = make_point({-2, 5});
    MovetkGeometryKernel::NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge1(p1, center1, radius);
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge2(p1, center2, radius);
    movetk_core::Wedge<MovetkGeometryKernel, Norm> intersection = wedge1 * wedge2;
    MovetkGeometryKernel::MovetkVector slope = intersection.slope();
    MovetkGeometryKernel::MovetkVector intercept = intersection.intercept();
    MovetkGeometryKernel::NT result_m1 = slope * e1;
    MovetkGeometryKernel::NT result_m2 = slope * e2;
    MovetkGeometryKernel::NT result_c1 = intercept * e1;
    MovetkGeometryKernel::NT result_c2 = intercept * e2;
    std::cout<<"m1:"<<result_m1<<"\n";
    std::cout<<"c1:"<<result_c1<<"\n";
    std::cout<<"m2:"<<result_m2<<"\n";
    std::cout<<"c2:"<<result_c2<<"\n";
    REQUIRE(abs(result_m1 - 0) < MOVETK_EPS);
    REQUIRE(abs(result_c1 - 0) < MOVETK_EPS);
    REQUIRE(abs(result_m2 - 0) < MOVETK_EPS);
    REQUIRE(abs(result_c2 - 0) < MOVETK_EPS);
}


TEST_CASE("Check wedge intersection 3", "[wedge_intersection_3]"){
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    MovetkGeometryKernel::MovetkPoint ORIGIN = make_point({0, 0});
    MovetkGeometryKernel::MovetkVector e1 = make_point({1, 0}) - ORIGIN;
    MovetkGeometryKernel::MovetkVector e2 = make_point({0, 1}) - ORIGIN;
    MovetkGeometryKernel::MovetkPoint p1 = make_point({-4, 3});
    MovetkGeometryKernel::MovetkPoint center1 = make_point({3, 3});
    MovetkGeometryKernel::MovetkPoint center2 = make_point({-0.5, 1});
    MovetkGeometryKernel::NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge1(p1, center1, radius);
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge2(p1, center2, radius);
    movetk_core::Wedge<MovetkGeometryKernel, Norm> intersection = wedge1 * wedge2;
    MovetkGeometryKernel::MovetkVector slope = intersection.slope();
    MovetkGeometryKernel::MovetkVector intercept = intersection.intercept();
    MovetkGeometryKernel::NT result_m1 = slope * e1;
    MovetkGeometryKernel::NT result_m2 = slope * e2;
    MovetkGeometryKernel::NT result_c1 = intercept * e1;
    MovetkGeometryKernel::NT result_c2 = intercept * e2;
    std::cout<<"m1:"<<result_m1<<"\n";
    std::cout<<"c1:"<<result_c1<<"\n";
    std::cout<<"m2:"<<result_m2<<"\n";
    std::cout<<"c2:"<<result_c2<<"\n";
    REQUIRE(abs(result_m1 + 0.1620937) < MOVETK_EPS);
    REQUIRE(abs(result_c1 - 2.3516250) < MOVETK_EPS);
    REQUIRE(abs(result_m2 + 0.2062842) < MOVETK_EPS);
    REQUIRE(abs(result_c2 - 2.1748630) < MOVETK_EPS);
}
