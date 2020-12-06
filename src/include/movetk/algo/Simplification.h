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

    /**
     * \brief 
     * Based on https://doi.org/10.1007/s00453-005-1165-y
     * \tparam GeometryTraits The geometry kernel
     * \tparam SqDistanceFunc Algorithm for computing the squared distance between points and points/segments.
     */
    template <class GeometryTraits, class SqDistanceFunc = movetk_core::squared_distance_algorithm<GeometryTraits,movetk_support::FiniteNorm<GeometryTraits,typename GeometryTraits::dim>,void>>
    class Agarwal
    {
        using NT = typename GeometryTraits::NT;
        NT m_epsilon;
        NT m_tolerance = static_cast<NT>(0.001);
    public:
        Agarwal(typename GeometryTraits::NT epsilon = static_cast<NT>(1.0)): m_epsilon(epsilon){}

        void setEpsilon(typename GeometryTraits::NT epsilon)
        {
            m_epsilon = epsilon;
        }
        typename GeometryTraits::NT epsilon() const
        {
            return m_epsilon;
        }
        void setTolerance(const typename GeometryTraits::NT& tolerance)
        {
            m_tolerance = tolerance;
        }
        typename GeometryTraits::NT tolerance() const
        {
            return m_tolerance;
        }


        template <class InputIterator, class OutputIterator,
            typename = movetk_core::requires_random_access_iterator<InputIterator>,
            typename = movetk_core::requires_output_iterator<OutputIterator>,
            typename = movetk_core::requires_movetk_point<GeometryTraits,
            typename InputIterator::value_type>,
            typename = std::enable_if_t<std::is_same_v<typename OutputIterator::value_type, InputIterator>,InputIterator>// Require output iterator assignable with input iterator
        >
        void operator()(InputIterator first, InputIterator beyond, OutputIterator result) const
        {
            InputIterator curr = first;

            // Empty: no simplification
            if(curr == beyond )
            {
                return;
            }
            // Degenerate point: return the point
            if(std::next(curr) == beyond)
            {
                *result = curr;
                return;
            }

            const auto polyLength = std::distance(first, beyond);

            // Assign initial point as output
            *result = first;

            // Exponential search offset. Note that offset=1 is always trivially within epsilon with itself...
            std::size_t offset = 2;
            // Index within the polyline of the first point that we are searching with
            std::size_t startPointIndex = 0;
            // Setup Frechet distance
            movetk_support::StrongFrechet<GeometryTraits, SqDistanceFunc> sfd;
            sfd.setTolerance(m_tolerance);

            // The segment to check with
            std::array<typename GeometryTraits::MovetkPoint, 2> segment;
            segment[0] = *curr;
            
            while(true)
            {
                // The bounds to search in when the Frechet predicate fails
                std::size_t searchLower = offset / 2;
                std::size_t searchUpper = offset;

                // Handle termination
                if(startPointIndex + offset >= polyLength)
                {
                    // Assign the last segment
                    segment[1] = *(std::prev(beyond));
                    
                    // If the segment to the end point is within epsilon or we only have one segment left, we are done
                    if(offset == 2 || sfd.decide(segment.begin(), segment.end(), curr, beyond, m_epsilon))
                    {
                        *result = std::prev(beyond);
                        break;
                    }
                    // Otherwise, set the upperbound for binary search
                    searchUpper = polyLength - 1 - startPointIndex;
                }
                 
                // Assign the current segment to check
                segment[1] = *(curr + searchUpper);

                // Distance is larger than epsilon
                if(!sfd.decide(segment.begin(), segment.end(), curr, curr + searchUpper + 1, m_epsilon))
                {
                    // Perform binary search to find p_j such that the subpath to p_{j+1} is above epsilon
                    auto lower = searchLower; // Guaranteed to be within epsilon
                    auto upper = searchUpper; // Guaranteed to be outside epsilon
                    while(upper > lower + 1)
                    {
                        auto mid = (lower + upper) / 2; //Floored when uneven.
                        segment[1] = *(curr + mid);
                        // Higher than epsilon
                        if (!sfd.decide(segment.begin(), segment.end(), curr, curr + mid + 1, m_epsilon))
                        {
                            upper = mid;
                        }
                        else
                        {
                            lower = mid;
                        }
                    }
                    // Assign the output
                    *result = curr + upper - 1;
                    startPointIndex += upper - 1;
                    // Update current point iterator
                    curr += upper - 1;
                    // Update offset
                    offset = 2;
                    // Update query segment with new start point
                    segment[0] = *curr;
                }
                // Less than epsilon Frechet distance, so continue the exponential search
                else
                {
                    offset *= 2; // Or offset <<= 1;
                }
            }
        }
    };
} // namespace movetk_algorithms

#endif //MOVETK_SIMPLIFICATION_H
