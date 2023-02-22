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

#ifndef MOVETK_HIGHFREQUENCYTRAJECTORYREADER_H
#define MOVETK_HIGHFREQUENCYTRAJECTORYREADER_H

#include <type_traits>

#include "movetk/io/HighFrequencyTrajectorySplitter.h"
#include "movetk/io/ProbeReader.h"
#include "movetk/io/SortedProbeReader.h"
#include "movetk/io/TrajectoryReader.h"

namespace movetk::io {
template <class HighFrequencyTrajectoryTraits>
struct GroupedHighFrequencyTrajectoryReader {
	using TrajectoryTraits = typename HighFrequencyTrajectoryTraits::TrajectoryTraits;
	using ProbeTraits = typename TrajectoryTraits::ProbeTraits;
	using ProbeInputIterator = typename ProbeTraits::ProbeInputIterator;
	using sorted_probe_reader_type = SortedProbeReader<ProbeInputIterator,
	                                                   TrajectoryTraits::SplitByFieldIdx>;  // not used
	using trajectory_reader_type = movetk::io::TrajectoryReader<TrajectoryTraits, ProbeInputIterator>;
};

template <class HighFrequencyTrajectoryTraits>
struct NotGroupedHighFrequencyTrajectoryReader {
	using TrajectoryTraits = typename HighFrequencyTrajectoryTraits::TrajectoryTraits;
	using ProbeTraits = typename TrajectoryTraits::ProbeTraits;
	using ProbeInputIterator = typename ProbeTraits::ProbeInputIterator;
	using sorted_probe_reader_type = SortedProbeReader<ProbeInputIterator, TrajectoryTraits::SplitByFieldIdx>;
	using SortedProbeInputIterator = typename sorted_probe_reader_type::iterator;
	using trajectory_reader_type = movetk::io::TrajectoryReader<TrajectoryTraits, SortedProbeInputIterator>;
};

template <class HighFrequencyTrajectoryTraits, bool TrajectoriesAreGrouped>
class HighFrequencyTrajectoryReader
    : public std::conditional<TrajectoriesAreGrouped,
                              GroupedHighFrequencyTrajectoryReader<HighFrequencyTrajectoryTraits>,
                              NotGroupedHighFrequencyTrajectoryReader<HighFrequencyTrajectoryTraits>>::type {
public:
	using base_type =
	    typename std::conditional<TrajectoriesAreGrouped,
	                              GroupedHighFrequencyTrajectoryReader<HighFrequencyTrajectoryTraits>,
	                              NotGroupedHighFrequencyTrajectoryReader<HighFrequencyTrajectoryTraits>>::type;
	using TrajectoryTraits = typename base_type::TrajectoryTraits;
	using ProbeTraits = typename base_type::ProbeTraits;
	using ProbeInputIterator = typename base_type::ProbeInputIterator;
	using trajectory_reader_type = typename base_type::trajectory_reader_type;
	using TrajectoryInputIterator = typename trajectory_reader_type::iterator;
	using sorted_probe_reader_type = typename base_type::sorted_probe_reader_type;
	using SortedProbeInputIterator = typename sorted_probe_reader_type::iterator;
	using hifreq_splitter_type = HighFrequencyTrajectorySplitter<TrajectoryInputIterator,
	                                                             HighFrequencyTrajectoryTraits::DateIdx,
	                                                             HighFrequencyTrajectoryTraits::LatIdx,
	                                                             HighFrequencyTrajectoryTraits::LonIdx>;

	HighFrequencyTrajectoryReader(double time_diff_threshold_s, double distance_threshold_m, std::size_t min_points)
	    : _time_diff_threshold_s(time_diff_threshold_s)
	    , _distance_threshold_m(distance_threshold_m)
	    , _min_points(min_points) {}

	void _init() {
		if constexpr (TrajectoriesAreGrouped) {
			// Process trajectories in a streaming fashion. segment represents a trajectory as a vector of probe point tuples.
			trajectory_reader =
			    std::make_unique<TrajectoryReader<TrajectoryTraits, ProbeInputIterator>>(probe_reader->begin(),
			                                                                             probe_reader->end());
		} else {
			sorted_probe_reader = std::make_unique<sorted_probe_reader_type>(probe_reader->begin(), probe_reader->end());
			trajectory_reader =
			    std::make_unique<TrajectoryReader<TrajectoryTraits, SortedProbeInputIterator>>(sorted_probe_reader->begin(),
			                                                                                   sorted_probe_reader->end());
		}
		hifreq_splitter = std::make_unique<hifreq_splitter_type>(trajectory_reader->begin(),
		                                                         trajectory_reader->end(),
		                                                         _time_diff_threshold_s,
		                                                         _distance_threshold_m,
		                                                         _min_points);
	}

	/**
	 * @brief Initialize the reader from string data
	 * @param csv_string CSV string data
	*/
	void init_from_string(const char* csv_string) {
		probe_reader = movetk::io::ProbeReaderFactory::create_from_string<ProbeTraits>(csv_string);
		_init();
	}

	/**
	 * @brief Initialize the reader using a file
	 * @param file_name The file
	*/
	void init(std::string file_name) {
		// Process trajectories from a (zipped) CSV file
		probe_reader = movetk::io::ProbeReaderFactory::create<ProbeTraits>(file_name.c_str());
		_init();
	}

	typename hifreq_splitter_type::iterator begin() { return std::begin(*hifreq_splitter); }

	typename hifreq_splitter_type::iterator end() { return std::end(*hifreq_splitter); }

	auto columns() { return probe_reader->columns(); }

private:
	double _time_diff_threshold_s;
	double _distance_threshold_m;
	std::size_t _min_points;  // min number of points required for a split to qualify as a trajectory

	std::unique_ptr<movetk::io::ProbeReader<ProbeTraits>> probe_reader;
	std::unique_ptr<sorted_probe_reader_type> sorted_probe_reader;  // only for not grouped trajectories
	std::unique_ptr<trajectory_reader_type> trajectory_reader;
	std::unique_ptr<hifreq_splitter_type> hifreq_splitter;
	typename hifreq_splitter_type::iterator hifreq_splitter_it;
};
}  // namespace movetk::io
#endif  // MOVETK_HIGHFREQUENCYTRAJECTORYREADER_H