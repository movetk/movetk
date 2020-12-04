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


/*! @file Requirements.h
 *  @brief  An collection of requirements for types used in movetk
 *  @authors Aniket Mitra (aniket.mitra@here.com)
 */

#ifndef MOVETK_REQUIREMENTS_H
#define MOVETK_REQUIREMENTS_H


#include <cstdlib>

#include "movetk/utils/TypeChecks.h"

#define MOVETK_EPS 0.00001

namespace movetk_core {

    /*!
     * @brief Requirement for template \c T to be of type \c size_t
     * @details defines \c requires_size_t as  size_t iff \c T is \c size_t, otherwise
     * defines requires_size_t as \c void
     */
    template<class T = void>
    using requires_size_t =  typename std::enable_if<movetk_core::is_size_t<T>::value, T>::type;
    
    /*!
     * @brief Requirement for template \c T to be of type \c date type
     * @details defines \c requires_date_t as  \c size_t or \c ParseDate iff \c T is \c size_t or \c ParseDate, otherwise
     * defines requires_size_t as \c void
     */
    template<class T = void>
    using requires_date_t =  typename std::enable_if<movetk_core::is_date<T>::value, T>::type;
    

    /*!
     * @brief Requirement for template \c T to be of type \c Traits::NT
     * @details defines \c requires_NT as NT iff \c T is \c NT, otherwise
     * defines requires_NT as \c void
     */
    template<class Traits, class T = void>
    using requires_NT =  typename std::enable_if<movetk_core::is_NT<Traits, T>::value, T>::type;

    /*!
     *@brief Requirement for  template \c T  to be a random access iterator
     * @details defines \c requires_random_access_iterator as \c std::random_access_iterator iff
     * \c std::iterator_traits<T>::iterator_category = \c std::random_access_iterator_tag , otherwise
     * defines \c requires_random_access_iterator as \c void
     */
    template<class T = void>
    using requires_random_access_iterator =  typename std::enable_if<movetk_core::is_random_access_iterator<
            typename std::iterator_traits<T>::iterator_category>::value,T>::type;
    /*!
     *@brief Requirement for  template \c T  to be an input iterator
     * @details defines \c requires_atleast_input_iterator as \c T iff
     * \c std::iterator_traits<T>::iterator_category is a base of \c std::input_iterator_tag , otherwise
     * defines \c requires_random_access_iterator as \c void
     */
    template<class T = void>
    using requires_atleast_input_iterator = typename std::enable_if<
        std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<T>::iterator_category>,
    T>::type;

    /*!
     *@brief Requirement for template \c T to be an output_iterator
     * @details defines \c requires_output_iterator as \c std::output_iterator iff
     * \c std::iterator_traits<T>::iterator_category = \c std::output_iterator_tag, otherwise
     * defines \c requires_output_iterator as \c void
     */
    template<class T = void>
    using requires_output_iterator = typename std::enable_if<movetk_core::is_output_iterator<
            typename std::iterator_traits<T>::iterator_category>::value, T>::type;

    /*!
     *@brief Requirement for template \c T to be a \c MovetkPoint
     * @details defines \c requires_movetk_point as \c Traits::MovetkPoint iff
     * \c T is \c MovetkPoint, otherwise defines \c requires_movetk_point as \c void
     */
    template<class Traits, class T = void>
    using requires_movetk_point = typename std::enable_if<movetk_core::is_MovetkPoint<Traits, T>::value, T>::type;


    /*!
    *@brief Requirement for template \c T to be a \c MovetkSegment
    * @details defines \c requires_movetk_segment as \c Traits::MovetkSegment iff
    * \c T is \c MovetkSegment, otherwise defines \c requires_movetk_segment as \c void
    */
    template<class Traits, class T = void>
    using requires_movetk_segment = typename std::enable_if<movetk_core::is_MovetkSegment<Traits, T>::value, T>::type;


    /*!
    *@brief Requirement for template \c T to be a \c MovetkLine
    *@details defines \c requires_movetk_Line as \c Traits::MovetkLine iff
    * \c T is \c MovetkLine, otherwise defines \c requires_movetk_Line as \c void
    */   
    template<class Traits, class T = void>
    using requires_movetk_Line = typename std::enable_if<movetk_core::is_MovetkLine<Traits, T>::value, T>::type;

    /*!
     *@brief Requirement for template \c T to be a \c Wrapper_Point
     * @details defines \c requires_wrapper_point as \c Traits::Wrapper_Point iff
     * \c T is \c Wrapper_Point, otherwise defines \c requires_movetk_point as \c void
     */
    template <class Traits,class T = void>
    using requires_wrapper_point = typename std::enable_if<movetk_core::is_Wrapper_Point<Traits, T>::value, T>::type;

    /*!
     *@brief Requirement for template \c T to be a \c std::pair
     * @details defines \c requires_pair as \c std::pair iff \c T is std::pair,
     * otherwise defines \c requires_pair as \c void
     */
    template<class T = void>
    using requires_pair = typename std::enable_if<movetk_core::is_pair<T>::value, T>::type;

    /*!
     *@brief Requirement for template \c T2 to be equal to template \c T1
     * @details defines \c requires_equality as \c T2 iff \c T2 is the same type as \c T1,
     * otherwise defines \c requires_equality as \c void
     */
    template<class T1 = void , class T2 = void>
    using requires_equality = typename std::enable_if<movetk_core::is_equal_to<T1, T2>::value, T2>::type;

    /*!
     *@brief Requirement for template \c T to be either \c Traits::NT or \c std::size_t
     * or \c Traits::MovetkPoint
     *@details defines \c requires_valid_type as \c void if requirement is not met
     */
    template<class Traits, class T = void>
    using requires_valid_type = typename std::enable_if<movetk_core::is_NT<Traits, T>::value ||
                                                        movetk_core::is_size_t<T>::value ||
                                                        movetk_core::is_date<ParseDate>::value ||
                                                        movetk_core::is_MovetkPoint<Traits, T>::value, T>::type;

    /*!
     *@brief Requirement for template \c T to be a \c std::string
     * @details defines \c requires_string as \c std::string iff \c T is a std::string, otherwise
     * defines \c requires_string as \c void
     */
    template<class T = void>
    using requires_string = typename std::enable_if<movetk_core::is_string<T>::value, T>::type;

    /*!
     *@brief Requirement for template \c T to statisfy \c std::is_arithmetic<T>
     * @details defines \c requires_arithmetic  as \c void if the requirement is
     * not met
     */
    template<class T = void>
    using requires_arithmetic = typename std::enable_if<std::is_arithmetic<T>::value,T>::type;

    /*!
    *@brief Requirement for template \c T to be either \c Traits::MovetkPoint or \c std::MovetkLine
    * or \c Traits::MovetkSegment
    *@details defines \c requires_valid_type as \c void if requirement is not met
    */
    template<class Traits, class T=void>
    using requires_valid_type_distance_computation = typename std::enable_if<
            movetk_core::is_MovetkPoint<Traits, T>::value ||
            movetk_core::is_MovetkLine<Traits, T>::value || movetk_core::is_MovetkSegment<Traits, T>::value, T>::type;


    template<class T = void>
    using requires_planar_geometry = typename std::enable_if<movetk_core::is_planar_geometry<T::dim>::value, T>::type;

    template<class T = void>
    using requires_L2_norm = typename std::enable_if<movetk_core::is_L2_norm<T::P>::value, T>::type;


    template<class Traits, class T = void>
    using requires_simple_graph = typename std::enable_if<movetk_core::is_simple_graph<Traits,
            typename T::out_edge_list_selector>::value, T>::type;


    template<class T = void>
    using requires_directed_graph = typename std::enable_if<movetk_core::is_directed_graph<
            typename T::directed_selector>::value, T>::type;


    template<class T = void>
    using requires_tuple = typename std::enable_if<movetk_core::is_tuple<T>::value, T>::type;


    template<std::size_t I, class T>
    using requires_tuple_element_as_arithmetic = typename movetk_core::requires_arithmetic<std::tuple_element_t<I, T> >;

    template<std::size_t I, class T>
    using requires_tuple_element_as_size_t = typename movetk_core::requires_size_t<std::tuple_element_t<I, T> >;

    template<class Traits ,std::size_t I, class T>
    using requires_tuple_element_as_movetk_point = typename movetk_core::requires_movetk_point<Traits, std::tuple_element_t<I, T> >;

    template<class Traits, std::size_t I, class T>
    using requires_tuple_element_as_NT = typename movetk_core::requires_NT<Traits, std::tuple_element_t<I, T> >;


    /*template<class T1, class T2 = void>
    using requires_stack  = typename std::enable_if<movetk_core::is_stack<T1, T2>::value, T2>::type;*/

};
#endif //MOVETK_REQUIREMENTS_H
