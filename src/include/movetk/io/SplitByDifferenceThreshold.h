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

#ifndef MOVETK_SPLITBYDIFFERENCETHRESHOLD_H
#define MOVETK_SPLITBYDIFFERENCETHRESHOLD_H

#include <string>
using std::string;
#include <tuple>
#include <optional>
#include <cmath>

template <int FieldIndex, class ProbePoint>
class SplitByDifferenceThreshold {

public:
    using field_type = typename std::tuple_element<FieldIndex, ProbePoint>::type;

    explicit SplitByDifferenceThreshold(float threshold) : _threshold(threshold) {}

    bool operator()(const ProbePoint& p) {
        if (!prev_field_value) {
            prev_field_value = std::get<FieldIndex>(p);
            return true;
        }
        else {
            field_type curr_field_value = std::get<FieldIndex>(p);
            bool above_threshold = std::abs(curr_field_value - prev_field_value.value()) > _threshold;
            prev_field_value = curr_field_value;
            return above_threshold;
        }
    }

private:
    std::optional<field_type> prev_field_value;
    float _threshold;
};

#endif //MOVETK_SPLITBYDIFFERENCETHRESHOLD_H
