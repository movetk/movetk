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

#include "ExampleSetup.h"
#include "GeolifeTrajectoryTraits.h"
#include "movetk/geom/trajectory_to_interface.h"
#include "movetk/io/ProbeReader.h"
#include "movetk/io/TrajectoryReader.h"
#include "movetk/utils/GeometryBackendTraits.h"
#include "test_data.h"

struct Example {
	static constexpr const char* NAME = "geolife_distance_heading";
	static constexpr const char* DESCRIPTION = "Computes statistics on a trajectory";
	void add_options(cxxopts::OptionAdder& adder) { adder("f,file", "Input file", cxxopts::value<std::string>()); }

	template <typename Kernel>
	void run(cxxopts::ParseResult& arguments) {
		std::ios_base::sync_with_stdio(false);

		// Specializations for the Geolife raw probe format
		using TrajectoryTraits = geolife::c2d::raw::TabularTrajectoryTraits;
		using ProbeTraits = typename TrajectoryTraits::ProbeTraits;

		// Create trajectory reader
		std::unique_ptr<movetk::io::ProbeReader<ProbeTraits>> probe_reader;
		if (arguments.count("file") == 0) {
			// Use built-in test data if a file is not specified
			probe_reader = movetk::io::ProbeReaderFactory::create_from_string<ProbeTraits>(testdata::geolife_raw_csv);
		} else {
			// Example: Process trajectories from a (zipped) CSV file (e.g., probe_data_lametro.20180918.wayne.csv.gz)
			probe_reader = movetk::io::ProbeReaderFactory::create<ProbeTraits>(arguments["file"].as<std::string>());
		}
		using ProbeInputIterator = decltype(probe_reader->begin());
		auto trajectory_reader =
		    movetk::io::TrajectoryReader<TrajectoryTraits, ProbeInputIterator>(probe_reader->begin(), probe_reader->end());

		for (auto trajectory : trajectory_reader) {
			std::cout << "=====================================" << std::endl;
			std::cout << "======== New trajectory: " << trajectory.template get<ProbeTraits::ProbeColumns::TRAJID>()[0];
			std::cout << " Size:" << trajectory.size() << std::endl;
			double prev_lat, prev_lon;
			bool first = true;

			constexpr int LAT = ProbeTraits::ProbeColumns::LAT;
			constexpr int LON = ProbeTraits::ProbeColumns::LON;
			for (auto probe : trajectory) {
				if (first) {
					prev_lat = get<LAT>(probe);
					prev_lon = get<LON>(probe);
					first = false;
				} else {
					double curr_lat = get<LAT>(probe);
					double curr_lon = get<LON>(probe);
					double d = movetk::geo::distance_exact(prev_lat, prev_lon, curr_lat, curr_lon);
					double b = movetk::geo::bearing_exact(prev_lat, prev_lon, curr_lat, curr_lon);
					std::cout << d << " " << b;
					prev_lat = curr_lat;
					prev_lon = curr_lon;
				}
			}
			std::cout << "=====================================" << std::endl;
		}
	}
};

int main(int argc, char** argv) {
	return movetk::examples::ExampleRunner().run_example<Example>(argc, argv);
}