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

#ifndef MOVETK_METRIC_DISTANCES_DISCRETEFRECHET_H
#define MOVETK_METRIC_DISTANCES_DISCRETEFRECHET_H


#include "movetk/geom/GeometryInterface.h"

namespace movetk::metric {
/**
 * @brief Functor for computing the discrete Frechet distance
 * @tparam Kernel The kernel to use
 * @tparam Norm The norm to use
 */
template <class Kernel, class Norm>
class Discrete_Frechet {
private:
	using NT = typename Kernel::NT;

	/**
	 * @brief Compute the distance between two points, referenced by the given iterators
	 * @param iter_a First iterator
	 * @param iter_b Second iterator
	 * @return Distance between the iterators.
	 */
	template <utils::RandomAccessIterator<typename Kernel::MovetkPoint> InputIterator>
	NT distance(InputIterator iter_a, InputIterator iter_b) {
		Norm norm;
		typename Kernel::MovetkVector v = *iter_b - *iter_a;
		return norm(v);
	}

public:
	/**
	 * @brief Computes the discrete Frechet distance between two polyline, given as two coordinate
	 * ranges.
	 * @param polyline_a_first Start of the coordinate range of the first polyline
	 * @param polyline_a_beyond Endof the coordinate range of the first polyline
	 * @param polyline_b_first Start of the coordinate range of the second polyline
	 * @param polyline_b_beyond End of the coordinate range of the second polyline
	 * @return The discrete Frechet distance
	 */
	template <utils::RandomAccessIterator<typename Kernel::MovetkPoint> PolylineCoordIterator>
	NT operator()(PolylineCoordIterator polyline_a_first,
	              PolylineCoordIterator polyline_a_beyond,
	              PolylineCoordIterator polyline_b_first,
	              PolylineCoordIterator polyline_b_beyond) {
		std::size_t size_polyline_b = std::distance(polyline_b_first, polyline_b_beyond);
		// Initialize the dynamic programming table row.
		std::vector<NT> dp_row(size_polyline_b, static_cast<NT>(-1));

		// Compute the Frechet distance using dynamic programming.
		size_t i = 0;
		for (auto it_a = polyline_a_first; it_a != polyline_a_beyond; ++it_a, ++i) {
			std::size_t j = 0;
			NT previous = -1, current = -1;
			for (auto it_b = polyline_b_first; it_b != polyline_b_beyond; ++it_b, ++j) {
				if (i == 0 && j == 0) {
					current = distance(it_a, it_b);
					dp_row[0] = current;
				} else if (i != 0 && j == 0) {
					current = std::max(dp_row[0], distance(it_a, it_b));
					dp_row[0] = current;
				} else if (i == 0 && j != 0) {
					current = std::max(previous, distance(it_a, it_b));
					dp_row[j - 1] = previous;
				} else {
					current = std::max(std::min({dp_row[j], previous, dp_row[j - 1]}), distance(it_a, it_b));
					dp_row[j - 1] = previous;
				}
				previous = current;
			}
			dp_row[j - 1] = previous;
		}

		// Return the normed distance of the last element, this is now the discrete Frechet distance.
		NT dfd = dp_row.back();
		NT n = 1 / static_cast<NT>(Norm::P);
		return std::pow(dfd, n);
	}
};
}  // namespace movetk::metric
#endif