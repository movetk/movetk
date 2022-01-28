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
// Created by Mitra, Aniket on 02/01/2019.
//

#ifndef MOVETK_ASSERTS_H
#define MOVETK_ASSERTS_H

/*! @file Asserts.h
 *  @brief  Collection of compile time checks for movetk
 *  @authors Aniket Mitra (aniket.mitra@here.com)
 */

#include "TypeChecks.h"

    /*!
     * \def ASSERT_RANDOM_ACCESS_ITERATOR(IteratorType)
     * @brief Checks whether the iterator is a random access iterator
     * @param IteratorType
     */
    #define  ASSERT_RANDOM_ACCESS_ITERATOR(IteratorType) \
             static_assert(movetk::utils::is_random_access_iterator< \
                typename std::iterator_traits<IteratorType>::iterator_category>::value, \
                "Expects the Iterator category to be std::random_access_iterator_tag")

    /*!
     * \def ASSERT_FORWARD_ITERATOR(IteratorType)
     * @brief Checks whether the iterator is a forward
     * @param IteratorType
     */
    #define ASSERT_FORWARD_ITERATOR(IteratorType) \
            static_assert(movetk::utils::is_forward_iterator< \
                typename std::iterator_traits<IteratorType>::iterator_category>::value, \
                "Expects the Iterator category to be std::forward_iterator_tag")

    /*!
     * \def ASSERT_OUTPUT_ITERATOR(IteratorType)
     * @brief Checks whether the iterator is a output
     * @param IteratorType
     */
    #define ASSERT_OUTPUT_ITERATOR(IteratorType) \
            static_assert(movetk::utils::is_output_iterator< \
                typename std::iterator_traits<IteratorType>::iterator_category>::value, \
                "Expects the Iterator category to be std::output_iterator_tag")

    /*!
    * \def ASSERT_NUMBER_TYPE(Kernel, Iterator)
    * @brief Checks whether the value pointed to by the iterator is
     * same as that defined in the Kernel
    * @param Kernel - A traits class containing collection of expected types
    * @param Iterator
    */
    #define ASSERT_NUMBER_TYPE(Kernel, Iterator) \
            static_assert(movetk::utils::is_NT<Kernel, decltype(*Iterator)>::value, \
            "Expects Iterator over Kernel::NT")

    /*!
    * \def ASSERT_WRAPPER_POINT_TYPE(Kernel, Iterator)
    * @brief Checks whether the value pointed to by the iterator is a
     * point with the same type as defined in the Wrapper Kernel
    * @param Kernel - A traits class containing collection of expected types
    * @param Iterator
    */
    #define ASSERT_WRAPPER_POINT_TYPE(Kernel, Iterator) \
            static_assert(movetk::utils::is_Wrapper_Point<Kernel, decltype(*Iterator)>::value, \
            "Expects Iterator over Kernel::Wrapper_Point")

/*!
* \def ASSERT_MOVETK_POINT_TYPE(Kernel, Iterator)
* @brief Checks whether the value pointed to by the iterator is a
* point with the same type as defined in the movetk geometry Kernel
* @param Kernel - A traits class containing collection of expected types
* @param Iterator
*/
#define ASSERT_MOVETK_POINT_TYPE(Kernel, Iterator) \
            static_assert(movetk::utils::is_MovetkPoint<GeometryTraits,decltype(*Iterator)>::value, \
            "Expects Iterator over Kernel::MovetkPoint")

    /*!
    * \def ASSERT_IS_STRING(IteratorType)
    * @brief Checks whether the value pointed to by the iterator is a \c std::string
    * @param Iterator
    */
    #define ASSERT_IS_STRING(IteratorType) \
            static_assert(movetk::utils::is_string<typename IteratorType::value_type>::value, \
            "Expects Iterator over std::string")

    /*!
    * \def ASSERT_IS_PAIR(IteratorType)
    * @brief Checks whether the value pointed to by the iterator is a \c std::pair
    * @param Iterator
    */
    #define ASSERT_IS_PAIR(IteratorType) \
            static_assert(movetk::utils::is_pair<typename IteratorType::value_type>::value,\
            "Expects Iterator over std::pair<T1,T2>" )

    #define ASSERT_IS_SIZE_T(IteratorType) \
          static_assert(movetk::utils::is_size_t<typename IteratorType::value_type>::value,\
            "Expects Iterator over std::size_t")

    /*!
    * \def ASSERT_IS_REFERENCE_OF(IteratorType1, IteratorType2)
    * @brief Checks whether the value the value pointed to
    * by an iterator are in-turn reference to another iterator
    * @param IteratorType1 - The Iterarator to be referred
    * @param IteratorType2 - The Iterator that refers
    */
    #define ASSERT_IS_EQUAL(T1,T2) \
            static_assert(movetk::utils::is_equal_to<T1,T2>::value, \
            "Expects Iterator2 to be an iterator over Iterator1")

    #define ASSERT_MIN_NORM(p)\
            assert(p > 0)


#endif //MOVETK_ASSERTS_H
