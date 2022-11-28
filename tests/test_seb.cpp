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

#include <iostream>
#include <vector>

#include "catch2/catch.hpp"
#include "movetk/geom/BoostGeometryTraits.h"
#include "movetk/geom/BoostGeometryWrapper.h"
#include "movetk/geom/GeometryInterface.h"
#include "third_party/miniball/Seb.h"

struct MiniballTests {
	static constexpr size_t dimension = 2;
	using NT = double;
	using MovetkGeometryKernel =movetk::backends::boost::KernelFor<NT, dimension>;
};

TEST_CASE_METHOD(MiniballTests, "miniball", "[miniball]") {
	typedef double FT;  // miniball library requires FT to be double (not float!)
	typedef Seb::Point<FT> Point;
	typedef Seb::Smallest_enclosing_ball<FT> Miniball;

	std::vector<std::array<FT, dimension>> pts = {{0., 0.}, {1., 0.}, {1., 1.}, {0., 1.}, {0., 0.}};
	std::vector<Point> seb_points;
	for (auto xy : pts) {
		seb_points.push_back(Point(dimension, std::cbegin(xy)));
	}

	Miniball mb(dimension, seb_points);
	FT seb_radius = mb.radius();

	std::vector<FT> res;  // center.x, center.y
	Miniball::Coordinate_iterator center_it = mb.center_begin();
	for (int j = 0; j < dimension; ++j) {
		res.push_back(center_it[j]);
	}
	REQUIRE(seb_radius == Approx(0.707).epsilon(0.001));
	REQUIRE(res[0] == Approx(0.5));
	REQUIRE(res[1] == Approx(0.5));
}

TEST_CASE_METHOD(MiniballTests, "miniball2", "[miniball]") {
	// Create some MovetkPoints
	using ExamplePoint = std::array<typename MovetkGeometryKernel::NT, dimension>;
	std::vector<ExamplePoint> points = {{0., 0.}, {1., 0.}, {1., 1.}, {0., 1.}, {0., 0.}};

	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	std::vector<typename MovetkGeometryKernel::MovetkPoint> movetk_points;

	for (auto p : points) {
		MovetkGeometryKernel::MovetkPoint pt = make_point(std::cbegin(p), std::cend(p));
		movetk_points.push_back(pt);
	}

	typedef double FT;  // miniball library requires FT to be double (not float!)
	typedef Seb::Point<FT> Point;
	typedef Seb::Smallest_enclosing_ball<FT> Miniball;

	std::vector<Point> seb_points;
	for (auto movetk_point : movetk_points) {
		Point p = Point(dimension, std::cbegin(movetk_point));
		seb_points.push_back(p);
	}

	Miniball mb(dimension, seb_points);
	FT seb_radius = mb.radius();
	
	std::vector<FT> res;  // center.x, center.y
	Miniball::Coordinate_iterator center_it = mb.center_begin();
	for (int j = 0; j < dimension; ++j) {
		res.push_back(center_it[j]);
	}
	REQUIRE(seb_radius == Approx(0.707).epsilon(0.001));
	REQUIRE(res[0] == Approx(0.5));
	REQUIRE(res[1] == Approx(0.5));
}

TEST_CASE_METHOD(MiniballTests, "miniball3", "[miniball]") {
	// Create some MovetkPoints
	using ExamplePoint = std::array<typename MovetkGeometryKernel::NT, dimension>;
	std::vector<ExamplePoint> points = {{0., 0.}, {1., 0.}, {1., 1.}, {0., 1.}, {0., 0.}};

	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	std::vector<typename MovetkGeometryKernel::MovetkPoint> movetk_points;

	for (auto p : points) {
		MovetkGeometryKernel::MovetkPoint pt = make_point(std::cbegin(p), std::cend(p));
		movetk_points.push_back(pt);
	}

	// Compute min sphere
	std::vector<typename MovetkGeometryKernel::NT> CenterMinSphere;
	movetk::geom::MakeMinSphere<MovetkGeometryKernel> min_sphere;
	MovetkGeometryKernel::NT radius = min_sphere(std::cbegin(movetk_points),
	                                             std::cend(movetk_points),
	                                             std::back_inserter(CenterMinSphere));

	auto cit = std::cbegin(CenterMinSphere);
	REQUIRE(radius == Approx(0.707).epsilon(0.001));
	REQUIRE(*cit == Approx(0.5));
	REQUIRE(*(cit + 1) == Approx(0.5));
}
