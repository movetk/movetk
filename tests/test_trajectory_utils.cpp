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
#include "helpers/CustomCatchTemplate.h"

template <typename Backend>
struct TrajectoryUtilsTests : public test_helpers::GeometryConstructors<Backend> {
	using NT = test_helpers::NTFromBackend<Backend>;
	using MovetkGeometryKernel = test_helpers::MovetkKernelFromBackend<Backend>;
	using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;
	using PointList = std::vector<typename MovetkGeometryKernel::MovetkPoint>;
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
	using Fixture = TrajectoryUtilsTests<TestType>;
	std::vector<typename Fixture::NT> values{3.1, 2.2, 0, 1};
	REQUIRE(*movetk::utils::min_non_zero_element<typename Fixture::MovetkGeometryKernel>(cbegin(values), cend(values)) ==
	        1);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(TrajectoryUtilsTests, "Calculate tdiff", "[test_get_time_diffs]") {
	std::vector<size_t> ts{1460029938, 1460029945, 1460029954};
	std::vector<size_t> result;
	movetk::utils::get_time_diffs(cbegin(ts), cend(ts), std::back_inserter(result));
	REQUIRE(result[0] == 0);
	REQUIRE(result[1] == 7);
	REQUIRE(result[2] == 9);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(TrajectoryUtilsTests,
                                      "Calculate Euclidean distance",
                                      "[test_calculate_distance]") {
	using FixtureType = TrajectoryUtilsTests<TestType>;
	typename FixtureType::PointList points{FixtureType::make_point({-8226858, 4993143}),
	                                       FixtureType::make_point({-8226854, 4993146}),
	                                       FixtureType::make_point({-8226855, 4993146})};
	std::vector<typename FixtureType::NT> result;
	movetk::utils::get_distances<typename FixtureType::MovetkGeometryKernel>(
	    cbegin(points),
	    cend(points),
	    std::back_inserter(result));
	REQUIRE(result[0] == 0);
	REQUIRE(result[1] == 5);
	REQUIRE(result[2] == 1);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(TrajectoryUtilsTests, "Calculate Speed", "[test_get_speeds]") {
	using FixtureType = TrajectoryUtilsTests<TestType>;
	std::vector<typename FixtureType::NT> distance{0, 5, 1};
	std::vector<size_t> tdiff{0, 7, 9};
	std::vector<typename FixtureType::NT> result;
	movetk::utils::get_speeds<typename FixtureType::MovetkGeometryKernel>(
	    cbegin(tdiff),
	    cend(tdiff),
	    cbegin(distance),
	    std::back_inserter(result));
	REQUIRE(result[0] == 0);
	REQUIRE(abs(result[1] - 0.714286) < MOVETK_EPS);
	REQUIRE(abs(result[2] - 0.111111) < MOVETK_EPS);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(TrajectoryUtilsTests, "Calculate Heading", "[test_get_headings]") {
	using FixtureType = TrajectoryUtilsTests<TestType>;
	std::vector<std::pair<typename FixtureType::NT, typename FixtureType::NT>> GeoCoords{
	    std::make_pair(40.75348, -73.90441),
	    std::make_pair(40.75348, -73.90439),
	    std::make_pair(40.75352, -73.90439)};
	typename FixtureType::PointList projections;
	std::vector<typename FixtureType::NT> result;
	projections.push_back(FixtureType::make_point({-8227001.290000, 4976047.740000}));
	projections.push_back(FixtureType::make_point({-8226999.060000, 4976047.740000}));
	projections.push_back(FixtureType::make_point({-8226974.570000, 4976053.620000}));
	movetk::utils::get_headings<typename FixtureType::MovetkGeometryKernel>(
	    cbegin(GeoCoords),
	    cend(GeoCoords),
	    cbegin(projections),
	    std::back_inserter(result));
	REQUIRE(result[0] == 0);
	REQUIRE(abs(result[1] - 90) < MOVETK_EPS);
	REQUIRE(result[2] == 0);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(TrajectoryUtilsTests, "Calculate Velocity", "[test_get_velocities]") {
	using FixtureType = TrajectoryUtilsTests<TestType>;
	std::vector<typename FixtureType::NT> speed{1, 1, 4};
	std::vector<typename FixtureType::NT> heading{46, 12, 297};
	typename FixtureType::PointList expected_velocities{FixtureType::make_point({0.694658, 0.719340}),
	                                                    FixtureType::make_point({0.978148, 0.207912}),
	                                                    FixtureType::make_point({1.815962, -3.564026})};

	typename FixtureType::PointList actual_velocties;
	movetk::utils::get_velocities<typename FixtureType::MovetkGeometryKernel>(
	    cbegin(speed),
	    cend(speed),
	    cbegin(heading),
	    std::back_inserter(actual_velocties));
	const auto vector = actual_velocties[0] - expected_velocities[0];
	REQUIRE((vector * vector) < MOVETK_EPS);
}

template <typename Backend>
struct IntervalTests {
	using NT = typename Backend::NT;
	using Interval = std::pair<NT, NT>;
	using IntervalList = std::vector<Interval>;
	using MovetkGeometryKernel = typename Backend::MovetkGeometryKernel;

	struct TestCase {
		IntervalList input;
		IntervalList expected;
	};

	auto get_testcases() {
		std::map<std::string, TestCase> test_cases;
		test_cases["test1"] = TestCase{IntervalList{{1, 3}, {6, 8}, {5, 7}, {2, 4}}, {{5, 8}, {1, 4}}};
		test_cases["test2"] = TestCase{{{1, 3}, {1, 8}, {1, 8}, {1, 4}}, {{1, 8}}};
		test_cases["test3"] = TestCase{{{1, 2}, {3, 4}, {5, 6}, {7, 8}}, {{7, 8}, {5, 6}, {3, 4}, {1, 2}}};
		test_cases["test4"] = TestCase{{{1, 4}, {2, 4}, {1, 2}, {7, 8}}, {{7, 8}, {1, 4}}};
		test_cases["test5"] = TestCase{{{1, 7}, {2, 4}, {1, 2}, {7, 8}}, {{7, 8}, {1, 7}}};
		return test_cases;
	}
};

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(IntervalTests, "Verify merge_intervals correctness", "[test_merge_intervals]") {
	using Fixture = IntervalTests<TestType>;
	auto test_cases = Fixture::get_testcases();

	for (auto& [name, test_case] : test_cases) {
		SECTION(name) {
			auto beyond = movetk::utils::merge_intervals<typename Fixture::MovetkGeometryKernel>(std::begin(test_case.input),
			                                                                                     std::end(test_case.input));
			auto it = std::begin(test_case.input);
			REQUIRE(std::distance(it, beyond) == test_case.expected.size());
			auto eit = std::begin(test_case.expected);
			while (it != beyond) {
				REQUIRE(abs(eit->first - it->first) < MOVETK_EPS);
				REQUIRE(abs(eit->second - it->second) < MOVETK_EPS);
				eit++;
				it++;
			}
		}
	}
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(TrajectoryUtilsTests, "Compute Curve Length", "[compute_curve_length]") {
	using FixtureType = TrajectoryUtilsTests<TestType>;
	using NT = typename FixtureType::NT;
	using PointList = typename FixtureType::PointList;
	PointList polyline{FixtureType::make_point({2, 2}),
	                   FixtureType::make_point({4, 6}),
	                   FixtureType::make_point({8, 6}),
	                   FixtureType::make_point({6, 4}),
	                   FixtureType::make_point({8, 4}),
	                   FixtureType::make_point({7.16, 2.84}),
	                   FixtureType::make_point({4.93, 2.69}),
	                   FixtureType::make_point({4.64, 4.65}),
	                   FixtureType::make_point({6.05, 6.37}),
	                   FixtureType::make_point({5.1, 7.89}),
	                   FixtureType::make_point({1.67, 7.51}),
	                   FixtureType::make_point({1.93, 4.63})};
	std::vector<NT> result;
	movetk::utils::compute_curve_squared_length<typename FixtureType::MovetkGeometryKernel, typename FixtureType::Norm>(
	    std::begin(polyline),
	    std::end(polyline),
	    std::back_inserter(result));
	// TODO: checks
}