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
#include "movetk/utils/AlgorithmUtils.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/Requirements.h"

/**
 * @brief Contains a collection of similarity algorithms for trajectory processing
 */
namespace movetk::similarity {
/**
 * @brief Functor for computing the longest common subsequence
 * @details Implementation is based on doi=10.1.1.78.240
 * http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.78.240&rep=rep1&type=pdf
 * @tparam GeometryTraits The kernel to use
 * @tparam Norm The norm to use
 */
template <class GeometryTraits, class Norm>
class LongestCommonSubSequence {
private:
	typedef typename GeometryTraits::NT NT;
	NT eps;
	std::size_t del;
	template <class T>
	static T unsigned_abs(const T& a, const T& b) {
		return a > b ? a - b : b - a;
	}

public:
	/**
	 * @brief Constructs the longest common subsequence functor
	 * @param epsilon The maximum distance allowed to match elements.
	 * @param delta The maximum coordinate index distance allowed to match
	*/
	LongestCommonSubSequence(NT epsilon, std::size_t delta) {
		eps = epsilon;
		del = delta;
	}

	
	template <utils::RandomAccessIterator<typename GeometryTraits::MovetkPoint> InputIterator,
	          utils::OutputIterator<std::pair<InputIterator, InputIterator>> OutputIterator>
	std::size_t operator()(InputIterator polyline_a_first,
	                       InputIterator polyline_a_beyond,
	                       InputIterator polyline_b_first,
	                       InputIterator polyline_b_beyond,
	                       OutputIterator result) {
		utils::Output<OutputIterator> outputter(result);
		return compute_lcss(polyline_a_first, polyline_a_beyond, polyline_b_first, polyline_b_beyond, outputter);
	}
	template <utils::RandomAccessIterator<typename GeometryTraits::MovetkPoint> InputIterator>
	std::size_t operator()(InputIterator polyline_a_first,
	                       InputIterator polyline_a_beyond,
	                       InputIterator polyline_b_first,
	                       InputIterator polyline_b_beyond) {
		utils::Output<void> stub;
		return compute_lcss(polyline_a_first, polyline_a_beyond, polyline_b_first, polyline_b_beyond, stub);
	}

private:
	/**
	 * @brief Predicate for determining whether two values can be matched
	 * @tparam VALUE_TYPE The value type to use
	 * @param i Index of the first element
	 * @param i_value Value of the first element
	 * @param j Index of the second element
	 * @param j_value Value of the second element
	 * @return Whether these elements can be matched
	*/
	template <typename VALUE_TYPE>
	bool lcss_predicate(size_t i, const VALUE_TYPE& i_value, size_t j, const VALUE_TYPE& j_value) {
		auto v = i_value - j_value;
		Norm norm;
		NT distance = norm(v);
		return distance < eps && unsigned_abs(i, j) < del;
	}

	template <utils::RandomAccessIterator<typename GeometryTraits::MovetkPoint> InputIterator, typename OutputType>
	size_t compute_lcss(InputIterator polyline_a_first,
	                    InputIterator polyline_a_beyond,
	                    InputIterator polyline_b_first,
	                    InputIterator polyline_b_beyond,
	                    utils::Output<OutputType>& output) {
		std::size_t size_polyline_b = std::distance(polyline_b_first, polyline_b_beyond);
		std::vector<std::size_t> dp_row(size_polyline_b + 1);
		std::fill(std::begin(dp_row), std::begin(dp_row) + size_polyline_b + 1, 0);
		InputIterator it_a = polyline_a_first;
		std::size_t i = 1, prev_value = 0, prev_cell = 0;

		[[maybe_unused]] std::vector<std::pair<InputIterator, InputIterator>> stored_output;
		for (; it_a != polyline_a_beyond; ++it_a, ++i) {
			std::size_t j = 1, previous = 0, current = 0;
			for (auto it_b = polyline_b_first; it_b != polyline_b_beyond; ++it_b, ++j) {
				if (lcss_predicate(i, *it_a, j, *it_b)) {
					current = dp_row[j - 1] + 1;
					if (current != prev_value) {
						prev_value = current;
						prev_cell = j;
						if constexpr (output.requires_output()) {
							stored_output.push_back(std::make_pair(it_a, it_b));
						}
					} else {
						if (j < prev_cell) {
							prev_cell = j;
							if constexpr (output.requires_output()) {
								assert(!stored_output.empty());
								stored_output.back() = std::make_pair(it_a, it_b);
							}
						}
					}
				} else {
					current = std::max(dp_row[j], previous);
				}

				dp_row[j - 1] = previous;
				previous = current;
			}
			dp_row[j - 1] = previous;
		}
		if constexpr (output.requires_output()) {
			std::copy(stored_output.begin(), stored_output.end(), output.target);
		}
		return dp_row.back();
	}
};
}  // namespace movetk::similarity

#endif  // MOVETK_SIMILARITY_H
