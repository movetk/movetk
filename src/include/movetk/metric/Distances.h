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

#ifndef MOVETK_DISTANCES_H
#define MOVETK_DISTANCES_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <iostream>

#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/distances/DiscreteFrechet.h"
#include "movetk/metric/distances/DiscreteHausdorff.h"
#include "movetk/metric/distances/StrongFrechet.h"
#include "movetk/metric/distances/WeakFrechet.h"

/**
 * @brief Collection of metric distance computations
*/
namespace movetk::metric {

/**
 * @brief Squared distance functor for computing the squared distance between geometric objects
 * @tparam Kernel The kernel of the geometric objects
 * @tparam _Norm The used norm.
 */
template <class Kernel, class _Norm>
struct squared_distance_d {
	using Norm = _Norm;
	using MovetkPoint = typename Kernel::MovetkPoint;

	/**
	 * @brief Compute the squared distance between a point and a line
	 * @param point The point
	 * @param line The line
	 * @return The squared distance
	 */
	typename Kernel::NT operator()(const MovetkPoint &point, const typename Kernel::MovetkLine &line) const {
		Norm n;
		const auto line_direction = line[1] - line[0];
		const auto line_to_point_direction = point - line[0];
		const auto product = line_direction * line_to_point_direction;
		n(product);
		const auto vv = n ^ 2;
		product *= (product / vv);
		const auto Pb = line[0] + line_to_point_direction;
		const auto v2 = point - Pb;
		return n(v2);
	}
	/**
	 * @brief Compute the squared distance between a point and a line
	 * @param line The line
	 * @param point The point
	 * @return The squared distance
	 */
	typename Kernel::NT operator()(const typename Kernel::MovetkLine &line, const MovetkPoint &point) {
		return this->operator()(point, line);
	}

	/**
	 * @brief Compute the squared distance between a point and a segment
	 * @param point The point
	 * @param segment The segment
	 * @return The squared distance
	 */
	typename Kernel::NT operator()(const MovetkPoint &point, const typename Kernel::MovetkSegment &segment) const {
		Norm n;
		typename Kernel::MovetkVector v = segment[1] - segment[0];
		typename Kernel::MovetkVector u = point - segment[0];
		typename Kernel::MovetkVector v1 = point - segment[1];
		typename Kernel::NT uv = u * v;
		if (uv <= 0) {
			return n(u);
		}
		n(v);
		typename Kernel::NT vv = n ^ Norm::P;
		if (vv <= uv) {
			return n(v1);
		}
		v *= (uv / vv);
		typename Kernel::MovetkPoint Pb = segment[0] + v;
		typename Kernel::MovetkVector v2 = point - Pb;
		return n(v2);
	}

	/**
	 * @brief Compute the squared distance between a point and a segment
	 * @param point The point
	 * @param segment The segment
	 * @return The squared distance
	 */
	typename Kernel::NT operator()(typename Kernel::MovetkSegment &segment, const MovetkPoint &point) const {
		return this->operator()(point, segment);
	}

	/**
	 * @brief Compute the squared distance between two points
	 * @param p1 First point
	 * @param p2 Second point
	 * @return The squared distance
	*/
	typename Kernel::NT operator()(const MovetkPoint &p1, const MovetkPoint &p2) const {
		Norm n;
		typename Kernel::MovetkVector v = p2 - p1;
		return n(v);
	}
};

}  // namespace movetk::metric
#endif  // MOVETK_DISTANCES_H
