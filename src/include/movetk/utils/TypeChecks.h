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
#ifndef MOVETK_TYPECHECKS_H
#define MOVETK_TYPECHECKS_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/unordered_set.hpp>
#include <iterator>
#include <set>
#include <stack>
#include <utility>

#include "movetk/io/ParseDate.h"

#define MOVETK_DEFINE_HAS_TYPE_METAMETHOD(target_type)  \
	template <typename T, typename = void>                \
	struct has_##target_type : public std::false_type {}; \
	template <typename T>                                 \
	struct has_##target_type<T, std::void_t<typename T::target_type>> : public std::true_type {};


/*!
 *
 *  @namespace movetk::utils
 *  @brief the core of movetk
 *  @author Aniket Mitra (aniket.mitra@here.com) 
 *	@author Bram Custers (b.a.custers@gmail.com)
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
struct is_random_access_iterator : public std::is_same<IteratorTag, std::random_access_iterator_tag> {};

/*!@struct is_forward_iterator
 * @brief  A class for checking whether an iterator is
 * a forward iterator
 * @tparam IteratorTag - The iterator category as defined
 * in std::iterator_traits
 */
template <class IteratorTag>
struct is_forward_iterator : public std::is_same<IteratorTag, std::forward_iterator_tag> {};

/*!
 * @brief  A class for checking whether an iterator is
 * an output iterator
 * @tparam IteratorTag - The iterator category as defined
 * in std::iterator_traits
 */
template <class IteratorTag>
struct is_output_iterator {
	static constexpr bool value = std::is_same_v<IteratorTag, std::output_iterator_tag>;
};

/**
 * @brief Predicate to check if the iterator tag corresponds to an input iterator
 * @tparam IteratorTag The iterator tag
*/
template <class IteratorTag>
struct is_input_iterator {
	static constexpr bool value = std::is_same_v<IteratorTag, std::input_iterator_tag>;
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
struct is_NT : public std::is_same<typename Kernel::NT, std::remove_cv_t<std::remove_reference_t<Type>>> {};

#define MAKE_KERNEL_ELEMENT_CHECK(type_name)                                                      \
	template <typename Kernel, typename Type>                                                       \
	struct is_##type_name {                                                                         \
		static constexpr bool value = std::is_same_v<std::decay_t<Type>, typename Kernel::type_name>; \
	};

/*!@struct is_Wrapper_Point
 * @brief  A class for checking if the  value pointed
 * to by the iterator is of type \c Kernel::Wrapper_Point
 * @tparam Kernel - A traits class containing collection of expected types
 * @tparam Type - The actual type
 */
MAKE_KERNEL_ELEMENT_CHECK(Wrapper_Point)


/*!@struct is_MovetkPoint
 * @brief A class for checking if the value pointed to
 * by the iterator is of type \c Kernel::MovetkPoint
 * @tparam Kernel - A traits class containing collection of expected types
 * @tparam Type - The actual type
 */
MAKE_KERNEL_ELEMENT_CHECK(MovetkPoint)

/*!@struct is_MovetkLine
 * @brief A class for checking if the value pointed to
 * by the iterator is of type \c Kernel::MovetkLine
 * @tparam Kernel - A traits class containing collection of expected types
 * @tparam Type - The actual type
 */
MAKE_KERNEL_ELEMENT_CHECK(MovetkLine)

/*!@struct is_MovetkSegment
 * @brief A class for checking if the value pointed to
 * by the iterator is of type \c Kernel::MovetkSegment
 * @tparam Kernel - A traits class containing collection of expected types
 * @tparam Type - The actual type
 */
MAKE_KERNEL_ELEMENT_CHECK(MovetkSegment)

/*!@struct is_string
 * @brief A class for checking if the value pointed to
 * by an iterator is of type \c std::string
 * @tparam Type - The actual type
 */
template <class Type>
struct is_string : public std::is_same<std::decay_t<Type>, std::string> {};

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

/**
 * @concept Pair
 * @brief Models the concept of a Pair: an element being exactly a std::pair<U,V> for now.
*/
template<typename T>
concept Pair = requires() {
	typename T::first_type;
	typename T::second_type;
	requires std::same_as<T, std::pair<typename T::first_type, typename T::second_type>>;
};

template<typename T>
concept L2Norm = requires() {
	{T::P}->std::convertible_to<size_t>;
	requires T::P == 2;
};

template <std::size_t p>
struct is_L2_norm {
	static const bool value = p == 2;
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
struct is_tuple
    : public std::conditional_t<std::is_same_v<std::decay_t<T>, T>, std::false_type, is_tuple<std::decay_t<T>>> {};

template <class... Types>
struct is_tuple<std::tuple<Types...>> : public std::true_type {};

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
	using decayed_t = std::decay_t<T>;
	static const bool value = std::is_same_v<decayed_t, std::size_t> || std::is_same_v<decayed_t, io::ParseDate> ||
	                          std::is_same_v<decayed_t,std::time_t>;
};
}      // namespace movetk::utils
#endif  // MOVETK_TYPECHECKS_H

