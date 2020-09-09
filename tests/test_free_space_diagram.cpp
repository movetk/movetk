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
// Created by Mitra, Aniket on 2019-09-16.
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
#include "movetk/ds/FreeSpaceDiagram.h"

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

TEST_CASE("Check free space cell 1", "[free_space_cell_1]") {
    typedef typename MovetkGeometryKernel::NT NT;
    typedef typename MovetkGeometryKernel::MovetkPoint MovetkPoint;
    typedef movetk_core::IntersectionTraits<MovetkGeometryKernel, Norm, movetk_core::sphere_segment_intersection_tag> IntersectionTraits;
    typedef movetk_support::FreeSpaceCellTraits<IntersectionTraits> FreeSpaceCellTraits;
    typedef std::vector<IntersectionTraits::value_type> ExpectedIntersections;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    using edge_orientation = FreeSpaceCellTraits::edge_orientation;
    using vertex_orientation = FreeSpaceCellTraits::vertex_orientation;
    ExpectedIntersections expected{std::make_tuple(edge_orientation::Left, -1, -1, make_point({1.000000, 1.000000})),
                                   std::make_tuple(edge_orientation::Top, 1, 1, make_point({6.000000, 6.000000})),
                                   std::make_tuple(edge_orientation::Top, 1, 0.36, make_point({4.000000, 4.000000})),
                                   std::make_tuple(edge_orientation::Right, 0, 1, make_point({6.000000, 4.000000})),
                                   std::make_tuple(edge_orientation::Bottom, 1, 0.36, make_point({4.000000, 4.000000})),
                                   std::make_tuple(edge_orientation::Bottom, 1, 0.04,
                                                   make_point({2.000000, 2.000000}))};

    std::vector<std::size_t> expected_free_vertices{vertex_orientation::TopRight};


    movetk_core::MakeSegment<MovetkGeometryKernel> make_segment;
    MovetkGeometryKernel::MovetkSegment P = make_segment({2, 4}, {6, 4});
    MovetkGeometryKernel::MovetkSegment Q = make_segment({1, 1}, {6, 6});
    movetk_support::FreeSpaceCell<FreeSpaceCellTraits> fsc(P, Q, 2);
    std::size_t size = std::distance(fsc.begin(), fsc.end());
    std::cout << "Num Intersections: " << size << "\n";
    REQUIRE(size == expected.size());
    auto eit = expected.begin();
    std::cout << "EdgeID, SIGN_DISCRIMINANT, INTERSECTION_SQUARED_RATIO, INTERSECTION_POINT\n";
    for (auto intersection: fsc) {
        std::cout << std::get<IntersectionTraits::Attributes::ID>(intersection) << "," <<
                  std::get<IntersectionTraits::Attributes::SIGN_DISCRIMINANT>(intersection) << "," <<
                  std::get<IntersectionTraits::Attributes::SQUARED_RATIO>(intersection) << "," <<
                  std::get<IntersectionTraits::Attributes::POINT>(intersection) << "\n";
        REQUIRE (std::get<IntersectionTraits::Attributes::ID>(intersection) ==
                 std::get<IntersectionTraits::Attributes::ID>(*eit));
        REQUIRE (std::get<IntersectionTraits::Attributes::SIGN_DISCRIMINANT>(intersection) ==
                 std::get<IntersectionTraits::Attributes::SIGN_DISCRIMINANT>(*eit));
        REQUIRE (abs(std::get<IntersectionTraits::Attributes::SQUARED_RATIO>(intersection) -
                     std::get<IntersectionTraits::Attributes::SQUARED_RATIO>(*eit)) < MOVETK_EPS);
        MovetkGeometryKernel::MovetkVector v = std::get<IntersectionTraits::Attributes::POINT>(intersection) -
                                              std::get<IntersectionTraits::Attributes::POINT>(*eit);
        REQUIRE(v * v < MOVETK_EPS);
        eit++;
    }

    REQUIRE(std::distance(fsc.vertices_begin(), fsc.vertices_end()) == expected_free_vertices.size());
    std::cout << "Free Vertices: {";
    auto vit = fsc.vertices_begin();
    auto evit = expected_free_vertices.begin();
    while (vit != fsc.vertices_end()) {
        std::cout << *vit << ",";
        REQUIRE (*vit == *evit);
        vit++;
        evit++;
    }
    std::cout << "}\n";

}


TEST_CASE("Check free space cell 2", "[free_space_cell_2]") {
    typedef typename MovetkGeometryKernel::NT NT;
    typedef typename MovetkGeometryKernel::MovetkPoint MovetkPoint;
    typedef movetk_core::IntersectionTraits<MovetkGeometryKernel, Norm, movetk_core::sphere_segment_intersection_tag> IntersectionTraits;
    typedef movetk_support::FreeSpaceCellTraits<IntersectionTraits> FreeSpaceCellTraits;
    typedef std::vector<IntersectionTraits::value_type> ExpectedIntersections;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    using edge_orientation = FreeSpaceCellTraits::edge_orientation;
    using vertex_orientation = FreeSpaceCellTraits::vertex_orientation;
    ExpectedIntersections expected{
            std::make_tuple(edge_orientation::Left, 1, 0.1875, make_point({-6.267949, -4.000000})),
            std::make_tuple(edge_orientation::Left, -1, 0.1875, make_point({-8.000000, -4.000000})),
            std::make_tuple(edge_orientation::Top, -1, 1.69, make_point({-4.000000, -3.000000})),
            std::make_tuple(edge_orientation::Top, 1, 0.25, make_point({-6.000000, -4.000000})),
            std::make_tuple(edge_orientation::Right, -1, 2.05353, make_point({-4.000000, -4.000000})),
            std::make_tuple(edge_orientation::Right, 1, 0.321475, make_point({-5.732051, -4.000000})),
            std::make_tuple(edge_orientation::Bottom, 1, 0.25, make_point({-6.000000, -4.000000})),
            std::make_tuple(edge_orientation::Bottom, -1, 0.09, make_point({-8.000000, -5.000000}))};

    std::vector<std::size_t> expected_free_vertices{vertex_orientation::BottomLeft, vertex_orientation::TopRight};
    movetk_core::MakeSegment<MovetkGeometryKernel> make_segment;
    MovetkGeometryKernel::MovetkSegment P = make_segment({-8, -4}, {-4, -4});
    MovetkGeometryKernel::MovetkSegment Q = make_segment({-8, -5}, {-4, -3});
    movetk_support::FreeSpaceCell<FreeSpaceCellTraits> fsc(P, Q, 2);
    std::size_t num_intersections = std::distance(fsc.begin(), fsc.end());
    std::cout << "Num Intersections: " << num_intersections << "\n";
    REQUIRE(num_intersections == expected.size());
    auto eit = expected.begin();
    std::cout << "EdgeID, SIGN_DISCRIMINANT, INTERSECTION_SQUARED_RATIO, INTERSECTION_POINT\n";
    for (auto intersection: fsc) {
        std::cout << std::get<IntersectionTraits::Attributes::ID>(intersection) << "," <<
                  std::get<IntersectionTraits::Attributes::SIGN_DISCRIMINANT>(intersection) << "," <<
                  std::get<IntersectionTraits::Attributes::SQUARED_RATIO>(intersection) << "," <<
                  std::get<IntersectionTraits::Attributes::POINT>(intersection) << "\n";
        REQUIRE (std::get<IntersectionTraits::Attributes::ID>(intersection) ==
                 std::get<IntersectionTraits::Attributes::ID>(*eit));
        REQUIRE (std::get<IntersectionTraits::Attributes::SIGN_DISCRIMINANT>(intersection) ==
                 std::get<IntersectionTraits::Attributes::SIGN_DISCRIMINANT>(*eit));
        REQUIRE (abs(std::get<IntersectionTraits::Attributes::SQUARED_RATIO>(intersection) -
                     std::get<IntersectionTraits::Attributes::SQUARED_RATIO>(*eit)) < MOVETK_EPS);
        MovetkGeometryKernel::MovetkVector v = std::get<IntersectionTraits::Attributes::POINT>(intersection) -
                                              std::get<IntersectionTraits::Attributes::POINT>(*eit);
        REQUIRE(v * v < MOVETK_EPS);
        eit++;
    }

    REQUIRE(std::distance(fsc.vertices_begin(), fsc.vertices_end()) == expected_free_vertices.size());
    std::cout << "Free Vertices: {";
    auto vit = fsc.vertices_begin();
    auto evit = expected_free_vertices.begin();
    while (vit != fsc.vertices_end()) {
        std::cout << *vit << ",";
        REQUIRE (*vit == *evit);
        vit++;
        evit++;
    }
    std::cout << "}\n";

}


TEST_CASE("Check free space cell 3", "[free_space_cell_3]") {
    typedef typename MovetkGeometryKernel::NT NT;
    typedef typename MovetkGeometryKernel::MovetkPoint MovetkPoint;
    typedef movetk_core::IntersectionTraits<MovetkGeometryKernel, Norm, movetk_core::sphere_segment_intersection_tag> IntersectionTraits;
    typedef movetk_support::FreeSpaceCellTraits<IntersectionTraits> FreeSpaceCellTraits;
    typedef std::vector<IntersectionTraits::value_type> ExpectedIntersections;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    using edge_orientation = FreeSpaceCellTraits::edge_orientation;
    using vertex_orientation = FreeSpaceCellTraits::vertex_orientation;
    ExpectedIntersections expected{
            std::make_tuple(edge_orientation::Left, 1, 0.158384, make_point({18.266276, 8.142512})),
            std::make_tuple(edge_orientation::Left, -1, 0.0144317, make_point({18.442300, 7.575660})),
            std::make_tuple(edge_orientation::Top, -1, 2.25, make_point({18.500000, 8.500000})),
            std::make_tuple(edge_orientation::Top, 1, 0.25, make_point({18.000000, 8.000000})),
            std::make_tuple(edge_orientation::Right, -1, 1.47265, make_point({18.000000, 9.000000})),
            std::make_tuple(edge_orientation::Right, -1, 0.00277772, make_point({18.442300, 7.575660})),
            std::make_tuple(edge_orientation::Bottom, -1, 1.14011, make_point({18.500000, 8.500000})),
            std::make_tuple(edge_orientation::Bottom, -1, 0.00247992, make_point({17.500000, 7.500000}))
    };
    std::vector<std::size_t> expected_free_vertices{vertex_orientation::BottomLeft,
                                                    vertex_orientation::TopRight,
                                                    vertex_orientation::BottomRight};


    movetk_core::MakeSegment<MovetkGeometryKernel> make_segment;
    MovetkGeometryKernel::MovetkSegment P = make_segment({18.4423, 7.57566}, {18, 9});
    MovetkGeometryKernel::MovetkSegment Q = make_segment({17.5, 7.5}, {18.5, 8.5});
    movetk_support::FreeSpaceCell<FreeSpaceCellTraits> fsc(P, Q, 1);
    std::size_t num_intersections = std::distance(fsc.begin(), fsc.end());
    std::cout << "Num Intersections: " << num_intersections << "\n";
    REQUIRE(num_intersections == expected.size());
    auto eit = expected.begin();
    std::cout << "EdgeID, SIGN_DISCRIMINANT, INTERSECTION_SQUARED_RATIO, INTERSECTION_POINT\n";
    for (auto intersection: fsc) {
        std::cout << std::get<IntersectionTraits::Attributes::ID>(intersection) << "," <<
                  std::get<IntersectionTraits::Attributes::SIGN_DISCRIMINANT>(intersection) << "," <<
                  std::get<IntersectionTraits::Attributes::SQUARED_RATIO>(intersection) << "," <<
                  std::get<IntersectionTraits::Attributes::POINT>(intersection) << "\n";
        REQUIRE (std::get<IntersectionTraits::Attributes::ID>(intersection) ==
                 std::get<IntersectionTraits::Attributes::ID>(*eit));
        REQUIRE (std::get<IntersectionTraits::Attributes::SIGN_DISCRIMINANT>(intersection) ==
                 std::get<IntersectionTraits::Attributes::SIGN_DISCRIMINANT>(*eit));
        REQUIRE (abs(std::get<IntersectionTraits::Attributes::SQUARED_RATIO>(intersection) -
                     std::get<IntersectionTraits::Attributes::SQUARED_RATIO>(*eit)) < MOVETK_EPS);
        MovetkGeometryKernel::MovetkVector v = std::get<IntersectionTraits::Attributes::POINT>(intersection) -
                                              std::get<IntersectionTraits::Attributes::POINT>(*eit);
        REQUIRE(v * v < MOVETK_EPS);
        eit++;
    }

    REQUIRE(std::distance(fsc.vertices_begin(), fsc.vertices_end()) == expected_free_vertices.size());
    std::cout << "Free Vertices: {";
    auto vit = fsc.vertices_begin();
    auto evit = expected_free_vertices.begin();
    while (vit != fsc.vertices_end()) {
        std::cout << *vit << ",";
        REQUIRE (*vit == *evit);
        vit++;
        evit++;
    }
    std::cout << "}\n";

}


TEST_CASE("Check free space diagram 1", "[free_space_diagram_1]") {
    typedef typename MovetkGeometryKernel::NT NT;
    typedef typename MovetkGeometryKernel::MovetkPoint MovetkPoint;
    typedef movetk_core::IntersectionTraits<MovetkGeometryKernel, Norm, movetk_core::sphere_segment_intersection_tag> IntersectionTraits;
    typedef movetk_support::FreeSpaceCellTraits<IntersectionTraits> FreeSpaceCellTraits;
    typedef movetk_support::FreeSpaceCell<FreeSpaceCellTraits> FreeSpaceCell;
    typedef movetk_support::FreeSpaceDiagramTraits<FreeSpaceCell> FreeSpaceDiagramTraits;
    typedef movetk_support::FreeSpaceDiagram<FreeSpaceDiagramTraits> FreeSpaceDiagram;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    typedef std::vector<IntersectionTraits::value_type> ExpectedIntersections;
    typedef std::vector<ExpectedIntersections> ExpectedFsd;
    using edge_orientation = FreeSpaceCellTraits::edge_orientation;
    ExpectedIntersections cell1{
            std::make_tuple(edge_orientation::Left, 1, 0.158384, make_point({18.266276, 8.142512})),
            std::make_tuple(edge_orientation::Left, -1, 0.0144317, make_point({18.442300, 7.575660})),
            std::make_tuple(edge_orientation::Top, -1, 2.25, make_point({18.500000, 8.500000})),
            std::make_tuple(edge_orientation::Top, 1, 0.25, make_point({18.000000, 8.000000})),
            std::make_tuple(edge_orientation::Right, -1, 1.47265, make_point({18.000000, 9.000000})),
            std::make_tuple(edge_orientation::Right, -1, 0.00277772, make_point({18.442300, 7.575660})),
            std::make_tuple(edge_orientation::Bottom, -1, 1.14011, make_point({18.500000, 8.500000})),
            std::make_tuple(edge_orientation::Bottom, -1, 0.00247992, make_point({17.500000, 7.500000}))
    };
    ExpectedIntersections cell2{
            std::make_tuple(edge_orientation::Left, -1, 1.47265, make_point({18.000000, 9.000000})),
            std::make_tuple(edge_orientation::Left, -1, 0.00277772, make_point({18.442300, 7.575660})),
            std::make_tuple(edge_orientation::Top, 1, 0.0595443, make_point({18.866025, 8.500000})),
            std::make_tuple(edge_orientation::Top, -1, 0.829345, make_point({18.500000, 8.500000})),
            std::make_tuple(edge_orientation::Bottom, 1, 0.0466185, make_point({18.823870, 8.500000})),
            std::make_tuple(edge_orientation::Bottom, -1, 0.0857591, make_point({18.500000, 8.500000}))
    };
    ExpectedIntersections cell4{
            std::make_tuple(edge_orientation::Right, 1, 0.442829, make_point({18.147970, 8.523493})),
            std::make_tuple(edge_orientation::Right, -1, 0.118202, make_point({18.442300, 7.575660})),
            std::make_tuple(edge_orientation::Bottom, -1, 2.91247, make_point({19.000000, 8.000000})),
            std::make_tuple(edge_orientation::Bottom, 1, 0.73266, make_point({19.144044, 8.288089}))
    };
    ExpectedIntersections cell5{
            std::make_tuple(edge_orientation::Left, 1, 0.442829, make_point({18.147970, 8.523493})),
            std::make_tuple(edge_orientation::Left, -1, 0.118202, make_point({18.442300, 7.575660})),
            std::make_tuple(edge_orientation::Bottom, 1, 0.0537631, make_point({19.347803, 8.000000})),
            std::make_tuple(edge_orientation::Bottom, -1, 0.951539, make_point({19.000000, 8.000000}))
    };

    ExpectedFsd expected{ cell1, cell2, cell4, cell5 };

    using vertex_orientation = FreeSpaceCellTraits::vertex_orientation;
    typedef  std::vector<std::vector<std::size_t>> ExpectedVertices;
    std::vector<std::size_t> vertices_cell_1{vertex_orientation::BottomLeft,
                                                    vertex_orientation::TopRight,
                                                    vertex_orientation::BottomRight};

    std::vector<std::size_t> vertices_cell_2{vertex_orientation::BottomLeft,
                                             vertex_orientation::TopLeft};

    std::vector<std::size_t> vertices_cell_4{vertex_orientation::BottomRight};

    std::vector<std::size_t> vertices_cell_5{vertex_orientation::BottomLeft};

    ExpectedVertices expected_free_vertices{vertices_cell_1,
                                                                 vertices_cell_2,
                                                                 vertices_cell_4,
                                                                 vertices_cell_5};

    std::vector<MovetkGeometryKernel::MovetkPoint> polyline_a{make_point({18.4423, 7.57566}), make_point({18, 9})};

    std::vector<MovetkGeometryKernel::MovetkPoint> polyline_b{make_point({17.5, 7.5}), make_point({18.5, 8.5}),
                                                             make_point({20, 8.5}), make_point({20, 10}),
                                                             make_point({19, 8}), make_point({20.5, 8}),
                                                             make_point({21.5, 9})};
    FreeSpaceDiagram fsd(std::begin(polyline_a), std::end(polyline_a),
                         std::begin(polyline_b), std::end(polyline_b), 1);

    std::size_t cell_idx = 1;
    auto eit = movetk_core::movetk_grid_iterator<ExpectedFsd>(expected.begin());
    auto it = expected.begin();
    auto evit = movetk_core::movetk_grid_iterator<ExpectedVertices>(expected_free_vertices.begin());
    std::cout << "CellId, EdgeID, Intersection_Ratio, Enclosed, Intersects\n";
    for (auto cell: fsd) {
        std::size_t num_intersections = 0;
        bool does_intersect = false;
        for (auto intersection: cell) {
            // SIGN_DISCRIMINANT = -1 AND SQUARED_RATIO = -1 implies that there is no intersection
            // SIGN_DISCRIMINANT = -1 AND SQUARED_RATIO >= 0 implies that the end point of the segment is inside the sphere
            if (std::get<IntersectionTraits::Attributes::SQUARED_RATIO>(intersection) != -1) {
                std::cout << cell_idx << "," <<
                          std::get<IntersectionTraits::Attributes::ID>(intersection) << "," <<
                          std::get<IntersectionTraits::Attributes::SIGN_DISCRIMINANT>(intersection) << "," <<
                          std::get<IntersectionTraits::Attributes::SQUARED_RATIO>(intersection) << "," <<
                          std::get<IntersectionTraits::Attributes::POINT>(intersection) << ",";
                if (std::get<IntersectionTraits::Attributes::SIGN_DISCRIMINANT>(intersection) == -1) {
                    std::cout << "1,0\n";
                } else {
                    std::cout << "0,1\n";
                }

                REQUIRE (std::get<IntersectionTraits::Attributes::ID>(intersection) ==
                         std::get<IntersectionTraits::Attributes::ID>(*eit));
                REQUIRE (std::get<IntersectionTraits::Attributes::SIGN_DISCRIMINANT>(intersection) ==
                         std::get<IntersectionTraits::Attributes::SIGN_DISCRIMINANT>(*eit));
                REQUIRE (abs(std::get<IntersectionTraits::Attributes::SQUARED_RATIO>(intersection) -
                             std::get<IntersectionTraits::Attributes::SQUARED_RATIO>(*eit)) < MOVETK_EPS);
                MovetkGeometryKernel::MovetkVector v = std::get<IntersectionTraits::Attributes::POINT>(intersection) -
                                                      std::get<IntersectionTraits::Attributes::POINT>(*eit);
                REQUIRE(v * v < MOVETK_EPS);
                eit++;
                num_intersections++;
                does_intersect = true;
            }
        }

        if (does_intersect) {
            std::cout << "Num Intersections: " << num_intersections << "\n";
            REQUIRE(num_intersections == it->size());
            std::cout << "Free Vertices: {";
            auto vit = cell.vertices_begin();
            while (vit != cell.vertices_end()) {
                std::cout << *vit << ",";
                REQUIRE (*vit == *evit);
                vit++;
                evit++;
            }
            std::cout << "}\n";
            it++;
        }
        cell_idx++;
    }


}


TEST_CASE("Check free space diagram 2", "[free_space_diagram_2]") {
    typedef typename MovetkGeometryKernel::NT NT;
    typedef typename MovetkGeometryKernel::MovetkPoint MovetkPoint;
    typedef movetk_core::IntersectionTraits<MovetkGeometryKernel, Norm, movetk_core::sphere_segment_intersection_tag> IntersectionTraits;
    typedef movetk_support::FreeSpaceCellTraits<IntersectionTraits> FreeSpaceCellTraits;
    typedef movetk_support::FreeSpaceCell<FreeSpaceCellTraits> FreeSpaceCell;
    typedef movetk_support::FreeSpaceDiagramTraits<FreeSpaceCell> FreeSpaceDiagramTraits;
    typedef movetk_support::FreeSpaceDiagram<FreeSpaceDiagramTraits> FreeSpaceDiagram;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    typedef std::vector<IntersectionTraits::value_type> ExpectedIntersections;
    typedef std::vector<ExpectedIntersections> ExpectedFsd;
    using edge_orientation = FreeSpaceCellTraits::edge_orientation;
    ExpectedIntersections cell1{
            std::make_tuple(edge_orientation::Left, 1, 0.158384, make_point({18.266276, 8.142512})),
            std::make_tuple(edge_orientation::Left, -1, 0.0144317, make_point({18.442300, 7.575660})),
            std::make_tuple(edge_orientation::Top, -1, 2.25, make_point({18.500000, 8.500000})),
            std::make_tuple(edge_orientation::Top, 1, 0.25, make_point({18.000000, 8.000000})),
            std::make_tuple(edge_orientation::Right, -1, 1.47265, make_point({18.000000, 9.000000})),
            std::make_tuple(edge_orientation::Right, -1, 0.00277772, make_point({18.442300, 7.575660})),
            std::make_tuple(edge_orientation::Bottom, -1, 1.14011, make_point({18.500000, 8.500000})),
            std::make_tuple(edge_orientation::Bottom, -1, 0.00247992, make_point({17.500000, 7.500000}))
    };
    ExpectedIntersections cell2{
            std::make_tuple(edge_orientation::Left, -1, 1.47265, make_point({18.000000, 9.000000})),
            std::make_tuple(edge_orientation::Left, -1, 0.00277772, make_point({18.442300, 7.575660})),
            std::make_tuple(edge_orientation::Top, 1, 0.0595443, make_point({18.866025, 8.500000})),
            std::make_tuple(edge_orientation::Top, -1, 0.829345, make_point({18.500000, 8.500000})),
            std::make_tuple(edge_orientation::Bottom, 1, 0.0466185, make_point({18.823870, 8.500000})),
            std::make_tuple(edge_orientation::Bottom, -1, 0.0857591, make_point({18.500000, 8.500000}))
    };
    ExpectedIntersections cell4{
            std::make_tuple(edge_orientation::Right, 1, 0.442829, make_point({18.147970, 8.523493})),
            std::make_tuple(edge_orientation::Right, -1, 0.118202, make_point({18.442300, 7.575660})),
            std::make_tuple(edge_orientation::Bottom, -1, 2.91247, make_point({19.000000, 8.000000})),
            std::make_tuple(edge_orientation::Bottom, 1, 0.73266, make_point({19.144044, 8.288089}))
    };
    ExpectedIntersections cell5{
            std::make_tuple(edge_orientation::Left, 1, 0.442829, make_point({18.147970, 8.523493})),
            std::make_tuple(edge_orientation::Left, -1, 0.118202, make_point({18.442300, 7.575660})),
            std::make_tuple(edge_orientation::Bottom, 1, 0.0537631, make_point({19.347803, 8.000000})),
            std::make_tuple(edge_orientation::Bottom, -1, 0.951539, make_point({19.000000, 8.000000}))
    };

    ExpectedFsd expected{ cell1, cell2, cell4, cell5 };

    using vertex_orientation = FreeSpaceCellTraits::vertex_orientation;
    typedef  std::vector<std::vector<std::size_t>> ExpectedVertices;
    std::vector<std::size_t> vertices_cell_1{vertex_orientation::BottomLeft,
                                             vertex_orientation::TopRight,
                                             vertex_orientation::BottomRight};

    std::vector<std::size_t> vertices_cell_2{vertex_orientation::BottomLeft,
                                             vertex_orientation::TopLeft};

    std::vector<std::size_t> vertices_cell_4{vertex_orientation::BottomRight};

    std::vector<std::size_t> vertices_cell_5{vertex_orientation::BottomLeft};

    std::vector<std::size_t> vertices_cell_7{vertex_orientation::BottomRight};

    std::vector<std::size_t> vertices_cell_8{vertex_orientation::BottomLeft,
                                             vertex_orientation::TopRight};

    std::vector<std::size_t> vertices_cell_9{vertex_orientation::TopLeft,
                                             vertex_orientation::TopRight};

    std::vector<std::size_t> vertices_cell_10{vertex_orientation::TopLeft};

    std::vector<std::size_t> vertices_cell_11{vertex_orientation::TopRight};

    std::vector<std::size_t> vertices_cell_12{vertex_orientation::TopLeft};

    std::vector<std::size_t> vertices_cell_14{vertex_orientation::TopRight,
                                              vertex_orientation::BottomRight};

    std::vector<std::size_t> vertices_cell_15{vertex_orientation::BottomLeft,
                                              vertex_orientation::TopLeft,
                                              vertex_orientation::BottomRight};

    std::vector<std::size_t> vertices_cell_16{vertex_orientation::BottomLeft};

    std::vector<std::size_t> vertices_cell_17{vertex_orientation::TopRight,
                                              vertex_orientation::BottomRight};

    std::vector<std::size_t> vertices_cell_18{vertex_orientation::BottomLeft,
                                              vertex_orientation::TopLeft};

    std::vector<std::size_t> vertices_cell_20{vertex_orientation::BottomRight};

    std::vector<std::size_t> vertices_cell_21{vertex_orientation::BottomLeft};

    std::vector<std::size_t> vertices_cell_23{vertex_orientation::BottomRight};

    std::vector<std::size_t> vertices_cell_24{vertex_orientation::BottomLeft,
                                              vertex_orientation::TopRight};


    ExpectedVertices expected_free_vertices{vertices_cell_1,
                                            vertices_cell_2,
                                            vertices_cell_4,
                                            vertices_cell_5,
                                            vertices_cell_7,
                                            vertices_cell_8,
                                            vertices_cell_9,
                                            vertices_cell_10,
                                            vertices_cell_11,
                                            vertices_cell_12,
                                            vertices_cell_14,
                                            vertices_cell_15,
                                            vertices_cell_16,
                                            vertices_cell_17,
                                            vertices_cell_18,
                                            vertices_cell_20,
                                            vertices_cell_21,
                                            vertices_cell_23,
                                            vertices_cell_24};

    std::vector<MovetkGeometryKernel::MovetkPoint> polyline_a{make_point({18.4423, 7.57566}),
                                                              make_point({18, 8.967887}),
                                                              make_point({20.3636, 8.969}), make_point({20.35458, 8.4889}),
                                                              make_point({21.4, 9.4})};

    std::vector<MovetkGeometryKernel::MovetkPoint> polyline_b{make_point({17.5, 7.5}), make_point({18.5, 8.5}),
                                                              make_point({20, 8.5}), make_point({20, 9.5}),
                                                              make_point({19, 8}), make_point({20.3369, 8}),
                                                              make_point({21.4, 8.92048})};
    FreeSpaceDiagram fsd(std::begin(polyline_a), std::end(polyline_a),
                         std::begin(polyline_b), std::end(polyline_b), 1);

    std::size_t cell_idx = 1;
    auto eit = movetk_core::movetk_grid_iterator<ExpectedFsd>(expected.begin());
    auto it = expected.begin();
    auto evit = movetk_core::movetk_grid_iterator<ExpectedVertices>(expected_free_vertices.begin());
    std::cout << "CellId, EdgeID, Intersection_Ratio, Enclosed, Intersects\n";
    for (auto cell: fsd) {
        std::size_t num_intersections = 0;
        bool does_intersect = false;
        for (auto intersection: cell) {
            // SIGN_DISCRIMINANT = -1 AND SQUARED_RATIO = -1 implies that there is no intersection
            // SIGN_DISCRIMINANT = -1 AND SQUARED_RATIO >= 0 implies that the end point of the segment is inside the sphere
            if (std::get<IntersectionTraits::Attributes::SQUARED_RATIO>(intersection) != -1) {
                std::cout << cell_idx << "," <<
                          std::get<IntersectionTraits::Attributes::ID>(intersection) << "," <<
                          std::get<IntersectionTraits::Attributes::SIGN_DISCRIMINANT>(intersection) << "," <<
                          std::get<IntersectionTraits::Attributes::SQUARED_RATIO>(intersection) << "," <<
                          std::get<IntersectionTraits::Attributes::POINT>(intersection) << ",";
                if (std::get<IntersectionTraits::Attributes::SIGN_DISCRIMINANT>(intersection) == -1) {
                    std::cout << "1,0\n";
                } else {
                    std::cout << "0,1\n";
                }
                num_intersections++;
                does_intersect = true;
            }
        }

        if (does_intersect) {
            //std::cout << "Num Intersections: " << num_intersections << "\n";
            //REQUIRE(num_intersections == it->size());
            std::cout << "Free Vertices: {";
            auto vit = cell.vertices_begin();
            while (vit != cell.vertices_end()) {
                std::cout << *vit << ",";
                REQUIRE (*vit == *evit);
                vit++;
                evit++;
            }
            std::cout << "}\n";
            it++;
        }
        cell_idx++;
    }

}
