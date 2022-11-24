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

#ifndef MOVETK_OUTLIERDETECTION_SMARTGREEDYOUTLIERDETECTOR_H
#define MOVETK_OUTLIERDETECTION_SMARTGREEDYOUTLIERDETECTOR_H


#include "OutlierDetection.h"
#include "movetk/geo/geo.h"
#include "movetk/io/CartesianProbeTraits.h"
#include "movetk/utils/Requirements.h"

namespace movetk::outlierdetection {

// based on B. Custers & M. van de Kerkhof & W. Meulemans and B. Speckmann & F. Staals (2019) .
// Maximum Physically Consistent Trajectories
// published in SIGSPATIAL 2019
struct smart_greedy_outlier_detector_tag;
// Physics-based outlier detection running in O(nk) time, where k is the number of outliers
template <class GeometryKernel, class Predicate>
class OutlierDetection<GeometryKernel, Predicate, smart_greedy_outlier_detector_tag> {
private:
	using NT = typename GeometryKernel::NT;
	Predicate m_predicate;
	NT m_threshold;

public:
	/*!
	 *@param InThreshold
	 */
	OutlierDetection(NT threshold) {
		m_threshold = threshold;
		m_predicate = Predicate(m_threshold);
	};

	/*!
	 *
	 * @tparam InputIterator
	 * @tparam OutputIterator
	 * @param first
	 * @param beyond
	 * @param result
	 */
	template <std::random_access_iterator InputIterator,
	          typename Container
				//,typename = movetk::utils::requires_random_access_iterator<InputIterator>,
	          //typename = movetk::utils::requires_equality<InputIterator, typename Container::value_type::value_type>
	>
	typename Container::const_iterator operator()(InputIterator first, InputIterator beyond, Container& sequences) {
		auto bit = std::back_inserter(sequences);
		for (auto it = first; it != beyond; it++) {
			bool extend_subsequence = false;
			for (auto sit = std::begin(sequences); sit != std::end(sequences); sit++) {
				InputIterator prev = *(sit->cend() - 1);
				if (m_predicate(*prev, *it)) {
					auto node_bit = std::back_inserter(*sit);
					node_bit = it;
					extend_subsequence = true;
				}
			}
			if (!extend_subsequence) {
				typename Container::value_type node;
				auto node_bit = std::back_inserter(node);
				node_bit = it;
				bit = node;
			}
		}

		std::sort(std::begin(sequences), std::end(sequences), [](auto a, auto b) {
			std::size_t size_a = std::distance(std::begin(a), std::end(a));
			std::size_t size_b = std::distance(std::begin(b), std::end(b));
			return size_a > size_b;
		});

		auto sit = std::cbegin(sequences);
		std::size_t max_size = sit->size();

		auto eit = std::remove_if(std::begin(sequences), std::end(sequences), [max_size](auto a) {
			std::size_t size_a = std::distance(std::begin(a), std::end(a));
			return size_a != max_size;
		});

		return eit;

	}  // operator()
};
}  // namespace movetk::outlierdetection


#endif  // MOVETK_OUTLIERDETECTION_H
