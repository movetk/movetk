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
#include <catch2/catch.hpp>

#include "movetk/geom/GeometryInterface.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "movetk/metric/Norm.h"

#include "test_includes.h"

template<typename Backend>
struct WedgeTests : public test_helpers::BaseTestFixture<Backend> {
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
    using Wedge = geom::Wedge<MovetkGeometryKernel, Norm>;
};

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WedgeTests, "Check wedge construction 1","[is_valid_wedge_1]") {
    auto ORIGIN = make_point({0, 0});
    auto e1 = make_point({1, 0}) - ORIGIN;
    auto e2 = make_point({0, 1}) - ORIGIN;
    auto p = make_point({-4, 3});
    auto center = make_point({3, 3});
    NT radius = 1.41421356;
    Wedge wedge(p, center, radius);
    auto slope = wedge.slope();
    auto intercept = wedge.intercept();
    const auto result_m1 = slope * e1;
    const auto result_m2 = slope * e2;
    const auto result_c1 = intercept * e1;
    const auto result_c2 = intercept * e2;
    REQUIRE(abs(result_m1 - 0.206284249) < MOVETK_EPS);
    REQUIRE(abs(result_c1 - 3.825136997) < MOVETK_EPS);
    REQUIRE(abs(result_m2 + 0.206284249) < MOVETK_EPS);
    REQUIRE(abs(result_c2 - 2.17486300) < MOVETK_EPS);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WedgeTests, "Check wedge construction 2", "[is_valid_wedge_2]") {
    auto ORIGIN = make_point({0, 0});
    auto e1 = make_point({1, 0}) - ORIGIN;
    auto e2 = make_point({0, 1}) - ORIGIN;
    auto p = make_point({1, 2});
    auto center = make_point({3, 3});
    NT radius = 1.41421356;
    Wedge wedge(p, center, radius);
    auto slope = wedge.slope();
    auto intercept = wedge.intercept();
    const auto result_m1 = slope * e1;
    const auto result_m2 = slope * e2;
    const auto result_c1 = intercept * e1;
    const auto result_c2 = intercept * e2;
    REQUIRE(abs(result_m1 - 2.2247448) < MOVETK_EPS);
    REQUIRE(abs(result_c1 + 0.2247448) < MOVETK_EPS);
    REQUIRE(abs(result_m2 + 0.2247448) < MOVETK_EPS);
    REQUIRE(abs(result_c2 - 2.2247448) < MOVETK_EPS);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WedgeTests,"Check wedge construction 3", "[is_valid_wedge_3]") {
    auto ORIGIN = make_point({0, 0});
    auto e1 = make_point({1, 0}) - ORIGIN;
    auto e2 = make_point({0, 1}) - ORIGIN;
    auto p = make_point({-1, -2});
    auto center = make_point({3, 3});
    NT radius = 1.41421356;
    Wedge wedge(p, center, radius);
    auto slope = wedge.slope();
    auto intercept = wedge.intercept();
    NT result_m1 = slope * e1;
    NT result_m2 = slope * e2;
    NT result_c1 = intercept * e1;
    NT result_c2 = intercept * e2;
    REQUIRE(abs(result_m1 - 2.059411490) < MOVETK_EPS);
    REQUIRE(abs(result_c1 - 0.059411490) < MOVETK_EPS);
    REQUIRE(abs(result_m2 - 0.797731366) < MOVETK_EPS);
    REQUIRE(abs(result_c2 + 1.202268633) < MOVETK_EPS);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WedgeTests,"Check wedge construction 4", "[is_valid_wedge_4]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    auto ORIGIN = make_point({0, 0});
    auto e1 = make_point({1, 0}) - ORIGIN;
    auto e2 = make_point({0, 1}) - ORIGIN;
    auto p = make_point({3, -2});
    auto center = make_point({3, 3});
    NT radius = 1.41421356;
    Wedge wedge(p, center, radius);
    auto slope = wedge.slope();
    auto intercept = wedge.intercept();
    NT result_m1 = slope * e1;
    NT result_m2 = slope * e2;
    NT result_c1 = intercept * e1;
    NT result_c2 = intercept * e2;
    REQUIRE(abs(result_m1 + 3.39116499) < MOVETK_EPS);
    REQUIRE(abs(result_c1 - 8.173494) < MOVETK_EPS);
    REQUIRE(abs(result_m2 - 3.39116499) < MOVETK_EPS);
    REQUIRE(abs(result_c2 + 12.173494) < MOVETK_EPS);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WedgeTests,"Check wedge construction 5", "[is_valid_wedge_5]") {
    auto ORIGIN = make_point({0, 0});
    auto e1 = make_point({1, 0}) - ORIGIN;
    auto e2 = make_point({0, 1}) - ORIGIN;
    auto p = make_point({5, 3});
    auto center = make_point({3, 3});
    NT radius = 1.41421356;
    Wedge wedge(p, center, radius);
    auto slope = wedge.slope();
    auto intercept = wedge.intercept();
    NT result_m1 = slope * e1;
    NT result_m2 = slope * e2;
    NT result_c1 = intercept * e1;
    NT result_c2 = intercept * e2;
    REQUIRE(abs(result_m1 + 1) < MOVETK_EPS);
    REQUIRE(abs(result_c1 - 8) < MOVETK_EPS);
    REQUIRE(abs(result_m2 - 1) < MOVETK_EPS);
    REQUIRE(abs(result_c2 + 2) < MOVETK_EPS);
}



MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WedgeTests,"Check wedge construction 6","[is_valid_wedge_6]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    auto ORIGIN = make_point({0, 0});
    auto e1 = make_point({1, 0}) - ORIGIN;
    auto e2 = make_point({0, 1}) - ORIGIN;
    auto p = make_point({-2, 7});
    auto center = make_point({3, 3});
    NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge(p, center, radius);
    auto slope = wedge.slope();
    auto intercept = wedge.intercept();
    const auto result_m1 = slope * e1;
    const auto result_m2 = slope * e2;
    const auto result_c1 = intercept * e1;
    const auto result_c2 = intercept * e2;
    REQUIRE(abs(result_m1 + 0.4855756145) < MOVETK_EPS);
    REQUIRE(abs(result_c1 - 6.0288487709) < MOVETK_EPS);
    REQUIRE(abs(result_m2 + 1.2535548202) < MOVETK_EPS);
    REQUIRE(abs(result_c2 - 4.4928903594) < MOVETK_EPS);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WedgeTests,"Check wedge construction 7","[is_valid_wedge_7]") {
    auto ORIGIN = make_point({0, 0});
    auto e1 = make_point({1, 0}) - ORIGIN;
    auto e2 = make_point({0, 1}) - ORIGIN;
    auto p = make_point({6, -2});
    auto center = make_point({3, 3});
    NT radius = 1.41421356;
    Wedge wedge(p, center, radius);
    auto slope = wedge.slope();
    auto intercept = wedge.intercept();
    NT result_m1 = slope * e1;
    NT result_m2 = slope * e2;
    NT result_c1 = intercept * e1;
    NT result_c2 = intercept * e2;
    REQUIRE(abs(result_m2 + 1) < MOVETK_EPS);
    REQUIRE(abs(result_c2 - 4) < MOVETK_EPS);
    REQUIRE(abs(result_m1 + 3.28571428) < MOVETK_EPS);
    REQUIRE(abs(result_c1 - 17.71428571) < MOVETK_EPS);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WedgeTests,"Check point in wedge 1","[is_point_in_wedge_1]") {
    auto ORIGIN = make_point({0, 0});
    auto e1 = make_point({1, 0}) - ORIGIN;
    auto e2 = make_point({0, 1}) - ORIGIN;
    auto p = make_point({6, -2});
    auto center = make_point({-3, -3});
    NT radius = 1.41421356;
    Wedge wedge(p, center, radius);
    auto p1 = make_point({-7, -5});
    auto p2 = make_point({-7, -6});
    auto slope = wedge.slope();
    auto intercept = wedge.intercept();
    NT result_m1 = slope * e1;
    NT result_m2 = slope * e2;
    NT result_c1 = intercept * e1;
    NT result_c2 = intercept * e2;
    REQUIRE(wedge * p1 == 1);
    REQUIRE(wedge * p2 == 0);
}



MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WedgeTests,"Check point in wedge 2","[is_point_in_wedge_2]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    auto ORIGIN = make_point({0, 0});
    auto e1 = make_point({1, 0}) - ORIGIN;
    auto e2 = make_point({0, 1}) - ORIGIN;
    auto p = make_point({-4, 3});
    auto center = make_point({3, 3});
    NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge(p, center, radius);
    auto p1 = make_point({-2, 3});
    auto p2 = make_point({-3, 5});
    auto slope = wedge.slope();
    auto intercept = wedge.intercept();
    NT result_m1 = slope * e1;
    NT result_m2 = slope * e2;
    NT result_c1 = intercept * e1;
    NT result_c2 = intercept * e2;
    std::cout<<"m1:"<<result_m1<<"\n";
    std::cout<<"c1:"<<result_c1<<"\n";
    std::cout<<"m2:"<<result_m2<<"\n";
    std::cout<<"c2:"<<result_c2<<"\n";
    REQUIRE(wedge * p1 == 1);
    REQUIRE(wedge * p2 == 0);
}



MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WedgeTests,"Check point in wedge 3","[is_point_in_wedge_3]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    auto ORIGIN = make_point({0, 0});
    auto e1 = make_point({1, 0}) - ORIGIN;
    auto e2 = make_point({0, 1}) - ORIGIN;
    auto p = make_point({-14, -10});
    auto center = make_point({-3, -3});
    NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge(p, center, radius);
    auto p1 = make_point({-7, -5});
    auto p2 = make_point({-7, -6});
    auto slope = wedge.slope();
    auto intercept = wedge.intercept();
    NT result_m1 = slope * e1;
    NT result_m2 = slope * e2;
    NT result_c1 = intercept * e1;
    NT result_c2 = intercept * e2;
    std::cout<<"m1:"<<result_m1<<"\n";
    std::cout<<"c1:"<<result_c1<<"\n";
    std::cout<<"m2:"<<result_m2<<"\n";
    std::cout<<"c2:"<<result_c2<<"\n";
    REQUIRE(wedge * p1 == 1);
    REQUIRE(wedge * p2 == 1);
}



MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WedgeTests,"Check point in wedge 4","[is_point_in_wedge_4]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    auto ORIGIN = make_point({0, 0});
    auto e1 = make_point({1, 0}) - ORIGIN;
    auto e2 = make_point({0, 1}) - ORIGIN;
    auto p = make_point({-14, 10});
    auto center = make_point({-3, -3});
    NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge(p, center, radius);
    auto p1 = make_point({0, -6});
    auto p2 = make_point({-7, -6});
    auto slope = wedge.slope();
    auto intercept = wedge.intercept();
    NT result_m1 = slope * e1;
    NT result_m2 = slope * e2;
    NT result_c1 = intercept * e1;
    NT result_c2 = intercept * e2;
    std::cout<<"m1:"<<result_m1<<"\n";
    std::cout<<"c1:"<<result_c1<<"\n";
    std::cout<<"m2:"<<result_m2<<"\n";
    std::cout<<"c2:"<<result_c2<<"\n";
    REQUIRE(wedge * p1 == 1);
    REQUIRE(wedge * p2 == 0);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WedgeTests,"Check point in wedge 5","[is_point_in_wedge_5]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    auto ORIGIN = make_point({0, 0});
    auto e1 = make_point({1, 0}) - ORIGIN;
    auto e2 = make_point({0, 1}) - ORIGIN;
    auto p = make_point({12, -6});
    auto center = make_point({3, -3});
    NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge(p, center, radius);
    auto p1 = make_point({-7, -6});
    auto p2 = make_point({-3.001944, 1.64825});
    auto slope = wedge.slope();
    auto intercept = wedge.intercept();
    NT result_m1 = slope * e1;
    NT result_m2 = slope * e2;
    NT result_c1 = intercept * e1;
    NT result_c2 = intercept * e2;
    std::cout<<"m1:"<<result_m1<<"\n";
    std::cout<<"c1:"<<result_c1<<"\n";
    std::cout<<"m2:"<<result_m2<<"\n";
    std::cout<<"c2:"<<result_c2<<"\n";
    REQUIRE(wedge * p1 == 0);
    REQUIRE(wedge * p2 == 0);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WedgeTests,"Check wedge intersection 1", "[wedge_intersection_1]"){
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    auto ORIGIN = make_point({0, 0});
    auto e1 = make_point({1, 0}) - ORIGIN;
    auto e2 = make_point({0, 1}) - ORIGIN;
    auto p1 = make_point({-4, 3});
    auto center1 = make_point({3, 3});
    auto center2 = make_point({-2, 3});
    NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge1(p1, center1, radius);
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge2(p1, center2, radius);
    movetk_core::Wedge<MovetkGeometryKernel, Norm> intersection = wedge1 * wedge2;
    auto slope = intersection.slope();
    auto intercept = intersection.intercept();
    NT result_m1 = slope * e1;
    NT result_m2 = slope * e2;
    NT result_c1 = intercept * e1;
    NT result_c2 = intercept * e2;
    std::cout<<"m1:"<<result_m1<<"\n";
    std::cout<<"c1:"<<result_c1<<"\n";
    std::cout<<"m2:"<<result_m2<<"\n";
    std::cout<<"c2:"<<result_c2<<"\n";
    REQUIRE(abs(result_m1 - 0.206284249) < MOVETK_EPS);
    REQUIRE(abs(result_c1 - 3.825136997) < MOVETK_EPS);
    REQUIRE(abs(result_m2 + 0.206284249) < MOVETK_EPS);
    REQUIRE(abs(result_c2 - 2.174863002) < MOVETK_EPS);
}



MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WedgeTests,"Check wedge intersection 2", "[wedge_intersection_2]"){
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    auto ORIGIN = make_point({0, 0});
    auto e1 = make_point({1, 0}) - ORIGIN;
    auto e2 = make_point({0, 1}) - ORIGIN;
    auto p1 = make_point({-4, 3});
    auto center1 = make_point({3, 3});
    auto center2 = make_point({-2, 5});
    NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge1(p1, center1, radius);
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge2(p1, center2, radius);
    movetk_core::Wedge<MovetkGeometryKernel, Norm> intersection = wedge1 * wedge2;
    auto slope = intersection.slope();
    auto intercept = intersection.intercept();
    NT result_m1 = slope * e1;
    NT result_m2 = slope * e2;
    NT result_c1 = intercept * e1;
    NT result_c2 = intercept * e2;
    std::cout<<"m1:"<<result_m1<<"\n";
    std::cout<<"c1:"<<result_c1<<"\n";
    std::cout<<"m2:"<<result_m2<<"\n";
    std::cout<<"c2:"<<result_c2<<"\n";
    REQUIRE(abs(result_m1 - 0) < MOVETK_EPS);
    REQUIRE(abs(result_c1 - 0) < MOVETK_EPS);
    REQUIRE(abs(result_m2 - 0) < MOVETK_EPS);
    REQUIRE(abs(result_c2 - 0) < MOVETK_EPS);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WedgeTests,"Check wedge intersection 3", "[wedge_intersection_3]"){
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    auto ORIGIN = make_point({0, 0});
    auto e1 = make_point({1, 0}) - ORIGIN;
    auto e2 = make_point({0, 1}) - ORIGIN;
    auto p1 = make_point({-4, 3});
    auto center1 = make_point({3, 3});
    auto center2 = make_point({-0.5, 1});
    NT radius = 1.41421356;
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge1(p1, center1, radius);
    movetk_core::Wedge<MovetkGeometryKernel, Norm> wedge2(p1, center2, radius);
    movetk_core::Wedge<MovetkGeometryKernel, Norm> intersection = wedge1 * wedge2;
    auto slope = intersection.slope();
    auto intercept = intersection.intercept();
    NT result_m1 = slope * e1;
    NT result_m2 = slope * e2;
    NT result_c1 = intercept * e1;
    NT result_c2 = intercept * e2;
    std::cout<<"m1:"<<result_m1<<"\n";
    std::cout<<"c1:"<<result_c1<<"\n";
    std::cout<<"m2:"<<result_m2<<"\n";
    std::cout<<"c2:"<<result_c2<<"\n";
    REQUIRE(abs(result_m1 + 0.1620937) < MOVETK_EPS);
    REQUIRE(abs(result_c1 - 2.3516250) < MOVETK_EPS);
    REQUIRE(abs(result_m2 + 0.2062842) < MOVETK_EPS);
    REQUIRE(abs(result_c2 - 2.1748630) < MOVETK_EPS);
}
