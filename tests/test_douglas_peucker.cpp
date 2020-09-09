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
// Created by Mitra, Aniket on 2019-02-22.
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

TEST_CASE("Check Douglas-Peucker Simplification 1", "[douglas_peucker_simplification_1]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    typedef std::vector<MovetkGeometryKernel::MovetkPoint> PolyLine;
    PolyLine polyline({
                              make_point({-6.19, -3.46}), make_point({-4.99, 1.16}),
                              make_point({-2.79, -2.22}), make_point({-1.87, 0.58}),
                              make_point({0.77, 0.22}), make_point({-1.15, 3.06}),
                              make_point({5.33, -1.12})
                      });
    PolyLine ExpectedPolyline;
    std::cout << "Polyline to be simplified: ";
    std::cout << "{";
    for (auto vertex: polyline) {
        cout << vertex;
        std::cout << ";";
    }
    std::cout << "}\n";

    std::vector<PolyLine::iterator> result;
    typedef movetk_algorithms::FindFarthest<MovetkGeometryKernel, Norm> FindFarthest;
    movetk_algorithms::DouglasPeucker<MovetkGeometryKernel, FindFarthest> DouglasPeucker1(5.47722);
    ExpectedPolyline = PolyLine({make_point({-6.19, -3.46}), make_point({5.33, -1.12})});
    DouglasPeucker1(std::begin(polyline), std::end(polyline), movetk_core::movetk_back_insert_iterator(result));
    std::cout << "Simplified polyline has: " << result.size() << " vertices\n";
    REQUIRE(result.size() == 2);
    std::cout << "Simplified Polyline: ";
    std::cout << "{";
    auto eit = std::begin(ExpectedPolyline);
    for (auto reference: result) {
        cout << *reference;
        MovetkGeometryKernel::MovetkVector v = *reference - *eit;
        REQUIRE((v * v) < MOVETK_EPS);
        eit++;
        std::cout << ";";
    }
    std::cout << "}\n";

}

TEST_CASE("Check Douglas-Peucker Simplification 2", "[douglas_peucker_simplification_2]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    typedef std::vector<MovetkGeometryKernel::MovetkPoint> PolyLine;
    PolyLine polyline({
                              make_point({-6.19, -3.46}), make_point({-4.99, 1.16}),
                              make_point({-2.79, -2.22}), make_point({-1.87, 0.58}),
                              make_point({0.77, 0.22}), make_point({-1.15, 3.06}),
                              make_point({5.33, -1.12})
                      });
    PolyLine ExpectedPolyline;
    std::cout << "Polyline to be simplified: ";
    std::cout << "{";
    for (auto vertex: polyline) {
        cout << vertex;
        std::cout << ";";
    }
    std::cout << "}\n";

    std::vector<PolyLine::iterator> result;
    typedef movetk_algorithms::FindFarthest<MovetkGeometryKernel, Norm> FindFarthest;
    movetk_algorithms::DouglasPeucker<MovetkGeometryKernel, FindFarthest> DouglasPeucker2(3.162277);
    ExpectedPolyline = PolyLine({make_point({-6.19, -3.46}), make_point({-4.99, 1.16}),
                                 make_point({0.77, 0.22}), make_point({-1.15, 3.06}),
                                 make_point({5.33, -1.12})});
    DouglasPeucker2(std::begin(polyline), std::end(polyline), movetk_core::movetk_back_insert_iterator(result));
    std::cout << "Simplified polyline has: " << result.size() << " vertices\n";
    REQUIRE(result.size() == 5);
    std::cout << "Simplified Polyline: ";
    std::cout << "{";
    auto eit = std::begin(ExpectedPolyline);
    for (auto reference: result) {
        cout << *reference;
        MovetkGeometryKernel::MovetkVector v = *reference - *eit;
        REQUIRE((v * v) < MOVETK_EPS);
        eit++;
        std::cout << ";";
    }
    std::cout << "}\n";


}


TEST_CASE("Check Douglas-Peucker Simplification 3", "[douglas_peucker_simplification_3]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    typedef std::vector<MovetkGeometryKernel::MovetkPoint> PolyLine;
    PolyLine polyline({
                              make_point({-6.19, -3.46}), make_point({-4.99, 1.16}),
                              make_point({-2.79, -2.22}), make_point({-1.87, 0.58}),
                              make_point({0.77, 0.22}), make_point({-1.15, 3.06}),
                              make_point({5.33, -1.12})
                      });
    PolyLine ExpectedPolyline;
    std::cout << "Polyline to be simplified: ";
    std::cout << "{";
    for (auto vertex: polyline) {
        cout << vertex;
        std::cout << ";";
    }
    std::cout << "}\n";

    std::vector<PolyLine::iterator> result;
    typedef movetk_algorithms::FindFarthest<MovetkGeometryKernel, Norm> FindFarthest;
    movetk_algorithms::DouglasPeucker<MovetkGeometryKernel, FindFarthest> DouglasPeucker3(1);
    ExpectedPolyline = PolyLine(
            {make_point({-6.19, -3.46}), make_point({-4.99, 1.16}),
             make_point({-2.79, -2.22}), make_point({-1.87, 0.58}),
             make_point({0.77, 0.22}), make_point({-1.15, 3.06}),
             make_point({5.33, -1.12})}
    );
    DouglasPeucker3(std::begin(polyline), std::end(polyline), movetk_core::movetk_back_insert_iterator(result));
    std::cout << "Simplified polyline has: " << result.size() << " vertices\n";
    REQUIRE(result.size() == 7);
    std::cout << "Simplified Polyline: ";
    std::cout << "{";
    auto eit = std::begin(ExpectedPolyline);
    for (auto reference: result) {
        cout << *reference;
        MovetkGeometryKernel::MovetkVector v = *reference - *eit;
        REQUIRE((v * v) < MOVETK_EPS);
        eit++;
        std::cout << ";";
    }
    std::cout << "}\n";

}



TEST_CASE("Check Douglas-Peucker Simplification 4", "[douglas_peucker_simplification_4]") {
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    typedef std::vector<MovetkGeometryKernel::MovetkPoint> PolyLine;
    PolyLine polyline({
        make_point({-6.19, -3.46}),  make_point({-6.19, -3.46}),
        make_point({-6.19, -3.46}),  make_point({-6.19, -3.46}),
        make_point({-6.19, -3.46}),  make_point({-6.19, -3.46}),
        make_point({-6.19, -3.46})
    });
    PolyLine ExpectedPolyline;
    std::cout << "Polyline to be simplified: ";
    std::cout << "{";
    for (auto vertex: polyline) {
        cout << vertex;
        std::cout << ";";
    }
    std::cout << "}\n";

    std::vector<PolyLine::iterator> result;
    typedef movetk_algorithms::FindFarthest<MovetkGeometryKernel, Norm> FindFarthest;
    movetk_algorithms::DouglasPeucker<MovetkGeometryKernel, FindFarthest> DouglasPeucker4(3.162277);
    ExpectedPolyline = PolyLine({
        make_point({-6.19, -3.46}), make_point({-6.19, -3.46})
    });
    DouglasPeucker4(std::begin(polyline), std::end(polyline), movetk_core::movetk_back_insert_iterator(result));
    std::cout << "Simplified polyline has: " << result.size() << " vertices\n";
    REQUIRE(result.size() == 2);
    std::cout << "Simplified Polyline: ";
    std::cout << "{";
    auto eit = std::begin(ExpectedPolyline);
    for (auto reference: result) {
        cout << *reference;
        MovetkGeometryKernel::MovetkVector v = *reference - *eit;
        REQUIRE((v * v) < MOVETK_EPS);
        eit++;
        std::cout << ";";
    }
    std::cout << "}\n";

}