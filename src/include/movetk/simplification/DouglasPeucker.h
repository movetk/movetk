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
 // Created by Mitra, Aniket on 2019-02-14.
 //
#ifndef MOVETK_ALGO_SIMPLIFICATION_DOUGLASPEUCKER_H
#define MOVETK_ALGO_SIMPLIFICATION_DOUGLASPEUCKER_H
#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/DistanceInterface.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/Requirements.h"
namespace movetk::simplification {

template <class GeometryTraits, class Norm>
struct FindFarthest {
	template <class InputIterator,
	          typename = utils::requires_random_access_iterator<InputIterator>,
	          typename = utils::requires_movetk_point<GeometryTraits, typename InputIterator::value_type>>
	InputIterator operator()(typename GeometryTraits::MovetkSegment& segment,
	                         InputIterator first,
	                         InputIterator beyond,
	                         typename GeometryTraits::NT& dist) {
		movetk::metric::ComputeSquaredDistance<GeometryTraits, Norm> squared_distance;
		Norm norm;
		InputIterator it = first + 1;
		InputIterator FarthestVertex = it;
		typename GeometryTraits::MovetkVector v = *it - segment[1];
		if (norm(v) < MOVETK_EPS) {
			dist = 0;
			return FarthestVertex;
		}
		dist = squared_distance(*it, segment);
		it++;
		while (it != (beyond - 1)) {
			typename GeometryTraits::NT CurrDist = squared_distance(*it, segment);
			if (CurrDist > dist) {
				FarthestVertex = it;
				dist = CurrDist;
			}
			it++;
		}
		return FarthestVertex;
	}
};

template <class GeometryTraits, class FindFarthest>
class DouglasPeucker {
	// based on http://doi.acm.org/10.1145/177424.178097
private:
	typedef typename GeometryTraits::NT NT;
	movetk::geom::MakeSegment<GeometryTraits> make_segment;
	FindFarthest farthest;
	NT eps;

public:
	DouglasPeucker(NT epsilon) { eps = epsilon * epsilon; }

	template <class InputIterator,
	          class OutputIterator,
	          typename = movetk::utils::requires_random_access_iterator<InputIterator>,
	          typename = movetk::utils::requires_output_iterator<OutputIterator>,
	          typename = movetk::utils::requires_movetk_point<GeometryTraits, typename InputIterator::value_type>,
	          typename = movetk::utils::requires_equality<typename InputIterator::value_type,
	                                                      typename OutputIterator::value_type::value_type>>
	void operator()(InputIterator first, InputIterator beyond, OutputIterator result, size_t level = 0) {
		typename GeometryTraits::MovetkSegment segment = make_segment(*first, *(beyond - 1));
		NT distance = 0;
		level++;
		InputIterator FarthestVertex = farthest(segment, first, beyond, distance);
		if (distance > eps) {
			(*this)(first, (FarthestVertex + 1), result, level);
			(*this)(FarthestVertex, beyond, result, level);
		} else {
			*result = first;
		}
		if (level == 1)
			*result = beyond - 1;
	}
};

}  // namespace movetk::algo::simplification
#endif