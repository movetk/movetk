/*
 * Copyright (C) 2018-2020
 * HERE Europe B.V.
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
// Created by Custers, Bram on 2020-02-08.
//


#include <vector>

#include "ExampleSetup.h"
#include "HereTrajectoryTraits.h"
#include "movetk/Statistics.h"
#include "movetk/geo/geo.h"
#include "movetk/io/ProbeReader.h"
#include "movetk/io/TrajectoryReader.h"
#include "movetk/utils/GeometryBackendTraits.h"
#include "test_data.h"

struct Example {
	static constexpr const char* NAME = "compute_statistics";
	static constexpr const char* DESCRIPTION = "Computes statistics on a trajectory";

	template <typename Kernel>
	void run(cxxopts::ParseResult& arguments) {
		std::ios_base::sync_with_stdio(false);
		// Run for columnar
		std::cout << "------------ Columnar";
		run_with_trajectory_traits<Kernel, here::c2d::raw::ColumnarTrajectoryTraits>(arguments);
		std::cout << "------------ Tabular";
		run_with_trajectory_traits<Kernel, here::c2d::raw::TabularTrajectoryTraits>(arguments);
	}

	void add_options(cxxopts::OptionAdder& adder) { adder("f,file", "Input file", cxxopts::value<std::string>()); }

	template <typename Kernel, typename TrajectoryTraits>
	void run_with_trajectory_traits(cxxopts::ParseResult& arguments) {
		using ProbeTraits = typename TrajectoryTraits::ProbeTraits;
		using MovetkPoint = typename Kernel::MovetkPoint;
		using NT = typename Kernel::NT;

		// Create trajectory reader
		std::unique_ptr<movetk::io::ProbeReader<ProbeTraits>> probe_reader;
		// No file specified
		if (arguments.count("file") == 0) {
			// Use built-in test data if a file is not specified
			probe_reader = movetk::io::ProbeReaderFactory::create_from_string<ProbeTraits>(testdata::c2d_format_geolife_csv);
		} else {
			// Example: Process trajectories from a (zipped) CSV file (e.g. some_probe_data.csv.gz)
			probe_reader = movetk::io::ProbeReaderFactory::create<ProbeTraits>(arguments["file"].as<std::string>());
		}
		using ProbeInputIterator = decltype(probe_reader->begin());
		auto trajectory_reader =
		    movetk::io::TrajectoryReader<TrajectoryTraits, ProbeInputIterator>(probe_reader->begin(), probe_reader->end());

		/**
		 * The points in the trajectory are assumed to be lat-lon coordinates. So, use the appropriate distance between the
		 * coordinates.
		 */
		struct Distance {
			NT operator()(MovetkPoint p0, MovetkPoint p1) {
				std::vector<NT> ll0;
				std::copy(p0.begin(), p0.end(), std::back_inserter(ll0));
				std::vector<NT> ll1;
				std::copy(p1.begin(), p1.end(), std::back_inserter(ll1));
				return movetk::geo::distance_exact(ll0[1], ll0[0], ll1[1], ll1[0]);
			}
		};

		// Process trajectories in a streaming fashion
		std::size_t count = 0;
		// Shorthands for fields
		constexpr int LON_Idx = ProbeTraits::ProbeColumns::LON;         // Longitude
		constexpr int LAT_Idx = ProbeTraits::ProbeColumns::LAT;         // Latitude
		constexpr int TS_Idx = ProbeTraits::ProbeColumns::SAMPLE_DATE;  // Timestamp
		constexpr int PROBE_ID = ProbeTraits::ProbeColumns::PROBE_ID;   // ID

		for (auto trajectory : trajectory_reader) {
			std::cout << "=====================================" << std::endl;
			std::cout << "======== New trajectory: " << trajectory.template get<PROBE_ID>()[0];
			std::cout << " Size:" << trajectory.size() << std::endl;
			using Trajectory_t = decltype(trajectory);

			// Get iterators for data
			auto lons = trajectory.template get<LON_Idx>();
			using CoordIt = decltype(lons.begin());
			auto lats = trajectory.template get<LAT_Idx>();
			auto ts = trajectory.template get<TS_Idx>();
			auto pointIterators = movetk::utils::point_iterators_from_coordinates<Kernel>(
			    std::array<std::pair<CoordIt, CoordIt>, 2>{std::make_pair(lons.begin(), lons.end()),
			                                               std::make_pair(lats.begin(), lats.end())});

			movetk::statistics::TrajectoryLength<Kernel, Distance> lenCalc;
			std::cout << "Trajectory length:" << lenCalc(lons.begin(), lons.end(), lats.begin(), lats.end()) << std::endl;
			movetk::statistics::TrajectoryDuration duration;
			std::cout << "Trajectory duration:" << duration(ts.begin(), ts.end()) << std::endl;

			// Show speed statistics:
			using SpeedStat = movetk::statistics::TrajectorySpeedStatistic<Kernel, Distance>;
			SpeedStat speedStat;
			auto compute_stat = [&pointIterators, &ts, &speedStat](auto stat_type) {
				return speedStat(pointIterators.first, pointIterators.second, ts.begin(), ts.end(), stat_type);
			};
			using Stat = typename SpeedStat::Statistic;
			std::cout << "Trajectory average speed:" << compute_stat(Stat::Mean) << std::endl;
			std::cout << "Trajectory median speed:" << compute_stat(Stat::Median) << std::endl;
			std::cout << "Trajectory min speed:" << compute_stat(Stat::Min) << std::endl;
			std::cout << "Trajectory max speed:" << compute_stat(Stat::Max) << std::endl;
			std::cout << "Trajectory variance of speed:" << compute_stat(Stat::Variance) << std::endl;

			// Show time mode
			movetk::statistics::ComputeDominantDifference timeMode;
			std::cout << "Most common time interval: " << timeMode(ts.begin(), ts.end(), 0) << std::endl;

			std::cout << "=====================================" << std::endl;
			count++;
		}
		std::cout << "Processed " << count << " trajectories" << std::endl;
	}
};

int main(int argc, char** argv) {
	return movetk::examples::ExampleRunner().run_example<Example>(argc, argv);
}
