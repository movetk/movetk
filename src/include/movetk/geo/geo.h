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
// Created by onur on 11/8/18.
//

#ifndef MOVETK_GEO_H
#define MOVETK_GEO_H

#include <GeographicLib/Geocentric.hpp>
#include <array>
#include <cmath>
#include <exception>
#include <iostream>
#include <iterator>
#include <numeric>

#include "movetk/geom/GeometryInterface.h"

/**
 * @brief Contains functions for working with geographic coordinates
 */
namespace movetk::geo {
/**
 * @brief Returns the geodesic distance between two geographical locations.
 * The coordinates are assumed to be in WGS84, and the resulting distance is in meters.
 * @param lat0 Latitude of the first location
 * @param lon0 Longitude of the first location
 * @param lat1 Latitude of the second location
 * @param lon1 Longitude of the second location
 * @return Distance between the locations, in meters
 */
double distance_exact(double lat0, double lon0, double lat1, double lon1);

/**
 * @brief Returns the destination location from a first location, moving in the provided direction (x,y).
 * x,y are specified in location cartesian coordinates
 * @param lat0 Start latitude
 * @param lon0 Start longitude
 * @param x Offset in x direction
 * @param y Offset in y direction
 * @param lat Resulting latitude
 * @param lon Resulting longitude
 */
void destination_exact(double lat0, double lon0, double x, double y, double& lat, double& lon);

/**
 * @brief 
 * @param lat0 
 * @param lon0 
 * @param bearing 
 * @param distance 
 * @param lat1 
 * @param lon1 
*/
void destination_by_bearing_exact(double lat0,
                                  double lon0,
                                  double bearing,
                                  double distance,
                                  double& lat1,
                                  double& lon1);

/**
 * @brief Computes the bearing from two locations
 * @param lat0 First location latitude
 * @param lon0 First location longitude
 * @param lat1 Second location latitude
 * @param lon1 Second location longitude
 * @return The bearing
*/
double bearing_exact(double lat0, double lon0, double lat1, double lon1);

/**
 * @brief Approximately computers the number of meters per degree change at a reference location
 * @param lat0 Latitude of the reference location
 * @param lon0 Longitude of the reference location
 * @param meters_per_lat_degree Output meters per latitude degree
 * @param meters_per_lon_degree Output meters per longitude degree
*/
void meters_per_degree(double lat0, double lon0, double& meters_per_lat_degree, double& meters_per_lon_degree);

double euclidean_distance_2d(double x0, double y0, double x1, double y1);

/**
 * @brief Computes the 3D Euclidean distance between two points
 * @param x0 First point x
 * @param y0 First point y
 * @param z0 First point z
 * @param x1 Second point x
 * @param y1 Second point x
 * @param z1 Second point x
 * @return The Euclidean distance
*/
double euclidean_distance_3d(double x0, double y0, double z0, double x1, double y1, double z1);


/**
 * Euclidean distance between n-dimensional points.
 * @tparam Point A point type with iterable coordinates
 * @param p1
 * @param p2
 * @return
 */
template <class Point>
double euclidean_distance(const Point& p1, const Point& p2) {
	using FT = typename std::iterator_traits<decltype(std::begin(p1))>::value_type;
	auto diff_squared = [](FT x1, FT x2) -> FT { return (x1 - x2) * (x1 - x2); };
	FT dist_squared = std::inner_product(std::begin(p1), std::end(p1), std::begin(p2), 0.0, std::plus<>{}, diff_squared);
	return sqrt(dist_squared);
}

/**
 * @brief Coordinate reference object that supports projecting forward and backward to the local coordinate system
 * @details Applies a simple linear interpolation, so will not be accurate for coordinates far away from the reference
 * location.
 * @tparam NT The number type to use
 */
template <class NT>
struct LocalCoordinateReference {
	//! Reference latitude
	NT ref_lat;
	//! Reference longitude
	NT ref_lon;
	//! Meters per degree at the reference latitude
	double mpd_lat;
	//! Meters per degree at the reference longitude
	double mpd_lon;

	LocalCoordinateReference() = default;

	/**
	 * @brief Construct a local coordinate reference system
	 * @param lat The reference latitude
	 * @param lon The reference longitude
	 */
	LocalCoordinateReference(NT lat, NT lon) : ref_lat(lat), ref_lon(lon) {
		meters_per_degree(static_cast<double>(ref_lat), static_cast<double>(ref_lon), mpd_lat, mpd_lon);
	}

	/**
	 * @brief Project the given latitude and longitude to local Cartesian coordinates
	 * @param lat The latitude
	 * @param lon The longitude
	 * @return (x,y) pair of local coordinates (x is directed in the "local" longitude direction)
	 */
	inline std::array<NT, 2> project(NT lat, NT lon) { return {(lon - ref_lon) * mpd_lon, (lat - ref_lat) * mpd_lat}; }

	/**
	 * @brief Projects local Cartesian coordinates back to latitude and longitude
	 * @param x The local x coordinate
	 * @param y The local y coordinate
	 * @return (latitude, longitude) values
	 */
	inline std::array<NT, 2> inverse(NT x, NT y) { return {ref_lat + y / mpd_lat, ref_lon + x / mpd_lon}; }
};

/**
 * @brief Converts geodetic to geocentric coordinates
 * @tparam NT The number type to use. Should be compatible with Geographiclib's reals
 * @param earth The coordinate conversion algorithm
 * @param lat The latitude
 * @param lon The longitude
 * @param height_m The height in meters above the ellipsoid
 * @return (x,y,z) geocentric coordinates
 */
template <class NT>
std::array<NT, 3> to_geocentric_coordinates(const GeographicLib::Geocentric& earth, NT lat, NT lon, NT height_m = 0.0) {
	try {
		double x, y, z;
		earth.Forward(static_cast<double>(lat), static_cast<double>(lon), static_cast<double>(height_m), x, y, z);
		return {static_cast<NT>(x), static_cast<NT>(y), static_cast<NT>(z)};
	} catch (const std::exception& e) {
		std::cerr << "Caught exception: " << e.what() << "\n";
		throw e;
	}
}

/**
 * @brief Converts latitude, longitude and height to geocentric coordinates
 * @tparam GeometryTraits Kernel to use
 * @param make_point The algorithm to construct points in the kernel
 * @param earth The coordinate conversion algorithm
 * @param lat The latitude
 * @param lon The longitude
 * @param height_m The height in meters above the ellipsoid
 * @return Point of the (x,y,z) geocentric coordinates
 */
template <class GeometryTraits>
typename GeometryTraits::MovetkPoint to_geocentric_coordinates(movetk::geom::MakePoint<GeometryTraits>& make_point,
                                                               const GeographicLib::Geocentric& earth,
                                                               typename GeometryTraits::NT lat,
                                                               typename GeometryTraits::NT lon,
                                                               typename GeometryTraits::NT height_m = 0.0) {
	using NT = typename GeometryTraits::NT;
	const auto coordinate_array = to_geocentric_coordinates(earth, lat, lon, height_m);
	return make_point(std::cbegin(coordinate_array), std::cend(coordinate_array));
}

/**
 * @brief Converts geocentric coordinates back to geodetic coordinates
 * @tparam NT The number type
 * @param earth The coordinate conversion algorithm
 * @param x Local x coordinate
 * @param y Local y coordinate
 * @param z Local z coordinate
 * @return Array of (lat, lon, height) coordinates
*/
template <class NT>
std::array<NT, 3> from_geocentric_coordinates(const GeographicLib::Geocentric& earth, NT x, NT y, NT z) {
	try {
		double lat, lon, height_m;
		earth.Reverse(static_cast<double>(x), static_cast<double>(y), static_cast<double>(z), lat, lon, height_m);
		return {static_cast<NT>(lat), static_cast<NT>(lon), static_cast<NT>(height_m)};
	} catch (const std::exception& e) {
		std::cerr << "Caught exception: " << e.what() << "\n";
		throw e;
	}
}
}  // namespace movetk::geo
#endif  // MOVETK_GEO_H
