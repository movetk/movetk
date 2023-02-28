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
// Other Interpolators added by Aniket Mitra (aniket.mitra@here.com)
//

#ifndef MOVETK_INTERPOLATION_H
#define MOVETK_INTERPOLATION_H

#include <algorithm>
#include <numeric>
#include <random>

#include "movetk/AlgorithmTraits.h"
#include "movetk/geo/geo.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/interpolation/KinematicInterpolator.h"
#include "movetk/interpolation/LinearInterpolator.h"
#include "movetk/interpolation/RandomInterpolator.h"
#include "movetk/utils/TrajectoryUtils.h"

namespace movetk::interpolation {

/**
 * @brief Convenience traits class for the interpolation algorithms
 * @tparam _GeometryTraits The kernel to use
 * @tparam _GeoProjection The geoprojection class to use
 * @tparam _ProbeTraits Traits of the probes to process
 * @tparam _Norm The norm to use for computing distances
 */
template <class _GeometryTraits, class _GeoProjection, class _ProbeTraits, class _Norm>
struct InterpolationTraits {
	using GeometryTraits = _GeometryTraits;
	using GeoProjection = _GeoProjection;
	using ProbeTraits = _ProbeTraits;
	using ProbePoint = typename ProbeTraits::ProbePoint;
	using MovetkPoint = typename GeometryTraits::MovetkPoint;
	using MovetkVector = typename GeometryTraits::MovetkVector;
	using NT = typename GeometryTraits::NT;
	using Norm = _Norm;
	using MinimumBoundingRectangle =
	    geom::MBR<GeometryTraits, Norm>;
};

template <class TrajectoryPoint, int LatIdx, int LonIdx, int SpeedIdx = -1, int HeadingIdx = -1>
class DummyOffsetInterpolator_2 {
public:
	/**
	 * Return an interpolated ProbePoint between p1 and p2 at the given offset from p1.
	 * This dummy interpolator only interpolates lat and lon and uses the speed and heading from p2.
	 */
	TrajectoryPoint operator()(const TrajectoryPoint& p1, const TrajectoryPoint& p2, double offset) {
		double lat1 = std::get<LatIdx>(p1), lon1 = std::get<LonIdx>(p1), lat2 = std::get<LatIdx>(p2),
		       lon2 = std::get<LonIdx>(p2);
		double bearing = movetk::geo::bearing_exact(lat1, lon1, lat2, lon2);

		double lat, lon;
		movetk::geo::destination_by_bearing_exact(lat1, lon1, bearing, offset, lat, lon);

		TrajectoryPoint p = p2;
		std::get<LatIdx>(p) = lat;
		std::get<LonIdx>(p) = lon;
		return p;
	}
};

}  // namespace movetk::interpolation

#endif  // MOVETK_INTERPOLATION_H
