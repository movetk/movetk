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

#include <vector>

#include "test_data.h"
#include "HereTrajectoryTraits.h"
#include "movetk/io/ProbeReader.h"
#include "movetk/io/TrajectoryReader.h"

/**
 * Example: Process a stream of probe points by
 *          - creating trajectories from input by
 *            - splitting by PROBE_ID
 *            - sorting by SAMPLE_DATE
 *          - adding a unique trajectory identifier to each point in the trajectories
 *          - writing processed trajectories to a CSV file.
 */
int main(int argc, char **argv)
{
    std::ios_base::sync_with_stdio(false);

    // Specializations for the Commit2Data raw probe format
    using TrajectoryTraits = here::c2d::raw::ColumnarTrajectoryTraits;
    using ProbeTraits = typename TrajectoryTraits::ProbeTraits;

    // Create trajectory reader
    std::unique_ptr<movetk::io::ProbeReader<ProbeTraits>> probe_reader;
    if (argc < 2)
    {
        // Use built-in test data if a file is not specified
        probe_reader = movetk::io::ProbeReaderFactory::create_from_string<ProbeTraits>(testdata::c2d_raw_csv);
    }
    else
    {
        // Example: Process trajectories from a (zipped) CSV file (e.g., probe_data_lametro.20180918.wayne.csv.gz)
        probe_reader = movetk::io::ProbeReaderFactory::create<ProbeTraits>(argv[1]);
    }
    using ProbeInputIterator = decltype(probe_reader->begin());
    auto trajectory_reader = movetk::io::TrajectoryReader<TrajectoryTraits, ProbeInputIterator>(probe_reader->begin(), probe_reader->end());

    // Create an output csv file
    std::ofstream ofcsv("output_trajectories.csv");

    // Write the header
    movetk::io::print_tuple(ofcsv, probe_reader->columns());
    ofcsv << ",TRAJ_ID" << std::endl;

    // Process trajectories in a streaming fashion
    std::size_t count = 0;
    for (auto trajectory : trajectory_reader)
    {
        constexpr int PROBE_ID = ProbeTraits::ProbeColumns::PROBE_ID;
        std::cout << "New trajectory: " << trajectory.get<PROBE_ID>()[0];

        // Create the new column
        std::vector<std::size_t> trajectory_id_col;
        trajectory_id_col.assign(trajectory.size(), count);

        // Add the field column to the trajectory
        auto new_trajectory = concat_field(trajectory, trajectory_id_col);

        // Declare an alias for the new field idx
        constexpr static std::size_t TRAJ_ID = new_trajectory.num_fields() - 1;

        // Trajectories are printable as CSV
        ofcsv << new_trajectory;

        // Accessing the new field
        for (const auto &trajectory_id : new_trajectory.get<TRAJ_ID>())
        {
            std::cout << trajectory_id;
        }

        count++;
    }
    std::cout << "Processed " << count << " trajectories" << std::endl;
    return 0;
}
