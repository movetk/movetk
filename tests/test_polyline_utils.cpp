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
#include <catch2/catch.hpp>
#include <iostream>
#include <numeric>
#include <vector>

#include "helpers/CustomCatchTemplate.h"
#include "movetk/PolylineUtils.h"
#include "movetk/io/TuplePrinter.h"
#include "movetk/utils/Iterators.h"


TEST_CASE("trajectory length", "[trajectory_length]") {
	using Point = std::array<double, 2>;
	std::vector<Point> pts = {{0., 0.}, {2., 0.}, {2., 2.}, {0., 2.}, {0., 0.}};

	auto length = movetk::algo::polyline_length_m(std::begin(pts), std::end(pts));
	REQUIRE(length == Approx(8.0));
}

TEST_CASE("trajectory length 2pts", "[trajectory_length_2pts]") {
	using Point = std::array<double, 2>;
	std::vector<Point> pts = {{2., 0.}, {2., 2.}};

	auto length = movetk::algo::polyline_length_m(std::begin(pts), std::end(pts));
	REQUIRE(length == Approx(2.0));
}

TEST_CASE("trajectory length 3d 2pts", "[trajectory_length_3d_2pts]") {
	using Point = std::array<double, 3>;
	std::vector<Point> pts = {{2., 0., 1.}, {2., 3., 5.}};

	auto length = movetk::algo::polyline_length_m(std::begin(pts), std::end(pts));
	REQUIRE(length == Approx(5.0));
}

TEST_CASE("trajectory length 3d 2pts 2", "[trajectory_length_3d_2pts_2]") {
	using Point = std::array<float, 3>;
	std::vector<Point> pts = {{186393.015625, -4740913.500000, 4248293.000000},
	                          {186378.343750, -4740926.000000, 4248280.000000}};

	auto length = movetk::algo::polyline_length_m(std::begin(pts), std::end(pts));
	std::cout << "length_m of segment: " << length << '\n';
	REQUIRE(length == Approx(23.249));
}

template <typename Backend>
struct PolylineUtilTests : public test_helpers::BaseTestFixture<Backend> {
	using MovetkGeometryKernel = typename test_helpers::BaseTestFixture<Backend>::MovetkGeometryKernel;
};

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(PolylineUtilTests,
                                      "trajectory length with movetk points",
                                      "[trajectory_length_movetk_pts]") {
	test_helpers::GeometryConstructors<TestType> c;
	auto polyline = c.make_polyline(
	    {c.make_point({0, 0}), c.make_point({1, 0}), c.make_point({1, 1}), c.make_point({0, 1}), c.make_point({0, 0})});
	double length = movetk::algo::polyline_length_m(std::begin(polyline), std::end(polyline));
	REQUIRE(length == Approx(4.0).epsilon(0.00001));
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(PolylineUtilTests, "trajectory monotonicity", "[trajectory_monotonicity]") {
	test_helpers::GeometryConstructors<TestType> c;
	auto polyline = c.make_polyline({c.make_point({4, 2}),
	                                 c.make_point({14, 10}),
	                                 c.make_point({18.812, 4.0566}),
	                                 c.make_point({20, 12}),
	                                 c.make_point({28, 14}),
	                                 c.make_point({30, 25}),
	                                 c.make_point({31.9003, 8.43149})});
	std::vector<bool> result, ExpectedResult({true, true, false, true, true, false});
	movetk::algo::is_monotone<typename PolylineUtilTests<TestType>::MovetkGeometryKernel> is_monotone;
	is_monotone(std::begin(polyline), std::end(polyline), std::back_inserter(result));

	REQUIRE(std::accumulate(std::begin(result), std::end(result), 0) ==
	        std::accumulate(std::begin(ExpectedResult), std::end(ExpectedResult), 0));
}
