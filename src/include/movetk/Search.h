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

/*! @file Search.h
 *  @brief  Trajectory Segmentation  Algorithms
 *  @authors Aniket Mitra (aniket.mitra@here.com)
 */


#ifndef MOVETK_SEARCH_H
#define MOVETK_SEARCH_H


#include <iterator>

#include "movetk/utils/Requirements.h"

namespace movetk::algo {
/**
 * @brief Binary search functor
 * @tparam PREDICATE The binary predicate to use. Should accept two iterators.
 */
template <class PREDICATE>
class BinarySearch {
private:
	PREDICATE test;

public:
	BinarySearch() = default;

	template <typename NT>
		requires(std::constructible_from<PREDICATE, NT>)
	BinarySearch(NT threshold) : test(threshold) {}
	/*!
	 *
	 * @tparam InputIterator
	 * @param first
	 * @param min
	 * @param left
	 * @param right
	 * @return
	 */
	template <std::random_access_iterator InputIterator>
		requires(std::is_invocable_r_v<bool, PREDICATE, InputIterator, InputIterator>)
	size_t operator()(InputIterator first, size_t min, size_t left, size_t right) {
		auto current_left = left, current_right = right;
		while (true) {
			if (left >= right)
				return right;
			size_t mid = (left + right) / 2;
			if (!test(first, first + min + mid)) {
				left += 1;
				right = mid;
			} else {
				left = mid + 1;
			}
		}
		// Never reached
		return 0;
	}
};

}  // namespace movetk::algo

#endif  // MOVETK_SEARCH_H
