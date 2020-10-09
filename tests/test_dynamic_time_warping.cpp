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
// Created by Stroila, Matei on 2020-10-8.
//

#include <array>
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
            // It is possible to choose
            // a higher dimension
            const static  size_t dimensions = 2;
            //const size_t dimensions = 7;
            //==============================

            //==============================
            // Define the Geometry Backend
            typedef movetk_support::CGALTraits<NT, dimensions> CGAL_GeometryBackend;
            //Using the Geometry Backend define the Movetk Geometry Kernel
            typedef movetk_core::MovetkGeometryKernel<
                    typename CGAL_GeometryBackend::Wrapper_CGAL_Geometry> MovetkGeometryKernel;
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


TEST_CASE("Check Dynamic Time Warping 1","[dynamic_time_warping_1]") {

    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    typedef std::vector<MovetkGeometryKernel::MovetkPoint> PolyLine;
    PolyLine polyline1({
        make_point({0,0}), make_point({1,1}), make_point({1,2}),
        make_point({2,1}), make_point({2,2})
    });

    PolyLine polyline2({
        make_point({1,0}), make_point({0,1}), make_point({1,1}),
        make_point({2,1}), make_point({3,1})
    });

    movetk_core::ComputeDynamicTimeWarpingDistance<MovetkGeometryKernel, Norm> dynamic_time_warping;
#if CGAL_BACKEND_ENABLED
    MovetkGeometryKernel::NT distance =  dynamic_time_warping(std::begin(polyline1), std::end(polyline1),
            std::begin(polyline2), std::end(polyline2));
#else
    MovetkGeometryKernel::NT distance = dynamic_time_warping(std::begin(polyline1), std::end(polyline1),
                                                            std::begin(polyline2), std::end(polyline2)); 
#endif
    std::cout<< distance<<"\n";
    REQUIRE(distance == 1);
}


TEST_CASE("Check Dynamic Time Warping 2","[dynamic_time_warping_2]") {

    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    typedef std::vector<MovetkGeometryKernel::MovetkPoint> PolyLine;
    PolyLine polyline1({
                               make_point({7, 10.5}), make_point({11, 10.5}),
                               make_point({11, 7}), make_point({7, 7}),
                               make_point({7, 10})
                       });

    PolyLine polyline2({
                               make_point({7.09, 10.2}), make_point({10.80,10.19}),
                               make_point({10.799, 7.312}), make_point({7.197, 7.312}),
                               make_point({7.203, 9.813}), make_point({10.6, 9.813}),
                               make_point({10.6, 7.505}), make_point({7.5, 7.5}),
                               make_point({7.5, 9.5})
                       });

    movetk_core::ComputeDynamicTimeWarpingDistance<MovetkGeometryKernel, Norm> dynamic_time_warping;
#if CGAL_BACKEND_ENABLED
    MovetkGeometryKernel::NT distance =  dynamic_time_warping(std::begin(polyline1), std::end(polyline1),
                                                           std::begin(polyline2), std::end(polyline2));
#else
    MovetkGeometryKernel::NT distance = dynamic_time_warping(std::begin(polyline1), std::end(polyline1),
                                                            std::begin(polyline2), std::end(polyline2));
#endif
    std::cout<<  distance<<"\n";
    REQUIRE(std::abs(distance - 0.37114) < MOVETK_EPS);
}


TEST_CASE("Check Dynamic Time Warping 3","[dynamic_time_warping_3]") {

    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    typedef std::vector<MovetkGeometryKernel::MovetkPoint> PolyLine;
    PolyLine polyline1({
                               make_point({27, 15}), make_point({26, 16}),
                               make_point({26.986, 15.596}), make_point({25, 18}),
                               make_point({27.220, 16.389}), make_point({24.483, 20.207}),
                               make_point({28.1394, 16.695}), make_point({28.481, 21.03}),
                               make_point({30, 17}), make_point({31.799, 20.817}),
                               make_point({31.1023, 17.454}), make_point({33.488, 19.073}),
                               make_point({31.13, 16.324}), make_point({32.483, 16.449}),
                               make_point({30.934, 15.4731})
                       });

    PolyLine polyline2({
                               make_point({26.33, 14.705}), make_point({26.24, 16.15}),
                               make_point({25.84, 17.607}), make_point({26.818, 18.347}),
                               make_point({29.26, 19.533}), make_point({31.674, 18.87}),
                               make_point({32.511, 17.482}), make_point({33.0417, 16.184}),
                               make_point({26.98, 15.33}), make_point({26.344, 17.287}),
                               make_point({27.223, 17.789}),make_point({29.107, 18.57}),
                               make_point({31.004, 18.375}),make_point({31.9116, 17.747}),
                               make_point({31.799, 16.536}), make_point({27.655, 16.449}),
                               make_point({28.395, 17.817}),make_point({30.6, 17.859})
                       });

    movetk_core::ComputeDynamicTimeWarpingDistance<MovetkGeometryKernel, Norm> dynamic_time_warping;
#if CGAL_BACKEND_ENABLED
    MovetkGeometryKernel::NT distance =  dynamic_time_warping(std::begin(polyline1), std::end(polyline1),
                                                           std::begin(polyline2), std::end(polyline2));
#else
    MovetkGeometryKernel::NT distance = dynamic_time_warping(std::begin(polyline1), std::end(polyline1),
                                                            std::begin(polyline2), std::end(polyline2));
#endif
    std::cout<<  distance<<"\n";
    REQUIRE(std::abs(distance - 2.93282) < MOVETK_EPS);
}