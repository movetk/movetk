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

/**
 * An example usage of the movetk library:
 *   Create trajectories from raw probe points by
 *     - buffering probe points in memory
 *     - sorting them by PROBE_ID
 *     - splitting by PROBE_ID
 *     - sorting each trajectoy points by SAMPLE_DATE
 *   Use monotonic segmentation with  difference and range criterion.
 *   Write segmented trajectories to a CSV file.
 *
 *   Authors: Aniket Mitra, Onur Derin
 */

#include <chrono>
#include <vector>

#ifdef _GLIBCXX_PARALLEL
#include <parallel/algorithm>
#endif

#include "HereTrajectoryTraits.h"
#include "Timer.h"
#include "movetk/ds/StartStopMatrix.h"
#include "movetk/geom/trajectory_to_interface.h"
#include "movetk/io/ProbeReader.h"
#include "movetk/io/SortedProbeReader.h"
#include "movetk/io/TrajectoryReader.h"
#include "movetk/segmentation/SegmentationTraits.h"
#include "movetk/utils/GeometryBackendTraits.h"
#include "test_data.h"

int main(int argc, char **argv) {
	std::ios_base::sync_with_stdio(false);
	std::cout.setf(std::ios::fixed);
	std::cout << "Started";
#ifdef _GLIBCXX_PARALLEL
	std::cout << "Using parallel STL";
#endif
#if CGAL_BACKEND_ENABLED
	std::cout << "Using CGAL Backend for Geometry";
#else
	std::cout << "Using Boost Backend for Geometry";
#endif

	// Specializations for the Commit2Data raw probe format
	using TrajectoryTraits = here::c2d::raw::TabularTrajectoryTraits;
	using ProbeTraits = typename TrajectoryTraits::ProbeTraits;

	// Create trajectory reader
	std::unique_ptr<movetk::io::ProbeReader<ProbeTraits>> probe_reader;
	if (argc < 2) {
		// Use built-in test data if a file is not specified
		probe_reader = movetk::io::ProbeReaderFactory::create_from_string<ProbeTraits>(testdata::c2d_raw_csv);
	} else {
		// Process trajectories from a (zipped) CSV file (e.g., probe_data_lametro.20180918.wayne.csv.gz)
		probe_reader = movetk::io::ProbeReaderFactory::create<ProbeTraits>(argv[1]);
	}
	using ProbeInputIterator = decltype(probe_reader->begin());

	constexpr int PROBE_ID = ProbeTraits::ProbeColumns::PROBE_ID;
	movetk::io::SortedProbeReader<ProbeInputIterator, PROBE_ID> sorted_probe_reader(probe_reader->begin(),
	                                                                                probe_reader->end());
	using SortedProbeInputIterator = decltype(sorted_probe_reader.begin());
	auto trajectory_reader =
	    movetk::io::TrajectoryReader<TrajectoryTraits, SortedProbeInputIterator>(sorted_probe_reader.begin(),
	                                                                             sorted_probe_reader.end());

	Timer timer(true);

	// Create an output csv file
	std::ofstream ofcsv("output_trajectories_speed_and_heading.csv");

	// Write the header
	movetk::io::print_tuple(ofcsv, probe_reader->columns());
	ofcsv << ",RAW_TRAJID,VELOCITY_SEG_ID\n";

	// Write time-sorted trajectories and segment them using Conjunction of Range and Diff Criteria
	using SegmentationTraits = movetk::segmentation::
	    SegmentationTraits<long double, typename GeometryKernel::MovetkGeometryKernel, GeometryKernel::dimensions>;
	using NT = GeometryKernel::MovetkGeometryKernel::NT;
	// typedef vector<SegmentationTraits::Point > PolyLine;
	typedef std::vector<NT> Headings, Speeds;

	typedef std::vector<Speeds::iterator> SegmentStartReferences;
	SegmentationTraits::SpeedSegmentation segment_by_speed(10);
	SegmentationTraits::HeadingSegmentation segment_by_heading(90);
	// TSL::MakePoint<SegmentationTraits::Geometry_Kernel > make_point;
	// std::array<NT, 2> pt;
	using SSD = movetk::ds::StartStopDiagram<movetk::ds::SsdType::compressed,
	                                         typename GeometryKernel::MovetkGeometryKernel,
	                                         std::vector<size_t>>;

	std::size_t trajectory_count = 0;
	for (auto trajectory : trajectory_reader) {
		std::cout << "New trajectory: \n";

		// auto lons = trajectory.get<ProbeTraits::ProbeColumns::LON>();
		// auto lats = trajectory.get<ProbeTraits::ProbeColumns::LAT>();

		auto headings = trajectory.get<ProbeTraits::ProbeColumns::HEADING>();

		auto speeds = trajectory.get<ProbeTraits::ProbeColumns::SPEED>();

		// std::vector<Point> polyline = movetk::to_geocentered_polyline(make_point, lats, lons);
		// Alternatively, use projection to local coordinates (requires also changing dimensions from 3 to 2):
		// PolyLine polyline = movetk::to_projected_polyline(make_point, lats, lons);

		Headings headings_;

		for (auto sit = std::cbegin(headings); sit != std::cend(headings); sit++) {
			headings_.push_back(*sit);
		}

		SegmentStartReferences segIdx;
		segment_by_heading(std::begin(headings_), std::end(headings_), std::back_inserter(segIdx));

		movetk::utils::SegmentIdGenerator make_segment_heading(std::begin(segIdx), std::end(segIdx));
		SSD heading_ssd;

		heading_ssd(std::begin(headings_), std::end(headings_), make_segment_heading);

		std::cout << "heading ssd:" << std::endl;
		std::cout << movetk::utils::join(heading_ssd.cbegin(), heading_ssd.cend());
		std::cout << std::endl;

		Speeds speeds_;
		segIdx.clear();

		for (auto sit = std::cbegin(speeds); sit != std::cend(speeds); sit++) {
			std::cout << *sit << std::endl;
			speeds_.push_back(*sit);
		}

		segment_by_speed(std::begin(speeds_), std::end(speeds_), std::back_inserter(segIdx));
		movetk::utils::SegmentIdGenerator make_segment_speed(std::begin(segIdx), std::end(segIdx));

		SSD speed_ssd;
		speed_ssd(std::begin(speeds_), std::end(speeds_), make_segment_speed);

		std::cout << "speed ssd: " << std::endl;
		std::cout << movetk::utils::join(speed_ssd.cbegin(), speed_ssd.cend());
		std::cout << std::endl;

		SSD conjunction_result = speed_ssd * heading_ssd;

		std::cout << "Conjunction ssd: " << std::endl;
		std::cout << movetk::utils::join(conjunction_result.cbegin(), conjunction_result.cend());
		std::cout << std::endl;

		std::vector<std::size_t> segment_id_col;

		SSD::const_iterator cit = conjunction_result.cbegin();

		SSD::const_iterator pit = cit;

		size_t id = 0;

		while (cit != conjunction_result.cend()) {
			if (!movetk::utils::is_sequence(pit, (cit + 1))) {
				id++;
				pit = cit;
			}
			segment_id_col.push_back(id);
			cit++;
		}

		// Create the new trajectory id column
		std::vector<std::size_t> trajectory_id_col;
		trajectory_id_col.assign(trajectory.size(), trajectory_count);
		// Add new fields to the trajectory
		auto segmented_trajectory = concat_field(trajectory, trajectory_id_col, segment_id_col);

		// Declare an alias for the new field idx if used later
		// constexpr static std::size_t TRAJ_ID = segmented_trajectory.num_fields() - 2;
		// constexpr static std::size_t MEB_SEG_ID = segmented_trajectory.num_fields() - 1;

		ofcsv << segmented_trajectory;
		++trajectory_count;
	}

	timer.stop();
	std::cout << " Time spent: " << timer << '\n';
	std::cout << "Wrote " << trajectory_count << " trajectories.";

	return 0;
}
