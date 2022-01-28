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
// Created by onur on 10/30/18.
//

#ifndef MOVETK_SPLITBYDISTANCEFFERENCETHRESHOLD_H
#define MOVETK_SPLITBYDISTANCEFFERENCETHRESHOLD_H

#include <cmath>
#include <functional>
#include <optional>
#include <string>
#include <tuple>

namespace movetk::io {
template <int LatFieldIndex, int LonFieldIndex, class ProbePoint>
class SplitByDistanceThreshold {
public:
	using field_type = typename std::tuple_element<LatFieldIndex, ProbePoint>::type;

	explicit SplitByDistanceThreshold(float threshold, std::function<double(float, float, float, float)> distance)
	    : _threshold(threshold)
	    , _distance(std::move(distance)) {}

	bool operator()(const ProbePoint& p) {
		//        if (!prev_coord) {
		//            prev_coord = std::make_tuple(std::get<LatFieldIndex>(p), std::get<LonFieldIndex>(p));
		if (!prev_lat_value) {
			prev_lat_value = std::get<LatFieldIndex>(p);
			prev_lon_value = std::get<LonFieldIndex>(p);
			return true;
		} else {
			//            auto curr_coord = std::make_tuple(std::get<LatFieldIndex>(p), std::get<LonFieldIndex>(p));
			field_type curr_lat_value = std::get<LatFieldIndex>(p);
			field_type curr_lon_value = std::get<LonFieldIndex>(p);
			bool above_threshold = _distance(curr_lat_value, curr_lon_value, *prev_lat_value, *prev_lon_value) > _threshold;
			//            bool above_threshold = _distance(curr_coord, prev_coord) > _threshold;
			//            bool above_threshold = std::abs(curr_field_value - prev_field_value.value()) > _threshold;
			//            prev_coord = curr_coord;
			prev_lat_value = curr_lat_value;
			prev_lon_value = curr_lon_value;
			return above_threshold;
		}
	}

private:
	//    std::optional<std::tuple<field_type, field_type>> prev_coord;
	std::optional<field_type> prev_lat_value;
	std::optional<field_type> prev_lon_value;
	float _threshold;
	std::function<double(float, float, float, float)> _distance;
};
}  // namespace movetk::io
#endif  // MOVETK_SPLITBYDISTANCEFFERENCETHRESHOLD_H
