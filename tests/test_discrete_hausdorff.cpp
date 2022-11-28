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
// Created by Mitra, Aniket on 2019-06-06.
//

#include <array>
#include <catch2/catch.hpp>

#include "helpers/TestJsonReader.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/DistanceInterface.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "helpers/CustomCatchTemplate.h"


template <typename Backend>
struct DiscreteHausdorffTests {
	using NT = test_helpers::NTFromBackend<Backend>;
	using MovetkGeometryKernel = test_helpers::MovetkKernelFromBackend<Backend>;
	using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;
	using PolyLine = std::vector<typename MovetkGeometryKernel::MovetkPoint>;

	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;

	PolyLine make_polyline(std::initializer_list<typename MovetkGeometryKernel::MovetkPoint> points) {
		return PolyLine(points);
	}

	movetk::metric::ComputeDiscreteHausdorffDistance<MovetkGeometryKernel, Norm> discrete_hausdorff;

	struct TestCase {
		PolyLine input0;
		PolyLine input1;
		NT expected_distance;

		void read(const rapidjson::Value& value) {
			using namespace movetk::test;
			io::load(value["input0"], input0, io::construct_point<MovetkGeometryKernel>);
			io::load(value["input1"], input1, io::construct_point<MovetkGeometryKernel>);
			long double expected_distance_ld;
			io::load(value["expected_distance"], expected_distance_ld);
			expected_distance = NT(expected_distance_ld);
		}
	};

	auto create_testcases() {
		std::map<std::string, TestCase> test_cases;
		auto doc = movetk::test::io::get_json_doc("data/discretehausdorff_tests.json");

		const auto& tests_root = doc["distance_tests"];
		assert(tests_root.IsObject());
		for (auto it = tests_root.MemberBegin(); it != tests_root.MemberEnd(); ++it) {
			TestCase test_case;
			test_case.read(it->value);
			std::string name = it->name.GetString();

			test_cases[name] = test_case;
		}
		return test_cases;
	}
};


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(DiscreteHausdorffTests,
                                      "Check Discrete Hausdorff distances",
                                      "[discrete_hausdorff]") {
	using Fixture = DiscreteHausdorffTests<TestType>;
	auto test_cases = Fixture::create_testcases();

	for (const auto& [name, test_case] : test_cases) {
		auto distance1 = Fixture::discrete_hausdorff(std::begin(test_case.input0),
		                                             std::end(test_case.input0),
		                                             std::begin(test_case.input1),
		                                             std::end(test_case.input1));
		auto distance2 = Fixture::discrete_hausdorff(std::begin(test_case.input1),
		                                             std::end(test_case.input1),
		                                             std::begin(test_case.input0),
		                                             std::end(test_case.input0));
		const auto distance = std::max(distance1, distance2);
		REQUIRE(distance == test_case.expected_distance);
	}
}
