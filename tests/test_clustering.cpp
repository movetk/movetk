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
// Created by Mitra, Aniket on 03/10/2019.
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
#include "movetk/algo/AlgorithmTraits.h"
#include "movetk/algo/Clustering.h"

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

TEST_CASE("Check subtrajectory clustering 1", "[subtrajectory_clustering_1]") {
    typedef typename MovetkGeometryKernel::NT NT;
    typedef typename MovetkGeometryKernel::MovetkPoint MovetkPoint;
    typedef movetk_core::IntersectionTraits<MovetkGeometryKernel, Norm, movetk_core::sphere_segment_intersection_tag> IntersectionTraits;
    typedef movetk_support::FreeSpaceCellTraits<IntersectionTraits> FreeSpaceCellTraits;
    typedef movetk_support::FreeSpaceCell<FreeSpaceCellTraits> FreeSpaceCell;
    typedef movetk_support::FreeSpaceDiagramTraits<FreeSpaceCell> FreeSpaceDiagramTraits;
    typedef movetk_support::FreeSpaceDiagram<FreeSpaceDiagramTraits> FreeSpaceDiagram;
    typedef movetk_algorithms::ClusteringTraits<FreeSpaceDiagram> ClusteringTraits;
    typedef movetk_algorithms::SubTrajectoryClustering<ClusteringTraits> SubTrajectoryClustering;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    std::vector<MovetkGeometryKernel::MovetkPoint> polyline_a{
            make_point({69.5, 10.5}), make_point({70, 11}),
            make_point({70.1511, 11.5301}), make_point({69.9253, 11.89154}),
            make_point({70.1511, 11.5301}),  make_point({69.9253, 11.89154}),
            make_point({70.5, 12.5}), make_point({71.2556, 12.5740}),
            make_point({71.85568, 13.574}), make_point({71, 14.5}),
            make_point({71.2714, 14.6743}), make_point({69.9343, 14.855}),
            make_point({68.624, 13.4003}), make_point({68.3802, 12.1896}),
            make_point({68.56099, 12.0632}), make_point({69.9975, 12.26197}),
            make_point({70.5, 13}), make_point({71.7277, 13}),
            make_point({71.669, 13.6624}), make_point({72.2472, 14.3942}),
            make_point({72.40986, 15.0899}), make_point({73.0152, 13.6443}),
            make_point({72.7622, 13.0028}),  make_point({73, 11}),
            make_point({71, 11}),  make_point({70.44929, 11.8825}),
            make_point({70.241488, 13.0570}), make_point({71.1359, 13.4365}),
            make_point({71.58768, 13.9243}),  make_point({71.74128, 14.45745}),
            make_point({71.62321, 15.1260}),  make_point({71.0094, 15.60489})
    };


    SubTrajectoryClustering clustering(std::begin(polyline_a),std::end(polyline_a), 3,0.5);
    std::cout<<"Number of edges:"<<std::distance(clustering.edges_begin(),clustering.edges_end())<<"\n";
    /*std::copy(clustering.edges_begin(), clustering.edges_end(),
              std::ostream_iterator<ClusteringTraits::Graph::edge_descriptor>{std::cout, "\n"});*/

    auto eit = clustering.edges_begin();
    std::cout<<"Edges: {";
    while (eit != clustering.edges_end()){
        std::cout<<"(";
        std::cout<<*eit;
        std::cout<<",";
        std::cout<<clustering.label[*eit];
        std::cout<<");";
        eit++;
    }
    std::cout<<"}\n";

    std::cout<<"Subtrajectory Length: "<<clustering.get_subtrajectory_cluster_length()<<"\n";
    std::cout<<"Subtrajectory Index: "<<clustering.get_subtrajectory_indices().first
             <<","<<clustering.get_subtrajectory_indices().second<<"\n";
    REQUIRE(clustering.get_subtrajectory_cluster_length() == 4);
    REQUIRE(clustering.get_subtrajectory_indices().first == 2);
    REQUIRE(clustering.get_subtrajectory_indices().second == 6);
}