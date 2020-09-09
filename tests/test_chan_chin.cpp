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
// Created by Mitra, Aniket on 2019-04-04.
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
#include "movetk/algo/Simplification.h"


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

TEST_CASE("Check Chan Chin Shortcuts 1", "[is_valid_shortcut_1]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    typedef std::vector<MovetkGeometryKernel::MovetkPoint> PolyLine;
    PolyLine polyline({
        make_point({-8, 4}), make_point({-2, 4}),
        make_point({4, 4})
    });
    std::cout << "Polyline to be simplified: ";
    std::cout << "{";
    for (auto &vertex: polyline) {
        cout << vertex;
        std::cout << ";";
    }
    std::cout << "}\n";
    typedef movetk_core::Wedge<MovetkGeometryKernel, Norm> Wedge;
    movetk_algorithms::ChanChin<MovetkGeometryKernel, Wedge> ChanChin(1);
    std::vector<std::pair<std::size_t, std::size_t> > edges;
    std::vector<std::pair<std::size_t, std::size_t> > ExpectedEdges({
                                                                            std::make_pair(0, 1), std::make_pair(0, 2),
                                                                            std::make_pair(1, 2)
                                                                    });
    ChanChin(std::begin(polyline), std::end(polyline), movetk_core::movetk_back_insert_iterator(edges));
    REQUIRE(edges.size() == ExpectedEdges.size());
    std::cout << "Valid Shortcuts: ";
    std::cout << "{";
    auto eit = std::begin(ExpectedEdges);
    for (auto &edge: edges) {
        std::cout << edge.first << "," << edge.second << ";";
        REQUIRE(edge == *eit);
        eit++;
    }
    std::cout << "}\n";
}

TEST_CASE("Check Chan Chin Shortcuts 2", "[is_valid_shortcut_2]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    typedef std::vector<MovetkGeometryKernel::MovetkPoint> PolyLine;
    PolyLine polyline({
        make_point({-9, 8}), make_point({-2, 4}),
        make_point({4, 4})
    });
    std::cout << "Polyline to be simplified: ";
    std::cout << "{";
    for (auto &vertex: polyline) {
        cout << vertex;
        std::cout << ";";
    }
    std::cout << "}\n";
    typedef movetk_core::Wedge<MovetkGeometryKernel, Norm> Wedge;
    movetk_algorithms::ChanChin<MovetkGeometryKernel, Wedge> ChanChin(1);
    std::vector<std::pair<std::size_t, std::size_t> > edges;
    std::vector<std::pair<std::size_t, std::size_t> > ExpectedEdges({
                                                                            std::make_pair(0, 1), std::make_pair(1, 2)
                                                                    });
    ChanChin(std::begin(polyline), std::end(polyline), movetk_core::movetk_back_insert_iterator(edges));
    REQUIRE(edges.size() == ExpectedEdges.size());
    std::cout << "Valid Shortcuts: ";
    std::cout << "{";
    auto eit = std::begin(ExpectedEdges);
    for (auto &edge: edges) {
        std::cout << edge.first << "," << edge.second << ";";
        REQUIRE(edge == *eit);
        eit++;
    }
    std::cout << "}\n";
}


TEST_CASE("Check Chan Chin Shortcuts 3", "[is_valid_shortcut_3]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    typedef std::vector<MovetkGeometryKernel::MovetkPoint> PolyLine;
    PolyLine polyline({
        make_point({-9, 8}), make_point({-2, 4}),
        make_point({3.503, 3.456})
    });
    std::cout << "Polyline to be simplified: ";
    std::cout << "{";
    for (auto &vertex: polyline) {
        cout << vertex;
        std::cout << ";";
    }
    std::cout << "}\n";
    typedef movetk_core::Wedge<MovetkGeometryKernel, Norm> Wedge;
    movetk_algorithms::ChanChin<MovetkGeometryKernel, Wedge> ChanChin(1);
    std::vector<std::pair<std::size_t, std::size_t> > edges;
    std::vector<std::pair<std::size_t, std::size_t> > ExpectedEdges({
                                                                            std::make_pair(0, 1), std::make_pair(1, 2)
                                                                    });
    ChanChin(std::begin(polyline), std::end(polyline), movetk_core::movetk_back_insert_iterator(edges));
    REQUIRE(edges.size() == ExpectedEdges.size());
    std::cout << "Valid Shortcuts: ";
    std::cout << "{";
    auto eit = std::begin(ExpectedEdges);
    for (auto &edge: edges) {
        std::cout << edge.first << "," << edge.second << ";";
        REQUIRE(edge == *eit);
        eit++;
    }
    std::cout << "}\n";
}


/*
TEST_CASE("Check Chan Chin Shortcuts 4", "[is_valid_shortcut_4]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    typedef std::vector<MovetkGeometryKernel::MovetkPoint> PolyLine;
    PolyLine polyline({
        make_point({1, -6}), make_point({4, -4}),
        make_point({5, -2}), make_point({6, -5}),
        make_point({7, -2}), make_point({8, -5}),
        make_point({9, -2}), make_point({10, -5}),
        make_point({11, -2}), make_point({13, -4})
    });
    std::cout << "Polyline to be simplified: ";
    std::cout << "{";
    for (auto &vertex: polyline) {
        cout << vertex;
        std::cout << ";";
    }
    std::cout << "}\n";
    typedef movetk_core::Wedge<MovetkGeometryKernel, Norm> Wedge;
    movetk_algorithms::ChanChin<MovetkGeometryKernel, Wedge> ChanChin(2);
    std::vector<std::pair<std::size_t, std::size_t> > edges;
    std::vector<std::pair<std::size_t, std::size_t> > ExpectedEdges({
        std::make_pair(0, 1), std::make_pair(1, 2),
        std::make_pair(2, 3), std::make_pair(3, 4),
        std::make_pair(2, 4), std::make_pair(4, 5),
        std::make_pair(5, 6), std::make_pair(6, 7),
        std::make_pair(5, 7), std::make_pair(7, 8),
        std::make_pair(1, 9), std::make_pair(7, 9),
        std::make_pair(8, 9)
    });
    ChanChin(std::begin(polyline), std::end(polyline), movetk_core::movetk_back_insert_iterator(edges));
    REQUIRE(edges.size() == ExpectedEdges.size());
    std::cout << "Valid Shortcuts: ";
    std::cout << "{";
    auto eit = std::begin(ExpectedEdges);
    for (auto &edge: edges) {
        std::cout << edge.first << "," << edge.second << ";";
        REQUIRE(edge == *eit);
        eit++;
    }
    std::cout << "}\n";
}*/
