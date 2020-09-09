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


/*! @file StartStopMatrix.h
 *  @brief  Start-stop matrix
 *  @authors Aniket Mitra (aniket.mitra@here.com)
 */

#ifndef MOVETK_STARTSTOPMATRIX_H
#define MOVETK_STARTSTOPMATRIX_H

#include <iterator>
#include <algorithm>
#include "movetk/utils/Iterators.h"
#include "movetk/utils/Requirements.h"

enum SsdType {
    compressed
};


namespace movetk_support {

    template<int SsdType, class GeometryTraits, class NodeType>
    class StartStopDiagram {
    };

    template<class GeometryTraits, class NodeType>
    class StartStopDiagram<SsdType::compressed, GeometryTraits, NodeType> {
        // based on  https://doi.org/10.1007/s00453-017-0329-x
        //TODO Requirements for  NodeType
    private:

        NodeType node;
        movetk_core::movetk_back_insert_iterator<NodeType> result = movetk_core::movetk_back_insert_iterator(node);
        ASSERT_OUTPUT_ITERATOR(decltype(result));
        ASSERT_IS_SIZE_T(decltype(result));
    public:
        typedef typename NodeType::iterator iterator;
        typedef typename NodeType::const_iterator const_iterator;

        /*!
         *
         * @tparam InputIterator
         * @param first
         * @param beyond
         */
        template<class InputIterator, class PolylineIdxIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_valid_type<GeometryTraits,
                        typename InputIterator::value_type> >
        void operator()(InputIterator first, InputIterator beyond,
                        movetk_core::SegmentIdGenerator<PolylineIdxIterator> &segments) {
            ASSERT_RANDOM_ACCESS_ITERATOR(InputIterator);
            size_t idx = 0;
            size_t SegmentId, PreviousSegmentId = 0;
            InputIterator it = first;
            while (it != beyond) {
                SegmentId = segments.getSegmentID(it);
                if (SegmentId != PreviousSegmentId)
                    idx = 0;
                idx++;
                *result = idx;
                PreviousSegmentId = SegmentId;
                it++;
            }
            segments.reset();
        }

        template<class InputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_size_t<typename InputIterator::value_type> >
        void operator()(InputIterator first, InputIterator beyond){
            ASSERT_RANDOM_ACCESS_ITERATOR(InputIterator);
            size_t idx = 0;
            InputIterator it = first;
            InputIterator previt = it;
            while (it != beyond) {
                if (*it != *previt)
                    idx = 0;
                idx++;
                *result = idx;
                previt = it;
                it++;
            }
        }


        void operator()(size_t idx) {
            *result = idx;
        }

        iterator begin() {
            return node.begin();
        }

        iterator end() {
            return node.end();
        }

        const_iterator cbegin() {
            return node.cbegin();
        }

        const_iterator cend() {
            return node.cend();
        }

        size_t size() {
            return node.size();
        }

        void operator=(StartStopDiagram rhs) {
            // TODO: Gets fixed when the return type of * / + operator overloading is fixed
            const_iterator it = rhs.cbegin();
            this->node.clear();
            this->result = movetk_core::movetk_back_insert_iterator(this->node);
            while (it != rhs.cend()) {
                *this->result = *it;
                it++;
            }
        }

        StartStopDiagram operator+(StartStopDiagram &rhs) {
            // TODO: Check how to return by reference...
            StartStopDiagram result;
            assert(this->size() == rhs.size());
            const_iterator rhs_it = rhs.cbegin();
            //const_iterator rhs_start_it = rhs_it;
            const_iterator lhs_it = this->cbegin();
            //const_iterator lhs_start_it = lhs_it;
            size_t rhs_sum = 0, lhs_sum = 0, rhs_series_sum = 0, lhs_series_sum = 0;
            size_t rhs_start_sum = 0, lhs_start_sum = 0, idx = 1;
            while (rhs_it != rhs.cend()) {
                rhs_sum += *rhs_it;
                lhs_sum += *lhs_it;
                rhs_series_sum = (*rhs_it * (*rhs_it + 1)) / 2 - rhs_start_sum;
                lhs_series_sum = (*lhs_it * (*lhs_it + 1)) / 2 - lhs_start_sum;
                if ((rhs_series_sum != rhs_sum) && (lhs_series_sum != lhs_sum)) {
                    rhs_sum = *rhs_it;
                    lhs_sum = *lhs_it;
                    rhs_start_sum = (*rhs_it * (*rhs_it - 1)) / 2;
                    lhs_start_sum = (*lhs_it * (*lhs_it - 1)) / 2;
                    idx = 1;
                }
                result(idx);
                rhs_it++;
                lhs_it++;
                idx++;
            }
            return result;

        }


        StartStopDiagram operator*(StartStopDiagram &rhs) {
            // TODO: Check how to return by reference...
            StartStopDiagram result;
            assert(this->size() == rhs.size());
            const_iterator rhs_it = rhs.cbegin();
            //const_iterator rhs_start_it = rhs_it;
            const_iterator lhs_it = this->cbegin();
            //const_iterator lhs_start_it = lhs_it;
            size_t rhs_sum = 0, lhs_sum = 0, rhs_series_sum = 0, lhs_series_sum = 0;
            size_t rhs_start_sum = 0, lhs_start_sum = 0, idx = 1;
            while (rhs_it != rhs.cend()) {
                rhs_sum += *rhs_it;
                lhs_sum += *lhs_it;
                rhs_series_sum = (*rhs_it * (*rhs_it + 1)) / 2 - rhs_start_sum;
                lhs_series_sum = (*lhs_it * (*lhs_it + 1)) / 2 - lhs_start_sum;
                if ((rhs_series_sum != rhs_sum) || (lhs_series_sum != lhs_sum)) {
                    rhs_sum = *rhs_it;
                    lhs_sum = *lhs_it;
                    rhs_start_sum = (*rhs_it * (*rhs_it - 1)) / 2;
                    lhs_start_sum = (*lhs_it * (*lhs_it - 1)) / 2;
                    idx = 1;
                }
                result(idx);
                rhs_it++;
                lhs_it++;
                idx++;
            }
            return result;
        }

    };

};


#endif //MOVETK_STARTSTOPMATRIX_H
