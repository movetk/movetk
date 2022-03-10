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

#include "catch2/catch.hpp"

// Defines the geometry kernel
#include <movetk/metric/Distances.h>
#include <movetk/metric/Norm.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "helpers/CustomCatchTemplate.h"

// Add a message to a Catch2 require
#define REQUIRE_MESSAGE(cond, msg) \
	do {                             \
		INFO(msg);                     \
		REQUIRE(cond);                 \
	} while ((void)0, 0)


struct StrongFrechetTestCase {
	// Two input lines, given as ipe paths, followed by the expected distance line.
	// Only the length of segment of expectedLine will be used.
	std::string polyA, polyB, expectedLine;
};

template <typename Backend>
struct StrongFrechetTests {
	using MovetkGeometryKernel = typename Backend::MovetkGeometryKernel;
	// The norm to be used in weak Frechet distance computations.
	using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;
	using SqDistance = movetk::metric::squared_distance_d<MovetkGeometryKernel, Norm>;
	using SFR = movetk::metric::StrongFrechet<MovetkGeometryKernel, SqDistance>;
	using MovetkPoint = typename MovetkGeometryKernel::MovetkPoint;
	using NT = typename MovetkGeometryKernel::NT;
	using PointList = std::vector<MovetkPoint>;
	static void parseIpePath(const std::string& pathData, std::vector<MovetkPoint>& points) {
		test_helpers::parse_ipe_path_contents<MovetkGeometryKernel>(pathData, points);
	}
	static constexpr const char* test_data_file = "data/strong_frechet_tests.xml";

	static std::map<std::string, StrongFrechetTestCase> load_test_cases() {
		std::map<std::string, StrongFrechetTestCase> test_cases;
		boost::property_tree::ptree tree;
		boost::property_tree::read_xml(test_data_file, tree);
		const auto& tests_tree = tree.get_child("tests");
		for (const auto& node : tests_tree) {
			const auto test_name = node.second.get<std::string>("name");
			StrongFrechetTestCase test_case;
			test_case.polyA = node.second.get<std::string>("input_a.ipeselection.path");
			test_case.polyB = node.second.get<std::string>("input_b.ipeselection.path");
			test_case.expectedLine = node.second.get<std::string>("expected_line.ipeselection.path");
			test_cases[test_name] = test_case;
		}
		return test_cases;
	}
};

const char* SFR_TAG = "[strong_frechet]";

TEMPLATE_LIST_TEST_CASE_METHOD(StrongFrechetTests,
                               "Check if polyline strong Frechet distance is correct",
                               SFR_TAG,
                               movetk::test::AvailableBackends) {
	using Fixture = StrongFrechetTests<TestType>;
	// Initialize algorithm.
	typename Fixture::SFR sfr;
	sfr.setMode(Fixture::SFR::Mode::DoubleAndSearch);
	sfr.setTolerance(0.0001);

	// Distance computer for expected distance
	typename Fixture::SqDistance sqDist;
	const auto testCases = Fixture::load_test_cases();
	for (const auto& pair : testCases) {
		StrongFrechetTestCase tc = pair.second;
		SECTION(pair.first) {
			// Read input
			typename Fixture::PointList polyA, polyB, expectedDistLine;
			Fixture::parseIpePath(tc.polyA, polyA);
			Fixture::parseIpePath(tc.polyB, polyB);
			// Expected distance element
			Fixture::parseIpePath(tc.expectedLine, expectedDistLine);
			// Compute expected distance
			auto expectedDist = std::sqrt(sqDist(expectedDistLine[0], expectedDistLine[1]));

			// Try strong frechet in both orders
			{
				auto dist = sfr(polyA.begin(), polyA.end(), polyB.begin(), polyB.end());
				REQUIRE(dist == Approx(expectedDist).margin(sfr.tolerance()));
			}
			{
				auto dist = sfr(polyB.begin(), polyB.end(), polyA.begin(), polyA.end());
				REQUIRE(dist == Approx(expectedDist).margin(sfr.tolerance()));
			}
		}
	}
}

TEMPLATE_LIST_TEST_CASE_METHOD(StrongFrechetTests,
                               "Check if polyline strong Frechet distance is correct with upperbounded search",
                               SFR_TAG,
                               movetk::test::AvailableBackends) {
	using Fixture = StrongFrechetTests<TestType>;
	// Initialize algorithm.
	typename Fixture::SFR sfr;
	sfr.setMode(Fixture::SFR::Mode::BisectionSearch);
	sfr.setTolerance(0.0001);

	// Distance computer for expected distance
	typename Fixture::SqDistance sqDist;

	// Test fractions of known distance
	std::vector<typename Fixture::NT> fractions = {0.1, 0.4, 0.55, 0.8, 1.0, 1.2, 5.0, 1000};
	std::vector<bool> expectSuccess = {false, false, false, false, true, true, true, true};

	const auto testCases = Fixture::load_test_cases();
	for (const auto& pair : testCases) {
		std::string testCaseName = pair.first;
		StrongFrechetTestCase tc = pair.second;
		SECTION(testCaseName) {
			// Read input
			typename Fixture::PointList polyA, polyB, expectedDistLine;
			Fixture::parseIpePath(tc.polyA, polyA);
			Fixture::parseIpePath(tc.polyB, polyB);
			// Expected distance element
			Fixture::parseIpePath(tc.expectedLine, expectedDistLine);
			// Compute expected distance
			auto expectedDist = std::sqrt(sqDist(expectedDistLine[0], expectedDistLine[1]));

			for (std::size_t i = 0; i < fractions.size(); ++i) {
				sfr.setUpperbound(expectedDist * fractions[i]);
				typename Fixture::NT epsilon = -1;
				// Try algorithm with both polyA and polyB as first polyline
				{
					bool success = sfr(polyA.begin(), polyA.end(), polyB.begin(), polyB.end(), epsilon);
					// Check whether we expect success or not.
					REQUIRE(success == expectSuccess[i]);
					if (success) {
						REQUIRE_MESSAGE(epsilon == Approx(expectedDist).margin(sfr.tolerance()),
						                (std::string("Failed at fraction ") + std::to_string(fractions[i])));
					}
				}
				{
					bool success = sfr(polyB.begin(), polyB.end(), polyA.begin(), polyA.end(), epsilon);
					REQUIRE(success == expectSuccess[i]);
					if (success) {
						REQUIRE(epsilon == Approx(expectedDist).margin(sfr.tolerance()));
					}
				}
			}
		}
	}
}

TEMPLATE_LIST_TEST_CASE_METHOD(StrongFrechetTests,
                               "Check if decision strong Frechet distance is correct",
                               "[strong_frechet]",
                               movetk::test::AvailableBackends) {
	using Fixture = StrongFrechetTests<TestType>;
	// Initialize algorithm.
	typename Fixture::SFR sfr;
	sfr.setTolerance(0.0001);

	// Distance computer for expected distance
	typename Fixture::SqDistance sqDist;

	// Test fractions of known distance
	std::vector<typename Fixture::NT> fractions = {0.1, 0.4, 0.55, 0.8, 1.0, 1.2, 5.0, 1000};
	std::vector<bool> expectSuccess = {false, false, false, false, true, true, true, true};

	const auto testCases = Fixture::load_test_cases();
	for (const auto& pair : testCases) {
		std::string testCaseName = pair.first;
		StrongFrechetTestCase tc = pair.second;
		SECTION(testCaseName) {
			// Read input
			typename Fixture::PointList polyA, polyB, expectedDistLine;
			Fixture::parseIpePath(tc.polyA, polyA);
			Fixture::parseIpePath(tc.polyB, polyB);
			// Expected distance element
			Fixture::parseIpePath(tc.expectedLine, expectedDistLine);
			// Compute expected distance
			auto expectedDist = std::sqrt(sqDist(expectedDistLine[0], expectedDistLine[1]));

			for (std::size_t i = 0; i < fractions.size(); ++i) {
				// Try algorithm with both polyA and polyB as first polyline
				{
					bool success =
					    sfr.decide(polyA.begin(), polyA.end(), polyB.begin(), polyB.end(), expectedDist * fractions[i]);
					// Check whether we expect success or not.
					REQUIRE(success == expectSuccess[i]);
				}
				{
					bool success =
					    sfr.decide(polyB.begin(), polyB.end(), polyA.begin(), polyA.end(), expectedDist * fractions[i]);
					REQUIRE(success == expectSuccess[i]);
				}
			}
		}
	}
}