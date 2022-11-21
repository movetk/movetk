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
 // Created by Mitra, Aniket on 2019-02-22.
 //

#include <array>
#include <catch2/catch.hpp>

#include "helpers/CustomCatchTemplate.h"
#include "helpers/TestJsonReader.h"
#include "movetk/metric/Norm.h"
#include "movetk/simplification/DouglasPeucker.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"

template <typename Backend>
struct DouglasPeuckerTests {
    using MovetkGeometryKernel = typename Backend::MovetkGeometryKernel;
    using NT = typename Backend::NT;
    using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;
    movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
    using PolyLine = std::vector<typename MovetkGeometryKernel::MovetkPoint>;
    using FindFarthest = movetk::simplification::FindFarthest<MovetkGeometryKernel, Norm>;
    using DouglasPeucker = movetk::simplification::DouglasPeucker<MovetkGeometryKernel, FindFarthest>;

    template <typename... ARGS>
    auto create_douglas_peucker(ARGS&&... args) {
        return DouglasPeucker(std::forward<ARGS>(args)...);
    }
    struct TestCase {
        PolyLine input;
        PolyLine expected;
        NT epsilon;
        void read(const rapidjson::Value& value) {
            using namespace movetk::test;
            io::load(value["input"], input, io::construct_point<MovetkGeometryKernel>);
            io::load(value["expected"], expected, io::construct_point<MovetkGeometryKernel>);
            long double epsilon_ld;
            io::load(value, epsilon_ld);
            epsilon = NT(epsilon_ld);
        }
    };
    auto create_test_cases() {
        std::map<std::string, TestCase> test_cases;
        auto doc = movetk::test::io::get_json_doc("data/douglaspeucker_tests.json");

        const auto& tests_root = doc["simplification_tests"];
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


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(DouglasPeuckerTests,
    "Check Douglas-Peucker Simplification are correct",
    "[douglas_peucker_simplification]") {
    using Fixture = DouglasPeuckerTests<TestType>;
    auto test_cases = Fixture::create_test_cases();
    for (const auto& [name, test_case] : test_cases) {
        auto douglas_peucker = Fixture::create_douglas_peucker(test_case.epsilon);
        std::vector<decltype(test_case.input.begin())> result;
        douglas_peucker(std::begin(test_case.input),
            std::end(test_case.input),
            std::back_inserter(result));
        REQUIRE(result.size() == test_case.expected.size());
        auto eit = std::begin(test_case.expected);
        for (auto reference : result) {
            auto v = *reference - *eit;
            REQUIRE((v * v) < MOVETK_EPS);
            eit++;
        }
    }
}
