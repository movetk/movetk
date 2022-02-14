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

#include "catch2/catch.hpp"

#include <iostream>
#include "movetk/utils/Transpose.h"

TEST_CASE("convert row to col", "[row2col]")
{

    using Row = std::tuple<std::string, int, double>;
    Row row0{ "abc", 5, 33.13 };
    Row row1{ "ghi", 4, 11.30 };
    Row row2{ "aaa", 3, 21.20 };
    std::vector<Row> input = { row0, row1, row2 };

    std::vector<std::string> col0{ "abc", "ghi", "aaa" };
    std::vector<int> col1{ 5, 4, 3 };
    std::vector<double> col2{ 33.13, 11.30, 21.20 };

    auto output = movetk::utils::Transpose(input)();

    auto [out0, out1, out2] = output;

    REQUIRE(col0 == out0);
    REQUIRE(col1 == out1);
    REQUIRE(col2 == out2);
}
