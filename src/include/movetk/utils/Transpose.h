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
// Created by onur on 10/6/18.
//

#ifndef MOVETK_TRANSPOSE_H
#define MOVETK_TRANSPOSE_H

#include <string>
#include <tuple>
#include <vector>

namespace movetk::utils {
template <class... FIELDS>
struct Transpose {
	constexpr static std::size_t N = sizeof...(FIELDS);
	typedef std::tuple<FIELDS...> row_tuple_type;
	typedef std::tuple<std::vector<FIELDS>...> tuple_of_vectors_type;
	std::vector<row_tuple_type> &_inp;

	explicit Transpose(std::vector<row_tuple_type> &inp) : _inp(inp) {}

	template <std::size_t idx>
	std::vector<typename std::tuple_element<idx, std::tuple<FIELDS...>>::type> convert_rows_to_column() {
		using type = typename std::tuple_element<idx, std::tuple<FIELDS...>>::type;
		std::vector<type> v;
		for (auto x : _inp) {
			v.push_back(std::get<idx>(x));
		}
		return v;
	}

	template <std::size_t... idx>
	tuple_of_vectors_type convert_rows_to_columns(std::index_sequence<idx...>) {
		return std::make_tuple(convert_rows_to_column<idx>()...);
	}

	tuple_of_vectors_type operator()() { return convert_rows_to_columns(std::make_index_sequence<N>{}); }
};
}  // namespace movetk::utils
#endif  // MOVETK_TRANSPOSE_H
