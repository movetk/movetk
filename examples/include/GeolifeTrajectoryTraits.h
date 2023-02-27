/*
 * Copyright (C) 2018-2020
 * HERE Europe B.V.
 * Utrecht University (The Netherlands).
 * TU/e (The Netherlands).
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
// Created by Mitra, Aniket on 2019-02-21 (aniket.mitra@here.com),
// Wouter Jongeling (wouter.jongeling@gmail.com)
//


#ifndef MOVETK_GEOLIFETRAJECTORYTRAITS_H
#define MOVETK_GEOLIFETRAJECTORYTRAITS_H

#include "GeolifeProbeTraits.h"
#include "movetk/ds/ColumnarTrajectory.h"
#include "movetk/ds/TabularTrajectory.h"
#include "movetk/io/TrajectoryTraits.h"

namespace geolife::c2d::raw {

typedef geolife::c2d::raw::ProbeTraits ProbeTraits;
constexpr static int SplitByFieldIdx = ProbeTraits::ProbeColumns::TRAJID;
constexpr static int SortByFieldIdx = ProbeTraits::ProbeColumns::DATETIME;
using tabular_trajectory_type =
    movetk::ds::TabularTrajectory<double, double, double, std::time_t, std::string, std::string>;
using TabularTrajectoryTraits =
    movetk::io::_TrajectoryTraits<ProbeTraits, SplitByFieldIdx, SortByFieldIdx, tabular_trajectory_type>;
}  // namespace geolife::c2d::raw

#endif  // MOVETK_GEOLIFETRAJECTORYTRAITS_H
