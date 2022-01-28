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
// Created by Mitra, Aniket on 09/01/2019.
//

#include <array>
#include <catch2/catch.hpp>

#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "test_includes.h"

template <typename Backend>
struct TrajectoryUtilsTests : public test_helpers::BaseTestFixture<Backend> {
	using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;
	using PointList = std::vector<typename MovetkGeometryKernel::MovetkPoint>;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
};


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(TrajectoryUtilsTests, "Check if values are in sequence", "[test_is_sequence]") {
	std::vector<size_t> values1{1, 2, 3, 5, 7};
	std::vector<size_t> values2{1, 2, 3, 4, 5, 6, 7};
	REQUIRE(movetk::utils::is_sequence(cbegin(values1), cend(values1)) == false);

	REQUIRE(movetk::utils::is_sequence(cbegin(values2), cend(values2)) == true);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(TrajectoryUtilsTests,
                                      "Find minimum non zero element",
                                      "[test_min_non_zero_element]") {
	std::vector<NT> values{3.1, 2.2, 0, 1};
	REQUIRE(*movetk::utils::min_non_zero_element<MovetkGeometryKernel>(cbegin(values), cend(values)) == 1);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(TrajectoryUtilsTests, "Calculate tdiff", "[test_get_time_diffs]") {
	std::vector<size_t> ts{1460029938, 1460029945, 1460029954};
	std::vector<size_t> result;
	movetk::utils::get_time_diffs(cbegin(ts), cend(ts), movetk::utils::movetk_back_insert_iterator(result));
	REQUIRE(result[0] == 0);
	REQUIRE(result[1] == 7);
	REQUIRE(result[2] == 9);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(TrajectoryUtilsTests,
                                      "Calculate Euclidean distance",
                                      "[test_calculate_distance]") {
	PointList points;
	std::vector<NT> result;
	points.push_back(make_point({-8226858, 4993143}));
	points.push_back(make_point({-8226854, 4993146}));
	points.push_back(make_point({-8226855, 4993146}));
	movetk::utils::get_distances<MovetkGeometryKernel>(cbegin(points),
	                                                   cend(points),
	                                                   movetk::utils::movetk_back_insert_iterator(result));
	REQUIRE(result[0] == 0);
	REQUIRE(result[1] == 5);
	REQUIRE(result[2] == 1);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(TrajectoryUtilsTests, "Calculate Speed", "[test_get_speeds]") {
	std::vector<NT> distance{0, 5, 1};
	std::vector<size_t> tdiff{0, 7, 9};
	std::vector<NT> result;
	movetk::utils::get_speeds<MovetkGeometryKernel>(cbegin(tdiff),
	                                                cend(tdiff),
	                                                cbegin(distance),
	                                                movetk::utils::movetk_back_insert_iterator(result));
	REQUIRE(result[0] == 0);
	REQUIRE(abs(result[1] - 0.714286) < MOVETK_EPS);
	REQUIRE(abs(result[2] - 0.111111) < MOVETK_EPS);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(TrajectoryUtilsTests, "Calculate Heading", "[test_get_headings]") {
	std::vector<std::pair<NT, NT>> GeoCoords{std::make_pair(40.75348, -73.90441),
	                                         std::make_pair(40.75348, -73.90439),
	                                         std::make_pair(40.75352, -73.90439)};
	PointList projections;
	std::vector<NT> result;
	projections.push_back(make_point({-8227001.290000, 4976047.740000}));
	projections.push_back(make_point({-8226999.060000, 4976047.740000}));
	projections.push_back(make_point({-8226974.570000, 4976053.620000}));
	movetk::utils::get_headings<MovetkGeometryKernel>(cbegin(GeoCoords),
	                                                  cend(GeoCoords),
	                                                  cbegin(projections),
	                                                  movetk::utils::movetk_back_insert_iterator(result));
	REQUIRE(result[0] == 0);
	REQUIRE(abs(result[1] - 90) < MOVETK_EPS);
	REQUIRE(result[2] == 0);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(TrajectoryUtilsTests, "Calculate Velocity", "[test_get_velocities]") {
	std::vector<NT> speed{1, 1, 4};
	std::vector<NT> heading{46, 12, 297};
	PointList expected_velocities, actual_velocties;
	expected_velocities.push_back(make_point({0.694658, 0.719340}));
	expected_velocities.push_back(make_point({0.978148, 0.207912}));
	expected_velocities.push_back(make_point({1.815962, -3.564026}));

	movetk::utils::get_velocities<MovetkGeometryKernel>(cbegin(speed),
	                                                    cend(speed),
	                                                    cbegin(heading),
	                                                    movetk::utils::movetk_back_insert_iterator(actual_velocties));
	MovetkGeometryKernel::MovetkVector vector = actual_velocties[0] - expected_velocities[0];
	REQUIRE((vector * vector) < MOVETK_EPS);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(TrajectoryUtilsTests, "Merge Intervals 1", "[test_merge_intervals_1]") {
	std::vector<std::pair<NT, NT>> items{{1, 3}, {6, 8}, {5, 7}, {2, 4}};
	std::vector<std::pair<NT, NT>> expected{{5, 8}, {1, 4}};
	auto beyond = movetk::utils::merge_intervals<MovetkGeometryKernel>(std::begin(items), std::end(items));
	auto it = std::begin(items);
	REQUIRE(std::distance(it, beyond) == expected.size());
	auto eit = std::begin(expected);
	while (it != beyond) {
		REQUIRE(abs(eit->first - it->first) < MOVETK_EPS);
		REQUIRE(abs(eit->second - it->second) < MOVETK_EPS);
		eit++;
		it++;
	}
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(TrajectoryUtilsTests, "Merge Intervals 2", "[test_merge_intervals_2]") {
	std::vector<std::pair<NT, NT>> items{{1, 3}, {1, 8}, {1, 8}, {1, 4}};
	std::vector<std::pair<NT, NT>> expected{{1, 8}};
	auto beyond = movetk::utils::merge_intervals<MovetkGeometryKernel>(std::begin(items), std::end(items));
	auto it = std::begin(items);
	REQUIRE(std::distance(it, beyond) == expected.size());
	auto eit = std::begin(expected);
	while (it != beyond) {
		REQUIRE(abs(eit->first - it->first) < MOVETK_EPS);
		REQUIRE(abs(eit->second - it->second) < MOVETK_EPS);
		eit++;
		it++;
	}
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(TrajectoryUtilsTests, "Merge Intervals 3", "[test_merge_intervals_3]") {
	std::vector<std::pair<NT, NT>> items{{1, 2}, {3, 4}, {5, 6}, {7, 8}};
	std::vector<std::pair<NT, NT>> expected{{7, 8}, {5, 6}, {3, 4}, {1, 2}};
	auto beyond = movetk::utils::merge_intervals<MovetkGeometryKernel>(std::begin(items), std::end(items));
	auto it = std::begin(items);
	REQUIRE(std::distance(it, beyond) == expected.size());
	std::cout << "Merged Interval:\n";
	auto eit = std::begin(expected);
	while (it != beyond) {
		std::cout << it->first << "," << it->second << "\n";
		REQUIRE(abs(eit->first - it->first) < MOVETK_EPS);
		REQUIRE(abs(eit->second - it->second) < MOVETK_EPS);
		eit++;
		it++;
	}
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(TrajectoryUtilsTests, "Merge Intervals 4", "[test_merge_intervals_4]") {
	std::vector<std::pair<NT, NT>> items{{1, 4}, {2, 4}, {1, 2}, {7, 8}};
	std::vector<std::pair<NT, NT>> expected{{7, 8}, {1, 4}};
	auto beyond = movetk::utils::merge_intervals<MovetkGeometryKernel>(std::begin(items), std::end(items));
	auto it = std::begin(items);
	REQUIRE(std::distance(it, beyond) == expected.size());
	std::cout << "Merged Interval:\n";
	auto eit = std::begin(expected);
	while (it != beyond) {
		std::cout << it->first << "," << it->second << "\n";
		REQUIRE(abs(eit->first - it->first) < MOVETK_EPS);
		REQUIRE(abs(eit->second - it->second) < MOVETK_EPS);
		eit++;
		it++;
	}
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(TrajectoryUtilsTests, "Merge Intervals 5", "[test_merge_intervals_5]") {
	std::vector<std::pair<NT, NT>> items{{1, 7}, {2, 4}, {1, 2}, {7, 8}};
	std::vector<std::pair<NT, NT>> expected{{7, 8}, {1, 7}};
	auto beyond = movetk::utils::merge_intervals<MovetkGeometryKernel>(std::begin(items), std::end(items));
	auto it = std::begin(items);
	REQUIRE(std::distance(it, beyond) == expected.size());
	std::cout << "Merged Interval:\n";
	auto eit = std::begin(expected);
	while (it != beyond) {
		std::cout << it->first << "," << it->second << "\n";
		REQUIRE(abs(eit->first - it->first) < MOVETK_EPS);
		REQUIRE(abs(eit->second - it->second) < MOVETK_EPS);
		eit++;
		it++;
	}
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(TrajectoryUtilsTests, "Compute Curve Length", "[compute_curve_length]") {
	PointList polyline{make_point({2, 2}),
	                   make_point({4, 6}),
	                   make_point({8, 6}),
	                   make_point({6, 4}),
	                   make_point({8, 4}),
	                   make_point({7.16, 2.84}),
	                   make_point({4.93, 2.69}),
	                   make_point({4.64, 4.65}),
	                   make_point({6.05, 6.37}),
	                   make_point({5.1, 7.89}),
	                   make_point({1.67, 7.51}),
	                   make_point({1.93, 4.63})};
	std::vector<NT> result;
	movetk::utils::compute_curve_squared_length<MovetkGeometryKernel, Norm>(
	    std::begin(polyline),
	    std::end(polyline),
	    movetk::utils::movetk_back_insert_iterator(result));
	// TODO: checks
}