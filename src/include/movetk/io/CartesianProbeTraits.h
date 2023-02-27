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
// Created by Mitra, Aniket on 2019-07-22.
//

#ifndef MOVETK_CARTESIANPROBETRAITS_H
#define MOVETK_CARTESIANPROBETRAITS_H

#include "ProbeTraits.h"
#include "movetk/io/CategoricalField.h"
#include "movetk/io/ParseDate.h"

namespace movetk::io {
enum ProbeColumns { PROJECTED_COORDS, SAMPLE_DATE };

template <class GeometryTraits>
using ProbePoint = typename std::tuple<typename GeometryTraits::MovetkPoint, std::size_t>;

class ProbeParseDate : public ParseDate {
public:
	explicit ProbeParseDate(std::time_t ts = 0, std::string date_format = "%Y-%m-%d %H:%M:%S")
	    : ParseDate(ts, std::move(date_format)) {}
};

template <class GeometryTraits>
using ProbeCsv = std::vector<ProbePoint<GeometryTraits>>;

template <class GeometryTraits>
using ProbeTraits = _ProbeTraits<ProbeColumns, ProbeCsv<GeometryTraits>, ProbePoint<GeometryTraits>>;

}  // namespace movetk::io
#endif  // MOVETK_CARTESIANPROBETRAITS_H
