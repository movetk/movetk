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

    using Row = std::tuple<std::string, int, float>;
    Row row0{"abc", 5, 33.13};
    Row row1{"ghi", 4, 11.30};
    Row row2{"aaa", 3, 21.20};
    std::vector<Row> input = {row0, row1, row2};

    std::vector<std::string> col0{"abc", "ghi", "aaa"};
    std::vector<int> col1{5, 4, 3};
    std::vector<float> col2{33.13, 11.30, 21.20};

    std::tuple<std::vector<std::string>,
               std::vector<int>, std::vector<float>>
        expected = {col0, col1, col2};

    auto output = Transpose(input)();
    //    auto output = r2call.convert_row_to_column<0, 1,2>();

    //    auto output = convert_rows_to_cols(input);
    //    auto output = expected;

    //    Row2Col<0> r2c0;
    //    auto output0 = r2c0.convert_row_to_column(input);
    //    Row2Col<1> r2c1;
    //    auto output1 = r2c1.convert_row_to_column(input);
    //    Row2Col<2> r2c2;
    //    auto output2 = r2c2.convert_row_to_column(input);
    //    auto output = std::make_tuple(output0, output1, output2);

    std::cout << "col0: ";
    auto out0 = std::get<0>(output);
    for (auto x : out0)
    {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    std::cout << "col1: ";
    auto out1 = std::get<1>(output);
    for (auto x : out1)
    {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    std::cout << "col2: ";
    auto out2 = std::get<2>(output);
    for (auto x : out2)
    {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    REQUIRE(col0 == out0);
    REQUIRE(col1 == out1);
    REQUIRE(col2 == out2);
}
