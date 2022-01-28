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
// Created by onur on 12/3/18.
//

#ifndef MOVETK_HIGH_FREQUENCY_TRAJECTORYTRAITS_H
#define MOVETK_HIGH_FREQUENCY_TRAJECTORYTRAITS_H

namespace movetk::io {
/**
 * Packs the template parameters required for high freq traj generation (fed to HighFrequencyTrajectoryReader)
 * @tparam _TrajectoryTraits
 * @tparam _DateIdx  Index of the field that represents date
 * @tparam _LatIdx  Index of the field that represents latitude
 * @tparam _LonIdx  Index of the field that represents longitude
 */
template <class _TrajectoryTraits, int _DateIdx, int _LatIdx, int _LonIdx>
struct _HighFrequencyTrajectoryTraits {
	typedef _TrajectoryTraits TrajectoryTraits;
	constexpr static int DateIdx = _DateIdx;
	constexpr static int LatIdx = _LatIdx;
	constexpr static int LonIdx = _LonIdx;
};
}  // namespace movetk::io


#endif  // MOVETK_HIGH_FREQUENCY_TRAJECTORYTRAITS_H
