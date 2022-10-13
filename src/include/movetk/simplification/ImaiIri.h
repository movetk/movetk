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
 // Created by Mitra, Aniket on 2019-02-14.
 //
#ifndef MOVETK_ALGO_SIMPLIFICATION_IMAIIRI_H
#define MOVETK_ALGO_SIMPLIFICATION_IMAIIRI_H
#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/DistanceInterface.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/Requirements.h"
namespace movetk::simplification {

template <class GeometryTraits, class EdgeCreator, typename = movetk::utils::requires_planar_geometry<GeometryTraits>>
class ImaiIri {
	// based on https://doi.org/10.1142/S0218195996000058
private:
	typedef typename GeometryTraits::NT NT;
	NT eps;
	EdgeCreator create_edges;
	typedef boost::adjacency_list<boost::vecS,
	                              boost::vecS,
	                              boost::directedS,
	                              boost::property<boost::vertex_index_t, int>,
	                              boost::property<boost::edge_index_t, int>>
	    Graph;
	typedef typename boost::graph_traits<Graph>::vertex_iterator vertex_iterator;
	typedef typename boost::graph_traits<Graph>::edge_iterator edge_iterator;
	typedef boost::property_map<Graph, boost::vertex_index_t>::type VertexId_PMap;

public:
	ImaiIri(NT epsilon) : eps(epsilon) { create_edges = EdgeCreator(eps); }

	template <class InputIterator,
	          class OutputIterator,
	          typename = movetk::utils::requires_random_access_iterator<InputIterator>,
	          typename = movetk::utils::requires_output_iterator<OutputIterator>,
	          typename = movetk::utils::requires_movetk_point<GeometryTraits, typename InputIterator::value_type>,
	          typename = movetk::utils::requires_equality<typename InputIterator::value_type,
	                                                      typename OutputIterator::value_type::value_type>>
	void operator()(InputIterator first, InputIterator beyond, OutputIterator result) {
		std::vector<std::pair<std::size_t, std::size_t>> edges;
		std::size_t NumElems = std::distance(first, beyond);
		std::vector<std::size_t> indexes;
		std::vector<std::size_t> predecessors(NumElems);
		std::size_t start_idx = 0;
		predecessors[start_idx] = 0;

		create_edges(first, beyond, movetk::utils::movetk_back_insert_iterator(edges));

		Graph graph{edges.begin(), edges.end(), NumElems};

		VertexId_PMap vertex_index = boost::get(boost::vertex_index, graph);

		boost::iterator_property_map<std::vector<std::size_t>::iterator, VertexId_PMap, std::size_t, std::size_t&>
		    predecessors_pa(predecessors.begin(), vertex_index);

		auto predecessor_recorder = boost::record_predecessors(predecessors_pa, boost::on_tree_edge{});
		auto bfs_visitor = boost::make_bfs_visitor(predecessor_recorder);
		boost::breadth_first_search(graph, start_idx, boost::visitor(bfs_visitor));

		vertex_iterator vi, vi_end;
		std::tie(vi, vi_end) = boost::vertices(graph);
		auto idx = boost::get(predecessors_pa, *(vi_end - 1));

		indexes.push_back(*(vi_end - 1));
		while (idx != boost::get(predecessors_pa, *vi)) {
			indexes.push_back(idx);
			idx = predecessors_pa[idx];
		}
		indexes.push_back(idx);
		std::reverse(std::begin(indexes), std::end(indexes));
		for (auto& idx : indexes) {
			*result = first + idx;
		}
		graph.clear();
	}
};

}  // namespace movetk::simplification
#endif