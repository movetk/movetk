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

#ifndef MOVETK_TRAJECTORYTRAITS_H
#define MOVETK_TRAJECTORYTRAITS_H

namespace movetk::io {
enum class StorageScheme { tabular, columnar };

/**
 *
 * @tparam _ProbeTraits
 * @tparam _SortedSessions  All points of a trajectory are already sorted by the field at index SortByFieldIdx
 * @tparam _SplitByFieldIdx  The field at this index specifies the trajectory (session)
 * @tparam _SortByFieldIdx  The trajectory is to be sorted in ascending order by the values of this field (typically
 * time)
 * @tparam _trajectory_type  Specialization of the Trajectory class type with specific field types of trajectory points
 */
template <class _ProbeTraits,
          //        bool _SortedSessions,
          int _SplitByFieldIdx,
          int _SortByFieldIdx,
          class _trajectory_type>
struct _TrajectoryTraits {
	using ProbeTraits = _ProbeTraits;
	//    constexpr static bool SortedSessions = _SortedSessions;
	static constexpr int SplitByFieldIdx = _SplitByFieldIdx;
	static constexpr int SortByFieldIdx = _SortByFieldIdx;
	using trajectory_type = _trajectory_type;
};

}  // namespace movetk::io
#endif  // MOVETK_TRAJECTORYTRAITS_H
