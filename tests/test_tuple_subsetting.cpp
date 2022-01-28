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
#include <string>
using std::string;
#include <tuple>

#include "movetk/io/TuplePrinter.h"

template<int... indices>
struct pack { };

//struct fields_of_interest : public pack<1, 3> {};
typedef pack<1, 3> fields_of_interest;

template <class... fields, int... idx>
auto subset_tuple(std::tuple<fields...> t, pack<idx...>)
-> std::tuple<typename std::tuple_element<idx, std::tuple<fields...>>::type...>
{
    return std::make_tuple(std::get<idx>(t)...);
}

TEST_CASE( "Tuple subsetting", "[tuplesubsetting]" ) {

    using Row = std::tuple<std::string, std::string, int, float, double>;
    Row row0 {"abc", "def", 5, 1.2, 33.13};
//    Row row1 {"ghi", "jkl", 4, 2.5, 11.30};
//    Row row2 {"aaa", "bbb", 3, 3.4, 21.20};
//    Row row3 {"ddd", "ttt", 2, 4.3, 12.10};
//    Row row4 {"eee", "vvv", 1, 5.4, 44.14};
//    std::array<Row, 5> expected = {row0, row1, row2, row3, row4};

//    constexpr static int[] fields_of_interest = {1, 3};
    fields_of_interest foi;
    auto row0_ = subset_tuple(row0, foi);
//    auto row0_ = subset_tuple<1, 3>(row0);
    movetk::io::print_tuple(std::cout, row0_);
    std::cout << std::endl;

//    REQUIRE( os.str() == "Provider1" );
}
