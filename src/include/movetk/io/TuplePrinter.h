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
// Created by onur on 9/23/18.
//

#ifndef MOVETK_TUPLEPRINTER_H
#define MOVETK_TUPLEPRINTER_H

#include <iomanip>
#include <ostream>
#include <tuple>

namespace movetk::io {

namespace detail {
/**
 * @brief Write the given arguments to stream, using the provided separator
 * @tparam SEPARATOR The separator type
 * @tparam ARG Type of first argument to write
 * @tparam ...ARGS Types of the rest of the arguments to write
 * @param stream The stream to write to
 * @param separator The separator to write between elements
 * @param arg First argument
 * @param ...args Rest of the arguments
 * @return The stream
*/
template <typename SEPARATOR, typename ARG, typename... ARGS>
std::ostream& write_separated(std::ostream& stream, SEPARATOR&& separator, ARG&& arg, ARGS&&... args) {
	stream << std::forward<ARG>(arg);
	((stream << separator << args), ...);
	return stream;
}
template <typename SEPARATOR, typename TUPLE, size_t... IS>
std::ostream& write_separated_tuple(std::ostream& stream,
                                    SEPARATOR&& separator,
                                    TUPLE&& tuple,
                                    std::index_sequence<IS...>) {
	return write_separated(stream, std::forward<SEPARATOR>(separator), std::get<IS>(std::forward<TUPLE>(tuple))...);
}
}  // namespace detail

template <typename Tuple>
void print_tuple(std::ostream& os, const Tuple& t, int precision = 8) {
	os.setf(std::ios::fixed);
	os << std::setprecision(precision);
	detail::write_separated_tuple(os, ',', t, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
}

/**
 * Print arguments to os enclosed by square braces, for example, for printing coordinates in GeoJSON.
 */
template <class Arg, class... Args>
inline void variadic_print(std::ostream& os, Arg&& first, Args&&... rest) {
	os << "[";
	detail::write_separated(os, ',', std::forward<Arg>(first), std::forward<Args>(rest)...);
	os << "]";
}

}  // namespace movetk::io
#endif  // MOVETK_TUPLEPRINTER_H
