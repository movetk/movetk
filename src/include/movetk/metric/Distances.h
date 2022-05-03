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
#include "movetk/utils/Asserts.h"

namespace movetk::metric {

// based on Eiter, T., & Mannila, H. (1994). Computing Discrete Fréchet Distance ∗.

template <class Kernel, class _Norm>
struct squared_distance_d {
	using Norm = _Norm;
	using MovetkPoint = typename Kernel::MovetkPoint;

	typename Kernel::NT operator()(const MovetkPoint &p, const typename Kernel::MovetkLine &l) const {
		Norm n;
		typename Kernel::MovetkVector v = l[1] - l[0];
		typename Kernel::MovetkVector u = p - l[0];
		typename Kernel::NT uv = u * v;
		n(v);
		typename Kernel::NT vv = n ^ 2;
		v *= (uv / vv);
		typename Kernel::MovetkPoint Pb = l[0] + v;
		typename Kernel::MovetkVector v2 = p - Pb;
		return n(v2);
	}

	typename Kernel::NT operator()(typename Kernel::MovetkLine &l, const MovetkPoint &p) {
		return this->operator()(p, l);
	}

	typename Kernel::NT operator()(const MovetkPoint &p, const typename Kernel::MovetkSegment &s) const {
		Norm n;
		typename Kernel::MovetkVector v = s[1] - s[0];
		typename Kernel::MovetkVector u = p - s[0];
		typename Kernel::MovetkVector v1 = p - s[1];
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
		typename Kernel::MovetkPoint Pb = s[0] + v;
		typename Kernel::MovetkVector v2 = p - Pb;
		return n(v2);
	}
	typename Kernel::NT operator()(typename Kernel::MovetkSegment &s, const MovetkPoint &p) const {
		return this->operator()(p, s);
	}

	typename Kernel::NT operator()(const MovetkPoint &p1, const MovetkPoint &p2) const {
		Norm n;
		typename Kernel::MovetkVector v = p2 - p1;
		return n(v);
	}
};

}  // namespace movetk::metric
#endif  // MOVETK_DISTANCES_H
