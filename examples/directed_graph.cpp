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

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/property_map/property_map.hpp>

#include "movetk/utils/GeometryBackendTraits.h"
#include "movetk/utils/Iterators.h"

int main(int argc, char **argv) {
#if CGAL_BACKEND_ENABLED
	std::cerr << "Using CGAL Backend for Geometry\n";
#else
	std::cerr << "Using Boost Backend for Geometry\n";
#endif

	std::cout.setf(std::ios::fixed);

	movetk::geom::MakePoint<GeometryKernel::MovetkGeometryKernel> make_point;
	typedef std::vector<GeometryKernel::MovetkGeometryKernel::MovetkPoint> PolyLine;
	typedef boost::adjacency_list<boost::vecS,
	                              boost::vecS,
	                              boost::directedS,
	                              boost::property<boost::vertex_index_t, int>,
	                              boost::property<boost::edge_index_t, int>>
	    Graph;
	typedef typename boost::graph_traits<Graph>::vertex_iterator vertex_iterator;
	typedef typename boost::graph_traits<Graph>::edge_iterator edge_iterator;
	typedef boost::property_map<Graph, boost::vertex_index_t>::type VertexId_PMap;

	PolyLine polyline(
	    {make_point({-6.19, -3.46}), make_point({-4.99, 1.16}), make_point({-2.79, -2.22}), make_point({-1.87, 0.58})});

	std::cout << "Polyline: ";
	std::cout << "{";
	for (auto &vertex : polyline) {
		std::cout << vertex;
		std::cout << ";";
	}
	std::cout << "}\n";

	std::vector<std::pair<std::size_t, std::size_t>> edges({
	    std::make_pair(0, 1),
	    std::make_pair(1, 2),
	    std::make_pair(0, 2),
	    std::make_pair(2, 3),
	});

	std::cout << "Edges: ";
	std::cout << "{";
	for (auto &i : edges) {
		std::cout << i.first << "," << i.second << ";";
	}
	std::cout << "}\n";

	Graph g{edges.begin(), edges.end(), polyline.size()};

	std::cout << "Number of vertices: " << boost::num_vertices(g) << '\n'
	          << "Number of edges: " << boost::num_edges(g) << '\n';

	std::vector<std::size_t> distances(polyline.size());
	std::vector<std::size_t> predecessors(polyline.size());
	predecessors[0] = 0;

	VertexId_PMap vertex_index = boost::get(boost::vertex_index, g);

	boost::iterator_property_map<std::vector<std::size_t>::iterator, VertexId_PMap, std::size_t, std::size_t &>
	    distances_pa(distances.begin(), vertex_index);

	boost::iterator_property_map<std::vector<std::size_t>::iterator, VertexId_PMap, std::size_t, std::size_t &>
	    predecessors_pa(predecessors.begin(), vertex_index);

	auto distance_recorder = boost::record_distances(distances_pa, boost::on_tree_edge{});
	auto predecessor_recorder = boost::record_predecessors(predecessors_pa, boost::on_tree_edge{});
	auto bfs_visitor = boost::make_bfs_visitor(std::make_pair(distance_recorder, predecessor_recorder));
	boost::breadth_first_search(g, 0, boost::visitor(bfs_visitor));

	vertex_iterator vi, vi_end;
	for (std::tie(vi, vi_end) = boost::vertices(g); vi != vi_end; ++vi) {
		std::cout << "vertex index: " << *vi << ", distance: " << boost::get(distances_pa, *vi) << std::endl;
	}

	for (std::tie(vi, vi_end) = boost::vertices(g); vi != vi_end; ++vi) {
		std::cout << "vertex index: " << *vi << ", predecessor: " << boost::get(predecessors_pa, *vi) << std::endl;
	}

	std::tie(vi, vi_end) = boost::vertices(g);
	auto idx = boost::get(predecessors_pa, *(vi_end - 1));
	std::vector<std::size_t> indexes;
	indexes.push_back(*(vi_end - 1));
	while (idx != boost::get(predecessors_pa, *vi)) {
		indexes.push_back(idx);
		idx = predecessors_pa[idx];
	}
	indexes.push_back(idx);
	std::reverse(std::begin(indexes), std::end(indexes));

	std::cout << "Shortest Path Polyline: ";
	std::cout << "{";
	for (auto &idx : indexes) {
		std::cout << polyline[idx] << ";";
	}
	std::cout << "}\n";
}
