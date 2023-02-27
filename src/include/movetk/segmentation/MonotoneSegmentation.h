/*
 * Copyright (C) 2018-2020
 * HERE Europe B.V.
 * Utrecht University (The Netherlands).
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


/*! @file Segmentation.h
 *  @brief  Trajectory Segmentation  Algorithms
 *  @authors Aniket Mitra (aniket.mitra@here.com), Wouter Jongeling (wouter.jongeling@gmail.com)
 */
#ifndef MOVETK_ALGO_SEGMENTATION_MONOTONESEGMENTATION_H
#define MOVETK_ALGO_SEGMENTATION_MONOTONESEGMENTATION_H
#include "movetk/Search.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/Requirements.h"
namespace movetk::segmentation {

/**
 * @brief Segmentation algorithm for a monotone predicate
 *
 * This segmentation algorithm considers monotone predicates:
 * Based on https://doi.org/10.1145/1869790.1869821
 * @tparam GeometryTraits The kernel
 * @tparam TestType
 *
 */
template <class Kernel, class PREDICATE>
class MonotoneSegmentation {
private:
	using NT = typename Kernel::NT;
	movetk::algo::BinarySearch<PREDICATE> m_binary_search;
	PREDICATE m_predicate;

public:
	/*!
	 * Constructs a monotone segmentation functor
	 * @param threshold The threshold to use for the binary predicate.
	 */
	explicit MonotoneSegmentation(NT threshold) : m_binary_search(threshold), m_predicate(threshold){};

	/*!
	 *
	 * @tparam InputIterator
	 * @tparam OutputIterator
	 * @param first
	 * @param beyond
	 * @param result
	 */
	template <std::random_access_iterator InputIterator, utils::OutputIterator<InputIterator> OutputIterator>
	void operator()(InputIterator first, InputIterator beyond, OutputIterator result) {
		auto it = first;
		size_t remainder = std::distance(first, beyond);
		size_t max_allowed_steps = 0;
		size_t steps = 1, a = 1;
		size_t range = 0;
		// Perform double-and-search: an exponential search, followed by a binary search, to find
		// the right measurement in the trajectory where the predicate changes from being true to being false.
		while (it != beyond) {
			steps = 1;
			a = 2;
			max_allowed_steps = floor(log2(remainder));
			while (steps <= max_allowed_steps) {
				if (m_predicate(it, it + a)) {
					steps++;
					a = 2 * a;
				} else {
					range = pow(2, steps) - pow(2, steps - 1);
					a = a / 2 + m_binary_search(it, a / 2, 0, range);
					if (a == 1)
						a++;
					break;
				}
			}
			if (a <= remainder) {
				*result = it + (a - 1);
				it = it + (a - 1);
			} else {
				if (m_predicate(it, it + remainder))
					it = beyond;
				else {
					a = a / 2 + m_binary_search(it, a / 2, 1, remainder - a / 2);
					if (a == 1)
						a++;
					if (a <= remainder) {
						*result = it + (a - 1);
					}
					it = it + (a - 1);
				}
			}
			remainder = std::distance(it, beyond);
		}
	}
};

}  // namespace movetk::segmentation
#endif