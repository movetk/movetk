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
// Created by Mitra, Aniket on 2019-06-04.
//

#include <array>
#include <movetk/algo/Similarity.h>
#include "catch2/catch.hpp"

#if CGAL_BACKEND_ENABLED
#include "movetk/geom/CGALTraits.h"
#else

#include "movetk/geom/BoostGeometryTraits.h"

#endif

#include "movetk/geom/GeometryInterface.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "movetk/metric/Norm.h"
#include "movetk/algo/Simplification.h"

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

typedef movetk_support::FiniteNorm<MovetkGeometryKernel, 1> Norm;

TEST_CASE("Check Longest Common Subsequence 1","[longest_common_sense_1]"){
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    typedef std::vector<MovetkGeometryKernel::MovetkPoint> PolyLine;
    PolyLine polyline1({
                              make_point({-5, -4}), make_point({-4.79, -1.62}),
                              make_point({-2, -1}), make_point({-0.87, -4.56}),
                              make_point({2.03, -0.74}), make_point({3.71, 1.28})
                      });

    std::cout << "Polyline 1: ";
    std::cout << "{";
    for (auto& vertex: polyline1) {
        cout << vertex;
        std::cout << ";";
    }
    std::cout << "}\n";

    PolyLine polyline2({
                            make_point({-4,-3}), make_point({-4.37,-1.88}),
                            make_point({-1.61, -1.02}), make_point({2.57, -0.56}),
                            make_point({-4.53, -3.94}), make_point({-1, -5})

                        });

    std::cout << "Polyline 2: ";
    std::cout << "{";
    for (auto& vertex: polyline2) {
        cout << vertex;
        std::cout << ";";
    }
    std::cout << "}\n";

    std::vector<std::pair<MovetkGeometryKernel::MovetkPoint, MovetkGeometryKernel::MovetkPoint> >
            ExpectedLCSS {
        std::make_pair(make_point({-4.79, -1.62}), make_point({-4.37,-1.88})),
        std::make_pair(make_point({-2, -1}), make_point({-1.61, -1.02})),
        std::make_pair(make_point({2.03, -0.74}),make_point({2.57, -0.56}))
    };
    std::vector<std::pair<PolyLine::const_iterator, PolyLine::const_iterator> > output;
    movetk_algorithms::LongestCommonSubSequence<MovetkGeometryKernel, Norm> lcs(2, 7);
    std::size_t lcs_length = lcs(std::cbegin(polyline1), std::cend(polyline1), std::cbegin(polyline2), std::cend(polyline2),
                                 movetk_core::movetk_back_insert_iterator(output));
    std::cout<<lcs_length<<"\n";
    REQUIRE(lcs_length == ExpectedLCSS.size());

    std::cout << "Longest Common Subsequence: ";
    std::cout << "{";
    auto eit = std::cbegin(ExpectedLCSS);
    for (auto reference: output) {
        std::cout << "{";
        std::cout<<*reference.first;
        MovetkGeometryKernel::MovetkVector v1 = *reference.first - eit->first;
        REQUIRE((v1 * v1) < MOVETK_EPS);
        std::cout<<";";
        std::cout<<*reference.second;
        MovetkGeometryKernel::MovetkVector v2 = *reference.second - eit->second;
        REQUIRE((v2 * v2) < MOVETK_EPS);
        std::cout<<"},";
        eit++;
    }
    std::cout << "}\n";

}


TEST_CASE("Check Longest Common Subsequence 2","[longest_common_sense_2]"){
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    typedef std::vector<MovetkGeometryKernel::MovetkPoint> PolyLine;
    PolyLine polyline1({
                               make_point({7, 10.5}), make_point({11, 10.5}),
                               make_point({11, 7}), make_point({7, 7}),
                               make_point({7, 10})
                       });

    std::cout << "Polyline 1: ";
    std::cout << "{";
    for (auto& vertex: polyline1) {
        cout << vertex;
        std::cout << ";";
    }
    std::cout << "}\n";

    PolyLine polyline2({
                               make_point({7.084, 10.182}), make_point({10.806,10.189}),
                               make_point({10.799, 7.312}), make_point({7.197, 7.312}),
                               make_point({7.203, 9.813}), make_point({10.502, 9.813}),
                               make_point({10.5, 7.5}), make_point({7.498, 7.5})

                       });

    std::cout << "Polyline 2: ";
    std::cout << "{";
    for (auto& vertex: polyline2) {
        cout << vertex;
        std::cout << ";";
    }
    std::cout << "}\n";

    std::vector<std::pair<MovetkGeometryKernel::MovetkPoint, MovetkGeometryKernel::MovetkPoint> >
            ExpectedLCSS {
            std::make_pair(make_point({7, 10.5}), make_point({7.084, 10.182})),
            std::make_pair(make_point({11, 10.5}), make_point({10.806,10.189})),
            std::make_pair(make_point({11, 7}), make_point({10.799, 7.312})),
            std::make_pair(make_point({7, 7}), make_point({7.197, 7.312})),
            std::make_pair(make_point({7, 10}), make_point({7.203, 9.813}))
    };
    std::vector<std::pair<PolyLine::const_iterator, PolyLine::const_iterator> > output;
    movetk_algorithms::LongestCommonSubSequence<MovetkGeometryKernel, Norm> lcs(2, 3);
    std::size_t lcs_length = lcs(std::cbegin(polyline1), std::cend(polyline1), std::cbegin(polyline2), std::cend(polyline2),
                                 movetk_core::movetk_back_insert_iterator(output));
    std::cout<<lcs_length<<"\n";
    REQUIRE(lcs_length == ExpectedLCSS.size());

    std::cout << "Longest Common Subsequence: ";
    std::cout << "{";
    auto eit = std::cbegin(ExpectedLCSS);
    for (auto reference: output) {
        std::cout << "{";
        std::cout<<*reference.first;
        MovetkGeometryKernel::MovetkVector v1 = *reference.first - eit->first;
        REQUIRE((v1 * v1) < MOVETK_EPS);
        std::cout<<";";
        std::cout<<*reference.second;
        MovetkGeometryKernel::MovetkVector v2 = *reference.second - eit->second;
        REQUIRE((v2 * v2) < MOVETK_EPS);
        std::cout<<"},";
        eit++;
    }
    std::cout << "}\n";

}



TEST_CASE("Check Longest Common Subsequence 3","[longest_common_sense_3]"){
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    typedef std::vector<MovetkGeometryKernel::MovetkPoint> PolyLine;
    PolyLine polyline1({
                               make_point({-5, -4}), make_point({-4.79, -1.62}),
                               make_point({-2, -1}), make_point({-0.87, -4.56}),
                               make_point({2.03, -0.74}), make_point({3.71, 1.28})
                       });

    std::cout << "Polyline 1: ";
    std::cout << "{";
    for (auto& vertex: polyline1) {
        cout << vertex;
        std::cout << ";";
    }
    std::cout << "}\n";

    PolyLine polyline2({
                               make_point({-4,-3}), make_point({-4.37,-1.88}),
                               make_point({-1.61, -1.02}), make_point({2.57, -0.56}),
                               make_point({-4.53, -3.94}), make_point({-1, -5})

                       });

    std::cout << "Polyline 2: ";
    std::cout << "{";
    for (auto& vertex: polyline2) {
        cout << vertex;
        std::cout << ";";
    }
    std::cout << "}\n";


    std::vector<std::pair<PolyLine::const_iterator, PolyLine::const_iterator> > output;
    movetk_algorithms::LongestCommonSubSequence<MovetkGeometryKernel, Norm> lcs(2, 0);
    std::size_t lcs_length = lcs(std::cbegin(polyline1), std::cend(polyline1), std::cbegin(polyline2), std::cend(polyline2),
                                 movetk_core::movetk_back_insert_iterator(output));
    std::cout<<lcs_length<<"\n";
    REQUIRE(lcs_length == 0);
    REQUIRE(output.size() == 0);

}