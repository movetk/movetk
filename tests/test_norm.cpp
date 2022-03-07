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
// Created by Mitra, Aniket on 2019-02-19.
//


#include <array>
#include <catch2/catch.hpp>

#include "helpers/CustomCatchTemplate.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"


MOVETK_TEMPLATE_LIST_TEST_CASE("Check euclidean norm", "[is_valid_euclidean_norm]") {
	using MovetkGeometryKernel = typename TestType::MovetkGeometryKernel;
	using MovetkPoint = typename MovetkGeometryKernel::MovetkPoint;
	typedef movetk::metric::FiniteNorm<MovetkGeometryKernel, 2> Norm;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	Norm euclidean_norm;
	const auto p1 = make_point({5.5, 3.1});
	const auto p2 = make_point({3.22, 1.3});
	const auto v = p2 - p1;

	REQUIRE(abs(euclidean_norm(v) - 8.4384) < MOVETK_EPS);
	const auto result = euclidean_norm ^ 2;
	REQUIRE(abs(result - 8.4384) < MOVETK_EPS);
}

MOVETK_TEMPLATE_LIST_TEST_CASE("Check Taxicab norm", "[is_valid_taxicab_norm]") {
	using MovetkGeometryKernel = typename TestType::MovetkGeometryKernel;
	using MovetkPoint = typename MovetkGeometryKernel::MovetkPoint;
	typedef movetk::metric::FiniteNorm<MovetkGeometryKernel, 1> Norm;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	Norm taxicab_norm;
	const auto p1 = make_point({5.5, 3.1});
	const auto p2 = make_point({3.22, 1.3});
	const auto v = p2 - p1;

	REQUIRE(abs(taxicab_norm(v) - 4.08) < MOVETK_EPS);
	typename MovetkGeometryKernel::NT result = taxicab_norm ^ 2;
	REQUIRE(abs(result - 16.6464) < MOVETK_EPS);
}


MOVETK_TEMPLATE_LIST_TEST_CASE("Check Infinity norm", "[is_valid_infinity_norm]") {
	using MovetkGeometryKernel = typename TestType::MovetkGeometryKernel;
	typedef movetk::metric::InfinityNorm<MovetkGeometryKernel> Norm;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	Norm infinity_norm;
	const auto p1 = make_point({5.5, 3.1});
	const auto p2 = make_point({3.22, 1.3});
	const auto v = p2 - p1;
	REQUIRE(abs(infinity_norm(v) - 2.28) < MOVETK_EPS);
	const auto result = infinity_norm ^ 2;
	REQUIRE(abs(result - 5.1984) < MOVETK_EPS);
}