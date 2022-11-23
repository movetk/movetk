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
        std::function<double(point_type&, point_type&)> distancefn =
            (double (*)(point_type&, point_type&)) & movetk::geo::euclidean_distance<point_type>;
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
            std::back_inserter(xyzs));
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
        std::function<double(point_type&, point_type&)> distancefn =
            (double (*)(point_type&, point_type&)) & movetk::geo::euclidean_distance<point_type>;
        std::vector<double> lengths;
        std::transform(start, beyond - 1, start + 1, std::back_inserter(lengths), distancefn);
        return lengths;
    }

    template <class GeometryTraits, utils::InputIterator<typename GeometryTraits::NT> LatIterator,
        utils::InputIterator<typename GeometryTraits::NT> LonIterator>
    std::vector<double> geopolyline_offsets_m(LatIterator lat_start, LatIterator lat_beyond, LonIterator lon_start) {
        using MovetkPoint = typename GeometryTraits::MovetkPoint;
        movetk::geom::MakePoint<GeometryTraits> make_point;
        std::vector<MovetkPoint> xyzs;
        movetk::geom::to_geocentered_polyline(make_point,
            lat_start,
            lat_beyond,
            lon_start,
            std::back_inserter(xyzs));

        auto lengths = polyline_lengths_m(std::begin(xyzs), std::end(xyzs));
        std::vector<double> offsets;
        offsets.reserve(1 + lengths.size());
        offsets.push_back(0);  // add origin offset as well
        std::partial_sum(std::begin(lengths), std::end(lengths), std::back_inserter(offsets));
        return offsets;
    }

    template <class GeometryTraits>
    struct is_monotone {
        /**
         * @brief Computes whether the angles between edges of the provided polyline are more or less than pi over 2.
         * We only consider the smallest of the two angles between consecutive edges and assign a result of true
         * when this angle greater than or equal to pi over 2 (90 degrees), false otherwise.
         * @param first Start of the polyline
         * @param beyond Exclusive end of the polyline
         * @param result Output iterator to write the results to.
        */
        template <utils::InputIterator<typename GeometryTraits::MovetkPoint> InputIterator, utils::OutputIterator<bool> OutputIterator>
        void operator()(InputIterator first, InputIterator beyond, OutputIterator result) {
            auto cit0 = first;
            if (cit0 == beyond) { return; }
            auto cit1 = std::next(first);
            if (cit1 == beyond) { return; }
            auto cit2 = std::next(first, 2);
            if (cit2 == beyond) { return; }
            *result = true;
            for (; cit2 != beyond; ++cit0, ++cit1, ++cit2) {
                typename GeometryTraits::MovetkVector u = *cit1 - *cit0;
                typename GeometryTraits::MovetkVector v = *cit2 - *cit1;
                typename GeometryTraits::NT uv = u * v; // Inner product
                if (uv < 0) {
                    *result = false;
                }
                else {
                    *result = true;
                }
            }
        }
    };

    /*template<class GeometryTraits, std::size_t>
    struct sort_by_X*/

}  // namespace movetk::algo

#endif  // MOVETK_POLYLINEUTILS_H
