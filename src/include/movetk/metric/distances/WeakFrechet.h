/*
 * Copyright (C) 2018-2022 TU Eindhoven
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
// Created by Custers, Bram on 2022-03-11
//

#ifndef MOVETK_METRIC_DISTANCES_WEAKFRECHET_H
#define MOVETK_METRIC_DISTANCES_WEAKFRECHET_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <iostream>

#include "movetk/geom/GeometryInterface.h"
#include "movetk/utils/Asserts.h"

namespace movetk::metric {
/**
 * \brief Implementation of the weak Frechet distance between two polylines
 *  Based on 'Computing the Fréchet distance between two polygonal curves', Alt & Godau, 1995
 *  http://www.staff.science.uu.nl/~kreve101/asci/ag-cfdbt-95.pdf
 * \tparam GeometryTraits The geometry kernel
 * \tparam SquaredDistance Squared difference computer that supports computing the distance between a point and a
 * segment
 */
template <class GeometryTraits, class SquaredDistance>
class WeakFrechet {
	using NT = typename GeometryTraits::NT;
	using FreeSpaceGraph = boost::adjacency_list<boost::vecS,
	                                             boost::vecS,
	                                             boost::undirectedS,
	                                             boost::no_property,
	                                             boost::property<boost::edge_weight_t, NT>>;
	using FreeSpaceGraphTraits = boost::graph_traits<FreeSpaceGraph>;
	using FreeSpaceGraphWeightMap = typename boost::property_map<FreeSpaceGraph, boost::edge_weight_t>::type;

	// Squared norm to use
	SquaredDistance m_sqDist;
	geom::MakeSegment<GeometryTraits> make_segment;

	// Combination function for searching minimax paths via boost dijkstra.
	struct MiniMaxCombineDistance {
		NT operator()(NT dist, NT weight) const { return std::max(dist, weight); }
	};

	/**
	 * \brief Setup the freespace grid-graph for computing the weak Frechet distance
	 * \tparam InIt Input iterator for points of polylines
	 * \param polyA First polyline, specified as a pair of input iterators (start and beyond)
	 * \param polyB Second polyline, specified as a pair of input iterators (start and beyond)
	 * \param freeSpaceGraph Output freespace graph
	 * \param weightMap Output weightmap for the edges of the freespace graph.
	 */
	template <typename InIt>
	void setup_freespace_graph(std::pair<InIt, InIt> polyA,
	                         std::pair<InIt, InIt> polyB,
	                         FreeSpaceGraph &freeSpaceGraph,
	                         FreeSpaceGraphWeightMap &weightMap) {
		const auto polyASegCount = std::distance(polyA.first, polyA.second) - 1;
		const auto polyBSegCount = std::distance(polyB.first, polyB.second) - 1;

		// Convert grid indices to single vector index
		auto vertIndex = [polyASegCount](int i, int j) { return i + j * polyASegCount; };

		const auto vertexCount = polyASegCount * polyBSegCount;
		freeSpaceGraph = FreeSpaceGraph(vertexCount);
		// Get the property map representing edge weights on the graph.
		weightMap = boost::get(boost::edge_weight, freeSpaceGraph);
		{
			auto pointAIt = polyA.first;
			for (int i = 0; i < polyASegCount; ++i, ++pointAIt) {
				// Create the segment
				auto seg = make_segment(*pointAIt, *std::next(pointAIt));
				// Determine closest distances and store as weights
				auto pointBIt = polyB.first;
				for (int j = 0; j < polyBSegCount - 1; ++j, ++pointBIt) {
					auto startVert = boost::vertex(vertIndex(i, j), freeSpaceGraph);
					auto endVert = boost::vertex(vertIndex(i, j + 1), freeSpaceGraph);
					auto edgeDescriptor = boost::add_edge(startVert, endVert, freeSpaceGraph);
					weightMap[edgeDescriptor.first] = m_sqDist(seg, *pointBIt);
				}
			}
		}
		{
			// Create horizontal edges in the freespace graph
			auto pointBIt = polyB.first;
			for (int j = 0; j < polyBSegCount; ++j, ++pointBIt) {
				// Create the segment
				auto seg = make_segment(*pointBIt, *std::next(pointBIt));
				// Determine closest distances and store as weights
				auto pointAIt = polyA.first;
				for (int i = 0; i < polyASegCount - 1; ++i, ++pointAIt) {
					auto startVert = boost::vertex(vertIndex(i, j), freeSpaceGraph);
					auto endVert = boost::vertex(vertIndex(i + 1, j), freeSpaceGraph);
					auto edgeDescriptor = boost::add_edge(startVert, endVert, freeSpaceGraph);
					weightMap[edgeDescriptor.first] = m_sqDist(seg, *pointAIt);
				}
			}
		}
	}

public:
	WeakFrechet() {}
	/**
	 * @brief Computes the weak Frechet distance between two polylines
	 * @tparam InputIterator Iterator type for polyline iterators
	 * @param polyline_a_first Start of first polyline
	 * @param polyline_a_beyond End of first polyline
	 * @param polyline_b_first Start of second polyline
	 * @param polyline_b_beyond End of second polyline
	 * @return The weak Frechet distance.
	 */
	template <class InputIterator,
	          typename = movetk::utils::requires_random_access_iterator<InputIterator>,
	          typename = movetk::utils::requires_movetk_point<GeometryTraits, typename InputIterator::value_type>>
	typename GeometryTraits::NT operator()(InputIterator polyline_a_first,
	                                       InputIterator polyline_a_beyond,
	                                       InputIterator polyline_b_first,
	                                       InputIterator polyline_b_beyond) {
		// Point counts of the polylines
		const auto polyASize = std::distance(polyline_a_first, polyline_a_beyond);
		const auto polyBSize = std::distance(polyline_b_first, polyline_b_beyond);

		// Distances between starts resp. ends of the polylines (squared)
		const auto startMatchDist = m_sqDist(*polyline_a_first, *polyline_b_first);
		const auto endMatchDist = m_sqDist(*std::prev(polyline_a_beyond), *std::prev(polyline_b_beyond));

		// Special case input
		if (polyASize <= 2 && polyBSize <= 2) {
			return std::sqrt(std::max(startMatchDist, endMatchDist));
		}

		// Setup free space grid graph. The weights on the edges are the minimum distances for the cell border, represented
		// by that edge, to open
		FreeSpaceGraph freeSpaceGraph;
		FreeSpaceGraphWeightMap weightMap;
		setup_freespace_graph(std::make_pair(polyline_a_first, polyline_a_beyond),
		                    std::make_pair(polyline_b_first, polyline_b_beyond),
		                    freeSpaceGraph,
		                    weightMap);

		std::vector<NT> distances(boost::num_vertices(freeSpaceGraph));
		auto distanceMap = boost::make_iterator_property_map(distances.begin(), get(boost::vertex_index, freeSpaceGraph));

		// Find min max path, uses default less comparison for weights and custom combine fucntion
		boost::dijkstra_shortest_paths(
		    freeSpaceGraph,
		    boost::vertex(0, freeSpaceGraph),
		    boost::distance_zero(0).distance_map(distanceMap).distance_combine(MiniMaxCombineDistance()));

		const auto freeSpaceMatchDist = distances[boost::num_vertices(freeSpaceGraph) - 1];

		return std::sqrt(std::max({startMatchDist, endMatchDist, freeSpaceMatchDist}));
	}

	/**
	 * \brief Computes the weak Frechet distance and returns a matching between the polylines that witnesses the returned
	 * distance. \tparam InputIterator Type of the input iterators, specifying the input polylines as points \tparam
	 * OutputIterator Type of the output iterator that accepts the matching in the form of std::pair<std::pair<int,int>,
	 * NT> elements \param polyline_a_first Start of the first polyline \param polyline_a_beyond End of the first polyline
	 * \param polyline_b_first Start of the second polyline
	 * \param polyline_b_beyond End of the second polyline
	 * \param matching_output Output iterator that will be supplied with the matching. The matching is described as a pair
	 * of indices giving the segment indices on polyline_a resp. polyline_b and a minimum distance to reach the next
	 * segment pair in the returned list. Note that the matching start with an entry of {{-1,-1}, startMatchDist} where
	 * startMatchDist is the distance to match the starting points of the two polylines. The minimum distance of the last
	 * entry is the distance to match the endpoints of the polylines. \return The weak Frechet distance between the given
	 * polylines.
	 */
	template <class InputIterator,
	          class OutputIterator,
	          typename = movetk::utils::requires_random_access_iterator<InputIterator>,
	          typename = movetk::utils::requires_movetk_point<GeometryTraits, typename InputIterator::value_type>,
	          typename = movetk::utils::requires_equality<std::pair<std::pair<int, int>, NT>,
	                                                      typename OutputIterator::value_type>>
	typename GeometryTraits::NT operator()(InputIterator polyline_a_first,
	                                       InputIterator polyline_a_beyond,
	                                       InputIterator polyline_b_first,
	                                       InputIterator polyline_b_beyond,
	                                       OutputIterator matching_output) {
		// Point counts of the polylines
		const auto polyASize = std::distance(polyline_a_first, polyline_a_beyond);
		const auto polyBSize = std::distance(polyline_b_first, polyline_b_beyond);

		// Distances between starts resp. ends of the polylines (squared)
		const auto startMatchDist = m_sqDist(*polyline_a_first, *polyline_b_first);
		const auto endMatchDist = m_sqDist(*std::prev(polyline_a_beyond), *std::prev(polyline_b_beyond));

		// Special case input
		if (polyASize <= 2 && polyBSize <= 2) {
			auto res = std::sqrt(std::max(startMatchDist, endMatchDist));
			// Return the segment matching path and the associated distance for the segment
			*matching_output = std::make_pair(std::make_pair(0, 0), res);
			return res;
		}

		// Setup free space grid graph. The weights on the edges are the minimum distances for the cell border, represented
		// by that edge, to open
		FreeSpaceGraph freeSpaceGraph;
		FreeSpaceGraphWeightMap weightMap;
		setup_freespace_graph(std::make_pair(polyline_a_first, polyline_a_beyond),
		                    std::make_pair(polyline_b_first, polyline_b_beyond),
		                    freeSpaceGraph,
		                    weightMap);

		std::vector<NT> distances(boost::num_vertices(freeSpaceGraph));
		auto distanceMap = boost::make_iterator_property_map(distances.begin(), get(boost::vertex_index, freeSpaceGraph));

		// Create the predecessors map that will track how we got to the minimax path value
		std::vector<typename FreeSpaceGraphTraits::vertex_descriptor> predecessors(boost::num_vertices(freeSpaceGraph));
		auto predecessorsMap =
		    boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index, freeSpaceGraph));

		// Find min max path, uses default less comparison for weights and custom combine fucntion
		boost::dijkstra_shortest_paths(freeSpaceGraph,
		                               boost::vertex(0, freeSpaceGraph),
		                               // Named arguments
		                               boost::distance_zero(0)
		                                   .distance_map(distanceMap)
		                                   .distance_combine(MiniMaxCombineDistance())
		                                   .predecessor_map(predecessorsMap));

		const auto freeSpaceMatchDist = distances[boost::num_vertices(freeSpaceGraph) - 1];

		auto vertDescToIndices = [polyASize](typename FreeSpaceGraph::vertex_descriptor desc) {
			int j = (int)(desc / (polyASize - 1));
			int i = desc - j * (polyASize - 1);
			return std::make_pair(i, j);
		};
		auto val = std::sqrt(std::max({startMatchDist, endMatchDist, freeSpaceMatchDist}));

		// Reconstruct the path in the freespace graph
		std::vector<std::pair<std::pair<int, int>, NT>> matching;
		auto matchingInserter = std::back_inserter(matching);
		auto curr = boost::vertex(boost::num_vertices(freeSpaceGraph) - 1, freeSpaceGraph);
		*matchingInserter = std::make_pair(vertDescToIndices(curr), std::sqrt(endMatchDist));
		while (true) {
			auto prev = curr;
			curr = predecessors[curr];
			auto edge = boost::edge(curr, prev, freeSpaceGraph);
			auto weight = weightMap[edge.first];
			*matchingInserter = std::make_pair(vertDescToIndices(curr), std::sqrt(weight));
			if (curr == boost::vertex(0, freeSpaceGraph))
				break;
		}
		// Add the start matching
		*matchingInserter = std::make_pair(std::make_pair(-1, -1), startMatchDist);

		// Reverse to get forward order matching
		std::reverse(matching.begin(), matching.end());
		// Copy to output
		std::copy(matching.begin(), matching.end(), matching_output);

		return std::sqrt(std::max({startMatchDist, endMatchDist, freeSpaceMatchDist}));
	}
};
}
#endif