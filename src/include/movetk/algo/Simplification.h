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

#ifndef MOVETK_SIMPLIFICATION_H
#define MOVETK_SIMPLIFICATION_H

#include <iterator>
#include "movetk/utils/Requirements.h"
#include "movetk/utils/Iterators.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/DistanceInterface.h"
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>

using namespace std;

namespace movetk_algorithms
{

    template <class GeometryTraits, class Norm>
    struct FindFarthest
    {

        template <class InputIterator,
                  typename = movetk_core::requires_random_access_iterator<InputIterator>,
                  typename = movetk_core::requires_movetk_point<GeometryTraits,
                                                                typename InputIterator::value_type>>
        InputIterator operator()(typename GeometryTraits::MovetkSegment &segment,
                                 InputIterator first, InputIterator beyond,
                                 typename GeometryTraits::NT &dist)
        {
            movetk_core::ComputeSquaredDistance<GeometryTraits, Norm> squared_distance;
            Norm norm;
            InputIterator it = first + 1;
            InputIterator FarthestVertex = it;
            typename GeometryTraits::MovetkVector v = *it - segment[1];
            if (norm(v) < MOVETK_EPS)
            {
                dist = 0;
                return FarthestVertex;
            }
            dist = squared_distance(*it, segment);
            it++;
            while (it != (beyond - 1))
            {
                typename GeometryTraits::NT CurrDist = squared_distance(*it, segment);
                if (CurrDist > dist)
                {
                    FarthestVertex = it;
                    dist = CurrDist;
                }
                it++;
            }
            return FarthestVertex;
        }
    };

    template <class GeometryTraits, class FindFarthest>
    class DouglasPeucker
    {
        // based on http://doi.acm.org/10.1145/177424.178097
    private:
        typedef typename GeometryTraits::NT NT;
        movetk_core::MakeSegment<GeometryTraits> make_segment;
        FindFarthest farthest;
        NT eps;

    public:
        DouglasPeucker(NT epsilon)
        {
            eps = epsilon * epsilon;
        }

        template <class InputIterator, class OutputIterator,
                  typename = movetk_core::requires_random_access_iterator<InputIterator>,
                  typename = movetk_core::requires_output_iterator<OutputIterator>,
                  typename = movetk_core::requires_movetk_point<GeometryTraits,
                                                                typename InputIterator::value_type>,
                  typename = movetk_core::requires_equality<typename InputIterator::value_type,
                                                            typename OutputIterator::value_type::value_type>>
        void operator()(InputIterator first, InputIterator beyond,
                        OutputIterator result, size_t level = 0)
        {
            typename GeometryTraits::MovetkSegment segment = make_segment(*first, *(beyond - 1));
            NT distance = 0;
            level++;
            InputIterator FarthestVertex = farthest(segment, first, beyond, distance);
            if (distance > eps)
            {
                (*this)(first, (FarthestVertex + 1), result, level);
                (*this)(FarthestVertex, beyond, result, level);
            }
            else
            {
                *result = first;
            }
            if (level == 1)
                *result = beyond - 1;
        }
    };

    template <class GeometryTraits, class Wedge,
              typename = movetk_core::requires_planar_geometry<GeometryTraits>>
    class ChanChin
    {
    private:
        typedef typename GeometryTraits::NT NT;
        NT eps;
        template <class InputIterator, class OutputIterator,
                  typename = movetk_core::requires_random_access_iterator<InputIterator>,
                  typename = movetk_core::requires_output_iterator<OutputIterator>,
                  typename = movetk_core::requires_movetk_point<GeometryTraits,
                                                                typename InputIterator::value_type>,
                  typename = movetk_core::requires_pair<typename OutputIterator::value_type>,
                  typename = movetk_core::requires_size_t<typename OutputIterator::value_type::first_type>,
                  typename = movetk_core::requires_size_t<typename OutputIterator::value_type::second_type>>
        void algorithm(InputIterator first, InputIterator beyond, OutputIterator result)
        {
            InputIterator it = first;
            InputIterator jit = first;
            std::size_t vertex1, vertex2;
            while (it != (beyond - 1))
            {
                Wedge Wi(*it, *(it + 1), eps);
                vertex1 = std::distance(first, it);
                vertex2 = std::distance(first, it + 1);
                *result = std::make_pair(vertex1, vertex2);
                if (it == (beyond - 2))
                    jit = beyond;
                else
                    jit = it + 2;
                while (jit != beyond)
                {
                    Wedge Wj(*it, *jit, eps);
                    Wi = Wi * Wj;
                    if (Wi.is_empty())
                    {
                        break;
                    }
                    if (Wi * (*jit))
                    {
                        vertex1 = std::distance(first, it);
                        vertex2 = std::distance(first, jit);
                        *result = std::make_pair(vertex1, vertex2);
                    }
                    jit++;
                }
                it++;
            }
        }

    public:
        ChanChin() = default;

        ChanChin(NT epsilon) : eps(epsilon) {}

        template <class InputIterator, class OutputIterator,
                  typename = movetk_core::requires_random_access_iterator<InputIterator>,
                  typename = movetk_core::requires_output_iterator<OutputIterator>,
                  typename = movetk_core::requires_movetk_point<GeometryTraits,
                                                                typename InputIterator::value_type>,
                  typename = movetk_core::requires_pair<typename OutputIterator::value_type>,
                  typename = movetk_core::requires_size_t<typename OutputIterator::value_type::first_type>,
                  typename = movetk_core::requires_size_t<typename OutputIterator::value_type::second_type>>
        void operator()(InputIterator first, InputIterator beyond, OutputIterator result)
        {
            std::vector<typename OutputIterator::value_type> edges1, edges2;
            std::size_t NumElements = std::distance(first, beyond);
            algorithm(first, beyond, movetk_core::movetk_back_insert_iterator(edges1));

            std::reverse(first, beyond);
            algorithm(first, beyond, movetk_core::movetk_back_insert_iterator(edges2));

            for (auto &edge : edges2)
            {
                edge = std::make_pair(NumElements - 1 - edge.second, NumElements - 1 - edge.first);
            }

            std::sort(std::begin(edges1), std::end(edges1), [](auto &i, auto &j) {
                return (i.first < j.first);
            });

            std::sort(std::begin(edges1), std::end(edges1), [](auto &i, auto &j) {
                return (i.second < j.second);
            });

            std::sort(std::begin(edges2), std::end(edges2), [](auto &i, auto &j) {
                return (i.first < j.first);
            });

            std::sort(std::begin(edges2), std::end(edges2), [](auto &i, auto &j) {
                return (i.second < j.second);
            });

            std::reverse(first, beyond);
            std::set_intersection(std::begin(edges1), std::end(edges1),
                                  std::begin(edges2), std::end(edges2), result, [](auto &i, auto &j) {
                                      return (i.first < j.first) || (i.second < j.second);
                                  });
        }
    };

    template <class GeometryTraits, class EdgeCreator,
              typename = movetk_core::requires_planar_geometry<GeometryTraits>>
    class ImaiIri
    {
        // based on https://doi.org/10.1142/S0218195996000058
    private:
        typedef typename GeometryTraits::NT NT;
        NT eps;
        EdgeCreator create_edges;
        typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
                                      boost::property<boost::vertex_index_t, int>, boost::property<boost::edge_index_t, int>>
            Graph;
        typedef typename boost::graph_traits<Graph>::vertex_iterator vertex_iterator;
        typedef typename boost::graph_traits<Graph>::edge_iterator edge_iterator;
        typedef boost::property_map<Graph, boost::vertex_index_t>::type VertexId_PMap;

    public:
        ImaiIri(NT epsilon) : eps(epsilon)
        {
            create_edges = EdgeCreator(eps);
        }

        template <class InputIterator, class OutputIterator,
                  typename = movetk_core::requires_random_access_iterator<InputIterator>,
                  typename = movetk_core::requires_output_iterator<OutputIterator>,
                  typename = movetk_core::requires_movetk_point<GeometryTraits,
                                                                typename InputIterator::value_type>,
                  typename = movetk_core::requires_equality<typename InputIterator::value_type,
                                                            typename OutputIterator::value_type::value_type>>
        void operator()(InputIterator first, InputIterator beyond,
                        OutputIterator result)
        {
            std::vector<std::pair<std::size_t, std::size_t>> edges;
            std::size_t NumElems = std::distance(first, beyond);
            std::vector<std::size_t> indexes;
            std::vector<std::size_t> predecessors(NumElems);
            std::size_t start_idx = 0;
            predecessors[start_idx] = 0;

            create_edges(first, beyond, movetk_core::movetk_back_insert_iterator(edges));

            Graph graph{edges.begin(), edges.end(), NumElems};

            VertexId_PMap vertex_index = boost::get(boost::vertex_index, graph);

            boost::iterator_property_map<std::vector<std::size_t>::iterator, VertexId_PMap,
                                         std::size_t, std::size_t &>
                predecessors_pa(predecessors.begin(), vertex_index);

            auto predecessor_recorder = boost::record_predecessors(predecessors_pa, boost::on_tree_edge{});
            auto bfs_visitor = boost::make_bfs_visitor(predecessor_recorder);
            boost::breadth_first_search(graph, start_idx, boost::visitor(bfs_visitor));

            vertex_iterator vi, vi_end;
            std::tie(vi, vi_end) = boost::vertices(graph);
            auto idx = boost::get(predecessors_pa, *(vi_end - 1));

            indexes.push_back(*(vi_end - 1));
            while (idx != boost::get(predecessors_pa, *vi))
            {
                indexes.push_back(idx);
                idx = predecessors_pa[idx];
            }
            indexes.push_back(idx);
            std::reverse(std::begin(indexes), std::end(indexes));
            for (auto &idx : indexes)
            {
                *result = first + idx;
            }
            graph.clear();
        }
    };
} // namespace movetk_algorithms

#endif //MOVETK_SIMPLIFICATION_H
