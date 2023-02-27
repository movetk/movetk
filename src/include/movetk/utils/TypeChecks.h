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

/*!
 *
 *  @namespace movetk::utils
 *  @brief the core of movetk
 *  @author Aniket Mitra (aniket.mitra@here.com) 
 *	@author Bram Custers (b.a.custers@gmail.com)
 */
namespace movetk::utils {
template <size_t dimensions>
struct is_planar_geometry {
	static const bool value = dimensions == 2;
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

