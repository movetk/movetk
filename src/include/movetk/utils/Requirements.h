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


#include <concepts>
#include <cstdlib>
#include <type_traits>

#include "movetk/utils/TypeChecks.h"

#define MOVETK_EPS 0.00001

namespace movetk::utils {

/*!
 * @brief Requirement for template \c T to be of type \c date type
 * @details defines \c requires_date_t as  \c size_t or \c ParseDate iff \c T is \c size_t or \c ParseDate, otherwise
 * defines requires_size_t as \c void
 */
template <class T = void>
using requires_date_t = typename std::enable_if<is_date<T>::value, T>::type;


/*!
 * @brief Requirement for template \c T to be of type \c Traits::NT
 * @details defines \c requires_NT as NT iff \c T is \c NT, otherwise
 * defines requires_NT as \c void
 */
template <class Traits, class T = void>
using requires_NT = typename std::enable_if<is_NT<Traits, T>::value, T>::type;

/*!
 *@brief Requirement for  template \c T  to be a random access iterator
 * @details defines \c requires_random_access_iterator as \c std::random_access_iterator iff
 * \c std::iterator_traits<T>::iterator_category = \c std::random_access_iterator_tag , otherwise
 * defines \c requires_random_access_iterator as \c void
 */
template <class T = void>
using requires_random_access_iterator =
    typename std::enable_if<is_random_access_iterator<typename std::iterator_traits<T>::iterator_category>::value,
                            T>::type;

template <typename T, template <typename...> typename ELEMENT>
struct is_specialization_of : public std::false_type {};

template <template <typename...> typename ELEMENT, typename... ARGS>
struct is_specialization_of<ELEMENT<ARGS...>, ELEMENT> : public std::true_type {};

template <typename T>
concept TupleType = is_specialization_of<T, std::tuple>::value;

template <typename T, typename... ARGS>
concept TupleTypeOf = std::is_same_v<T, std::tuple<ARGS...>>;

template <typename T>
using single_type_pair = std::pair<T, T>;

template <typename T, typename VALUE>
concept RandomAccessIterator = std::random_access_iterator<T> && std::is_convertible_v<typename T::value_type, VALUE>;

template <typename T, typename KERNEL>
concept RandomAccessPointIterator =
    std::random_access_iterator<T> && std::is_convertible_v<typename T::value_type, typename KERNEL::MovetkPoint>;

template <typename T, typename VALUE>
concept OutputIterator = std::output_iterator<T, VALUE>;

template <typename T, typename VALUE>
concept InputIterator = std::input_iterator<T> && std::is_convertible_v<std::iter_value_t<T>, VALUE>;

template <typename T, typename VALUE>
concept Iterable = requires(const T& t) {
	t.begin();
	t.end();
	{ *t.begin() } -> std::convertible_to<VALUE>;
};

template <typename T, typename VALUE>
concept Iterable2D = requires(const T& t) {
	t.begin();
	t.end();
	(*t.begin()).begin();
	(*t.begin()).end();
	{ *((*t.begin()).begin()) } -> std::convertible_to<VALUE>;
};

template <size_t tuple_index, typename EXPECTED_TYPE>
struct TypeAt {
	using type = EXPECTED_TYPE;
	static constexpr size_t value = tuple_index;
};

template <size_t tuple_index, template <typename> typename PREDICATE>
struct TypeSatisfying {
	static constexpr size_t index = tuple_index;

	template <typename T>
	static constexpr bool is_satisfied = PREDICATE<T>::value;
};

template <typename T, typename... TYPES_AT>
concept TupleWithTypes = TupleType<T> &&
    (std::same_as<std::tuple_element_t<TYPES_AT::value, T>, typename TYPES_AT::type>&&...);

template <typename T, typename VALUE>
concept BackInsertable = requires(T t, VALUE v) {
	t.push_back(v);
};


/*!
 *@brief Requirement for  template \c T  to be an input iterator
 * @details defines \c requires_atleast_input_iterator as \c T iff
 * \c std::iterator_traits<T>::iterator_category is a base of \c std::input_iterator_tag , otherwise
 * defines \c requires_random_access_iterator as \c void
 */
template <class T = void>
using requires_atleast_input_iterator = typename std::
    enable_if<std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<T>::iterator_category>, T>::type;

/*!
 *@brief Requirement for template \c T to be an output_iterator
 * @details defines \c requires_output_iterator as \c std::output_iterator iff
 * \c std::iterator_traits<T>::iterator_category = \c std::output_iterator_tag, otherwise
 * defines \c requires_output_iterator as \c void
 */
template <class T = void>
using requires_output_iterator =
    typename std::enable_if<is_output_iterator<typename std::iterator_traits<T>::iterator_category>::value, T>::type;

/*!
 *@brief Requirement for template \c T to be an iterator with value_type \c MovetkPoint
 * @details defines \c requires_movetk_point as \c T iff
 * \c T is \c MovetkPoint, otherwise defines \c requires_movetk_point as \c void
 */
template <class Traits, class T = void>
using requires_movetk_point_iterator =
    typename std::enable_if<is_MovetkPoint<Traits, typename std::iterator_traits<T>::value_type>::value, T>::type;

/*!
 *@brief Requirement for template \c T to be a \c MovetkPoint
 * @details defines \c requires_movetk_point as \c Traits::MovetkPoint iff
 * \c T is \c MovetkPoint, otherwise defines \c requires_movetk_point as \c void
 */
template <class Traits, class T = void>
using requires_movetk_point = typename std::enable_if<is_MovetkPoint<Traits, T>::value, T>::type;


/*!
 *@brief Requirement for template \c T to be a \c MovetkSegment
 * @details defines \c requires_movetk_segment as \c Traits::MovetkSegment iff
 * \c T is \c MovetkSegment, otherwise defines \c requires_movetk_segment as \c void
 */
template <class Traits, class T = void>
using requires_movetk_segment = typename std::enable_if<is_MovetkSegment<Traits, T>::value, T>::type;


/*!
 *@brief Requirement for template \c T to be a \c MovetkLine
 *@details defines \c requires_movetk_Line as \c Traits::MovetkLine iff
 * \c T is \c MovetkLine, otherwise defines \c requires_movetk_Line as \c void
 */
template <class Traits, class T = void>
using requires_movetk_Line = typename std::enable_if<is_MovetkLine<Traits, T>::value, T>::type;

/*!
 *@brief Requirement for template \c T to be a \c Wrapper_Point
 * @details defines \c requires_wrapper_point as \c Traits::Wrapper_Point iff
 * \c T is \c Wrapper_Point, otherwise defines \c requires_movetk_point as \c void
 */
template <class Traits, class T = void>
using requires_wrapper_point = typename std::enable_if<is_Wrapper_Point<Traits, T>::value, T>::type;

/*!
 *@brief Requirement for template \c T to be a \c std::pair
 * @details defines \c requires_pair as \c std::pair iff \c T is std::pair,
 * otherwise defines \c requires_pair as \c void
 */
template <class T = void>
using requires_pair = typename std::enable_if<is_pair<T>::value, T>::type;

/*!
 *@brief Requirement for template \c T to be either \c Traits::NT or \c std::size_t
 * or \c Traits::MovetkPoint
 *@details defines \c requires_valid_type as \c void if requirement is not met
 */
template <class Traits, class T = void>
using requires_valid_type =
    typename std::enable_if<is_NT<Traits, T>::value || std::is_same_v<T, size_t> || is_date<io::ParseDate>::value ||
                                is_MovetkPoint<Traits, T>::value,
                            T>::type;

/*!
 *@brief Requirement for template \c T to be a \c std::string
 * @details defines \c requires_string as \c std::string iff \c T is a std::string, otherwise
 * defines \c requires_string as \c void
 */
template <class T = void>
using requires_string = typename std::enable_if<is_string<T>::value, T>::type;

/*!
 *@brief Requirement for template \c T to statisfy \c std::is_arithmetic<T>
 * @details defines \c requires_arithmetic  as \c void if the requirement is
 * not met
 */
template <class T = void>
using requires_arithmetic = typename std::enable_if<std::is_arithmetic<T>::value, T>::type;

/*!
 *@brief Requirement for template \c T to be either \c Traits::MovetkPoint or \c std::MovetkLine
 * or \c Traits::MovetkSegment
 *@details defines \c requires_valid_type as \c void if requirement is not met
 */
template <class Traits, class T = void>
using requires_valid_type_distance_computation =
    typename std::enable_if<is_MovetkPoint<Traits, T>::value || is_MovetkLine<Traits, T>::value ||
                                is_MovetkSegment<Traits, T>::value,
                            T>::type;

template <typename T>
concept Kernel = requires() {
	typename T::NT;
	{ T::dim } -> std::convertible_to<size_t>;
};

template <typename T>
struct is_planar_geometry2 : public std::integral_constant<bool, T::dim == 2> {};

template <typename T, template <typename> typename... PREDICATES>
concept KernelSatisfying = Kernel<T> &&(PREDICATES<T>::value&&...);


template <class T = void>
using requires_planar_geometry = typename std::enable_if<is_planar_geometry<T::dim>::value, T>::type;

template <class T = void>
using requires_L2_norm = typename std::enable_if<is_L2_norm<T::P>::value, T>::type;


template <class Traits, class T = void>
using requires_simple_graph =
    typename std::enable_if<is_simple_graph<Traits, typename T::out_edge_list_selector>::value, T>::type;


template <class T = void>
using requires_directed_graph =
    typename std::enable_if<is_directed_graph<typename T::directed_selector>::value, T>::type;


template <class T = void>
using requires_tuple = typename std::enable_if<is_tuple<T>::value, T>::type;


template <std::size_t I, class T>
using requires_tuple_element_as_arithmetic = requires_arithmetic<std::tuple_element_t<I, T>>;

template <std::size_t I, class T>
using requires_tuple_element_as_size_t = std::enable_if_t<std::is_same_v<size_t, std::tuple_element_t<I, T>>>;

template <class Traits, std::size_t I, class T>
using requires_tuple_element_as_movetk_point = requires_movetk_point<Traits, std::tuple_element_t<I, T>>;

template <class Traits, std::size_t I, class T>
using requires_tuple_element_as_NT = requires_NT<Traits, std::tuple_element_t<I, T>>;


/*template<class T1, class T2 = void>
using requires_stack  = typename std::enable_if<movetk::utils::is_stack<T1, T2>::value, T2>::type;*/

}  // namespace movetk::utils
#endif  // MOVETK_REQUIREMENTS_H
