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
// Created by Mitra, Aniket on 2019-02-20.
//

#include <array>
#include <catch2/catch.hpp>

#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/DistanceInterface.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "helpers/CustomCatchTemplate.h"

template <typename Backend>
struct DistanceTests {
	using MovetkGeometryKernel = typename Backend::MovetkGeometryKernel;
	movetk::geom::MakePoint<typename Backend::MovetkGeometryKernel> make_point;
	using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;
};

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(DistanceTests,
                                      "Check Distance between point and segment",
                                      "[is_valid_dist_point_seg]") {
	using Test = DistanceTests<TestType>;
	auto p1 = Test::make_point({5.5, 3.1});
	auto p2 = Test::make_point({3.22, 1.3});
	auto p3 = Test::make_point({5, 5});
	movetk::geom::MakeSegment<typename Test::MovetkGeometryKernel> make_segment;
	auto seg = make_segment(p1, p2);
	movetk::metric::ComputeSquaredDistance<typename Test::MovetkGeometryKernel, typename Test::Norm> squared_dist;
	auto result = squared_dist(p3, seg);
	REQUIRE(abs(result - 3.86) < MOVETK_EPS);

	p3 = Test::make_point({3.85, 4.12});
	result = squared_dist(p3, seg);
	REQUIRE(abs(result - 3.3233) < MOVETK_EPS);

	p3 = Test::make_point({1.53, 2.6});
	result = squared_dist(p3, seg);
	REQUIRE(abs(result - 4.5461) < MOVETK_EPS);
}
