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
// Created by Mitra, Aniket on 2019-06-03.
//

#ifndef MOVETK_METRIC_DISTANCES_DISCRETEHAUSDORFF_H
#define MOVETK_METRIC_DISTANCES_DISCRETEHAUSDORFF_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <iostream>

#include "movetk/geom/GeometryInterface.h"

namespace movetk::metric {

/**
 * @brief Functor for computing the discrete Hausdorff distance
 * @tparam Kernel The kernel to use
 * @tparam Norm The norm to use
 */
template <class Kernel, class Norm>
class Discrete_Hausdorff {
private:
	/**
	 * @brief Compute the singlesided discrete Hausdorff distance from polyline a
	 * to polyline b
	 * @param a_first Start of the coordinate range of polyline a
	 * @param a_beyond End of the coordinate range of polyline a
	 * @param b_first Start of the coordinate range of polyline b
	 * @param b_beyond End of the coordinate range of polyline b
	 * @return The singlesided discrete Hausdorff distance.
	 */
	template <utils::RandomAccessIterator<typename Kernel::MovetkPoint> InputIterator>
	typename Kernel::NT singlesided_discrete_hausdorff(InputIterator a_first,
	                                                   InputIterator a_beyond,
	                                                   InputIterator b_first,
	                                                   InputIterator b_beyond) {
		Norm norm;

		typename Kernel::NT max_dist = 0;
		for (InputIterator it_a = a_first; it_a != a_beyond; ++it_a) {
			InputIterator it_b = b_first;
			auto v = *it_b - *it_a;
			auto distance = norm(v);
			auto min_dist = distance;
			it_b++;
			for (; it_b != b_beyond; ++it_b) {
				v = *it_b - *it_a;
				distance = norm(v);
				min_dist = std::min(distance, min_dist);
			}
			max_dist = std::max(max_dist, min_dist);
		}
		return max_dist;
	}

public:
	/**
	 * @brief Computes the discrete Hausdorff distance between two polylines a and b
	 * @param polyline_a_first Start of the coordinate range of polyline a
	 * @param polyline_a_beyond End of the coordinate range of polyline a
	 * @param polyline_b_first Start of the coordinate range of polyline b
	 * @param polyline_b_beyond End of the coordinate range of polyline b
	 * @return The discrete Hausdorff distance betwee poyline a and polyline b.
	 */
	template <utils::RandomAccessIterator<typename Kernel::MovetkPoint> PolylineCoordIterator>
	typename Kernel::NT operator()(PolylineCoordIterator polyline_a_first,
	                               PolylineCoordIterator polyline_a_beyond,
	                               PolylineCoordIterator polyline_b_first,
	                               PolylineCoordIterator polyline_b_beyond) {
		const auto distance_ab =
		    singlesided_discrete_hausdorff(polyline_a_first, polyline_a_beyond, polyline_b_first, polyline_b_beyond);

		const auto distance_ba =
		    singlesided_discrete_hausdorff(polyline_b_first, polyline_b_beyond, polyline_a_first, polyline_a_beyond);
		typename Kernel::NT n = 1 / static_cast<typename Kernel::NT>(Norm::P);
		return std::pow(std::max(distance_ab, distance_ba), n);
	}
};

}  // namespace movetk::metric
#endif