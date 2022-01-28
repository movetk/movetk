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

#include "test_includes.h"
#if CGAL_BACKEND_ENABLED
#include "movetk/geom/CGALTraits.h"
#else

#include "movetk/geom/BoostGeometryTraits.h"
#endif

#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"

using namespace std;

#if CGAL_BACKEND_ENABLED
//==============================
// Define the Number Type
// For the CGAL backend,
// One can choose from the
// following number types
typedef long double NT;
// typedef CGAL::Mpzf NT;
// typedef CGAL::Gmpfr NT;
// typedef CGAL::Gmpq NT;
//==============================

//==============================
// Define the Dimensions
const size_t dimensions = 2;
//==============================

//==============================
// Define the Geometry Backend
typedef movetk_support::CGALTraits<NT, dimensions> CGAL_GeometryBackend;
// Using the Geometry Backend define the Movetk Geometry Kernel
typedef movetk_core::MovetkGeometryKernel<typename CGAL_GeometryBackend::Wrapper_CGAL_Geometry> MovetkGeometryKernel;
//==============================
#else
//==============================
// Define the Number Type
// For the Boost Backend
typedef long double NT;
//==============================

//==============================
// Define the Dimensions
// It is possible to choose
// a higher dimension
// Note: Boost Geometry Adapter supports geometry in two
// dimensions only
const static size_t dimensions = 2;
//==============================

//==============================
// Define the Geometry Backend
typedef movetk::geom::BoostGeometryTraits<long double, dimensions> Boost_Geometry_Backend;
// Using the Geometry Backend define the Movetk Geometry Kernel
typedef movetk::geom::MovetkGeometryKernel<typename Boost_Geometry_Backend::Wrapper_Boost_Geometry>
    MovetkGeometryKernel;
//==============================
#endif

MOVETK_TEMPLATE_LIST_TEST_CASE("Check euclidean norm", "[is_valid_euclidean_norm]") {
	using MovetkGeometryKernel = typename TestType::MovetkGeometryKernel;
	typedef movetk::metric::FiniteNorm<MovetkGeometryKernel, 2> Norm;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	Norm euclidean_norm;
	MovetkGeometryKernel::MovetkPoint p1 = make_point({5.5, 3.1});
	MovetkGeometryKernel::MovetkPoint p2 = make_point({3.22, 1.3});
	MovetkGeometryKernel::MovetkVector v = p2 - p1;
	std::cout << "Squared Euclidean Norm: " << euclidean_norm(v) << std::endl;
	MovetkGeometryKernel::NT result = euclidean_norm ^ 2;
	std::cout << "Squared Euclidean Norm: " << result << std::endl;
	REQUIRE(abs(euclidean_norm(v) - 8.4384) < MOVETK_EPS);
	REQUIRE(abs(result - 8.4384) < MOVETK_EPS);
}

MOVETK_TEMPLATE_LIST_TEST_CASE("Check Taxicab norm", "[is_valid_taxicab_norm]") {
	using MovetkGeometryKernel = typename TestType::MovetkGeometryKernel;
	typedef movetk::metric::FiniteNorm<MovetkGeometryKernel, 1> Norm;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	Norm taxicab_norm;
	MovetkGeometryKernel::MovetkPoint p1 = make_point({5.5, 3.1});
	MovetkGeometryKernel::MovetkPoint p2 = make_point({3.22, 1.3});
	MovetkGeometryKernel::MovetkVector v = p2 - p1;
	std::cout << "Taxicab Norm: " << taxicab_norm(v) << std::endl;
	MovetkGeometryKernel::NT result = taxicab_norm ^ 2;
	std::cout << "Squared Taxicab Norm: " << result << std::endl;
	REQUIRE(abs(taxicab_norm(v) - 4.08) < MOVETK_EPS);
	REQUIRE(abs(result - 16.6464) < MOVETK_EPS);
}


MOVETK_TEMPLATE_LIST_TEST_CASE("Check Infinity norm", "[is_valid_infinity_norm]") {
	using MovetkGeometryKernel = typename TestType::MovetkGeometryKernel;
	typedef movetk::metric::InfinityNorm<MovetkGeometryKernel> Norm;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	Norm infinity_norm;
	MovetkGeometryKernel::MovetkPoint p1 = make_point({5.5, 3.1});
	MovetkGeometryKernel::MovetkPoint p2 = make_point({3.22, 1.3});
	MovetkGeometryKernel::MovetkVector v = p2 - p1;
	std::cout << "Infinity Norm: " << infinity_norm(v) << std::endl;
	MovetkGeometryKernel::NT result = infinity_norm ^ 2;
	std::cout << "Squared Infinity Norm: " << result << std::endl;
	REQUIRE(abs(infinity_norm(v) - 2.28) < MOVETK_EPS);
	REQUIRE(abs(result - 5.1984) < MOVETK_EPS);
}