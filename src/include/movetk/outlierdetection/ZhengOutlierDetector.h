/*
 * Copyright (C) 2018-2020
 * HERE Europe B.V.
 * Utrecht University (The Netherlands).
 * TU/e (The Netherlands).
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
 // Created by Mees van de Kerkhof (m.a.vandekerkhof@uu.nl),
 // Bram Custers (b.a.custers@tue.nl),
 // Kevin Verbeek (k.a.b.verbeek@tue.nl)
 // Modified by Aniket Mitra (aniket.mitra@here.com)
 //

#ifndef MOVETK_OUTLIERDETECTION_ZHENGOUTLIERDETECTOR_H
#define MOVETK_OUTLIERDETECTION_ZHENGOUTLIERDETECTOR_H


#include "movetk/geo/geo.h"
#include "movetk/io/CartesianProbeTraits.h"
#include "movetk/utils/Requirements.h"

namespace movetk::outlierdetection {

    struct zheng_outlier_detector_tag;

    // based on B. Custers & M. van de Kerkhof & W. Meulemans and B. Speckmann & F. Staals (2019) .
    // Maximum Physically Consistent Trajectories
    // published in SIGSPATIAL 2019
    // Physics-based outlier detection running in O(n) time
    template <class GeometryKernel, class Predicate>
    class OutlierDetection<GeometryKernel, Predicate, zheng_outlier_detector_tag> {
    public:
        using NT = typename GeometryKernel::NT;
        /*!
         *@param InThreshold
         */
        OutlierDetection(NT threshold, size_t min_seg_size) {
            m_threshold = threshold;
            m_predicate = Predicate(threshold);
            m_min_seg_size = min_seg_size;
        };

        /*!
         * Goes through the provided input range, and determines subranges such that
         * for elements at index i and i-1, the predicate holds. All subranges with a complexity
         * of at least the provided min_seg_size are retained.
         * @tparam InputIterator
         * @tparam OutputIterator
         * @param first
         * @param beyond
         * @param result
         */
        template <std::input_iterator InputIterator,
            utils::OutputIterator<InputIterator> OutputIterator>
            void operator()(InputIterator first, InputIterator beyond, OutputIterator result) {
            size_t segment_size = 1;
            auto range_start = first;
            for (auto prev = first, it = std::next(first); it != beyond; it++) {
                if (m_predicate(*prev, *it))
                    segment_size++;
                else {
                    if (segment_size > m_min_seg_size) {
                        for (auto it_to_copy = range_start; it_to_copy != it; ++it_to_copy) {
                            *result = it_to_copy;
                        }
                    }
                    range_start = it;
                    segment_size = 1;
                }
            }
        }  // operator()
    private:
        Predicate m_predicate;
        NT m_threshold;
        size_t m_min_seg_size;
    };

}  // namespace movetk::outlierdetection


#endif  // MOVETK_OUTLIERDETECTION_H
