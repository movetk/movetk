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
// Created by Mitra, Aniket on 28/10/2019.
//

#include <array>

#include "catch2/catch.hpp"
#include "helpers/CustomCatchTemplate.h"
#include "movetk/geo/geo.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "test_includes.h"
template <typename Backend>
struct MinimumBoundingRectTests {
	using MovetkGeometryKernel = typename Backend::MovetkGeometryKernel;
	using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;
	using NT = typename MovetkGeometryKernel::NT;
	using Point = typename MovetkGeometryKernel::MovetkPoint;
	Norm norm;
	using MinimumBoundingRectangle_ = typename movetk::geom::MBR<MovetkGeometryKernel, Norm>;
};
template <typename Fixture>
using Norm = typename Fixture::Norm;
template <typename Fixture>
using Point = typename Fixture::Point;
template <typename Fixture>
using MinimumBoundingRectangle = typename Fixture::MinimumBoundingRectangle_;

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(MinimumBoundingRectTests,
                                      "mbr of disk intersection 1",
                                      "[mbr_disk_intersection]") {
	using Fixture = MinimumBoundingRectTests<TestType>;
	using MovetkGeometryKernel = typename Fixture::MovetkGeometryKernel;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	auto expected_vertex_a = make_point({-0.283577, 2.512605});
	auto expected_vertex_b = make_point({1.818677, 0.013685});
	auto p_u = make_point({0, 0});
	auto p_v = make_point({2.57369, 4.23548});

	MinimumBoundingRectangle<Fixture> mbr;
	auto result = mbr(p_u, p_v, 2, 4);
	REQUIRE(this->norm(expected_vertex_a - result.first) < MOVETK_EPS);
	REQUIRE(this->norm(expected_vertex_b - result.second) < MOVETK_EPS);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(MinimumBoundingRectTests,
                                      "mbr of disk intersection 2",
                                      "[mbr_disk_intersection]") {
	using Fixture = MinimumBoundingRectTests<TestType>;
	using MovetkGeometryKernel = typename Fixture::MovetkGeometryKernel;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	auto expected_vertex_a = make_point({-1.936492, 2.000000});
	auto expected_vertex_b = make_point({1.936492, 0.000000});
	auto p_u = make_point({0, 0});
	auto p_v = make_point({0, 4});

	MinimumBoundingRectangle<Fixture> mbr;
	auto result = mbr(p_u, p_v, 2, 4);
	REQUIRE(this->norm(expected_vertex_a - result.first) < MOVETK_EPS);
	REQUIRE(this->norm(expected_vertex_b - result.second) < MOVETK_EPS);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(MinimumBoundingRectTests,
                                      "mbr of disk intersection 3",
                                      "[mbr_disk_intersection]") {
	using Fixture = MinimumBoundingRectTests<TestType>;
	using MovetkGeometryKernel = typename Fixture::MovetkGeometryKernel;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	auto expected_vertex_a = make_point({-0.752776, -2.075651});
	auto expected_vertex_b = make_point({-1.833011, -0.510135});
	auto p_u = make_point({0, 0});
	auto p_v = make_point({-4, -4});

	MinimumBoundingRectangle<Fixture> mbr;
	auto result = mbr(p_u, p_v, 2, 4);
	REQUIRE(this->norm(expected_vertex_a - result.first) < MOVETK_EPS);
	REQUIRE(this->norm(expected_vertex_b - result.second) < MOVETK_EPS);
}