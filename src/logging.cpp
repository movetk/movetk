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
#include "movetk/logging.h"

namespace logging = boost::log;
namespace keywords = boost::log::keywords;

void init_logging(logging::trivial::severity_level level = logging::trivial::info)
{
    static const std::string COMMON_FMT("[%TimeStamp%][%Severity%]:  %Message%");

    logging::register_simple_formatter_factory<logging::trivial::severity_level, char>("Severity");

    // Output message to console
    logging::add_console_log(
            std::cout,
            boost::log::keywords::format = COMMON_FMT,
            boost::log::keywords::auto_flush = true
    );

    logging::add_file_log
            (
                    keywords::file_name = "movetk.log",
                    keywords::format = COMMON_FMT
            );

    // Only output message with INFO or higher severity in Release
//#ifndef _DEBUG
    logging::core::get()->set_filter
            (
                    logging::trivial::severity >= level
//                    logging::trivial::severity >= logging::trivial::trace
//                    logging::trivial::severity >= logging::trivial::info
            );
//#endif

    logging::add_common_attributes();
}


void display(
        std::string name,
        std::chrono::time_point<std::chrono::high_resolution_clock> start,
        std::chrono::time_point<std::chrono::high_resolution_clock> end
)
{
    BOOST_LOG_TRIVIAL(info) << name << " " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()
                            << " ms";
}
