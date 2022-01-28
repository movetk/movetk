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

#include "movetk/algo/Simplification.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "test_includes.h"

template <typename Backend>
struct DouglasPeuckerTests {
	using MovetkGeometryKernel = typename Backend::MovetkGeometryKernel;
	using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	using PolyLine = std::vector<typename MovetkGeometryKernel::MovetkPoint>;
	using FindFarthest = movetk::algo::FindFarthest<MovetkGeometryKernel, Norm>;
	using DouglasPeucker = movetk::algo::DouglasPeucker<MovetkGeometryKernel, FindFarthest>;
};


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(DouglasPeuckerTests,
                                      "Check Douglas-Peucker Simplification 1",
                                      "[douglas_peucker_simplification_1]") {
	PolyLine polyline({make_point({-6.19, -3.46}),
	                   make_point({-4.99, 1.16}),
	                   make_point({-2.79, -2.22}),
	                   make_point({-1.87, 0.58}),
	                   make_point({0.77, 0.22}),
	                   make_point({-1.15, 3.06}),
	                   make_point({5.33, -1.12})});
	PolyLine ExpectedPolyline;
	std::vector<PolyLine::iterator> result;
	DouglasPeucker DouglasPeucker1(5.47722);
	ExpectedPolyline = PolyLine({make_point({-6.19, -3.46}), make_point({5.33, -1.12})});
	DouglasPeucker1(std::begin(polyline), std::end(polyline), movetk::utils::movetk_back_insert_iterator(result));
	REQUIRE(result.size() == 2);
	auto eit = std::begin(ExpectedPolyline);
	for (auto reference : result) {
		auto v = *reference - *eit;
		REQUIRE((v * v) < MOVETK_EPS);
		eit++;
	}
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(DouglasPeuckerTests,
                                      "Check Douglas-Peucker Simplification 2",
                                      "[douglas_peucker_simplification_2]") {
	PolyLine polyline({make_point({-6.19, -3.46}),
	                   make_point({-4.99, 1.16}),
	                   make_point({-2.79, -2.22}),
	                   make_point({-1.87, 0.58}),
	                   make_point({0.77, 0.22}),
	                   make_point({-1.15, 3.06}),
	                   make_point({5.33, -1.12})});
	PolyLine ExpectedPolyline;

	std::vector<PolyLine::iterator> result;
	DouglasPeucker DouglasPeucker2(3.162277);
	ExpectedPolyline = PolyLine({make_point({-6.19, -3.46}),
	                             make_point({-4.99, 1.16}),
	                             make_point({0.77, 0.22}),
	                             make_point({-1.15, 3.06}),
	                             make_point({5.33, -1.12})});
	DouglasPeucker2(std::begin(polyline), std::end(polyline), movetk::utils::movetk_back_insert_iterator(result));
	REQUIRE(result.size() == 5);
	auto eit = std::begin(ExpectedPolyline);
	for (auto reference : result) {
		MovetkGeometryKernel::MovetkVector v = *reference - *eit;
		REQUIRE((v * v) < MOVETK_EPS);
		eit++;
	}
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(DouglasPeuckerTests,
                                      "Check Douglas-Peucker Simplification 3",
                                      "[douglas_peucker_simplification_3]") {
	PolyLine polyline({make_point({-6.19, -3.46}),
	                   make_point({-4.99, 1.16}),
	                   make_point({-2.79, -2.22}),
	                   make_point({-1.87, 0.58}),
	                   make_point({0.77, 0.22}),
	                   make_point({-1.15, 3.06}),
	                   make_point({5.33, -1.12})});
	PolyLine ExpectedPolyline;

	std::vector<PolyLine::iterator> result;
	DouglasPeucker DouglasPeucker3(1);
	ExpectedPolyline = PolyLine({make_point({-6.19, -3.46}),
	                             make_point({-4.99, 1.16}),
	                             make_point({-2.79, -2.22}),
	                             make_point({-1.87, 0.58}),
	                             make_point({0.77, 0.22}),
	                             make_point({-1.15, 3.06}),
	                             make_point({5.33, -1.12})});
	DouglasPeucker3(std::begin(polyline), std::end(polyline), movetk::utils::movetk_back_insert_iterator(result));
	REQUIRE(result.size() == 7);
	auto eit = std::begin(ExpectedPolyline);
	for (auto reference : result) {
		auto v = *reference - *eit;
		REQUIRE((v * v) < MOVETK_EPS);
		eit++;
	}
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(DouglasPeuckerTests,
                                      "Check Douglas-Peucker Simplification 4",
                                      "[douglas_peucker_simplification_4]") {
	PolyLine polyline({make_point({-6.19, -3.46}),
	                   make_point({-6.19, -3.46}),
	                   make_point({-6.19, -3.46}),
	                   make_point({-6.19, -3.46}),
	                   make_point({-6.19, -3.46}),
	                   make_point({-6.19, -3.46}),
	                   make_point({-6.19, -3.46})});
	PolyLine ExpectedPolyline;
	std::vector<PolyLine::iterator> result;
	DouglasPeucker DouglasPeucker4(3.162277);
	ExpectedPolyline = PolyLine({make_point({-6.19, -3.46}), make_point({-6.19, -3.46})});
	DouglasPeucker4(std::begin(polyline), std::end(polyline), movetk::utils::movetk_back_insert_iterator(result));
	REQUIRE(result.size() == 2);
	auto eit = std::begin(ExpectedPolyline);
	for (auto reference : result) {
		MovetkGeometryKernel::MovetkVector v = *reference - *eit;
		REQUIRE((v * v) < MOVETK_EPS);
		eit++;
	}
}