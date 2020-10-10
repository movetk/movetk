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
// Created by Mitra, Aniket on 2019-06-03.
//

#ifndef MOVETK_SIMILARITY_H
#define MOVETK_SIMILARITY_H

#include <iterator>
#include <stdlib.h>
#include <boost/graph/grid_graph.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include "movetk/utils/Requirements.h"
#include "movetk/utils/Iterators.h"
#include "movetk/geom/GeometryInterface.h"


using namespace std;


namespace movetk_algorithms {


    template <class GeometryTraits, class Norm>
    class LongestCommonSubSequence{
        //based on doi=10.1.1.78.240
        //http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.78.240&rep=rep1&type=pdf
    private:
        typedef typename GeometryTraits::NT NT;
        NT eps;
        std::size_t del;
        template <class T>
        T abs(T& a, T& b){
            return a > b ? a - b : b - a;
        }
    public:
        LongestCommonSubSequence(NT epsilon, std::size_t delta){
            eps = epsilon;
            del = delta;
        }

        template <class InputIterator, class OutputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_output_iterator<OutputIterator>,
                typename = movetk_core::requires_movetk_point<GeometryTraits,
                        typename InputIterator::value_type>,
                typename = movetk_core::requires_pair<typename OutputIterator::value_type>,
                typename = movetk_core::requires_equality<typename InputIterator::value_type,
                        typename OutputIterator::value_type::first_type::value_type >,
                typename = movetk_core::requires_equality<typename InputIterator::value_type,
                        typename OutputIterator::value_type::second_type::value_type > >
        std::size_t operator()(InputIterator polyline_a_first, InputIterator polyline_a_beyond,
                        InputIterator polyline_b_first, InputIterator polyline_b_beyond, OutputIterator result){

            std::size_t size_polyline_b = std::distance( polyline_b_first, polyline_b_beyond );
            std::vector<std::size_t> dp_row (size_polyline_b + 1);
            std::fill( std::begin(dp_row), std::begin(dp_row) + size_polyline_b + 1, 0 );
            InputIterator it_a = polyline_a_first;
            std::size_t i = 1, prev_value = 0, prev_cell = 0;
            Norm norm;
            while (it_a != polyline_a_beyond){
                std::size_t j = 1, previous = 0, current = 0;
                InputIterator it_b = polyline_b_first;
                while (it_b != polyline_b_beyond){
                    typename GeometryTraits::MovetkVector v = *it_a - *it_b;
                    NT distance = norm(v);
                    if ( distance < eps && this->abs(i,j) < del ) {
                        current = dp_row[j - 1] + 1;
                        if ( current != prev_value ){
                            prev_value = current;
                            prev_cell = j;
                            *result = std::make_pair(it_a,it_b);
                        }
                        else{
                            if(j < prev_cell){
                                prev_cell = j;
                                *result -= std::make_pair(it_a,it_b);
                            }
                        }
                    }
                    else
                        current = std::max(dp_row[j], previous);
                    dp_row[j - 1] = previous;
                    previous = current;
                    j++;
                    it_b++;
                }
                dp_row[j - 1] = previous;
                it_a++;
                i++;
            }
            return dp_row.back();
        }
    };

    class DynamicTimeWarping{

    };

    template <class GeometryTraits, class Norm>
    class WeakFrechet
    {
        using NT = typename GeometryTraits::NT;
        using FreeSpaceGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_weight_t, NT>>;
        using FreeSpaceGraphTraits = boost::graph_traits<FreeSpaceGraph>;
        using FreeSpaceGraphWeightMap = typename boost::property_map< FreeSpaceGraph, boost::edge_weight_t >::type;

        Norm m_norm;
        movetk_core::MakeSegment<GeometryTraits> make_segment;
        template<typename U>
        using arr = std::array<U, 4>;

        // Combination function for searching minimax paths via boost dijkstra.
        struct MiniMaxCombineDistance
        {
            NT operator()(NT dist, NT weight) const
            {
                return std::max(dist, weight);
            }
        };

        /**
         * \brief Setup the freespace grid-graph for computing the weak Frechet distance
         * \tparam InIt Input iterator for points of polylines
         * \param polyA First polyline, specified as a pair of input iterators (start and beyond)
         * \param polyB Second polyline, specified as a pair of input iterators (start and beyond)
         * \param freeSpaceGraph Output freespace graph
         * \param weightMap Output weightmap for the edges of the freespace graph.
         */
        template<typename InIt>
        void setupFreeSpaceGraph(std::pair<InIt, InIt> polyA, std::pair<InIt, InIt> polyB, FreeSpaceGraph& freeSpaceGraph, FreeSpaceGraphWeightMap& weightMap)
        {
            const auto polyASegCount = std::distance(polyA.first, polyA.second) - 1;
            const auto polyBSegCount = std::distance(polyB.first, polyB.second) - 1;

            // Convert grid indices to single vector index
            auto vertIndex = [polyASegCount](int i, int j)
            {
                return i + j * polyASegCount;
            };

            const auto vertexCount = polyASegCount * polyBSegCount;
            freeSpaceGraph = FreeSpaceGraph(vertexCount);
            // Get the property map representing edge weights on the graph.
            weightMap = boost::get(boost::edge_weight, freeSpaceGraph);
            {
                auto pointAIt = polyA.first;
                for (int i = 0; i < polyASegCount; ++i, ++pointAIt)
                {
                    // Create the segment 
                    auto seg = make_segment(*pointAIt, *std::next(pointAIt));
                    // Determine closest distances and store as weights
                    auto pointBIt = polyB.first;
                    for (int j = 0; j < polyBSegCount -1; ++j, ++pointBIt)
                    {
                        auto startVert = boost::vertex(vertIndex(i, j), freeSpaceGraph);
                        auto endVert = boost::vertex(vertIndex(i, j + 1), freeSpaceGraph);
                        auto edgeDescriptor = boost::add_edge(startVert, endVert, freeSpaceGraph);
                        weightMap[edgeDescriptor.first] = m_norm(seg, *pointBIt);
                    }
                }
            }
            {
                // Create horizontal edges in the freespace graph
                auto pointBIt = polyB.first;
                for (int j = 0; j < polyBSegCount; ++j, ++pointBIt)
                {
                    // Create the segment 
                    auto seg = make_segment(*pointBIt, *std::next(pointBIt));
                    // Determine closest distances and store as weights
                    auto pointAIt = polyA.first;
                    for (int i = 0; i < polyASegCount - 1; ++i, ++pointAIt)
                    {
                        auto startVert = boost::vertex(vertIndex(i, j), freeSpaceGraph);
                        auto endVert = boost::vertex(vertIndex(i + 1, j), freeSpaceGraph);
                        auto edgeDescriptor = boost::add_edge(startVert, endVert, freeSpaceGraph);
                        weightMap[edgeDescriptor.first] = m_norm(seg, *pointAIt);
                    }
                }
            }
        }
    public:
        WeakFrechet(){}
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
            typename = movetk_core::requires_random_access_iterator<InputIterator>,
            typename = movetk_core::requires_movetk_point<GeometryTraits,
            typename InputIterator::value_type>
        >
            typename GeometryTraits::NT operator()(InputIterator polyline_a_first, InputIterator polyline_a_beyond,
                InputIterator polyline_b_first, InputIterator polyline_b_beyond)
        {
            // Point counts of the polylines
            const auto polyASize = std::distance(polyline_a_first, polyline_a_beyond);
            const auto polyBSize = std::distance(polyline_b_first, polyline_b_beyond);

            // Distances between starts resp. ends of the polylines (squared)
            const auto startMatchDist = m_norm(*polyline_a_first, *polyline_b_first);
            const auto endMatchDist = m_norm(*std::prev(polyline_a_beyond), *std::prev(polyline_b_beyond));

            // Special case input
            if(polyASize <= 2 && polyBSize <= 2)
            {
                return std::sqrt(std::max(startMatchDist, endMatchDist));
            }

            // Setup free space grid graph. The weights on the edges are the minimum distances for the cell border, represented by that edge, to open
            FreeSpaceGraph freeSpaceGraph;
            FreeSpaceGraphWeightMap weightMap;
            setupFreeSpaceGraph(std::make_pair(polyline_a_first, polyline_a_beyond), std::make_pair(polyline_b_first, polyline_b_beyond),
                freeSpaceGraph, weightMap);

            std::vector<NT> distances(boost::num_vertices(freeSpaceGraph));
            auto distanceMap = boost::make_iterator_property_map(distances.begin(), get(boost::vertex_index, freeSpaceGraph));
            
            // Find min max path, uses default less comparison for weights and custom combine fucntion
            boost::dijkstra_shortest_paths(freeSpaceGraph, boost::vertex(0, freeSpaceGraph), boost::distance_zero(0).distance_map(distanceMap).distance_combine(MiniMaxCombineDistance()));

            const auto freeSpaceMatchDist = distances[boost::num_vertices(freeSpaceGraph) - 1];
            
            return std::sqrt(std::max({ startMatchDist, endMatchDist, freeSpaceMatchDist }));
        }

        /**
         * \brief Computes the weak Frechet distance and returns a matching between the polylines that witnesses the returned distance.
         * \tparam InputIterator Type of the input iterators, specifying the input polylines as points
         * \tparam OutputIterator Type of the output iterator that accepts the matching in the form of std::pair<std::pair<int,int>, NT> elements
         * \param polyline_a_first Start of the first polyline
         * \param polyline_a_beyond End of the first polyline 
         * \param polyline_b_first Start of the second polyline
         * \param polyline_b_beyond End of the second polyline 
         * \param matching_output Output iterator that will be supplied with the matching. The matching is described as a pair of indices giving the
         * segment indices on polyline_a resp. polyline_b and a minimum distance to reach the next segment pair in the returned list. Note
         * that the matching start with an entry of {{-1,-1}, startMatchDist} where startMatchDist is the distance to match the starting points of the
         * two polylines. The minimum distance of the last entry is the distance to match the endpoints of the polylines.
         * \return The weak Frechet distance between the given polylines.
         */
        template <class InputIterator, class OutputIterator,
            typename = movetk_core::requires_random_access_iterator<InputIterator>,
            typename = movetk_core::requires_movetk_point<GeometryTraits, typename InputIterator::value_type>,
            typename = movetk_core::requires_equality<std::pair<std::pair<int,int>, NT>, typename OutputIterator::value_type>
        >
            typename GeometryTraits::NT operator()(InputIterator polyline_a_first, InputIterator polyline_a_beyond,
                InputIterator polyline_b_first, InputIterator polyline_b_beyond, OutputIterator matching_output)
        {
            // Point counts of the polylines
            const auto polyASize = std::distance(polyline_a_first, polyline_a_beyond);
            const auto polyBSize = std::distance(polyline_b_first, polyline_b_beyond);

            // Distances between starts resp. ends of the polylines (squared)
            const auto startMatchDist = m_norm(*polyline_a_first, *polyline_b_first);
            const auto endMatchDist = m_norm(*std::prev(polyline_a_beyond), *std::prev(polyline_b_beyond));

            // Special case input
            if (polyASize <= 2 && polyBSize <= 2)
            {
                auto res= std::sqrt(std::max(startMatchDist, endMatchDist));
                // Return the segment matching path and the associated distance for the segment
                *matching_output = std::make_pair(std::make_pair(0, 0), res);
                return res;
            }

            // Setup free space grid graph. The weights on the edges are the minimum distances for the cell border, represented by that edge, to open
            FreeSpaceGraph freeSpaceGraph;
            FreeSpaceGraphWeightMap weightMap;
            setupFreeSpaceGraph(std::make_pair(polyline_a_first, polyline_a_beyond), std::make_pair(polyline_b_first, polyline_b_beyond),
                freeSpaceGraph, weightMap);

            std::vector<NT> distances(boost::num_vertices(freeSpaceGraph));
            auto distanceMap = boost::make_iterator_property_map(distances.begin(), get(boost::vertex_index, freeSpaceGraph));

            // Create the predecessors map that will track how we got to the minimax path value
            std::vector<typename FreeSpaceGraphTraits::vertex_descriptor> predecessors(boost::num_vertices(freeSpaceGraph));
            auto predecessorsMap = boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index, freeSpaceGraph));

            // Find min max path, uses default less comparison for weights and custom combine fucntion
            boost::dijkstra_shortest_paths(freeSpaceGraph, boost::vertex(0, freeSpaceGraph),
                // Named arguments
                boost::distance_zero(0)
                .distance_map(distanceMap)
                .distance_combine(MiniMaxCombineDistance())
                .predecessor_map(predecessorsMap)
            );

            const auto freeSpaceMatchDist = distances[boost::num_vertices(freeSpaceGraph) - 1];

            auto vertDescToIndices = [polyASize](typename FreeSpaceGraph::vertex_descriptor desc)
            {
                int j = (int)(desc / (polyASize - 1));
                int i = desc - j * (polyASize - 1);
                return std::make_pair(i, j);
            };
            auto val = std::sqrt(std::max({ startMatchDist, endMatchDist, freeSpaceMatchDist }));

            // Reconstruct the path in the freespace graph
            std::vector<std::pair<std::pair<int, int>, NT>> matching;
            auto matchingInserter = std::back_inserter(matching);
            auto curr = boost::vertex(boost::num_vertices(freeSpaceGraph) - 1, freeSpaceGraph);
            *matchingInserter = std::make_pair(vertDescToIndices(curr), std::sqrt(endMatchDist));
            while(true)
            {
                auto prev = curr;
                curr = predecessors[curr];
                auto edge = boost::edge(curr, prev, freeSpaceGraph);
                auto weight = weightMap[edge.first];
                *matchingInserter = std::make_pair(vertDescToIndices(curr), std::sqrt(weight));
                if (curr == boost::vertex(0, freeSpaceGraph)) break;
            }
            // Add the start matching
            *matchingInserter = std::make_pair(std::make_pair(-1, -1), startMatchDist);

            // Reverse to get forward order matching
            std::reverse(matching.begin(), matching.end());
            // Copy to output
            std::copy(matching.begin(), matching.end(), matching_output);

            return std::sqrt(std::max({ startMatchDist, endMatchDist, freeSpaceMatchDist }));
        }
    };
}

#endif //MOVETK_SIMILARITY_H
