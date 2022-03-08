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
#include "movetk/algo/Search.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/Requirements.h"
namespace movetk::algo::segmentation {

/*!
 *
 * @tparam TestType
 */
template <class GeometryTraits, class TestType>
class MonotoneSegmentation {
	// based on https://doi.org/10.1145/1869790.1869821
	// TODO Requirements for TestType
private:
	typedef typename GeometryTraits::NT NT;
	typedef TestType TEST;
	movetk::algo::BinarySearch<GeometryTraits, TEST> binary_search;
	TEST test;

public:
	/*!
	 *
	 * @param threshold
	 */
	MonotoneSegmentation(NT threshold) : binary_search(threshold), test(threshold){};
	/*!
	 *
	 * @tparam InputIterator
	 * @tparam OutputIterator
	 * @param first
	 * @param beyond
	 * @param result
	 */
	template <class InputIterator,
	          class OutputIterator,
	          typename = movetk::utils::requires_random_access_iterator<InputIterator>,
	          typename = movetk::utils::requires_output_iterator<OutputIterator>,
	          typename = movetk::utils::requires_valid_type<GeometryTraits, typename InputIterator::value_type>,
	          typename = movetk::utils::requires_equality<typename InputIterator::value_type,
	                                                      typename OutputIterator::value_type::value_type>>
	void operator()(InputIterator first, InputIterator beyond, OutputIterator result) {
		// ASSERT_RANDOM_ACCESS_ITERATOR(InputIterator);
		// ASSERT_OUTPUT_ITERATOR(OutputIterator);
		InputIterator it = first;
		size_t remainder = std::distance(first, beyond);
		size_t MaxAllowedSteps = 0;
		size_t steps = 1, a = 1;
		size_t range = 0;
		while (it != beyond) {
			steps = 1;
			a = 2;
			MaxAllowedSteps = floor(log2(remainder));
			while (steps <= MaxAllowedSteps) {
				if (test(it, it + a)) {
					steps++;
					a = 2 * a;
				} else {
					range = pow(2, steps) - pow(2, steps - 1);
					a = a / 2 + binary_search(it, a / 2, 0, range);
					if (a == 1)
						a++;
					break;
				}
			}
			if (a <= remainder) {
				*result = it + (a - 1);
				it = it + (a - 1);
			} else {
				if (test(it, it + remainder))
					it = beyond;
				else {
					a = a / 2 + binary_search(it, a / 2, 1, remainder - a / 2);
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
		// cout<<endl;
	}
};

}  // namespace movetk::algo::segmentation
#endif