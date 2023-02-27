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
#include <iterator>
#include <type_traits>

#include "movetk/utils/TypeChecks.h"

#define MOVETK_EPS 0.00001

namespace movetk::utils {

template <typename T, template <typename...> typename ELEMENT>
struct is_specialization_of : public std::false_type {};

template <template <typename...> typename ELEMENT, typename... ARGS>
struct is_specialization_of<ELEMENT<ARGS...>, ELEMENT> : public std::true_type {};

/**
 * @concept TupleType
 * @brief Models a tuple. Considers something a tuple if it is a specialization of std::tuple
*/
template <typename T>
concept TupleType = is_specialization_of<T, std::tuple>::value;

/**
 * @concept TupleTypeOf
 * @brief Models a tuple with the given ARGS as expected types.
 */
template <typename T, typename... ARGS>
concept TupleTypeOf = std::is_same_v<T, std::tuple<ARGS...>>;

template <typename T>
using single_type_pair = std::pair<T, T>;

/**
 * @concept RandomAccessIterator
 * @brief Models a random access iterator whose value type is VALUE.
*/
template <typename T, typename VALUE>
concept RandomAccessIterator = std::random_access_iterator<T> && std::is_convertible_v<std::iter_value_t<T>, VALUE>;

/**
 * @concept RandomAccessPointIterator 
 * @brief Models a random access iterator whose value type is a point as defined by the MoveTK KERNEL
 */
template <typename T, typename KERNEL>
concept RandomAccessPointIterator = std::random_access_iterator<T> && std::is_convertible_v < std::iter_value_t<T>,
typename KERNEL::MovetkPoint > ;

/**
 * @concept OutputIterator 
 * @brief Models an output iterator that is assignable with a VALUE object
 */
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

template <typename T, typename... TYPES_AT>
concept RandomAccessIteratorOfTupleWith =
    std::random_access_iterator<T> && TupleWithTypes<typename T::value_type, TYPES_AT...>;

template <typename T>
concept Kernel = requires() {
	typename T::NT;
	{ T::dim } -> std::convertible_to<size_t>;
};

template <typename T>
struct is_planar_geometry2 : public std::integral_constant<bool, T::dim == 2> {};

template <typename T, template <typename> typename... PREDICATES>
concept KernelSatisfying = Kernel<T> &&(PREDICATES<T>::value&&...);
}  // namespace movetk::utils
#endif  // MOVETK_REQUIREMENTS_H
