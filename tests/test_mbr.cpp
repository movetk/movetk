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
#if CGAL_BACKEND_ENABLED
#include "movetk/geom/CGALTraits.h"
#else

#include "movetk/geom/BoostGeometryTraits.h"
#endif

#include "movetk/geo/geo.h"
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
typedef movetk::utils::MovetkGeometryKernel<typename CGAL_GeometryBackend::Wrapper_CGAL_Geometry> MovetkGeometryKernel;
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

typedef movetk::metric::FiniteNorm<MovetkGeometryKernel, 2> Norm;

TEST_CASE("mbr of disk intersection 1", "[mbr_disk_intersection_1]") {
	std::ios_base::sync_with_stdio(false);
	std::cout.setf(std::ios::fixed);
	Norm norm;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	typename MovetkGeometryKernel::MovetkPoint expected_vertex_a = make_point({-0.283577, 2.512605});
	typename MovetkGeometryKernel::MovetkPoint expected_vertex_b = make_point({1.818677, 0.013685});
	typename MovetkGeometryKernel::MovetkPoint p_u = make_point({0, 0});

	typename MovetkGeometryKernel::MovetkPoint p_v = make_point({2.57369, 4.23548});

	typedef typename movetk::geom::mbr_selector<
	    MovetkGeometryKernel,
	    Norm,
	    typename MovetkGeometryKernel::MovetkMinimumBoundingRectangle>::MinimumBoundingRectangle MinimumBoundingRectangle;
	MinimumBoundingRectangle mbr;
	auto result = mbr(p_u, p_v, 2, 4);
	std::cout << "Vertex A: " << result.first << "\n";
	std::cout << "Vertex B: " << result.second << "\n";
	MovetkGeometryKernel::MovetkVector eps = expected_vertex_a - result.first;
	REQUIRE((eps * eps) < MOVETK_EPS);
	eps = expected_vertex_b - result.second;
	REQUIRE((eps * eps) < MOVETK_EPS);
}


TEST_CASE("mbr of disk intersection 2", "[mbr_disk_intersection_2]") {
	std::ios_base::sync_with_stdio(false);
	std::cout.setf(std::ios::fixed);
	Norm norm;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	typename MovetkGeometryKernel::MovetkPoint expected_vertex_a = make_point({-1.936492, 2.000000});
	typename MovetkGeometryKernel::MovetkPoint expected_vertex_b = make_point({1.936492, 0.000000});
	typename MovetkGeometryKernel::MovetkPoint p_u = make_point({0, 0});

	typename MovetkGeometryKernel::MovetkPoint p_v = make_point({0, 4});

	typedef typename movetk::geom::mbr_selector<
	    MovetkGeometryKernel,
	    Norm,
	    typename MovetkGeometryKernel::MovetkMinimumBoundingRectangle>::MinimumBoundingRectangle MinimumBoundingRectangle;
	MinimumBoundingRectangle mbr;
	auto result = mbr(p_u, p_v, 2, 4);
	std::cout << "Vertex A: " << result.first << "\n";
	std::cout << "Vertex B: " << result.second << "\n";
	MovetkGeometryKernel::MovetkVector eps = expected_vertex_a - result.first;
	REQUIRE((eps * eps) < MOVETK_EPS);
	eps = expected_vertex_b - result.second;
	REQUIRE((eps * eps) < MOVETK_EPS);
}


TEST_CASE("mbr of disk intersection 3", "[mbr_disk_intersection_3]") {
	std::ios_base::sync_with_stdio(false);
	std::cout.setf(std::ios::fixed);
	Norm norm;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	typename MovetkGeometryKernel::MovetkPoint expected_vertex_a = make_point({-0.752776, -2.075651});
	typename MovetkGeometryKernel::MovetkPoint expected_vertex_b = make_point({-1.833011, -0.510135});
	typename MovetkGeometryKernel::MovetkPoint p_u = make_point({0, 0});

	typename MovetkGeometryKernel::MovetkPoint p_v = make_point({-4, -4});

	typedef typename movetk::geom::mbr_selector<
	    MovetkGeometryKernel,
	    Norm,
	    typename MovetkGeometryKernel::MovetkMinimumBoundingRectangle>::MinimumBoundingRectangle MinimumBoundingRectangle;
	MinimumBoundingRectangle mbr;
	auto result = mbr(p_u, p_v, 2, 4);
	std::cout << "Vertex A: " << result.first << "\n";
	std::cout << "Vertex B: " << result.second << "\n";
	MovetkGeometryKernel::MovetkVector eps = expected_vertex_a - result.first;
	REQUIRE((eps * eps) < MOVETK_EPS);
	eps = expected_vertex_b - result.second;
	REQUIRE((eps * eps) < MOVETK_EPS);
}