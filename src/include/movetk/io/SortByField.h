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
// Created by onur on 9/23/18.
//

#ifndef MOVETK_SORTBYFIELD_H
#define MOVETK_SORTBYFIELD_H

#include <string>
#include <tuple>

namespace movetk::io {
/**
 * @brief Custom sorting predicate for probes, using a field in the probe
 * @tparam ProbePoint The type of the probe
 */
template <int FieldIndex, class ProbePoint, bool ascending = true>
class SortByField {
public:
	bool operator()(const ProbePoint& a, const ProbePoint& b) const {
		if constexpr (ascending) {
			return std::get<FieldIndex>(a) < std::get<FieldIndex>(b);
		} else {
			return std::get<FieldIndex>(a) > std::get<FieldIndex>(b);
		}
	}
};
}  // namespace movetk::io
#endif  // MOVETK_SORTBYFIELD_H
