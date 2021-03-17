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
// Created by Mitra, Aniket on 2019-04-03.
//

#include "movetk/utils/GeometryBackendTraits.h"
#include "movetk/utils/Iterators.h"
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>

// treat a trajectory as a directed graph and
// use the BFS algorithm to find the shortest path
int main(int argc, char **argv)
{
#if CGAL_BACKEND_ENABLED
    std::cerr << "Using CGAL Backend for Geometry\n";
#else
    std::cerr << "Using Boost Backend for Geometry\n";
#endif

    std::cout.setf(std::ios::fixed);

    // PolyLine is a data structure that to store a collection of points
    typedef std::vector<GeometryKernel::MovetkGeometryKernel::MovetkPoint> PolyLine;

    // Define a graph using boost::adjacency_list
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
                                  boost::property<boost::vertex_index_t, int>, boost::property<boost::edge_index_t, int>>
        Graph;

    // Vertex iterator type
    typedef typename boost::graph_traits<Graph>::vertex_iterator vertex_iterator;
    // Edge iterator type
    typedef typename boost::graph_traits<Graph>::edge_iterator edge_iterator;

    typedef boost::property_map<Graph, boost::vertex_index_t>::type VertexId_PMap;

    // functor to create a point
    movetk_core::MakePoint<GeometryKernel::MovetkGeometryKernel> make_point;

    // create polyline, each point in the polyline is vertex
    PolyLine polyline({make_point({-6.19, -3.46}), make_point({-4.99, 1.16}),
                       make_point({-2.79, -2.22}), make_point({-1.87, 0.58})});

    // print each vertex
    std::cout << "Polyline: ";
    std::cout << "{";
    for (auto &vertex : polyline)
    {
        std::cout << vertex;
        std::cout << ";";
    }
    std::cout << "}\n";

    // edges to connect vertex pairs
    std::vector<std::pair<std::size_t, std::size_t>> edges({
        std::make_pair(0, 1),
        std::make_pair(1, 2),
        std::make_pair(0, 2),
        std::make_pair(2, 3),
    });

    // print the edges
    std::cout << "Edges: ";
    std::cout << "{";
    for (auto &i : edges)
    {
        std::cout << i.first << "," << i.second << ";";
    }
    std::cout << "}\n";

    /** Pass iterator over edges and the number of vertices
     *  to create boost graph
    */
    Graph g{edges.begin(), edges.end(), polyline.size()};

    std::cout << "Number of vertices: " << boost::num_vertices(g) << '\n';
    std::cout << "Number of edges: " << boost::num_edges(g) << '\n';

    // container stores computed distances
    std::vector<std::size_t> distances(polyline.size());
    //
    std::vector<std::size_t> predecessors(polyline.size());

    predecessors[0] = 0;

    VertexId_PMap vertex_index = boost::get(boost::vertex_index, g);

    // iterator property map for recording distances
    boost::iterator_property_map<std::vector<std::size_t>::iterator, VertexId_PMap,
                                 std::size_t, std::size_t &>
        distances_pa(distances.begin(), vertex_index);

    /* iterator property map for recording the previous point as the
    * bfs algorithm proceeds to the next point
    */
    boost::iterator_property_map<std::vector<std::size_t>::iterator, VertexId_PMap,
                                 std::size_t, std::size_t &>
        predecessors_pa(predecessors.begin(), vertex_index);

    /** the helper function to create a visitor that record distances
    * distance is the number of lines that have to be crossed to get from one 
    * point to another starting from the point that is passed as a second 
    * argument to  boost::breadth_first_search
    */
    auto distance_recorder = boost::record_distances(distances_pa, boost::on_tree_edge{});
    /* returns a visitor to store the predecessor of every point
    *  whenever boost::breadth_first_search() visits a new point, the previous
    *  point is stored in the property map passed to boost::record_predecessors
    */
    auto predecessor_recorder = boost::record_predecessors(predecessors_pa, boost::on_tree_edge{});
    /* an adaptor that binds the two visitors distance_recorder & predecessor_recorder
    * to the algorithm 
    */
    auto bfs_visitor = boost::make_bfs_visitor(std::make_pair(distance_recorder, predecessor_recorder));

    // the bfs algorithm on graph g
    // bfs_visitor stores the result
    // the algorithm starts at the first point as specified in the second argument
    boost::breadth_first_search(g, 0, boost::visitor(bfs_visitor));

    /* iterate over the graph and print all vertices, the distance
    * between two vertices, and the predecessor for each vertex
    */
    vertex_iterator vi, vi_end;
    for (std::tie(vi, vi_end) = boost::vertices(g); vi != vi_end; ++vi)
    {
        std::cout << "vertex index: " << *vi << ", distance: " << boost::get(distances_pa, *vi) << std::endl;
    }

    for (std::tie(vi, vi_end) = boost::vertices(g); vi != vi_end; ++vi)
    {
        std::cout << "vertex index: " << *vi << ", predecessor: " << boost::get(predecessors_pa, *vi) << std::endl;
    }

    /* get the index of the vertices returned by the
    * shortest path
    */
    std::tie(vi, vi_end) = boost::vertices(g);
    auto idx = boost::get(predecessors_pa, *(vi_end - 1));
    std::vector<std::size_t> indexes;
    indexes.push_back(*(vi_end - 1));
    while (idx != boost::get(predecessors_pa, *vi))
    {
        indexes.push_back(idx);
        idx = predecessors_pa[idx];
    }
    indexes.push_back(idx);

    /* the order of the vertices returned by the bfs algorithm
    * starts with the last vertex, so reverse the indexes 
    * so that it can be used to access points from the polyline
    */
    std::reverse(std::begin(indexes), std::end(indexes));

    std::cout << "Shortest Path Polyline: ";
    std::cout << "{";
    for (auto &idx : indexes)
    {
        std::cout << polyline[idx] << ";";
    }
    std::cout << "}\n";
}
