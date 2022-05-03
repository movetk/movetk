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

#include <chrono>
#include <vector>

#ifdef _GLIBCXX_PARALLEL
#include <parallel/algorithm>
#endif

#include "HereProbeTraits.h"
#include "movetk/geo/geo.h"
#include "movetk/io/ProbeReader.h"
#include "movetk/io/SortByField.h"
#include "movetk/io/SplitByDifferenceThreshold.h"
#include "movetk/io/SplitByDistanceThreshold.h"
#include "movetk/io/SplitByField.h"
#include "movetk/io/Splitter.h"
#include "test_data.h"


class Timer {
public:
	using time_t = std::decay_t<decltype(std::chrono::high_resolution_clock::now())>;
	Timer(bool auto_start = false) {
		if (auto_start) {
			start();
		}
	}
	void start() {
		if (m_is_running) {
			throw std::runtime_error("Timer already running");
		}
		m_is_running = true;
		m_start = std::chrono::high_resolution_clock::now();
	}
	void stop() {
		m_is_running = false;
		m_end = std::chrono::high_resolution_clock::now();
	}
	friend std::ostream &operator<<(std::ostream &stream, const Timer &timer) {
		stream << timer.elapsed_ns() << " ns";
		return stream;
	}

	long long elapsed_ns() const { return (m_end - m_start).count(); }

private:
	time_t m_start;
	time_t m_end;
	bool m_is_running = false;
};

/**
 * Example: Create trajectories from raw probe points by
 *          - buffering probe points in memory
 *          - sorting them by PROBE_ID
 *          - splitting by PROBE_ID
 *          - sorting each trajectoy points by SAMPLE_DATE
 *          - writing trajectories to a CSV file.
 */
int main(int argc, char **argv) {
	std::ios_base::sync_with_stdio(false);
	std::cout << "Started";
#ifdef _GLIBCXX_PARALLEL
	std::cout << "Using parallel STL";
#endif

	// Specializations for the Commit2Data raw probe format
	using ProbeTraits = here::c2d::raw::ProbeTraits;

	// Create trajectory reader
	std::unique_ptr<movetk::io::ProbeReader<ProbeTraits>> probe_reader;
	if (argc < 2) {
		// Use built-in test data if a file is not specified
		probe_reader = movetk::io::ProbeReaderFactory::create_from_string<ProbeTraits>(testdata::c2d_raw_csv);
	} else {
		// Process trajectories from a (zipped) CSV file (e.g., probe_data_lametro.20180918.wayne.csv.gz)
		probe_reader = movetk::io::ProbeReaderFactory::create<ProbeTraits>(argv[1]);
	}

	// Store probe points in memory
	Timer read_timer(true);
	std::vector<ProbeTraits::ProbePoint> buffered_probe;
	std::size_t probe_count = 0;
	for (const auto &probe_point : *probe_reader) {
		buffered_probe.push_back(probe_point);
		++probe_count;
	}
	read_timer.stop();
	std::cout << "read probe [time taken: " << read_timer << "]\n";
	std::cout << "Buffered " << probe_count << " probe points.";

	// Sort all probe points by PROBE_ID
	constexpr int PROBE_ID = ProbeTraits::ProbeColumns::PROBE_ID;
	movetk::io::SortByField<PROBE_ID, typename ProbeTraits::ProbePoint> sort_by_probe_id_asc;
	Timer sort_timer(true);
#ifdef _GLIBCXX_PARALLEL
	__gnu_parallel::sort(buffered_probe.begin(), buffered_probe.end(), sort_by_probe_id_asc);
#else
	std::sort(buffered_probe.begin(), buffered_probe.end(), sort_by_probe_id_asc);
	// with <execution> header, since C++17, but not yet available on any compiler/platform.
	// std::sort(std::execution::par, buffered_probe.begin(), buffered_probe.end(), sort_by_probe_id_asc);
#endif
	sort_timer.stop();
	std::cout << "sort in " << sort_timer << '\n';

	//    // Write sorted probe
	//    for (const auto& point: buffered_probe) {
	//        print_tuple(ofcsv, point);
	//        ofcsv << '\n';
	//    }

	Timer rest_timer(true);

	// Split probe points into trajectories by PROBE_ID
	using SplitByProbeId = movetk::io::SplitByField<PROBE_ID, typename ProbeTraits::ProbePoint>;
	using ProbeInputIterator = decltype(buffered_probe.begin());
	movetk::io::Splitter<SplitByProbeId, ProbeInputIterator> splitter(buffered_probe.begin(), buffered_probe.end());

	// Create an output csv file
	std::ofstream ofcsv("output_trajectories.csv");

	// Write the header
	movetk::io::print_tuple(ofcsv, probe_reader->columns());
	ofcsv << ",RAW_TRAJID\n";
	//    ofcsv << '\n';

	// Write time-sorted trajectories
	constexpr int SAMPLE_DATE = ProbeTraits::ProbeColumns::SAMPLE_DATE;
	using ProbePoint = typename ProbeTraits::ProbePoint;
	std::size_t trajectory_count = 0;
	for (auto trajectory_points : splitter) {
		// Sort trajectory by SAMPLE_DATE
		movetk::io::SortByField<SAMPLE_DATE, ProbePoint> sort_by_date_asc;
		std::sort(trajectory_points.begin(), trajectory_points.end(), sort_by_date_asc);

		//        for(const auto& point: trajectory_points) {
		//            print_tuple(ofcsv, point);
		//            ofcsv << '\n';
		//        }

		using SplitByTimeDiff = movetk::io::SplitByDifferenceThreshold<SAMPLE_DATE, ProbePoint>;
		SplitByTimeDiff split_by_time_diff(30.0);
		using SplitByDistance = movetk::io::
		    SplitByDistanceThreshold<ProbeTraits::ProbeColumns::LAT, ProbeTraits::ProbeColumns::LON, ProbePoint>;
		std::function<double(float, float, float, float)> distancefn = movetk::geo::distance_exact;
		SplitByDistance split_by_dist(900.0, distancefn);
		auto split_by_time_diff_or_distance = [&](const ProbeTraits::ProbePoint &p) {
			bool t = split_by_time_diff(p);
			bool d = split_by_dist(p);
			return (t || d);
		};

		using ProbeInputIterator = decltype(trajectory_points.begin());
		movetk::io::Splitter<decltype(split_by_time_diff_or_distance), ProbeInputIterator> hf_splitter(
		    trajectory_points.begin(),
		    trajectory_points.end(),
		    split_by_time_diff_or_distance);
		//        Splitter<decltype(split_by_dist), ProbeInputIterator> hf_splitter(trajectory_points.begin(),
		//        trajectory_points.end(), split_by_dist); Splitter<SplitByTimeDiff, ProbeInputIterator>
		//        hf_splitter(trajectory_points.begin(), trajectory_points.end(), split_by_time_diff);

		for (auto hf_trajectory_points : hf_splitter) {
			for (const auto &point : hf_trajectory_points) {
				movetk::io::print_tuple(ofcsv, point);
				ofcsv << ',' << trajectory_count << '\n';
			}
			++trajectory_count;
		}

		//        ++trajectory_count;
	}
	rest_timer.stop();
	std::cout << "rest in " << rest_timer << '\n';
	std::cout << "Wrote " << trajectory_count << " trajectories.";

	return 0;
}
