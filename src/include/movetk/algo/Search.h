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

/*! @file Search.h
 *  @brief  Trajectory Segmentation  Algorithms
 *  @authors Aniket Mitra (aniket.mitra@here.com)
 */


#ifndef MOVETK_SEARCH_H
#define MOVETK_SEARCH_H


#include <iterator>
#include "movetk/utils/Requirements.h"

/*!
 * @brief a collection of algorithms in movetk
 */
namespace movetk::algo {
    /*!
     *
     * @tparam TestType
     */
    template <class GeometryTraits, class TestType>
    class BinarySearch{
        // based on https://doi.org/10.1145/1869790.1869821
        // TODO Requirements for TestType
    private:
        typedef TestType TEST;
        typedef typename GeometryTraits::NT NT;
        TEST test;
    public:
        BinarySearch() = default;
        /*!
         *
         * @param threshold
         */
        BinarySearch(NT threshold){
            test = TEST(threshold);
        }
        /*!
         *
         * @tparam InputIterator
         * @param first
         * @param min
         * @param left
         * @param right
         * @return
         */
        template<class InputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_valid_type<GeometryTraits,
                        typename InputIterator::value_type> >
        size_t operator()(InputIterator first,size_t min,size_t left,size_t right){
            //ASSERT_RANDOM_ACCESS_ITERATOR(InputIterator);
            if (left >= right) return right;
            size_t mid = ( left + right ) / 2;
            if ( !test( first, first + min + mid  ) )
                return (*this)(first, min, left + 1, mid);
            else
                return (*this)(first, min, mid + 1, right);
        }
    };

}

#endif //MOVETK_SEARCH_H
