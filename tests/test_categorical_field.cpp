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
#include <sstream>
#include <string>
#include <sstream>
using std::string;

#include "movetk/io/CategoricalField.h"

class CategoricalStringField : public movetk::io::CategoricalField<std::string, CategoricalStringField> {};


TEST_CASE( "Categorical field", "[categoricalfield]" ) {

    CategoricalStringField c1;
    CategoricalStringField c2;
    CategoricalStringField c3;

    std::istringstream is1("Provider1");
    std::istringstream is2("LongProviderName2");
    std::istringstream is3("Provider1");

    is1 >> c1;
    is2 >> c2;
    is3 >> c3;

    REQUIRE( c1.idx() == 0 );
    REQUIRE( c2.idx() == 1 );
    REQUIRE( c3.idx() == 0 );
    REQUIRE( sizeof(c1) == sizeof(c2) );
    REQUIRE( sizeof(c2) == sizeof(c3) );

    std::ostringstream os;
    os << c3;
    REQUIRE( os.str() == "Provider1" );
}
