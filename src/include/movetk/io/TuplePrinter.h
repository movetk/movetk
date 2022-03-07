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
// helper function to print a tuple of any size
template <class Tuple, std::size_t N>
struct TuplePrinter {
	static void print_tuple(std::ostream& os, const Tuple& t, int precision) {
		TuplePrinter<Tuple, N - 1>::print_tuple(os, t, precision);
		os << "," << std::setprecision(precision) << std::get<N - 1>(t);
	}
};

template <class Tuple>
struct TuplePrinter<Tuple, 1> {
	static void print_tuple(std::ostream& os, const Tuple& t, int precision) {
		os << std::setprecision(precision) << std::get<0>(t);
	}
};

template <class... Args>
void print_tuple(std::ostream& os, const std::tuple<Args...>& t, int precision = 8) {
	os.setf(std::ios::fixed);
	TuplePrinter<decltype(t), sizeof...(Args)>::print_tuple(os, t, precision);
}
// end helper function

/**
 * Print arguments to os enclosed by square braces, for example, for printing coordinates in GeoJSON.
 */
template <class Arg, class... Args>
inline void variadic_print(std::ostream& os, Arg&& first, Args&&... rest) {
	os << "[";
	os << std::forward<Arg>(first);
	((os << ", " << std::setprecision(8) << std::forward<Args>(rest)), ...);
	os << "]";
}

}  // namespace movetk::io
#endif  // MOVETK_TUPLEPRINTER_H
