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

#include <catch2/catch.hpp>
using namespace Catch::literals;  // 2.1_a : approximately 2.1

#include <cmath>
#include <iostream>

#include "movetk/geo/geo.h"

TEST_CASE("Euclidean distance", "[euclideandistance]") {
	using Point = std::array<double, 2>;
	Point p1 = {0.1, 0.1};
	Point p2 = {1.1, 0.1};
	auto d = movetk::geo::euclidean_distance(p1, p2);
	REQUIRE(d == Approx(1.0));
}

TEST_CASE("Euclidean distance 3d", "[euclideandistance3d]") {
	using Point = std::array<double, 3>;
	Point p1 = {186393.016, -4740913.5, 4248293};
	Point p2 = {186378.344, -4740926, 4248280};
	auto d = movetk::geo::euclidean_distance(p1, p2);
	REQUIRE(d == Approx(23.249));
}

TEST_CASE("Distance between coordinates", "[geodistance]") {
	// American football half pitch (50 yds = 45.72m)
	// https://her.is/2zylpbe
	double lat0 = 37.77818097, lon0 = -122.49134085, lat1 = 37.77859013, lon1 = -122.49136901;

	double d = movetk::geo::distance_exact(lat0, lon0, lat1, lon1);
	REQUIRE(d == Approx(45.72).epsilon(0.01));
}

TEST_CASE("Bearing between coordinates", "[geobearing]") {
	double lat0 = 33.439361, lon0 = -112.084793,  // bottom-left
	    lat1 = 33.457393, lon1 = -112.063282;     // top-right (2kmx2km)

	double bearing = movetk::geo::bearing_exact(lat0, lon0, lat1, lon1);
	REQUIRE(bearing == Approx(45.).epsilon(0.01));
}

TEST_CASE("Projection to local coordinates", "[localprojection]") {
	// American football half pitch (50 yds = 45.72m)
	// https://her.is/2zylpbe
	double lat0 = 37.77818097, lon0 = -122.49134085, lat1 = 37.77859013, lon1 = -122.49136901;
	double expected_dist = movetk::geo::distance_exact(lat0, lon0, lat1, lon1);

	movetk::geo::LocalCoordinateReference<double> ref(lat0, lon0);
	auto xy = ref.project(lat1, lon1);

	double dist = sqrt(xy[0] * xy[0] + xy[1] * xy[1]);
	REQUIRE(dist == Approx(expected_dist).epsilon(0.01));

	auto latlon = ref.inverse(xy[0], xy[1]);
	REQUIRE(latlon[0] == Approx(lat1).epsilon(.001));
	REQUIRE(latlon[1] == Approx(lon1).epsilon(.001));
}

TEST_CASE("projection_errors_by_bbox_size", "[projection_errors_by_bbox_size]") {
	double bbox_length[3] = {1000, 2000, 3000};
	for (auto length : bbox_length) {
		// Phoenix_downtown
		double lat0 = 33.439361, lon0 = -112.084793;  // bottom-left
		// lat1 = 33.457393, lon1 = -112.063282; // top-right (2kmx2km)
		double lat1, lon1;
		movetk::geo::destination_exact(lat0, lon0, length, length, lat1, lon1);

		movetk::geo::LocalCoordinateReference<double> ref(lat0, lon0);
		auto latlon = ref.inverse(length, length);
		REQUIRE(latlon[0] == Approx(lat1).epsilon(0.0001));
		REQUIRE(latlon[1] == Approx(lon1).epsilon(0.0001));
	}
}

TEST_CASE("destination by bearing and distance", "[destination_by_bearing_and_distance]") {
	// Phoenix_downtown
	double lat0 = 33.439361, lon0 = -112.084793;                    // bottom-left
	double expected_lat1 = 33.457393, expected_lon1 = -112.063282;  // top-right (2kmx2km)
	double lat1, lon1;
	movetk::geo::destination_by_bearing_exact(lat0, lon0, 45, 2000 * sqrt(2.), lat1, lon1);

	REQUIRE(lat1 == Approx(expected_lat1).epsilon(0.0001));
	REQUIRE(lon1 == Approx(expected_lon1).epsilon(0.0001));
}

TEST_CASE("Geocentric coordinates", "[geocentric]") {
	// double lat = 27.99, lon = 86.93, h = 8820; // Mt Everest
	// American football half pitch (50 yds = 45.72m)
	// https://her.is/2zylpbe
	double lat0 = 37.77818097, lon0 = -122.49134085, lat1 = 37.77859013, lon1 = -122.49136901;
	double expected_dist = movetk::geo::distance_exact(lat0, lon0, lat1, lon1);

	const GeographicLib::Geocentric& earth = GeographicLib::Geocentric::WGS84();
	auto xyz0 = movetk::geo::to_geocentric_coordinates<double>(earth, lat0, lon0);

	SECTION("Distance by geocentric coordinates") {
		auto xyz1 = movetk::geo::to_geocentric_coordinates<double>(earth, lat1, lon1);

		auto dist = movetk::geo::euclidean_distance_3d(xyz0[0], xyz0[1], xyz0[2], xyz1[0], xyz1[1], xyz1[2]);
		REQUIRE(dist == Approx(expected_dist).epsilon(0.01));
	}

	SECTION("Forward and reverse") {
		auto latlonh0 = movetk::geo::from_geocentric_coordinates(earth, xyz0[0], xyz0[1], xyz0[2]);

		REQUIRE(latlonh0[0] == Approx(lat0).epsilon(0.000001));
		REQUIRE(latlonh0[1] == Approx(lon0).epsilon(0.000001));
		REQUIRE(std::abs(latlonh0[2]) < 1e-6);
	}
}