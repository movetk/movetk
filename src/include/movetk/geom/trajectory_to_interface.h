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
 // Created by onur on 10/11/18.
 //

#ifndef MOVETK_TRAJECTORY_TO_INTERFACE_H
#define MOVETK_TRAJECTORY_TO_INTERFACE_H

#include <GeographicLib/Geocentric.hpp>
#include <iostream>
#include <optional>
#include <vector>

#include "movetk/geo/geo.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/utils/Requirements.h"

/**
 * Contains adapter functions to make Trajectory class work with agnostic geometry interface.
 */
namespace movetk::geom {

    // Requires GeometryTraits::MovetkPoint has dimension 2
    template <class GeometryTraits,
        utils::InputIterator<typename GeometryTraits::NT> LatIterator,
        utils::InputIterator<typename GeometryTraits::NT> LonIterator,
        utils::OutputIterator<typename GeometryTraits::MovetkPoint> OutputIterator,
        class LocalCoordRef>
        OutputIterator to_projected_polyline(geom::MakePoint<GeometryTraits>& make_point,
            LatIterator lat_start,
            LatIterator lat_beyond,
            LonIterator lon_start,
            OutputIterator polyline_first,
            LocalCoordRef& ref) {
        while (lat_start != lat_beyond) {
            auto projected_point = ref.project(*lat_start++, *lon_start++);
            *polyline_first++ = make_point(std::cbegin(projected_point), std::cend(projected_point));
        }
        return polyline_first;
    }

    template <class GeometryTraits,
        utils::InputIterator<typename GeometryTraits::NT> LatIterator,
        utils::InputIterator<typename GeometryTraits::NT> LonIterator,
        utils::OutputIterator<typename GeometryTraits::MovetkPoint> OutputIterator>
        OutputIterator to_projected_polyline(geom::MakePoint<GeometryTraits>& make_point,
            LatIterator lat_start,
            LatIterator lat_beyond,
            LonIterator lon_start,
            OutputIterator polyline_first) {
        movetk::geo::LocalCoordinateReference<typename GeometryTraits::NT> ref(*lat_start, *lon_start);
        return to_projected_polyline(make_point, lat_start, lat_beyond, lon_start, polyline_first, ref);
    }

    /**
     * Creates a vector of points from the given ranges of latitude and longitude, and converts them to
     * geocentric coordinates.
     * @tparam GeometryTraits The kernel
     * @tparam LatIterator Iterator type for the latitude range
     * @tparam LonIterator Iterator type for the longitude range
     * @tparam OutputIterator Output iterator to write result to
     * @param make_point Point creation algorithm
     * @param lat_start Start of the latitude range
     * @param lat_beyond End of the latitude range
     * @param lon_start Start of the longitude range
     * The longitude range is assumed to be at least as large as the latitude range
     * @param polyline_first Output iterator to write to
     * @return The used output iterator
     */
    template <class GeometryTraits,
        utils::InputIterator<typename GeometryTraits::NT> LatIterator,
        utils::InputIterator<typename GeometryTraits::NT> LonIterator,
        utils::OutputIterator<typename GeometryTraits::MovetkPoint> OutputIterator>
        OutputIterator to_geocentered_polyline(geom::MakePoint<GeometryTraits>& make_point,
            LatIterator lat_start,
            LatIterator lat_beyond,
            LonIterator lon_start,
            OutputIterator polyline_first) {
        const GeographicLib::Geocentric& earth = GeographicLib::Geocentric::WGS84();
        while (lat_start != lat_beyond) {
            *polyline_first++ = movetk::geo::to_geocentric_coordinates(make_point, earth, *lat_start++, *lon_start++);
        }
        return polyline_first;
    }


}  // namespace movetk::geom

#endif  // MOVETK_TRAJECTORY_TO_INTERFACE_H
