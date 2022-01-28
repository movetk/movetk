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

/*! @file IteratorTypeCheck.h
 *  @brief  A collection of compile time iterator type checks
 *  @authors Aniket Mitra (aniket.mitra@here.com)
 */

#ifndef MOVETK_TYPECHECKS_H
#define MOVETK_TYPECHECKS_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/unordered_set.hpp>
#include <iterator>
#include <set>
#include <stack>
#include <utility>

#include "movetk/io/csv/ParseDate.h"

#define MOVETK_DEFINE_HAS_TYPE(target_type)             \
	template <typename T, typename = void>                \
	struct has_##target_type : public std::false_type {}; \
	template <typename T>                                 \
	    struct has_##target_type<T, std::void_t<typename T::target_type>> > : public std::true_type {};


/*!
 *
 *  @namespace movetk_core
 *  @brief the core of movetk
 */
namespace movetk::utils {

/*!@struct is_random_access_iterator
 * @brief A class for checking whether an iterator is
 * a random access iterator
 * @note default value is always set to false
 * @tparam IteratorTag - The iterator category as defined
 * in std::iterator_traits
 */
template <class IteratorTag>
struct is_random_access_iterator {
	static constexpr bool value = std::is_same_v<IteratorTag, std::random_access_iterator_tag>;
};

/*!@struct is_forward_iterator
 * @brief  A class for checking whether an iterator is
 * a forward iterator
 * @tparam IteratorTag - The iterator category as defined
 * in std::iterator_traits
 */
template <class IteratorTag>
struct is_forward_iterator {
	static constexpr bool value = std::is_same_v<IteratorTag, std::forward_iterator_tag>;
};

/*!@struct is_output_iterator
 * @brief  A class for checking whether an iterator is
 * an output iterator
 * @tparam IteratorTag - The iterator category as defined
 * in std::iterator_traits
 */
template <class IteratorTag>
struct is_output_iterator {
	static constexpr bool value = std::is_same_v<IteratorTag, std::output_iterator_tag>;
};

template <size_t dimensions>
struct is_planar_geometry {
	static const bool value = dimensions == 2;
};

/*!@struct  is_NT
 * @brief  A class for checking if the value pointed to
 * by the iterator is of type Kernel::NT
 * @tparam Kernel - A traits class containing collection of expected types
 * @tparam Type - The actual type
 */
template <class Kernel, class Type>
struct is_NT {
	static const bool value = std::is_same_v<typename Kernel::NT, std::remove_cv_t<std::remove_reference_t<Type>>>;
};


/*!@struct is_Wrapper_Point
 * @brief  A class for checking if the  value pointed
 * to by the iterator is of type \c Kernel::Wrapper_Point
 * @tparam Kernel - A traits class containing collection of expected types
 * @tparam Type - The actual type
 */
template <class Kernel, class Type>
struct is_Wrapper_Point {
	static const bool value = false;
};

/*!
 * @brief A partial specialization of is_Wrapper_Point
 * @details sets value to true iff \c Type == \c const Kernel::Wrapper_Point&
 */
template <class Kernel>
struct is_Wrapper_Point<Kernel, const typename Kernel::Wrapper_Point &> {
	static const bool value = true;
};

/*!
 * @brief A partial specialization of is_Wrapper_Point
 * @details sets value to true iff \c Type == \c Kernel::Wrapper_Point
 */
template <class Kernel>
struct is_Wrapper_Point<Kernel, typename Kernel::Wrapper_Point> {
	static const bool value = true;
};

/*!@struct is_MovetkPoint
 * @brief A class for checking if the value pointed to
 * by the iterator is of type \c Kernel::MovetkPoint
 * @tparam Kernel - A traits class containing collection of expected types
 * @tparam Type - The actual type
 */
template <class Kernel, class Type>
struct is_MovetkPoint {
	static const bool value = false;
};

/*!
 *@brief A partial specialization of is_MovetkPoint
 *@details sets value to true iff \c Type == \c const Kernel::MovetkPoint&
 */
template <class Kernel>
struct is_MovetkPoint<Kernel, const typename Kernel::MovetkPoint &> {
	static const bool value = true;
};

/*!
 *@brief A partial specialization of is_MovetkPoint
 *@details sets value to true iff \c Type == \c Kernel::MovetkPoint
 */
template <class Kernel>
struct is_MovetkPoint<Kernel, typename Kernel::MovetkPoint> {
	static const bool value = true;
};

template <class Kernel>
struct is_MovetkPoint<Kernel, const typename Kernel::MovetkPoint> {
	static const bool value = true;
};

/*!@struct is_MovetkLine
 * @brief A class for checking if the value pointed to
 * by the iterator is of type \c Kernel::MovetkLine
 * @tparam Kernel - A traits class containing collection of expected types
 * @tparam Type - The actual type
 */
template <class Kernel, class Type>
struct is_MovetkLine {
	static const bool value = false;
};

/*!
 *@brief A partial specialization of is_MovetkLine
 *@details sets value to true iff \c Type == \c const Kernel::MovetkLine&
 */
template <class Kernel>
struct is_MovetkLine<Kernel, const typename Kernel::MovetkLine &> {
	static const bool value = true;
};

/*!
 *@brief A partial specialization of is_MovetkLine
 *@details sets value to true iff \c Type == \c Kernel::MovetkLine
 */
template <class Kernel>
struct is_MovetkLine<Kernel, typename Kernel::MovetkLine> {
	static const bool value = true;
};

/*!@struct is_MovetkSegment
 * @brief A class for checking if the value pointed to
 * by the iterator is of type \c Kernel::MovetkSegment
 * @tparam Kernel - A traits class containing collection of expected types
 * @tparam Type - The actual type
 */
template <class Kernel, class Type>
struct is_MovetkSegment {
	static const bool value = false;
};

/*!
 *@brief A partial specialization of is_MovetkSegment
 *@details sets value to true iff \c Type == \c const Kernel::MovetkSegment&
 */
template <class Kernel>
struct is_MovetkSegment<Kernel, const typename Kernel::MovetkSegment &> {
	static const bool value = true;
};

/*!
 *@brief A partial specialization of is_MovetkSegment
 *@details sets value to true iff \c Type == \c Kernel::MovetkSegment
 */
template <class Kernel>
struct is_MovetkSegment<Kernel, typename Kernel::MovetkSegment> {
	static const bool value = true;
};

/*!@struct is_string
 * @brief A class for checking if the value pointed to
 * by an iterator is of type \c std::string
 * @tparam Type - The actual type
 */
template <class Type>
struct is_string {
	static const bool value = false;
};

/*!
 * @brief A partial specialization of is_string
 * @details sets value to true iff \c Type == \c std::string
 */
template <>
struct is_string<std::string> {
	static const bool value = true;
};

/*!@struct is_pair
 * @brief A class for checking if the value pointed to
 * by an iterator is of type \c std::pair
 * @tparam Type - The actual type
 */
template <class Type>
struct is_pair {
	static const bool value = false;
};

/*!
 * @brief A partial specialization of is_pair
 * @details sets value to true iff \c Type == \c std::pair
 */
template <class T1, class T2>
struct is_pair<std::pair<T1, T2>> {
	static const bool value = true;
};

/*!@struct is_equal_to
 * @brief tests equality of two input types
 * @tparam T1
 * @tparam T2
 */
template <class T1, class T2>
struct is_equal_to {
	static const bool value = false;
};

/*!
 * @brief A partial specialization of is_equal_to
 * @details sets value to true iff \c T1 == \c T2
 */
template <class T1>
struct is_equal_to<T1, T1> {
	static const bool value = true;
};

/*!@struct is_size_t
 *@brief checks whether \c T is \c std::size_t
 * @tparam Type
 */
template <class Type>
struct is_size_t {
	static const bool value = false;
};

/*!
 * @brief a partial specialization of is_size_t
 * @details sets value to true iff \c Type == \c std::size_t
 */
template <>
struct is_size_t<std::size_t> {
	static const bool value = true;
};

template <>
struct is_size_t<const std::size_t> {
	static const bool value = true;
};

template <std::size_t p>
struct is_L2_norm {
	static const bool value = false;
};

template <>
struct is_L2_norm<2> {
	static const bool value = true;
};

template <class ValueType, class Container>
struct is_simple_graph {
	static const bool value = false;
};

template <class ValueType>
struct is_simple_graph<ValueType, std::set<typename ValueType::MovetkPoint>> {
	static const bool value = true;
};

template <class ValueType>
struct is_simple_graph<ValueType, boost::unordered_set<typename ValueType::MovetkPoint>> {
	static const bool value = true;
};

template <class T>
struct is_directed_graph {
	static const bool value = false;
};

template <>
struct is_directed_graph<boost::directedS> {
	static const bool value = true;
};

template <class T>
struct is_tuple {
	static const bool value = false;
};

template <class... Types>
struct is_tuple<std::tuple<Types...>> {
	static const bool value = true;
};

template <class... Types>
struct is_tuple<const std::tuple<Types...>> {
	static const bool value = true;
};

/*template <class T, class Container>
struct is_stack{
    static const bool value = false;
};

template <class T>
struct is_stack<T, std::stack<T>>{
    static const bool value = true;
};*/

template <class T>
struct is_date {
	static const bool value = false;
};

template <>
struct is_date<std::size_t> {
	static const bool value = true;
};

template <>
struct is_date<const ParseDate> {
	static const bool value = true;
};

template <>
struct is_date<ParseDate> {
	static const bool value = true;
};

template <>
struct is_date<std::time_t> {
	static const bool value = true;
};

};      // namespace movetk::utils
#endif  // MOVETK_TYPECHECKS_H
