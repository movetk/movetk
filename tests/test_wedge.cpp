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

#include "helpers/TestJsonReader.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "helpers/CustomCatchTemplate.h"

template <typename Backend>
struct WedgeTests : public test_helpers::BaseTestFixture<Backend> {
	using Base = test_helpers::BaseTestFixture<Backend>;
	using MovetkGeometryKernel = typename test_helpers::BaseTestFixture<Backend>::MovetkGeometryKernel;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	using MovetkPoint = typename MovetkGeometryKernel::MovetkPoint;
	using NT = typename MovetkGeometryKernel::NT;
	using Wedge = movetk::geom::Wedge<MovetkGeometryKernel, typename Base::Norm>;

	struct WedgeArgs {
		MovetkPoint point;
		MovetkPoint center;
		NT radius;

		void read(const rapidjson::Value& object) {
			movetk::test::io::read_point<MovetkGeometryKernel>(object["point"], point);
			movetk::test::io::read_point<MovetkGeometryKernel>(object["center"], center);
			radius = NT(object["radius"].GetDouble());
		}
		auto create_wedge() const { return Wedge(point, center, radius); }
	};
	auto create_wedge(const WedgeArgs& args) { return Wedge(args.point, args.center, args.radius); }
	struct Expected {
		NT m1;
		NT c1;
		NT m2;
		NT c2;

		void read(const rapidjson::Value& object) {
			m1 = NT(object["m1"].GetDouble());
			m2 = NT(object["m2"].GetDouble());
			c1 = NT(object["c1"].GetDouble());
			c2 = NT(object["c2"].GetDouble());
		}
	};
	struct ConstructionTestCase {
		WedgeArgs input;
		Expected expected;
		void read(const rapidjson::Value& value) {
			input.read(value["input"]);
			expected.read(value["expected"]);
		}
	};
	static auto create_expected(NT m1, NT c1, NT m2, NT c2) { return Expected{m1, c1, m2, c2}; }
	auto create_construction_testcases() {
		std::map<std::string, ConstructionTestCase> test_cases;
		auto doc = movetk::test::io::get_json_doc("data/wedge_tests.json");

		const auto& tests_root = doc["construction_tests"];
		assert(tests_root.IsObject());
		for (auto it = tests_root.MemberBegin(); it != tests_root.MemberEnd(); ++it) {
			ConstructionTestCase test_case;
			test_case.read(it->value);
			std::string name = it->name.GetString();

			test_cases[name] = test_case;
		}
		return test_cases;
	}
	struct PointInsideTestCase {
		WedgeArgs input;
		std::vector<MovetkPoint> points;
		std::vector<bool> expected_inside;
		void read(const rapidjson::Value& value) {
			input.read(value["input"]);
			movetk::test::io::load(value["points"], points, movetk::test::io::construct_point<MovetkGeometryKernel>);
			movetk::test::io::load(value["expected_inside"], expected_inside);
		}
	};
	auto create_point_inside_test_cases() {
		std::map<std::string, PointInsideTestCase> test_cases;
		auto doc = movetk::test::io::get_json_doc("data/wedge_tests.json");

		const auto& tests_root = doc["inclusion_tests"];
		assert(tests_root.IsObject());
		for (auto it = tests_root.MemberBegin(); it != tests_root.MemberEnd(); ++it) {
			PointInsideTestCase test_case;
			test_case.read(it->value);
			std::string name = it->name.GetString();
			test_cases[name] = test_case;
		}
		return test_cases;
	}
	struct IntersectionTestCase {
		WedgeArgs wedge0_args;
		WedgeArgs wedge1_args;
		Expected expected_arguments;
		void read(const rapidjson::Value& value) {
			wedge0_args.read(value["wedge0_args"]);
			wedge1_args.read(value["wedge1_args"]);
			expected_arguments.read(value["expected_arguments"]);
		}
	};
	auto create_intersection_test_cases() {
		std::map<std::string, IntersectionTestCase> test_cases;
		auto doc = movetk::test::io::get_json_doc("data/wedge_tests.json");

		const auto& tests_root = doc["intersection_tests"];
		assert(tests_root.IsObject());
		for (auto it = tests_root.MemberBegin(); it != tests_root.MemberEnd(); ++it) {
			IntersectionTestCase test_case;
			test_case.read(it->value);
			std::string name = it->name.GetString();
			test_cases[name] = test_case;
		}
		return test_cases;
	}
};

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WedgeTests, "Check wedge constructions are valid", "[are_valid_wedges]") {
	using Fixture = WedgeTests<TestType>;
	auto make_point = Fixture::make_point;
	auto ORIGIN = make_point({0, 0});
	auto e1 = make_point({1, 0}) - ORIGIN;
	auto e2 = make_point({0, 1}) - ORIGIN;

	auto test_cases = Fixture::create_construction_testcases();
	for (const auto& [name, test_case] : test_cases) {
		SECTION(name) {
			auto p = make_point({-4, 3});
			auto center = make_point({3, 3});
			auto wedge = Fixture::create_wedge(test_case.input);
			auto slope = wedge.slope();
			auto intercept = wedge.intercept();
			const auto result_m1 = slope * e1;
			const auto result_m2 = slope * e2;
			const auto result_c1 = intercept * e1;
			const auto result_c2 = intercept * e2;
			REQUIRE(abs(result_m1 - test_case.expected.m1) < MOVETK_EPS);
			REQUIRE(abs(result_c1 - test_case.expected.c1) < MOVETK_EPS);
			REQUIRE(abs(result_m2 - test_case.expected.m2) < MOVETK_EPS);
			REQUIRE(abs(result_c2 - test_case.expected.c2) < MOVETK_EPS);
		}
	}
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WedgeTests, "Check point inclusion in wedge is correct", "[is_point_in_wedge]") {
	using Fixture = WedgeTests<TestType>;
	auto make_point = Fixture::make_point;
	auto ORIGIN = make_point({0, 0});
	auto e1 = make_point({1, 0}) - ORIGIN;
	auto e2 = make_point({0, 1}) - ORIGIN;
	const auto test_cases = Fixture::create_point_inside_test_cases();
	for (const auto& [name, test_case] : test_cases) {
		SECTION(name) {
			auto wedge = test_case.input.create_wedge();
			for (std::size_t i = 0; i < test_case.points.size(); ++i) {
				REQUIRE(wedge * test_case.points[i] == test_case.expected_inside[i]);
			}
		}
	}
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(WedgeTests, "Check wedge intersections are correct", "[wedge_intersections]") {
	using Fixture = WedgeTests<TestType>;
	auto make_point = Fixture::make_point;
	auto ORIGIN = make_point({0, 0});
	auto e1 = make_point({1, 0}) - ORIGIN;
	auto e2 = make_point({0, 1}) - ORIGIN;
	auto test_cases = Fixture::create_intersection_test_cases();
	for (const auto& [name, test_case] : test_cases) {
		SECTION(name) {
			auto wedge0 = test_case.wedge0_args.create_wedge();
			auto wedge1 = test_case.wedge1_args.create_wedge();
			const auto intersection = wedge0 * wedge1;
			const auto& intercept = intersection.intercept();
			const auto& slope = intersection.slope();
			const auto result_m1 = slope * e1;
			const auto result_m2 = slope * e2;
			const auto result_c1 = intercept * e1;
			const auto result_c2 = intercept * e2;
			REQUIRE(abs(result_m1 - test_case.expected_arguments.m1) < MOVETK_EPS);
			REQUIRE(abs(result_c1 - test_case.expected_arguments.c1) < MOVETK_EPS);
			REQUIRE(abs(result_m2 - test_case.expected_arguments.m2) < MOVETK_EPS);
			REQUIRE(abs(result_c2 - test_case.expected_arguments.c2) < MOVETK_EPS);
		}
	}
}
