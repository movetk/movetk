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
// Created by onur on 10/11/18.
//

#ifndef MOVETK_SORTEDPROBEREADER_H
#define MOVETK_SORTEDPROBEREADER_H

#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

#include "SortByField.h"

namespace movetk::io {
template <class ProbeInputIterator, int SortByFieldIdx>
class SortedProbeReader {
public:
	using ProbePoint = typename std::iterator_traits<ProbeInputIterator>::value_type;
	using iterator = typename std::vector<ProbePoint>::iterator;

	SortedProbeReader(ProbeInputIterator start, ProbeInputIterator beyond) {
		// Store probe points in memory
		auto t_start = std::chrono::high_resolution_clock::now();
		std::size_t probe_count = 0;
		for (auto pit = start; pit != beyond; ++pit) {
			buffered_probe.push_back(*pit);
			++probe_count;
		}
		auto t_end = std::chrono::high_resolution_clock::now();
		display("read probe", t_start, t_end);
		std::cout << "Buffered " << probe_count << " probe points.";

		// Sort all probe points by SortByFieldIdx
		SortByField<SortByFieldIdx, ProbePoint> sort_by_field_id_asc;
		t_start = std::chrono::high_resolution_clock::now();
#ifdef _GLIBCXX_PARALLEL
		__gnu_parallel::sort(buffered_probe.begin(), buffered_probe.end(), sort_by_field_id_asc);
#else
		std::sort(buffered_probe.begin(), buffered_probe.end(), sort_by_field_id_asc);
		// with <execution> header, since C++17, but not yet available on any compiler/platform.
		// std::sort(std::execution::par, buffered_probe.begin(), buffered_probe.end(), sort_by_field_id_asc);
#endif
		t_end = std::chrono::high_resolution_clock::now();
		display("sort", t_start, t_end);
	}

	iterator begin() { return std::begin(buffered_probe); }

	iterator end() { return std::end(buffered_probe); }

private:
	std::vector<ProbePoint> buffered_probe;
};

}  // namespace movetk::io
#endif  // MOVETK_SORTEDPROBEREADER_H
