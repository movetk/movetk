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
// Created by onur on 10/12/18.
//

#ifndef MOVETK_HERETRAJECTORYTRAITS_H
#define MOVETK_HERETRAJECTORYTRAITS_H

#include "HereProbeTraits.h"
#include "movetk/ds/ColumnarTrajectory.h"
#include "movetk/ds/TabularTrajectory.h"
#include "movetk/io/TrajectoryTraits.h"

namespace here::c2d::raw {

typedef here::c2d::raw::ProbeTraits ProbeTraits;
constexpr static int SplitByFieldIdx = ProbeTraits::ProbeColumns::PROBE_ID;
constexpr static int SortByFieldIdx = ProbeTraits::ProbeColumns::SAMPLE_DATE;
using columnar_trajectory_type = movetk::ds::ColumnarTrajectory<std::string,
                                                                ProbeParseDate,
                                                                double,
                                                                double,
                                                                double,
                                                                double,
                                                                here::c2d::raw::ProviderCategoricalField>;
using tabular_trajectory_type = movetk::ds::TabularTrajectory<std::string,
                                                              ProbeParseDate,
                                                              double,
                                                              double,
                                                              double,
                                                              double,
                                                              here::c2d::raw::ProviderCategoricalField>;

using ColumnarTrajectoryTraits =
    movetk::io::_TrajectoryTraits<ProbeTraits, SplitByFieldIdx, SortByFieldIdx, columnar_trajectory_type>;
using TabularTrajectoryTraits =
    movetk::io::_TrajectoryTraits<ProbeTraits, SplitByFieldIdx, SortByFieldIdx, tabular_trajectory_type>;
}  // namespace here::c2d::raw

#endif  // MOVETK_HERETRAJECTORYTRAITS_H
