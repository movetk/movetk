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

#ifndef MOVETK_SPLITBYFIELD_H
#define MOVETK_SPLITBYFIELD_H

#include <optional>
#include <string>
#include <tuple>

namespace movetk::io {
/**
 * @brief Predicate object for splitting by a field value
 * @tparam FieldIndex Index of the field value
 * @tparam ProbePoint The probe point type
 */
template <int FieldIndex, class ProbePoint>
class SplitByField {
public:
	using field_type = typename std::tuple_element<FieldIndex, ProbePoint>::type;

	bool operator()(const ProbePoint& p) {
		if (!prev_field_value) {
			prev_field_value = std::get<FieldIndex>(p);
			return true;
		} else {
			field_type curr_field_value = std::get<FieldIndex>(p);
			if (curr_field_value == prev_field_value)
				return false;
			else {
				prev_field_value = curr_field_value;
				return true;
			}
		}
	}

private:
	std::optional<field_type> prev_field_value;
};
}  // namespace movetk::io
#endif  // MOVETK_SPLITBYFIELD_H
