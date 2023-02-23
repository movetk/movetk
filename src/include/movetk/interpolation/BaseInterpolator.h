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
#ifndef MOVETK_INTERPOLATION_BASEINTERPOLATOR_H
#define MOVETK_INTERPOLATION_BASEINTERPOLATOR_H

/**
 * @brief Collection of interpolation algorithms for trajectories
*/
namespace movetk::interpolation {

/**
 * @brief Base interpolator
 * @tparam AlgorithmTag Tag for the algorithm used
 * @tparam InterpolationTraits Interpolation traits.
 * @tparam LatIdx Index in a tuple-like point that gives the latitude
 * @tparam LonIdx Index in a tuple-like point that gives the longitude
 * @tparam TsIdx Index in a tuple-like point that gives the timestamp
 * @tparam SpeedIdx Index in a tuple-like point that gives the speed
 * @tparam HeadingIdx Index in a tuple-like point that gives the heading
 */
template <class AlgorithmTag,
          class InterpolationTraits,
          int LatIdx,
          int LonIdx,
          int TsIdx,
          int SpeedIdx = -1,
          int HeadingIdx = -1>
class Interpolator {};
}  // namespace movetk::interpolation
#endif