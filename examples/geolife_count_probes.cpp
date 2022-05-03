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

// Created by Wouter Jongeling (wouter.jongeling@gmail.com)

#include <vector>

#include "GeolifeTrajectoryTraits.h"
#include "movetk/io/ProbeReader.h"
#include "movetk/io/TrajectoryReader.h"
#include "test_data.h"

int main(int argc, char **argv) {
	std::ios_base::sync_with_stdio(false);

	// Specializations for the Geolife raw probe format
	using TrajectoryTraits = geolife::c2d::raw::TabularTrajectoryTraits;
	using ProbeTraits = typename TrajectoryTraits::ProbeTraits;

	// Create trajectory reader
	std::unique_ptr<movetk::io::ProbeReader<ProbeTraits>> probe_reader;
	if (argc < 2) {
		// Use built-in test data if a file is not specified
		probe_reader = movetk::io::ProbeReaderFactory::create_from_string<ProbeTraits>(testdata::geolife_raw_csv);
	} else {
		// Example: Process trajectories from a (zipped) CSV file (e.g., probe_data_lametro.20180918.wayne.csv.gz)
		probe_reader = movetk::io::ProbeReaderFactory::create<ProbeTraits>(argv[1]);
	}
	using ProbeInputIterator = decltype(probe_reader->begin());
	auto trajectory_reader =
	    movetk::io::TrajectoryReader<TrajectoryTraits, ProbeInputIterator>(probe_reader->begin(), probe_reader->end());

	// Create an output csv file
	std::ofstream ofcsv("output_trajectories.csv");

	// Write the header
	movetk::io::print_tuple(ofcsv, probe_reader->columns());
	ofcsv << ",TRAJ_ID" << std::endl;

	// Process trajectories in a streaming fashion
	std::size_t count = 0;
	for (auto trajectory : trajectory_reader) {
		constexpr int TRAJID = ProbeTraits::ProbeColumns::TRAJID;
		std::string TRAJ_ID = trajectory.get<TRAJID>()[0];

		int probe_count = 0;
		for (auto probe : trajectory) {
			probe_count++;
		}

		std::string str = "Trajectory " + TRAJ_ID + " has " + std::to_string(probe_count) + " probes";

		// Log to console and csv
		std::cout << str;
		ofcsv << str;

		count++;
	}
	std::cout << "Processed " << count << " trajectories" << std::endl;
	return 0;
}