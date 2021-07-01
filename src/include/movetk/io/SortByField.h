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
using std::string;
#include <tuple>


// sort using a custom function object
template <int FieldIndex, class ProbePoint, bool asc = true>
class SortByField {

public:

    template <bool enable=asc>
    bool operator()(ProbePoint a, ProbePoint b, typename std::enable_if<enable>::type* = 0) const
    {
        return std::get<FieldIndex>(a) < std::get<FieldIndex>(b);
    }

    template <bool enable=asc>
    bool operator()(ProbePoint a, ProbePoint b, typename std::enable_if<!enable>::type* = 0) const
    {
        return !(std::get<FieldIndex>(a) < std::get<FieldIndex>(b));
    }
};

#endif //MOVETK_SORTBYFIELD_H
