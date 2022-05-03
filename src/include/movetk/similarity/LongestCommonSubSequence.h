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
// Created by Mitra, Aniket on 2019-06-03.
//

#ifndef MOVETK_ALGO_SIMILARITY_LONGESTCOMMONSUBSEQUENCE_H
#define MOVETK_ALGO_SIMILARITY_LONGESTCOMMONSUBSEQUENCE_H

#include <stdlib.h>

#include <iterator>

#include "movetk/geom/GeometryInterface.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/Requirements.h"


namespace movetk::similarity {


template <class GeometryTraits, class Norm>
class LongestCommonSubSequence {
	// based on doi=10.1.1.78.240
	// http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.78.240&rep=rep1&type=pdf
private:
	typedef typename GeometryTraits::NT NT;
	NT eps;
	std::size_t del;
	template <class T>
	T abs(T& a, T& b) {
		return a > b ? a - b : b - a;
	}

public:
	LongestCommonSubSequence(NT epsilon, std::size_t delta) {
		eps = epsilon;
		del = delta;
	}

	template <class InputIterator,
	          class OutputIterator,
	          typename = movetk::utils::requires_random_access_iterator<InputIterator>,
	          typename = movetk::utils::requires_output_iterator<OutputIterator>,
	          typename = movetk::utils::requires_movetk_point<GeometryTraits, typename InputIterator::value_type>,
	          typename = movetk::utils::requires_pair<typename OutputIterator::value_type>,
	          typename = movetk::utils::requires_equality<typename InputIterator::value_type,
	                                                      typename OutputIterator::value_type::first_type::value_type>,
	          typename = movetk::utils::requires_equality<typename InputIterator::value_type,
	                                                      typename OutputIterator::value_type::second_type::value_type>>
	std::size_t operator()(InputIterator polyline_a_first,
	                       InputIterator polyline_a_beyond,
	                       InputIterator polyline_b_first,
	                       InputIterator polyline_b_beyond,
	                       OutputIterator result) {
		std::size_t size_polyline_b = std::distance(polyline_b_first, polyline_b_beyond);
		std::vector<std::size_t> dp_row(size_polyline_b + 1);
		std::fill(std::begin(dp_row), std::begin(dp_row) + size_polyline_b + 1, 0);
		InputIterator it_a = polyline_a_first;
		std::size_t i = 1, prev_value = 0, prev_cell = 0;
		Norm norm;
		while (it_a != polyline_a_beyond) {
			std::size_t j = 1, previous = 0, current = 0;
			InputIterator it_b = polyline_b_first;
			while (it_b != polyline_b_beyond) {
				auto v = *it_a - *it_b;
				NT distance = norm(v);
				if (distance < eps && this->abs(i, j) < del) {
					current = dp_row[j - 1] + 1;
					if (current != prev_value) {
						prev_value = current;
						prev_cell = j;
						*result = std::make_pair(it_a, it_b);
					} else {
						if (j < prev_cell) {
							prev_cell = j;
							*result -= std::make_pair(it_a, it_b);
						}
					}
				} else
					current = std::max(dp_row[j], previous);
				dp_row[j - 1] = previous;
				previous = current;
				j++;
				it_b++;
			}
			dp_row[j - 1] = previous;
			it_a++;
			i++;
		}
		return dp_row.back();
	}
	template <class InputIterator,
	          typename = movetk::utils::requires_random_access_iterator<InputIterator>,
	          typename = movetk::utils::requires_movetk_point<GeometryTraits, typename InputIterator::value_type>>
	std::size_t operator()(InputIterator polyline_a_first,
	                       InputIterator polyline_a_beyond,
	                       InputIterator polyline_b_first,
	                       InputIterator polyline_b_beyond) {
		std::size_t size_polyline_b = std::distance(polyline_b_first, polyline_b_beyond);
		std::vector<std::size_t> dp_row(size_polyline_b + 1);
		std::fill(std::begin(dp_row), std::begin(dp_row) + size_polyline_b + 1, 0);
		InputIterator it_a = polyline_a_first;
		std::size_t i = 1, prev_value = 0, prev_cell = 0;
		Norm norm;
		while (it_a != polyline_a_beyond) {
			std::size_t j = 1, previous = 0, current = 0;
			InputIterator it_b = polyline_b_first;
			while (it_b != polyline_b_beyond) {
				typename GeometryTraits::MovetkVector v = *it_a - *it_b;
				NT distance = norm(v);
				if (distance < eps && this->abs(i, j) < del) {
					current = dp_row[j - 1] + 1;
					if (current != prev_value) {
						prev_value = current;
						prev_cell = j;
					} else {
						if (j < prev_cell) {
							prev_cell = j;
						}
					}
				} else
					current = std::max(dp_row[j], previous);
				dp_row[j - 1] = previous;
				previous = current;
				j++;
				it_b++;
			}
			dp_row[j - 1] = previous;
			it_a++;
			i++;
		}
		return dp_row.back();
	}
};
}  // namespace movetk::similarity

#endif  // MOVETK_SIMILARITY_H
