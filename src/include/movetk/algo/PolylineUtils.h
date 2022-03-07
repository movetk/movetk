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
// Created by onur on 1/24/18.
//

#ifndef MOVETK_POLYLINEUTILS_H
#define MOVETK_POLYLINEUTILS_H

#include <numeric>

#include "movetk/geo/geo.h"
#include "movetk/geom/trajectory_to_interface.h"
#include "movetk/utils/Iterators.h"

namespace movetk::algo {


/**
 * Compute trajectory length in meters
 * @tparam PointIterator iterator type to a list of Points with Cartesian coordinates in meters.
 *                       Requires Points to be CoordinateIterable.
 * @return length in meters
 */
template <class PointIterator>
double polyline_length_m(PointIterator start, PointIterator beyond) {
	using point_type = typename std::iterator_traits<PointIterator>::value_type;
	std::function<double(point_type &, point_type &)> distancefn =
	    (double (*)(point_type &, point_type &)) & movetk::geo::euclidean_distance<point_type>;
	return std::inner_product(start, beyond - 1, start + 1, 0.0, std::plus<>(), distancefn);
}


template <class GeometryTraits, class LatIterator, class LonIterator>
double geopolyline_length_m(LatIterator lat_start, LatIterator lat_beyond, LonIterator lon_start) {
	using MovetkPoint = typename GeometryTraits::MovetkPoint;
	movetk::geom::MakePoint<GeometryTraits> make_point;
	std::vector<MovetkPoint> xyzs;
	movetk::geom::to_geocentered_polyline(make_point,
	                                lat_start,
	                                lat_beyond,
	                                lon_start,
	                                movetk::utils::movetk_back_insert_iterator(xyzs));
	return polyline_length_m(std::begin(xyzs), std::end(xyzs));
}

/**
 * Calculates the lengths between consecutive points.
 * Points should be in Euclidean space and are required to have iterable coordinates.
 * @tparam PointIterator
 * @param start
 * @param beyond
 * @return
 */
template <class PointIterator>
std::vector<double> polyline_lengths_m(PointIterator start, PointIterator beyond) {
	using point_type = typename std::iterator_traits<PointIterator>::value_type;
	std::function<double(point_type &, point_type &)> distancefn =
	    (double (*)(point_type &, point_type &)) & movetk::geo::euclidean_distance<point_type>;
	std::vector<double> lengths;
	std::transform(start, beyond - 1, start + 1, std::back_inserter(lengths), distancefn);
	return lengths;
}

template <class GeometryTraits, class LatIterator, class LonIterator>
std::vector<double> geopolyline_offsets_m(LatIterator lat_start, LatIterator lat_beyond, LonIterator lon_start) {
	using MovetkPoint = typename GeometryTraits::MovetkPoint;
	movetk::geom::MakePoint<GeometryTraits> make_point;
	std::vector<MovetkPoint> xyzs;
	movetk::geom::to_geocentered_polyline(make_point,
	                                lat_start,
	                                lat_beyond,
	                                lon_start,
	                                movetk::utils::movetk_back_insert_iterator(xyzs));

	auto lengths = polyline_lengths_m(std::begin(xyzs), std::end(xyzs));
	std::vector<double> offsets;
	offsets.reserve(1 + lengths.size());
	offsets.push_back(0);  // add origin offset as well
	std::partial_sum(std::begin(lengths), std::end(lengths), std::back_inserter(offsets));
	return offsets;
}

template <class GeometryTraits>
struct is_monotone {
	template <class InputIterator, class OutputIterator>
	void operator()(InputIterator first, InputIterator beyond, OutputIterator result) {
		InputIterator cit = first + 2;
		*result = true;
		while (cit != beyond) {
			typename GeometryTraits::MovetkVector u = *(cit - 1) - *(cit - 2);
			typename GeometryTraits::MovetkVector v = *cit - *(cit - 1);
			typename GeometryTraits::NT uv = u * v;
			if (uv < 0) {
				*result = false;
			} else {
				*result = true;
			}
			cit++;
		}
	}
};

/*template<class GeometryTraits, std::size_t>
struct sort_by_X*/

}  // namespace movetk::algo

#endif  // MOVETK_POLYLINEUTILS_H
