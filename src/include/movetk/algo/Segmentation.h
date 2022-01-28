/*
 * Copyright (C) 2018-2020
 * HERE Europe B.V.
 * Utrecht University (The Netherlands).
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


/*! @file Segmentation.h
 *  @brief  Trajectory Segmentation  Algorithms
 *  @authors Aniket Mitra (aniket.mitra@here.com), Wouter Jongeling (wouter.jongeling@gmail.com)
 */



#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <algorithm>
#include <iterator>
#include "movetk/utils/Requirements.h"
#include "movetk/utils/Iterators.h"
#include <cmath>
#include "movetk/algo/Search.h"

/*!
 * @brief a collection of algorithms in movetk
 */
namespace movetk::algo {

    /*!
     *
     * @tparam TestType
     */
    template<class GeometryTraits, class TestType>
    class MonotoneSegmentation{
        // based on https://doi.org/10.1145/1869790.1869821
        // TODO Requirements for TestType
    private:
        typedef typename GeometryTraits::NT NT;
        typedef TestType TEST;
        BinarySearch<GeometryTraits, TEST> binary_search;
        TEST test;
    public:
        /*!
         *
         * @param threshold
         */
        MonotoneSegmentation(NT threshold) {
            test = TEST(threshold);
            binary_search = BinarySearch<GeometryTraits, TEST>(threshold);
        };
        /*!
         *
         * @tparam InputIterator
         * @tparam OutputIterator
         * @param first
         * @param beyond
         * @param result
         */
        template <class InputIterator, class OutputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_output_iterator<OutputIterator>,
                typename = movetk_core::requires_valid_type<GeometryTraits,
                        typename InputIterator::value_type>,
                typename = movetk_core::requires_equality<typename InputIterator::value_type,
                        typename OutputIterator::value_type::value_type >>
        void operator()(InputIterator first, InputIterator beyond, OutputIterator result){
            //ASSERT_RANDOM_ACCESS_ITERATOR(InputIterator);
            //ASSERT_OUTPUT_ITERATOR(OutputIterator);
            InputIterator it = first;
            size_t remainder = std::distance (first,beyond);
            size_t MaxAllowedSteps = 0;
            size_t steps = 1,a = 1;
            size_t range = 0;
            while(it != beyond){
                steps = 1;
                a = 2;
                MaxAllowedSteps = floor(log2(remainder));
                while ( steps <= MaxAllowedSteps  ){
                    if (test(it, it + a )) {
                        steps++;
                        a = 2 * a;
                    }
                    else{
                        range = pow(2,steps) - pow( 2 , steps - 1 );
                        a = a/2 + binary_search(it, a/2 , 0 , range);
                        if ( a == 1) a++;
                        break;
                    }
                }
                if (a <= remainder){
                    *result = it + ( a - 1 );
                    it = it + ( a - 1 );
                }
                else{
                    if ( test( it, it + remainder ) ) it = beyond;
                    else{
                        a = a/2 + binary_search(it, a/2 , 1, remainder - a/2);
                        if ( a == 1 ) a++;
                        if ( a <= remainder ) {
                            *result = it + (a - 1);
                        }
                        it = it + ( a - 1 );
                    }
                }
                remainder = std::distance(it,beyond);
            }
            //cout<<endl;
        }

    };



    template <class GeometryTraits, class LogLikelihood >
    class ModelBasedSegmentation{

        // based on https://doi.org/10.1007/s00453-017-0329-x

        typedef typename GeometryTraits::NT NT;
        typedef std::vector<typename GeometryTraits::NT> Row;
        typedef std::vector<Row> Table;
        typedef typename Table::const_iterator TableIterator;

        struct InformationCriteria{
            NT penalty_factor;
            NT operator()(NT log_likelihood){
                return -2 * log_likelihood + penalty_factor;
            }
        }ic;

    public:
        ModelBasedSegmentation(NT p){
            ic.penalty_factor = p;
        }

        template<class RowIterator, class ColumnIterator, class OutputIterator,
                typename = movetk_core::requires_random_access_iterator<RowIterator>,
                typename = movetk_core::requires_random_access_iterator<ColumnIterator>,
                typename = movetk_core::requires_output_iterator<OutputIterator>,
                typename = movetk_core::requires_equality<
                        typename RowIterator::value_type,
                        typename OutputIterator::value_type::value_type>>
        void operator()(RowIterator rfirst, RowIterator rbeyond,
                               ColumnIterator cfirst, ColumnIterator cbeyond, OutputIterator result) {
            LogLikelihood ll;

            std::size_t num_rows = std::distance(rfirst, rbeyond);
            std::size_t num_columns = std::distance(cfirst, cbeyond);

            Row dp_row;
            Table dp_table (num_rows);

            // Fill up Dynamic Programming Table
            ll(*rfirst, cfirst, cbeyond, movetk_core::movetk_back_insert_iterator(dp_row));

            std::transform(std::begin(dp_row), std::end(dp_row), std::begin(dp_row),ic);
            auto min_ic = std::min_element(std::cbegin(dp_row), std::cend(dp_row));
            std::size_t i = 0;
            dp_table[i] = std::move(dp_row);
            i++;
            RowIterator rit = rfirst + 1;
            while(rit != rbeyond){
                ColumnIterator cit = cfirst;
                std::size_t j = 0;
                Row dp_row (num_columns);
                while(cit != cbeyond){
                    NT log_likelihood = ll(*rit, *cit);
                    NT ic_extend = dp_table[i - 1][j];
                    NT ic_append =  *min_ic + ic.penalty_factor;
                    dp_row[j] = min(ic_extend, ic_append) - 2 * log_likelihood;
                    cit++;
                    j++;
                }
                min_ic = std::min_element(std::cbegin(dp_row), std::cend(dp_row));
                dp_table[i] = std::move(dp_row);
                rit++;
                i++;
            }

            // Backtrack through the  Dynamic Programming Table
            TableIterator table_iterator = std::cend(dp_table) - 1;
            RowIterator parameter_iterator = rbeyond - 1;
            auto last_min_ic =  std::min_element(std::cbegin(*table_iterator), std::cend(*table_iterator));
            std::size_t last_position = std::distance(std::cbegin(*table_iterator), last_min_ic);
            *result = parameter_iterator;

            if (num_rows == 1)
                return;

            table_iterator--;
            parameter_iterator--;

            while (table_iterator != std::cbegin(dp_table)){
                min_ic = std::min_element(std::cbegin(*table_iterator), std::cend(*table_iterator));
                std::size_t position = std::distance(std::cbegin(*table_iterator), min_ic);
                if (last_position != position){
                    *result = parameter_iterator;
                }
                last_position = position;
                table_iterator--;
                parameter_iterator--;
            }

            min_ic = std::min_element(std::cbegin(*table_iterator), std::cend(*table_iterator));
            std::size_t position = std::distance(std::cbegin(*table_iterator), min_ic);

            if (last_position != position){
                *result = parameter_iterator;
            }

        }

    };



}

#endif /* SEGMENTATION_H */

