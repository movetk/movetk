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
using namespace Catch::literals;  // 2.1_a : approximately 2.1

#include <iostream>
#include <cmath>

#include "movetk/geo/geo.h"

TEST_CASE( "Euclidean distance", "[euclideandistance]")
{
    using Point = std::array<double, 2>;
    Point p1 = {0.1, 0.1};
    Point p2 = {1.1, 0.1};
    auto d = euclidean_distance(p1, p2);
    std::cout << "generic eucl. dist: " << d << '\n';
    REQUIRE( d == Approx(1.0) );
}

TEST_CASE( "Euclidean distance 3d", "[euclideandistance3d]")
{
    using Point = std::array<double, 3>;
    Point p1 = {186393.016, -4740913.5, 4248293};
    Point p2 = {186378.344, -4740926, 4248280};
    auto d = euclidean_distance(p1, p2);
    std::cout << "generic eucl. dist: " << d << '\n';
    REQUIRE( d == Approx(23.249) );
}

TEST_CASE( "Distance between coordinates", "[geodistance]" )
{
    // American football half pitch (50 yds = 45.72m)
    // https://her.is/2zylpbe
    double lat0 = 37.77818097, lon0 = -122.49134085,
           lat1 = 37.77859013, lon1 = -122.49136901;

    double d = distance_exact(lat0, lon0, lat1, lon1);
    std::cout << "dist: " << d << '\n';

    REQUIRE( d==Approx(45.72).epsilon(0.01) );
}

TEST_CASE( "Bearing between coordinates", "[geobearing]" )
{
    double lat0 = 33.439361, lon0 = -112.084793, // bottom-left
           lat1 = 33.457393, lon1 = -112.063282; // top-right (2kmx2km)

    double bearing = bearing_exact(lat0, lon0, lat1, lon1);
    std::cout << "bearing: " << bearing << '\n';

    REQUIRE( bearing==Approx(45.).epsilon(0.01) );
}

TEST_CASE( "Projection to local coordinates", "[localprojection]" )
{
    // American football half pitch (50 yds = 45.72m)
    // https://her.is/2zylpbe
    double lat0 = 37.77818097, lon0 = -122.49134085,
           lat1 = 37.77859013, lon1 = -122.49136901;
    double expected_dist = distance_exact(lat0, lon0, lat1, lon1);

    LocalCoordinateReference<double> ref(lat0, lon0);
    auto xy = ref.project(lat1, lon1);

    double dist = sqrt(xy[0] * xy[0] + xy[1] * xy[1]);
    std::cout << "Projected x=" << xy[0] << " y=" << xy[1] << " dist=" << dist << '\n';

    REQUIRE( dist == Approx(expected_dist).epsilon(0.01) );

    auto latlon = ref.inverse(xy[0], xy[1]);
    std::cout << "Inverse projected lat=" << latlon[0] << " lon=" << latlon[1] << '\n';

    REQUIRE( latlon[0] == Approx(lat1).epsilon(.001) );
    REQUIRE( latlon[1] == Approx(lon1).epsilon(.001) );
}

TEST_CASE( "projection_errors_by_bbox_size", "[projection_errors_by_bbox_size]" )
{
    double bbox_length[3] = {1000, 2000, 3000};
    for (auto length : bbox_length) {
        // Phoenix_downtown
        double lat0 = 33.439361, lon0 = -112.084793; // bottom-left
        // lat1 = 33.457393, lon1 = -112.063282; // top-right (2kmx2km)
        double lat1, lon1;
        destination_exact(lat0, lon0, length, length, lat1, lon1);

        std::cout << "expected top-right: (" << lat1 << ", " << lon1 << ")\n";

        LocalCoordinateReference<double> ref(lat0, lon0);
        auto latlon = ref.inverse(length, length);
        std::cout << "top-right: (" << latlon[0] << ", " << latlon[1] << ")\n";

        auto dist_error = distance_exact(latlon[0], latlon[1], lat1, lon1);
        std::cout << "projection_errors_by_bbox_size (" << length << "): " << dist_error << '\n';
        REQUIRE(latlon[0]==Approx(lat1).epsilon(0.0001));
        REQUIRE(latlon[1]==Approx(lon1).epsilon(0.0001));
    }
}

TEST_CASE( "destination by bearing and distance", "[destination_by_bearing_and_distance]" )
{
    // Phoenix_downtown
    double lat0 = 33.439361, lon0 = -112.084793; // bottom-left
    double expected_lat1 = 33.457393, expected_lon1 = -112.063282; // top-right (2kmx2km)
    double lat1, lon1;
    destination_by_bearing_exact(lat0, lon0, 45, 2000*sqrt(2.), lat1, lon1);

    std::cout << "expected top-right: (" << expected_lat1 << ", " << expected_lon1 << ")\n";
    std::cout << "calculated top-right: (" << lat1 << ", " << lon1 << ")\n";

    REQUIRE(lat1==Approx(expected_lat1).epsilon(0.0001));
    REQUIRE(lon1==Approx(expected_lon1).epsilon(0.0001));
}

TEST_CASE( "Geocentric coordinates", "[geocentric]" )
{
    // double lat = 27.99, lon = 86.93, h = 8820; // Mt Everest
    // American football half pitch (50 yds = 45.72m)
    // https://her.is/2zylpbe
    double lat0 = 37.77818097, lon0 = -122.49134085,
           lat1 = 37.77859013, lon1 = -122.49136901;
    double expected_dist = distance_exact(lat0, lon0, lat1, lon1);

    const GeographicLib::Geocentric& earth = GeographicLib::Geocentric::WGS84();
    auto xyz0 = to_geocentric_coordinates<double>(earth, lat0, lon0);

    SECTION( "Distance by geocentric coordinates" ) {
        auto xyz1 = to_geocentric_coordinates<double>(earth, lat1, lon1);

        auto dist = euclidean_distance_3d(xyz0[0], xyz0[1], xyz0[2], xyz1[0], xyz1[1], xyz1[2]);
        std::cout << "dist by geocentric: " << dist << std::endl;

        REQUIRE( dist == Approx(expected_dist).epsilon(0.01));
    }

    SECTION( "Forward and reverse" ) {

        auto latlonh0 = from_geocentric_coordinates(earth, xyz0[0], xyz0[1], xyz0[2]);

        REQUIRE( latlonh0[0] == Approx(lat0).epsilon(0.000001));
        REQUIRE( latlonh0[1] == Approx(lon0).epsilon(0.000001));
        REQUIRE( std::abs(latlonh0[2]) < 1e-6);
    }

}

    // TODO: project all probe points when read and add two new columns?
//    try {
//        // Geocentric earth(Constants::WGS84_a(), Constants::WGS84_f());
//        // Alternatively:
//        const Geocentric& earth = Geocentric::WGS84();
////        const double lat0 = 48 + 50/60.0, lon0 = 2 + 20/60.0; // Paris
////        const double lat0 = 51.0, lon0 = 1.9;
//        const double lat0 = 50.9001, lon0 = 1.8001;
//        {
//            const Geodesic& geod = Geodesic::WGS84();
//            const double lat1 = 50.9, lon1 = 1.8;
//            double s12;
//            geod.Inverse(lat0, lon0, lat1, lon1, s12);
//            std::cout << s12 << '\n'; // 13.162313 meters
//        }
//        LocalCartesian proj(lat0, lon0, 0, earth);
//        {
//            // Sample forward calculation
//            double lat = 50.9, lon = 1.8, h = 0; // Calais
//            double x, y, z;
//            proj.Forward(lat, lon, h, x, y, z);
//            std::cout << x << " " << y << " " << z << "\n";
//        }
//        {
//            // Sample reverse calculation
//            double x = -38e3, y = 230e3, z = -4e3;
//            double lat, lon, h;
//            proj.Reverse(x, y, z, lat, lon, h);
//            std::cout << lat << " " << lon << " " << h << "\n";
//        }
//    }
//    catch (const std::exception& e) {
//        std::cerr << "Caught exception: " << e.what() << "\n";
////        return 1;
//    }
//
//    std::cout << std::endl;
//
////    REQUIRE( os.str() == "Provider1" );
