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
using std::tuple;
#include <vector>
using std::vector;
#include <ogr_geometry.h>
#include "cpl_string.h"

#include "movetk/TabularTrajectory.h"
#include "movetk/TuplePrinter.h"
#include "movetk/geom/trajectory_gdal.h"


TEST_CASE( "Trajectory GDAL", "[trajectory-gdal]" )
{
    using ProbePoint = std::tuple<string, int, float>;
    ProbePoint p1 = {"abc", 1, 5.4};
    ProbePoint p2 = {"def", 2, 4.5};
    ProbePoint p3 = {"ghi", 3, 3.2};
    std::vector<ProbePoint> data = {p1, p2, p3};

    TabularTrajectory<string, int, float> t { data };

    std::cout << "trj: ";
    for (auto x: t.get<2>()) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    auto mp = to_multipoint(t.begin<1>(), t.end<1>(), t.begin<2>());
    char* wktStr;
    mp->exportToWkt(&wktStr);
    std::cout << wktStr << std::endl;
    CPLFree(wktStr);
    mp->empty();
}

TEST_CASE( "Intersection", "[intersection]" )
{

    SECTION("Has no intersection") {
        const char* polyWkt1 = "POLYGON ((51.444949111891695 5.483971066474908, 51.444935738 5.484652348, 51.444348957 5.484840102, 51.444248651 5.484089084, 51.444949111891695 5.483971066474908))";
        const char* mpWkt1 = "MULTIPOINT (51.4450444 5.483836956, 51.445071148 5.484236605, 51.445027683 5.484309025, 51.445015981 5.484381444, 51.44502434 5.484561152, 51.444970844 5.484571881, 51.444943261 5.48457054)";

        OGRPolygon p1;
        p1.importFromWkt((char**) &polyWkt1);
        OGRMultiPoint mp1;
        mp1.importFromWkt((char**) &mpWkt1);

        REQUIRE( !intersects(&p1, &mp1) );
    }

    SECTION("Has intersection") {
        const char* polyWkt2 = "POLYGON ((51.444949111891695 5.483971066474908, 51.444935738 5.484652348, 51.444348957 5.484840102, 51.444248651 5.484089084, 51.444949111891695 5.483971066474908))";
        const char* mpWkt2 = "MULTIPOINT (51.4450444 5.483836956, 51.445071148 5.484236605, 51.445027683 5.484309025, 51.445015981 5.484381444, 51.44502434 5.484561152, 51.444970844 5.484571881, 51.44492988698722 5.484570540189736)";

        OGRPolygon p2;
        p2.importFromWkt((char**) &polyWkt2);
        OGRMultiPoint mp2;
        mp2.importFromWkt((char**) &mpWkt2);

//        char* p2wkt;
//        p2.exportToWkt(&p2wkt);
//        char* mp2wkt;
//        mp2.exportToWkt(&mp2wkt);
//        std::cout << p2wkt << std::endl;
//        std::cout << mp2wkt << std::endl;
        REQUIRE( intersects(&p2, &mp2) );
    }
}

