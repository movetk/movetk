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
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <catch2/catch.hpp>

#include "helpers/CustomCatchTemplate.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/Distances.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"

// Helpers for testing
#include "helpers/CustomCatchTemplate.h"

using namespace std;


template <typename MovetkGeometryKernel>
struct WeakFrechetTestCase {
	// Two input lines, given as ipe paths, followed by the expected distance line.
	// Only the length of segment of expectedLine will be used.
	using MovetkPoint = typename MovetkGeometryKernel::MovetkPoint;
	using PointList = std::vector<MovetkPoint>;
	using NT = typename MovetkGeometryKernel::NT;
	PointList polyline_a, polyline_b;
	NT expected_distance;
};

template <typename Backend>
struct WeakFrechetTests {
	using MovetkGeometryKernel = typename Backend::MovetkGeometryKernel;
	// The norm to be used in weak Frechet distance computations.
	using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;
	using SqDistance = movetk::metric::squared_distance_d<MovetkGeometryKernel, Norm>;
	using WFR = movetk::metric::WeakFrechet<MovetkGeometryKernel, SqDistance>;
	using MovetkPoint = typename MovetkGeometryKernel::MovetkPoint;
	using NT = typename MovetkGeometryKernel::NT;
	using PointList = std::vector<MovetkPoint>;
	static void parseIpePath(const std::string& pathData, std::vector<MovetkPoint>& points) {
		test_helpers::parse_ipe_path_contents<MovetkGeometryKernel>(pathData, points);
	}
	static void parse_ipe_path_from_node_contents(const boost::property_tree::ptree& node,
	                                              const std::string& path,
	                                              std::vector<MovetkPoint>& points) {
		const auto data = node.get<std::string>(path);
		parseIpePath(data, points);
	}

	static constexpr const char* test_data_file = "data/weak_frechet_tests.xml";
	static std::map<std::string, WeakFrechetTestCase<MovetkGeometryKernel>> load_test_cases() {
		SqDistance sqDist;
		std::map<std::string, WeakFrechetTestCase<MovetkGeometryKernel>> test_cases;
		boost::property_tree::ptree tree;
		boost::property_tree::read_xml(test_data_file, tree);
		const auto& tests_tree = tree.get_child("tests");
		for (const auto& node : tests_tree) {
			const auto test_name = node.second.get<std::string>("name");
			WeakFrechetTestCase<MovetkGeometryKernel> test_case;
			parse_ipe_path_from_node_contents(node.second, "input_a.ipeselection.path", test_case.polyline_a);
			parse_ipe_path_from_node_contents(node.second, "input_b.ipeselection.path", test_case.polyline_b);
			std::vector<MovetkPoint> expected_dist_points;
			parse_ipe_path_from_node_contents(node.second, "expected_line.ipeselection.path", expected_dist_points);
			test_case.expected_distance = std::sqrt(sqDist(expected_dist_points[0], expected_dist_points[1]));
		}
		return test_cases;
	}
};

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WeakFrechetTests,
                                      "Check Weak frechet distance between polylines",
                                      "[weak_frechet]") {
	using Fixture = WeakFrechetTests<TestType>;
	typename Fixture::WFR wfr{};
	const auto test_cases = Fixture::load_test_cases();
	for (const auto& [test_case_name, test_data] : test_cases) {
		SECTION(test_case_name) {
			auto dist = wfr(test_data.polyline_a.begin(),
			                test_data.polyline_a.end(),
			                test_data.polyline_b.begin(),
			                test_data.polyline_b.end());
			REQUIRE(abs(dist - test_data.expected_distance) < MOVETK_EPS);
		}
	}
}
MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WeakFrechetTests,
                                      "Check Weak frechet matching between polylines",
                                      "[weak_frechet]") {
	using Fixture = WeakFrechetTests<TestType>;
	typename Fixture::WFR wfr{};
	typename Fixture::SqDistance sqDist;

	const auto test_cases = Fixture::load_test_cases();

	SECTION("Interweaved grid example") {
		const auto& test_data = test_cases.at("Interweaved grid example");
		// Get the matching with the output
		std::vector<std::pair<std::pair<int, int>, typename Fixture::NT>> matching;

		const auto dist = wfr(test_data.polyline_a.begin(),
		                      test_data.polyline_a.end(),
		                      test_data.polyline_b.begin(),
		                      test_data.polyline_b.end(),
		                      std::back_inserter(matching));
		const auto expectedDist = test_data.expected_distance;
		// Non-empty matching
		REQUIRE(!matching.empty());

		// All distances should be correct
		auto maxDist = std::numeric_limits<typename Fixture::NT>::min();
		for (const auto& el : matching) {
			REQUIRE(el.second <= expectedDist + MOVETK_EPS);
			maxDist = std::max(maxDist, el.second);
		}
		// Maximum distance should be approximately the expected distance
		REQUIRE(std::abs(maxDist - expectedDist) < MOVETK_EPS);

		// We can not really reason about the matching since there are multiple ways to go about this.
		// But we atleast check that there is some place where we temporarily move backwards
		int backwardsMatchMoveCount = 0;
		for (int i = 2; i < matching.size(); ++i) {
			auto currMatch = matching[i];
			auto prevMatch = matching[i - 1];
			if (currMatch.first.first < prevMatch.first.first || currMatch.first.second < prevMatch.first.second)
				++backwardsMatchMoveCount;
		}
		REQUIRE(backwardsMatchMoveCount > 0);
		// The resulting weak Frechet distance should be correct.
		REQUIRE(abs(dist - expectedDist) < MOVETK_EPS);
	}
}