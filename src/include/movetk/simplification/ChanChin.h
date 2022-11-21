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
#ifndef MOVETK_ALGO_SIMPLIFICATION_CHANCHIN_H
#define MOVETK_ALGO_SIMPLIFICATION_CHANCHIN_H
#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/DistanceInterface.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/Requirements.h"
namespace movetk::simplification {

    template <utils::KernelSatisfying<utils::is_planar_geometry2> GeometryTraits, class Wedge>
    class ChanChin {
    private:
        using NT = typename GeometryTraits::NT;
        NT eps;
        template <class InputIterator, class OutputIterator>
        void algorithm(InputIterator first, InputIterator beyond, OutputIterator result) {
            if (first == beyond) {
                return;
            }
            InputIterator it = first;
            InputIterator jit;
            std::size_t vertex1, vertex2;
            std::size_t current_index = 0;
            for (; it != (beyond - 1); ++it, ++current_index) {
                Wedge Wi(*it, *(it + 1), eps);
                vertex1 = current_index;
                vertex2 = current_index + 1;
                *result = std::make_pair(vertex1, vertex2);
                if (it == (beyond - 2)) {
                    break;
                }
                jit = it + 2;
                vertex2 = vertex1 + 2;
                for (; jit != beyond; ++jit, ++vertex2) {
                    Wedge Wj(*it, *jit, eps);
                    Wi = Wi * Wj;
                    if (Wi.is_empty()) {
                        break;
                    }
                    if (Wi * (*jit)) {
                        *result = std::make_pair(vertex1, vertex2);
                    }
                }
            }
        }

    public:
        ChanChin() = default;

        ChanChin(NT epsilon) : eps(epsilon) {}

        template <utils::RandomAccessIterator<typename GeometryTraits::MovetkPoint> InputIterator,
            utils::OutputIterator<std::pair<size_t, size_t>> OutputIterator>
            void operator()(InputIterator first, InputIterator beyond, OutputIterator result) {
            std::vector<std::pair<size_t, size_t>> edges1, edges2;
            std::size_t NumElements = std::distance(first, beyond);
            algorithm(first, beyond, std::back_inserter(edges1));

            algorithm(std::make_reverse_iterator(beyond),
                std::make_reverse_iterator(first),
                std::back_inserter(edges2));

            for (auto& edge : edges2) {
                edge = std::make_pair(NumElements - 1 - edge.second, NumElements - 1 - edge.first);
            }

            std::sort(std::begin(edges1), std::end(edges1), [](auto& i, auto& j) { return (i.first < j.first); });

            std::sort(std::begin(edges1), std::end(edges1), [](auto& i, auto& j) { return (i.second < j.second); });

            std::sort(std::begin(edges2), std::end(edges2), [](auto& i, auto& j) { return (i.first < j.first); });

            std::sort(std::begin(edges2), std::end(edges2), [](auto& i, auto& j) { return (i.second < j.second); });

            std::set_intersection(std::begin(edges1),
                std::end(edges1),
                std::begin(edges2),
                std::end(edges2),
                result,
                [](auto& i, auto& j) { return (i.first < j.first) || (i.second < j.second); });
        }
    };

}  // namespace movetk::simplification
#endif