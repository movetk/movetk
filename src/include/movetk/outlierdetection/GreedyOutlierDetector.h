/*
 * Copyright (C) 2018-2020
 * HERE Europe B.V.
 * Utrecht University (The Netherlands).
 * TU/e (The Netherlands).
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
// Created by Mees van de Kerkhof (m.a.vandekerkhof@uu.nl),
// Bram Custers (b.a.custers@tue.nl),
// Kevin Verbeek (k.a.b.verbeek@tue.nl)
// Modified by Aniket Mitra (aniket.mitra@here.com)
//

#ifndef MOVETK_OUTLIERDETECTION_GREEDYOUTLIERDETECTOR_H
#define MOVETK_OUTLIERDETECTION_H


#include "OutlierDetection.h"
#include "movetk/geo/geo.h"
#include "movetk/io/CartesianProbeTraits.h"
#include "movetk/utils/Requirements.h"

namespace movetk::outlierdetection {

struct greedy_outlier_detector_tag;

/**
 * @brief Greedy outlier detection running in O(n) time.
 * Outputs the iterators pointing to the element that should be retained after outlier
 * The first element of the range is always added. An arbitrary element is added if
 * the previous element and the element statisfy the predicate.
 * @tparam GeometryKernel Geometry kernel to use
 * @tparam BinaryPredicate Binary predicate type that should accept two values, dictated by the
 * input iterators provided to the operator call.
 */
template <class GeometryKernel, class BinaryPredicate>
class OutlierDetection<GeometryKernel, BinaryPredicate, greedy_outlier_detector_tag> {
public:
	using NT = typename GeometryKernel::NT;
	/*!
	 *@param InThreshold
	 */
	OutlierDetection(NT threshold) : m_threshold(threshold), m_predicate(m_threshold) {}

	/*!
	 *
	 * @tparam TrajectoryIterator
	 * @tparam OutputIterator
	 * @param first
	 * @param beyond
	 * @param result
	 */
	template <class InputIterator,
	          class OutputIterator,
	          typename = movetk::utils::requires_random_access_iterator<InputIterator>,
	          typename = movetk::utils::requires_output_iterator<OutputIterator>,
	          typename = movetk::utils::requires_equality<typename InputIterator::value_type,
	                                                      typename OutputIterator::value_type::value_type>>
	void operator()(InputIterator first, InputIterator beyond, OutputIterator result) {
		*result = first;
		for (auto it = std::next(first), prev_it = first; it != beyond; it++, ++prev_it) {
			if (m_predicate(*prev_it, *it)) {
				*result = it;
			}
		}
	}  // operator()
private:
	BinaryPredicate m_predicate;
	NT m_threshold;
};
}  // namespace movetk::outlierdetection


#endif  // MOVETK_OUTLIERDETECTION_H
