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
#include <catch2/catch.hpp>

#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/Distances.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "test_includes.h"

// Helpers for testing
#include "test_includes.h"

using namespace std;


struct WeakFrechetTestCase {
	// Two input lines, given as ipe paths, followed by the expected distance line.
	// Only the length of segment of expectedLine will be used.
	std::string polyA, polyB, expectedDistLine;
};

std::map<std::string, WeakFrechetTestCase> test_cases{{"Simple spike example",
                                                       WeakFrechetTestCase{
                                                           R"IPE(
        <path>
        96 448 m
        192 448 l
        256 448 l
        320 448 l
        384 448 l
        </path>
        )IPE",
                                                           R"IPE(
        <path>
        96 448 m
        128 448 l
        144 448 l
        192 448 l
        208 512 l
        224 448 l
        256 448 l
        304 448 l
        384 448 l
        </path>
        )IPE",
                                                           R"IPE(
        <path>
        208 448 m
        208 512 l
        </path>
        )IPE"}},
                                                      {"Interweaved grid example",
                                                       WeakFrechetTestCase{
                                                           R"IPE(
        <ipeselection pos="160 384">
        <path stroke="darkred" pen="heavier" arrow="normal/normal">
        128 384 m
        320 384 l
        320 352 l
        128 352 l
        128 320 l
        320 320 l
        320 288 l
        128 288 l
        128 256 l
        320 256 l
        320 224 l
        128 224 l
        128 192 l
        320 192 l
        </path>
        </ipeselection>
        )IPE",
                                                           R"IPE(
        <ipeselection>
        <path>
        128 384 m
        128 192 l
        160 192 l
        160 384 l
        192 384 l
        192 192 l
        224 192 l
        224 384 l
        256 384 l
        256 192 l
        288 192 l
        288 384 l
        320 384 l
        320 192 l
        </path>
        </ipeselection>
        )IPE",
                                                           R"IPE(
        <ipeselection>
        <path>
        352 384 m
        352 192 l
        </path>
        </ipeselection>
        )IPE"}}};

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
		test_helpers::parseIpePath<MovetkGeometryKernel>(pathData, points);
	}
};

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WeakFrechetTests,
                                      "Check Weak frechet distance between polylines",
                                      "[weak_frechet]") {
	WFR wfr{};
	SqDistance sqDist;
	for (const auto& [test_case_name, test_data] : test_cases) {
		SECTION(test_case_name) {
			PointList polyA, polyB, expectedDistLine;
			parseIpePath(test_data.polyA, polyA);
			parseIpePath(test_data.polyB, polyB);
			parseIpePath(test_data.expectedDistLine, expectedDistLine);
			auto dist = wfr(polyA.begin(), polyA.end(), polyB.begin(), polyB.end());
			auto expectedDist = std::sqrt(sqDist(expectedDistLine[0], expectedDistLine[1]));
			REQUIRE(abs(dist - expectedDist) < MOVETK_EPS);
		}
	}
}
MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WeakFrechetTests,
                                      "Check Weak frechet matching between polylines",
                                      "[weak_frechet]") {
	WFR wfr{};
	SqDistance sqDist;

	SECTION("Interweaved grid example") {
		const auto& test_data = test_cases.at("Interweaved grid example");
		PointList polyA, polyB, expectedDistLine;
		parseIpePath(test_data.polyA, polyA);
		parseIpePath(test_data.polyB, polyB);
		parseIpePath(test_data.expectedDistLine, expectedDistLine);

		auto expectedDist = std::sqrt(sqDist(expectedDistLine[0], expectedDistLine[1]));

		// Get the matching with the output
		std::vector<std::pair<std::pair<int, int>, NT>> matching;

		auto dist = wfr(polyA.begin(),
		                polyA.end(),
		                polyB.begin(),
		                polyB.end(),
		                movetk::utils::movetk_back_insert_iterator(matching));

		// Non-empty matching
		REQUIRE(!matching.empty());

		// All distances should be correct
		NT maxDist = std::numeric_limits<NT>::min();
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