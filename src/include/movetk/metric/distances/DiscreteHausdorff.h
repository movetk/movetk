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
#include "movetk/utils/Asserts.h"

namespace movetk::metric {

template <class Kernel, class Norm>
class Discrete_Hausdorff {
private:
	template <class InputIterator,
	          typename = movetk::utils::requires_random_access_iterator<InputIterator>,
	          typename = movetk::utils::requires_movetk_point<Kernel, typename InputIterator::value_type>>
	typename Kernel::NT singlesided_discrete_hausdorff(InputIterator a_first,
	                                                   InputIterator a_beyond,
	                                                   InputIterator b_first,
	                                                   InputIterator b_beyond) {
		Norm norm;
		InputIterator it_a = a_first;
		typename Kernel::NT max_dist = 0;
		while (it_a != a_beyond) {
			InputIterator it_b = b_first;
			typename Kernel::MovetkVector v = *it_b - *it_a;
			typename Kernel::NT distance = norm(v);
			typename Kernel::NT min_dist = distance;
			it_b++;
			while (it_b != b_beyond) {
				v = *it_b - *it_a;
				distance = norm(v);
				if (distance < min_dist) {
					min_dist = distance;
				}
				it_b++;
			}
			if (min_dist > max_dist) {
				max_dist = min_dist;
			}
			it_a++;
		}

		typename Kernel::NT n = 1 / static_cast<typename Kernel::NT>(Norm::P);
		return std::pow(max_dist, n);
	}

public:
	template <class InputIterator,
	          typename = movetk::utils::requires_random_access_iterator<InputIterator>,
	          typename = movetk::utils::requires_movetk_point<Kernel, typename InputIterator::value_type>>
	typename Kernel::NT operator()(InputIterator polyline_a_first,
	                               InputIterator polyline_a_beyond,
	                               InputIterator polyline_b_first,
	                               InputIterator polyline_b_beyond) {
		const auto hd_pq =
		    singlesided_discrete_hausdorff(polyline_a_first, polyline_a_beyond, polyline_b_first, polyline_b_beyond);

		const auto hd_qp =
		    singlesided_discrete_hausdorff(polyline_b_first, polyline_b_beyond, polyline_a_first, polyline_a_beyond);

		return std::max(hd_pq, hd_qp);
	}
};

}  // namespace movetk::metric
#endif