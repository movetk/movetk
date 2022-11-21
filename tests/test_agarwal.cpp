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
#include <map>

// Defines the geometry kernel
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "helpers/CustomCatchTemplate.h"
#include "movetk/Simplification.h"
#include "movetk/metric/Distances.h"
#include "movetk/metric/Norm.h"

// Add a message to a Catch2 require
#define REQUIRE_MESSAGE(cond, msg) \
	do {                             \
		INFO(msg);                     \
		REQUIRE(cond);                 \
	} while ((void)0, 0)

struct AgarwalTestcase {
	std::string polyline_input;
	// Ipe input for single edge that will define the epsilon for the testcase
	std::string epsilon_line_input;
	std::vector<std::size_t> expectedInds;

	static constexpr const char* test_data_file = "data/agarwal_simplification_tests.xml";

	static std::map<std::string, AgarwalTestcase> load_test_cases() {
		std::map<std::string, AgarwalTestcase> test_cases;
		boost::property_tree::ptree tree;
		boost::property_tree::read_xml(test_data_file, tree);
		const auto& tests_tree = tree.get_child("tests");
		for (const auto& node : tests_tree) {
			const auto test_name = node.second.get<std::string>("name");
			AgarwalTestcase test_case;
			test_case.polyline_input = node.second.get<std::string>("input.ipeselection.path");
			test_case.epsilon_line_input = node.second.get<std::string>("expected_line.ipeselection.path");
			const auto expected_indices_string = node.second.get<std::string>("expected_indices");
			test_helpers::vector_from_string(expected_indices_string, test_case.expectedInds);
			test_cases[test_name] = test_case;
		}
		return test_cases;
	}
};

TEMPLATE_LIST_TEST_CASE("Check that the simplifications are correct",
                        "[agarwal_simplification][simplification]",
                        movetk::test::AvailableBackends) {
	using MovetkGeometryKernel = typename TestType::MovetkGeometryKernel;
	// The norm to be used in weak Frechet distance computations.
	using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;
	using NT = typename MovetkGeometryKernel::NT;
	using SFR = movetk::metric::StrongFrechet<MovetkGeometryKernel,
	                                          movetk::metric::squared_distance_d<MovetkGeometryKernel, Norm>>;
	using SqDistance = movetk::metric::squared_distance_d<MovetkGeometryKernel, Norm>;
	movetk::simplification::Agarwal<MovetkGeometryKernel, SqDistance> simplifier;
	simplifier.setTolerance(0.0001);
	using PointList = std::vector<typename MovetkGeometryKernel::MovetkPoint>;
	const auto parseIpe = [](const std::string& string_data, PointList& output) {
		test_helpers::parse_ipe_path_contents<MovetkGeometryKernel>(string_data, output);
	};
	const auto test_cases = AgarwalTestcase::load_test_cases();
	for (const auto& [test_case_name, test_case] : test_cases) {
		SECTION(test_case_name) {
			std::cout << test_case_name << '\n';
			PointList points, epsilonPath;

			parseIpe(test_case.polyline_input, points);
			parseIpe(test_case.epsilon_line_input, epsilonPath);

			// Requested epsilon
			const auto epsilon = std::sqrt(SqDistance()(epsilonPath[0], epsilonPath[1]));
			simplifier.setEpsilon(epsilon);

			std::vector<decltype(points.begin())> output;

			simplifier(points.begin(), points.end(), std::back_inserter(output));
			REQUIRE(output.size() == test_case.expectedInds.size());
			// Compute indices of iterators
			std::vector<std::size_t> inds;
			for (const auto& el : output) {
				inds.push_back(std::distance(points.begin(), el));
			}
			REQUIRE(inds == test_case.expectedInds);
		}
	}
	SECTION("Single point and empty polyline") {
		std::vector<typename MovetkGeometryKernel::MovetkPoint> points;
		points.push_back(movetk::geom::MakePoint<MovetkGeometryKernel>()({(NT)2.0, (NT)5000.}));

		simplifier.setEpsilon(1.0);

		std::vector<decltype(points.begin())> output;

		simplifier(points.begin(), points.end(), std::back_inserter(output));

		REQUIRE(output.size() == 1);
		REQUIRE(output[0] == points.begin());

		// Now with empty
		output.clear();
		points.clear();

		simplifier(points.begin(), points.end(), std::back_inserter(output));
		REQUIRE(output.size() == 0);
	}
}