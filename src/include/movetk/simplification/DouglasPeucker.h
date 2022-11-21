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
        template <utils::RandomAccessIterator<typename GeometryTraits::MovetkPoint> InputIterator>
        InputIterator operator()(const typename GeometryTraits::MovetkSegment& segment,
            InputIterator first,
            InputIterator beyond,
            typename GeometryTraits::NT& dist) const {
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
            for (; it != (beyond - 1); ++it) {
                const auto curr_dist = squared_distance(*it, segment);
                if (curr_dist > dist) {
                    FarthestVertex = it;
                    dist = curr_dist;
                }
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

        template <utils::RandomAccessIterator<typename GeometryTraits::MovetkPoint> InputIterator,
            utils::OutputIterator<InputIterator> OutputIterator>
            void operator()(InputIterator first, InputIterator beyond, OutputIterator result, size_t level = 0) {
            typename GeometryTraits::MovetkSegment segment = make_segment(*first, *(beyond - 1));
            NT distance = 0;
            level++;
            InputIterator FarthestVertex = farthest(segment, first, beyond, distance);
            if (distance > eps) {
                (*this)(first, (FarthestVertex + 1), result, level);
                (*this)(FarthestVertex, beyond, result, level);
            }
            else {
                *result = first;
            }
            if (level == 1)
                *result = beyond - 1;
        }
    };

}  // namespace movetk::algo::simplification
#endif