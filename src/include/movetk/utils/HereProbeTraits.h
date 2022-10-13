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

#ifndef MOVETK_HEREPROBETRAITS_H
#define MOVETK_HEREPROBETRAITS_H

#include "movetk/io/csv/ParseDate.h"
#include "movetk/io/csv/CategoricalField.h"
#include "movetk/io/csv/csv.h"
#include "movetk/io/ProbeTraits.h"

namespace here
{
    namespace c2d
    {
        namespace raw
        {

            enum InputColumns
            {
                _PROBE_ID,
                _SAMPLE_DATE,
                _LAT,
                _LON,
                _HEADING,
                _SPEED,
                _PROBE_DATA_PROVIDER
            };

            // Fields of interest: all
            enum ProbeColumns
            {
                PROBE_ID,
                SAMPLE_DATE,
                LAT,
                LON,
                HEADING,
                SPEED,
                PROBE_DATA_PROVIDER
            };

            class ProbeParseDate : public ParseDate
            {
            public:
                explicit ProbeParseDate(std::time_t ts = 0, std::string date_format = "%Y-%m-%d %H:%M:%S")
                    : ParseDate(ts, std::move(date_format)) {}
            };

            class ProviderCategoricalField : public CategoricalField<std::string, ProviderCategoricalField>
            {
            };

            typedef csv<std::tuple<std::string, ProbeParseDate, double, double, double, double, ProviderCategoricalField>,
                        _PROBE_ID, _SAMPLE_DATE, _LAT, _LON, _HEADING, _SPEED, _PROBE_DATA_PROVIDER>
                ProbeCsv;

            typedef typename ProbeCsv::value_type ProbePoint;

            typedef _ProbeTraits<ProbeColumns, ProbeParseDate, ProbeCsv, ProbePoint> ProbeTraits;

        } // namespace raw
    }     // namespace c2d
} // namespace here

#endif //MOVETK_HEREPROBETRAITS_H
