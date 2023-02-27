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
// Created by Wouter Jongeling (wouter.jongeling@gmail.com)
//

#ifndef MOVETK_GEOLIFEPROBETRAITS_H
#define MOVETK_GEOLIFEPROBETRAITS_H

#include "movetk/io/CategoricalField.h"
#include "movetk/io/ParseDate.h"
#include "movetk/io/ProbeTraits.h"
#include "movetk/io/csv/csv.h"

namespace geolife {
namespace c2d {
namespace raw {

enum InputColumns { _LAT, _LON, _ALTITUDE, _DATETIME, _TRAJID, _MODEID };

// Fields of interest
enum ProbeColumns { LAT, LON, ALTITUDE, DATETIME, TRAJID, MODEID };

class ProbeParseDate : public movetk::io::ParseDate {
public:
	explicit ProbeParseDate(std::time_t ts = 0, std::string date_format = "%Y-%m-%d %H:%M:%S")
	    : ParseDate(ts, std::move(date_format)) {}
};

class ProviderCategoricalField : public movetk::io::CategoricalField<std::string, ProviderCategoricalField> {};

typedef movetk::io::csv::csv<std::tuple<double, double, double, std::time_t, std::string, std::string>,
                             _LAT,
                             _LON,
                             _ALTITUDE,
                             _DATETIME,
                             _TRAJID,
                             _MODEID>
    ProbeCsv;

using ProbePoint = typename ProbeCsv::value_type;

using ProbeTraits = movetk::io::_ProbeTraits<ProbeColumns, ProbeCsv, ProbePoint>;

}  // namespace raw
}  // namespace c2d
}  // namespace geolife

#endif  // MOVETK_GEOLIFEPROBETRAITS_H