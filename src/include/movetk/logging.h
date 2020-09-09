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
// Created by onur on 10/4/18.
//

#ifndef MOVETK_LOGGING_H
#define MOVETK_LOGGING_H

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <chrono>

namespace logging = boost::log;
namespace keywords = boost::log::keywords;

void init_logging(logging::trivial::severity_level level);

void display(
        std::string name,
        std::chrono::time_point<std::chrono::high_resolution_clock> start,
        std::chrono::time_point<std::chrono::high_resolution_clock> end
);

#endif //MOVETK_LOGGING_H
