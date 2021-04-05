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
// Created by Mitra, Aniket on 09/01/2019.
//

#include <array>
#include "catch2/catch.hpp"
#if CGAL_BACKEND_ENABLED
#include "movetk/geom/CGALTraits.h"
#else
#include "movetk/geom/BoostGeometryTraits.h"
#endif

#include "movetk/geom/GeometryInterface.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "movetk/utils/Iterators.h"
#include "movetk/metric/Norm.h"


using namespace std;

#if CGAL_BACKEND_ENABLED
    //==============================
    // Define the Number Type
    // For the CGAL backend,
    // One can choose from the
    // following number types
    typedef long double NT;
    //typedef CGAL::Mpzf NT;
    //typedef CGAL::Gmpfr NT;
    //typedef CGAL::Gmpq NT;
    //==============================

    //==============================
    // Define the Dimensions
    const size_t dimensions = 2;
    //==============================

    //==============================
    // Define the Geometry Backend
    typedef movetk_support::CGALTraits<NT, dimensions> CGAL_GeometryBackend;
    //Using the Geometry Backend define the Movetk Geometry Kernel
    typedef movetk_core::MovetkGeometryKernel<
            typename CGAL_GeometryBackend::Wrapper_CGAL_Geometry> MovetkGeometryKernel;
    //==============================
#else
    //==============================
    // Define the Number Type
    // For the Boost Backend
    typedef long double NT;
    //==============================

    //==============================
    // Define the Dimensions
    // It is possible to choose
    // a higher dimension
    // Note: Boost Geometry Adapter supports geometry in two
    // dimensions only
    const static size_t dimensions = 2;
    //==============================

    //==============================
    // Define the Geometry Backend
    typedef movetk_support::BoostGeometryTraits<long double, dimensions> Boost_Geometry_Backend;
//Using the Geometry Backend define the Movetk Geometry Kernel
typedef movetk_core::MovetkGeometryKernel<typename Boost_Geometry_Backend::Wrapper_Boost_Geometry> MovetkGeometryKernel;
    //==============================
#endif

typedef movetk_support::FiniteNorm<MovetkGeometryKernel, 2> Norm;

TEST_CASE("Check if values are in sequence", "[test_is_sequence]") {
    vector<size_t> values1{1, 2, 3, 5, 7};
    vector<size_t> values2{1, 2, 3, 4, 5, 6, 7};
    REQUIRE(movetk_core::is_sequence(cbegin(values1), cend(values1))
            == false);

    REQUIRE(movetk_core::is_sequence(cbegin(values2), cend(values2))
            == true);
}

TEST_CASE("Find minimum non zero element", "[test_min_non_zero_element]") {
    vector<typename MovetkGeometryKernel::NT> values{3.1, 2.2, 0, 1};
    REQUIRE (*movetk_core::min_non_zero_element<MovetkGeometryKernel>(cbegin(values), cend(values))
             == 1);
}

TEST_CASE("Calculate tdiff", "[test_get_time_diffs]") {
    vector<size_t> ts{1460029938,1460029945,1460029954};
    vector<size_t> result;
    movetk_core::get_time_diffs(cbegin(ts), cend(ts), movetk_core::movetk_back_insert_iterator(result), 0);
    REQUIRE ( result[0] == 0);
    REQUIRE ( result[1] == 7);
    REQUIRE ( result[2] == 9);
}

TEST_CASE("Calculate Euclidean distance", "[test_calculate_distance]"){
    array<typename MovetkGeometryKernel::NT, dimensions> p;
    vector<typename MovetkGeometryKernel::MovetkPoint> points;
    vector<typename MovetkGeometryKernel::NT> result;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    p = {-8226858, 4993143};
    points.push_back( make_point( cbegin(p), cend(p) ) );
    p = {-8226854, 4993146};
    points.push_back( make_point( cbegin(p), cend(p) ) );
    p = {-8226855, 4993146};
    points.push_back( make_point( cbegin(p), cend(p) ) );
    movetk_core::get_distances<MovetkGeometryKernel>(cbegin(points), cend(points),
                                                     movetk_core::movetk_back_insert_iterator(result), 0);
    REQUIRE ( result[0] == 0);
    REQUIRE ( result[1] == 5);
    REQUIRE ( result[2] == 1);
}

TEST_CASE("Calculate Speed", "[test_get_speeds]"){
    vector<typename MovetkGeometryKernel::NT> distance{0, 5, 1};
    vector<size_t> tdiff {0,7,9};
    vector<typename MovetkGeometryKernel::NT> result;
    movetk_core::get_speeds<MovetkGeometryKernel>(cbegin(tdiff), cend(tdiff), cbegin(distance),
                                                  movetk_core::movetk_back_insert_iterator(result));
    REQUIRE ( result[0] == 0 );
    REQUIRE (abs(result[1] - 0.714286) < MOVETK_EPS);
    REQUIRE (abs(result[2] - 0.111111) < MOVETK_EPS);
}

TEST_CASE ("Calculate Heading", "[test_get_headings]"){
    typedef typename MovetkGeometryKernel::NT NT;
    vector<pair<NT, NT>> GeoCoords { make_pair(40.75348, -73.90441),
                                     make_pair(40.75348, -73.90439),
                                     make_pair(40.75352, -73.90439) };
    vector<typename MovetkGeometryKernel::MovetkPoint> projections;
    array<typename MovetkGeometryKernel::NT, dimensions> p;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    vector< NT > result;
    p = {-8227001.290000, 4976047.740000};
    projections.push_back( make_point( cbegin(p), cend(p) ) );
    p = {-8226999.060000, 4976047.740000};
    projections.push_back( make_point( cbegin(p), cend(p) ) );
    p = {-8226974.570000, 4976053.620000};
    projections.push_back( make_point( cbegin(p), cend(p) ) );
    movetk_core::get_headings<MovetkGeometryKernel>(cbegin(GeoCoords), cend(GeoCoords),
                                                    cbegin(projections),
                                                    movetk_core::movetk_back_insert_iterator(result));
    REQUIRE ( result[0] == 0 );
    REQUIRE (abs(result[1] - 90) < MOVETK_EPS);
    REQUIRE ( result[2] == 0 );
}

TEST_CASE("Calculate Velocity", "[test_get_velocities]"){
    vector<typename MovetkGeometryKernel::NT> speed{1, 1, 4};
    vector<typename MovetkGeometryKernel::NT> heading{46, 12, 297};
    vector<typename MovetkGeometryKernel::MovetkPoint> expected_velocities, actual_velocties;
    array<typename MovetkGeometryKernel::NT, dimensions> p;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    p = {0.694658, 0.719340};
    expected_velocities.push_back( make_point( cbegin(p), cend(p) ) );
    p = {0.978148, 0.207912};
    expected_velocities.push_back( make_point( cbegin(p), cend(p) ) );
    p = {1.815962, -3.564026};
    expected_velocities.push_back( make_point( cbegin(p), cend(p) ) );

    movetk_core::get_velocities<MovetkGeometryKernel>(cbegin(speed), cend(speed),
                                                      cbegin(heading),
                                                      movetk_core::movetk_back_insert_iterator(actual_velocties));
    MovetkGeometryKernel::MovetkVector vector = actual_velocties[0] - expected_velocities[0];
    REQUIRE ((vector * vector) < MOVETK_EPS);
}

TEST_CASE("Merge Intervals 1", "[test_merge_intervals_1]"){
    typedef typename MovetkGeometryKernel::NT NT;
    std::vector<std::pair<NT, NT>> items {{1,3},{6,8},{5,7},{2,4}};
    std::vector<std::pair<NT, NT>> expected {{5,8},{1,4}};
    auto beyond = movetk_core::merge_intervals<MovetkGeometryKernel>(std::begin(items),
                                                                     std::end(items));
    auto it = std::begin(items);
    REQUIRE(std::distance(it, beyond) == expected.size());
    std::cout<<"Merged Interval:\n";
    auto eit = std::begin(expected);
    while (it != beyond){
        std::cout<<it->first<<","<<it->second<<"\n";
        REQUIRE(abs(eit->first - it->first) < MOVETK_EPS);
        REQUIRE(abs(eit->second - it->second) < MOVETK_EPS);
        eit++;
        it++;
    }

}

TEST_CASE("Merge Intervals 2", "[test_merge_intervals_2]"){
    typedef typename MovetkGeometryKernel::NT NT;
    std::vector<std::pair<NT, NT>> items {{1,3}, {1,8}, {1,8}, {1,4}};
    std::vector<std::pair<NT, NT>> expected {{1,8}};
    auto beyond = movetk_core::merge_intervals<MovetkGeometryKernel>(std::begin(items),
                                                                     std::end(items));
    auto it = std::begin(items);
    REQUIRE(std::distance(it, beyond) == expected.size());
    std::cout<<"Merged Interval:\n";
    auto eit = std::begin(expected);
    while (it != beyond){
        std::cout<<it->first<<","<<it->second<<"\n";
        REQUIRE(abs(eit->first - it->first) < MOVETK_EPS);
        REQUIRE(abs(eit->second - it->second) < MOVETK_EPS);
        eit++;
        it++;
    }

}



TEST_CASE("Merge Intervals 3", "[test_merge_intervals_3]"){
    typedef typename MovetkGeometryKernel::NT NT;
    std::vector<std::pair<NT, NT>> items {{1,2}, {3,4}, {5,6}, {7,8}};
    std::vector<std::pair<NT, NT>> expected {{7,8}, {5,6}, {3,4}, {1,2}};
    auto beyond = movetk_core::merge_intervals<MovetkGeometryKernel>(std::begin(items),
                                                                     std::end(items));
    auto it = std::begin(items);
    REQUIRE(std::distance(it, beyond) == expected.size());
    std::cout<<"Merged Interval:\n";
    auto eit = std::begin(expected);
    while (it != beyond){
        std::cout<<it->first<<","<<it->second<<"\n";
        REQUIRE(abs(eit->first - it->first) < MOVETK_EPS);
        REQUIRE(abs(eit->second - it->second) < MOVETK_EPS);
        eit++;
        it++;
    }

}


TEST_CASE("Merge Intervals 4", "[test_merge_intervals_4]"){
    typedef typename MovetkGeometryKernel::NT NT;
    std::vector<std::pair<NT, NT>> items {{1,4}, {2,4}, {1,2}, {7,8}};
    std::vector<std::pair<NT, NT>> expected {{7,8}, {1,4}};
    auto beyond = movetk_core::merge_intervals<MovetkGeometryKernel>(std::begin(items),
                                                                     std::end(items));
    auto it = std::begin(items);
    REQUIRE(std::distance(it, beyond) == expected.size());
    std::cout<<"Merged Interval:\n";
    auto eit = std::begin(expected);
    while (it != beyond){
        std::cout<<it->first<<","<<it->second<<"\n";
        REQUIRE(abs(eit->first - it->first) < MOVETK_EPS);
        REQUIRE(abs(eit->second - it->second) < MOVETK_EPS);
        eit++;
        it++;
    }

}


TEST_CASE("Merge Intervals 5", "[test_merge_intervals_5]"){
    typedef typename MovetkGeometryKernel::NT NT;
    std::vector<std::pair<NT, NT>> items {{1,7},{2,4},{1,2},{7,8}};
    std::vector<std::pair<NT, NT>> expected {{7,8}, {1,7}};
    auto beyond = movetk_core::merge_intervals<MovetkGeometryKernel>(std::begin(items),
                                                                     std::end(items));
    auto it = std::begin(items);
    REQUIRE(std::distance(it, beyond) == expected.size());
    std::cout<<"Merged Interval:\n";
    auto eit = std::begin(expected);
    while (it != beyond){
        std::cout<<it->first<<","<<it->second<<"\n";
        REQUIRE(abs(eit->first - it->first) < MOVETK_EPS);
        REQUIRE(abs(eit->second - it->second) < MOVETK_EPS);
        eit++;
        it++;
    }

}

TEST_CASE("Compute Curve Length", "[compute_curve_length]"){
    typedef typename MovetkGeometryKernel::MovetkPoint  MovetkPoint;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    std::vector<MovetkGeometryKernel::MovetkPoint> polyline{
        make_point({2,2}), make_point({4,6}), make_point({8,6}),
        make_point({6,4}), make_point({8,4}), make_point({7.16,2.84}),
        make_point({4.93,2.69}), make_point({4.64,4.65}), make_point({6.05,6.37}),
        make_point({5.1,7.89}),  make_point({1.67,7.51}), make_point({1.93,4.63})
    };
    std::vector<MovetkGeometryKernel::NT> result;
    movetk_core::compute_curve_squared_length<MovetkGeometryKernel, Norm>(
               std::begin(polyline), std::end(polyline),
               movetk_core::movetk_back_insert_iterator(result)
            );

    std::copy(std::begin(result), std::end(result),
            std::ostream_iterator<MovetkGeometryKernel::NT>(std::cout,"\n"));
}