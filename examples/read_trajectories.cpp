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

#include "movetk/logging.h"
#include "movetk/test_data.h"
#include "movetk/HereTrajectoryTraits.h"
#include "movetk/io/ProbeReader.h"
#include "movetk/SortedProbeReader.h"
#include "movetk/TrajectoryReader.h"
#include "movetk/HighFrequencyTrajectorySplitter.h"
#include "movetk/geo/geo.h"

/**
 * Example: Create trajectories from raw probe points by
 *          - buffering probe points in memory
 *          - sorting them by PROBE_ID
 *          - splitting by PROBE_ID
 *          - sorting each trajectoy points by SAMPLE_DATE
 *          - retaining only high frequency pieces (with time and distance threshold)
 *          - writing trajectories to a CSV file.
 */
int main(int argc, char** argv)
{
    std::ios_base::sync_with_stdio(false);
    init_logging(logging::trivial::trace);
    BOOST_LOG_TRIVIAL(info) << "Started";
#ifdef _GLIBCXX_PARALLEL
    BOOST_LOG_TRIVIAL(info) << "Using parallel STL";
#endif

    // Specializations for the Commit2Data raw probe format
    using TrajectoryTraits = here::c2d::raw::TabularTrajectoryTraits;
    using ProbeTraits = typename TrajectoryTraits::ProbeTraits;

    // Create trajectory reader
    std::unique_ptr<ProbeReader<ProbeTraits>> probe_reader;
    if (argc<2) {
        // Use built-in test data if a file is not specified
        probe_reader = ProbeReaderFactory::create_from_string<ProbeTraits>(testdata::c2d_raw_csv);
    }
    else {
        // Process trajectories from a (zipped) CSV file (e.g., probe_data_lametro.20180918.wayne.csv.gz)
        probe_reader = ProbeReaderFactory::create<ProbeTraits>(argv[1]);
    }
    using ProbeInputIterator = decltype(probe_reader->begin());

    // If the probe points are
    // a) already grouped into sessions (trajectories)
    // auto trajectory_reader = TrajectoryReader<TrajectoryTraits, ProbeInputIterator>(probe_reader->begin(), probe_reader->end());
    // b) not grouped
	constexpr int PROBE_ID = ProbeTraits::ProbeColumns::PROBE_ID;
    SortedProbeReader<ProbeInputIterator, PROBE_ID> sorted_probe_reader(probe_reader->begin(), probe_reader->end());
    using SortedProbeInputIterator = decltype(sorted_probe_reader.begin());
    auto trajectory_reader = TrajectoryReader<TrajectoryTraits, SortedProbeInputIterator>(sorted_probe_reader.begin(),
            sorted_probe_reader.end());

    using TrajectoryInputIterator = decltype(trajectory_reader.begin());
    auto hifreq_splitter = HighFrequencyTrajectorySplitter<TrajectoryInputIterator,
                                                           ProbeTraits::ProbeColumns::SAMPLE_DATE,
                                                           ProbeTraits::ProbeColumns::LAT,
                                                           ProbeTraits::ProbeColumns::LON>(
            trajectory_reader.begin(), trajectory_reader.end(), 30.0, 900.0, 2);

    auto t_start = std::chrono::high_resolution_clock::now();

    // Create an output csv file
    std::ofstream ofcsv("output_trajectories.csv");

    // Write the header
    print_tuple(ofcsv, probe_reader->columns());
    ofcsv << ",RAW_TRAJID,TS\n";

    // Write trajectories
    std::size_t trajectory_count = 0;
    for (auto trajectory: hifreq_splitter) {
        // std::cout << "New trajectory:\n";

        // Create the new trajectory id column
        std::vector<std::size_t> trajectory_id_col;
        trajectory_id_col.assign(trajectory.size(), trajectory_count);
        // Create the timestamp column
        std::vector<ProbeTraits::ProbeParseDate> dates = trajectory.get<ProbeTraits::ProbeColumns::SAMPLE_DATE>();
        std::vector<std::time_t> ts_col;
        for (auto& d: dates) {
            ts_col.push_back(d.ts());
        }

        // Add new fields to the trajectory
        auto augmented_trajectory = concat_field(trajectory, trajectory_id_col, ts_col);

        ofcsv << augmented_trajectory;

        ++trajectory_count;
    }
    auto t_end = std::chrono::high_resolution_clock::now();
    display("rest", t_start, t_end);
    BOOST_LOG_TRIVIAL(info) << "Wrote " << trajectory_count << " trajectories.";

    return 0;
}
