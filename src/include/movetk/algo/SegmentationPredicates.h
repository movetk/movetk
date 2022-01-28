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

/*! @file SegmentationPredicates.h
 *  @brief  A collection of Segmentation Predicates used by a trajectory segmentation algorithm
 *  @authors Aniket Mitra (aniket.mitra@here.com)
 */

#ifndef SEGMENTATIONPREDICATES_H
#define SEGMENTATIONPREDICATES_H

#include "movetk/geom/GeometryInterface.h"
#include "movetk/utils/TrajectoryUtils.h"
#include <iterator>
#include <algorithm>

using namespace std;

enum TestCriteria {
    meb, ratio, difference, range
};

namespace movetk::algo {

    /*!
     *
     * @tparam GeometryTraits
     */
    template<int TestCriteria, class GeometryTraits>
    class TEST {
    };

    /*!
     *
     * @tparam GeometryTraits
     */
    template<class GeometryTraits>
    class TEST<::TestCriteria::meb, GeometryTraits> {
        // based on https://doi.org/10.1145/1869790.1869821
    private:
        typedef TEST<TestCriteria::meb, GeometryTraits> TEST_;
        typedef typename GeometryTraits::NT NT;
        movetk_core::MakeMinSphere<GeometryTraits> make_min_sphere;
        NT threshold;
        size_t NumPoints = 0;
    public:
        /*!
         *
         * @tparam InputIterator
         * @param first
         * @param beyond
         * @return
         */
        template<class InputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_movetk_point<GeometryTraits,
                        typename InputIterator::value_type>>
        bool operator()(InputIterator first, InputIterator beyond) {
            //ASSERT_RANDOM_ACCESS_ITERATOR(InputIterator);
            NumPoints = std::distance(first, beyond);
            NT radius = make_min_sphere(first, beyond);
            return radius < threshold;
        }

        TEST() = default;

        /*!
         *
         * @param InThreshold
         */
        TEST(NT InThreshold) : threshold(InThreshold) {};
    };

    /*!
     *
     * @tparam GeometryTraits
     */
    template<class GeometryTraits>
    class TEST<::TestCriteria::ratio, GeometryTraits> {
        // based on https://doi.org/10.1145/1869790.1869821
    private:
        typedef TEST<TestCriteria::ratio, GeometryTraits> TEST_;
        typedef typename GeometryTraits::NT_ NT;
        NT threshold;
    public:
        /*!
         *
         * @tparam InputIterator
         * @param first
         * @param beyond
         * @return
         */
        template<class InputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_valid_type<GeometryTraits,
                        typename InputIterator::value_type >>
        bool operator()(InputIterator first, InputIterator beyond) {
            //ASSERT_RANDOM_ACCESS_ITERATOR(InputIterator);
            if (std::all_of(first, beyond, [](NT i) { return i == 0; }))
                return true;
            NT min = *movetk_core::min_non_zero_element<GeometryTraits>(first, beyond);
            NT max = *std::max_element(first, beyond);
            NT ratio = max / min;
            return ratio < threshold;
        }

        TEST() = default;

        /*!
         *
         * @param InThreshold
         */
        TEST(NT InThreshold) : threshold(InThreshold) {};
    };


    /*!
    *
    * @tparam GeometryTraits
    */
    template<class GeometryTraits>
    class TEST<::TestCriteria::difference, GeometryTraits> {
        // based on https://doi.org/10.1145/1869790.1869821
    private:
        typedef TEST<::TestCriteria::difference, GeometryTraits> TEST_;
        typedef typename GeometryTraits::NT NT;
        NT threshold;
    public:
        /*!
         *
         * @tparam InputIterator
         * @param first
         * @param beyond
         * @return
         */
        template<class InputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_valid_type<GeometryTraits,
                        typename InputIterator::value_type >>
        bool operator()(InputIterator first, InputIterator beyond) {
            //ASSERT_RANDOM_ACCESS_ITERATOR(InputIterator);
            NT min = *std::min_element(first, beyond);
            NT max = *std::max_element(first, beyond);
            NT diff = max - min;
            return diff < threshold;
        }

        TEST() = default;

        /*!
         *
         * @param InThreshold
         */
        TEST(NT InThreshold) : threshold(InThreshold) {};
    };


    /*!
   *
   * @tparam GeometryTraits
   */
    template<class GeometryTraits>
    class TEST<TestCriteria::range, GeometryTraits> {
        // based on https://doi.org/10.1145/1869790.1869821
    private:
        typedef TEST<TestCriteria::range, GeometryTraits> TEST_;
        typedef typename GeometryTraits::NT NT;
        NT threshold;
    public:
        /*!
         *
         * @tparam InputIterator
         * @param first
         * @param beyond
         * @return
         */
        template<class InputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_valid_type<GeometryTraits,
                        typename InputIterator::value_type >>
        bool operator()(InputIterator first, InputIterator beyond) {
            //ASSERT_RANDOM_ACCESS_ITERATOR(InputIterator);
            NT lb = *movetk_core::min_non_zero_element<GeometryTraits>(first, beyond);
            //NT lb = *std::min_element(first, beyond);
            NT ub = lb + threshold;
            InputIterator it = first;
            while (it != beyond) {
                if (*it == 0) {
                    it++;
                    continue;
                }
                if (*it < lb || *it > ub)
                    return false;
                it++;
            }
            return true;
        }

        TEST() = default;

        /*!
         *
         * @param InThreshold
         */
        TEST(NT InThreshold) : threshold(InThreshold) {};
    };

}


#endif /* SEGMENTATIONPREDICATES_H */

