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
#include "movetk/utils/Asserts.h"

namespace movetk::metric {
template <class Kernel, class Norm>
class Discrete_Frechet {
private:
	typedef typename Kernel::NT NT;

	template <utils::RandomAccessIterator<typename Kernel::MovetkPoint> InputIterator>
	NT distance(InputIterator iter_a, InputIterator iter_b) {
		Norm norm;
		typename Kernel::MovetkVector v = *iter_b - *iter_a;
		return norm(v);
	}

public:
	template <utils::RandomAccessIterator<typename Kernel::MovetkPoint> PolylineCoordIterator>
	NT operator()(PolylineCoordIterator polyline_a_first,
	              PolylineCoordIterator polyline_a_beyond,
	              PolylineCoordIterator polyline_b_first,
	              PolylineCoordIterator polyline_b_beyond) {
		std::size_t size_polyline_b = std::distance(polyline_b_first, polyline_b_beyond);
		std::vector<NT> dp_row(size_polyline_b);
		std::fill(std::begin(dp_row), std::begin(dp_row) + size_polyline_b, -1);
		auto it_a = polyline_a_first;
		while (it_a != polyline_a_beyond) {
			auto it_b = polyline_b_first;
			std::size_t j = 0;
			NT previous = -1, current = -1;
			while (it_b != polyline_b_beyond) {
				if ((it_a == polyline_a_first) && (it_b == polyline_b_first)) {
					current = distance(it_a, it_b);
					dp_row[0] = current;
				}

				else if ((it_a != polyline_a_first) && (it_b == polyline_b_first)) {
					current = std::max(dp_row[0], distance(it_a, it_b));
					dp_row[0] = current;
				} else if ((it_a == polyline_a_first) && (it_b != polyline_b_first)) {
					current = std::max(previous, distance(it_a, it_b));
					dp_row[j - 1] = previous;
				} else {
					current = std::max(std::min(std::min(dp_row[j], previous), dp_row[j - 1]), distance(it_a, it_b));
					dp_row[j - 1] = previous;
				}

				previous = current;
				j++;
				it_b++;
			}
			dp_row[j - 1] = previous;
			it_a++;
		}

		NT dfd = dp_row.back();
		NT n = 1 / static_cast<NT>(Norm::P);
		return std::pow(dfd, n);
	}
};
}  // namespace movetk::metric
#endif