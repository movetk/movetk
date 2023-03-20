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

#include "ExampleSetup.h"
#include "HereTrajectoryTraits.h"
#include "Timer.h"
#include "movetk/ds/StartStopMatrix.h"
#include "movetk/geo/geo.h"
#include "movetk/geom/trajectory_to_interface.h"
#include "movetk/io/ProbeReader.h"
#include "movetk/io/SortedProbeReader.h"
#include "movetk/io/TrajectoryReader.h"
#include "movetk/segmentation/SegmentationTraits.h"
#include "movetk/utils/GeometryBackendTraits.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "test_data.h"

struct Example {
	static constexpr const char *NAME = "rawprobe2trajectories";
	static constexpr const char *DESCRIPTION = R"(Create trajectories from raw probe points by
 - buffering probe points in memory
 - sorting them by PROBE_ID
 - splitting by PROBE_ID
 - sorting each trajectoy points by SAMPLE_DATE
 - writing trajectories to a CSV file.
)";
	enum class SegmentationType { Heading, MinimumEnclosingBall, Speed, Velocity };
	SegmentationType m_type = SegmentationType::Heading;

	void add_options(cxxopts::OptionAdder &adder, std::vector<std::string> &positionals) {
		adder("trajectory", "Trajectories file", cxxopts::value<std::string>());
		adder("m,mode",
		      "Segmentation mode. One of Heading, MinimumEnclosingBall, Speed, Velocity",
		      cxxopts::value<std::string>()->default_value("Heading"));
		positionals.push_back("trajectory");
	}
	template <typename ProbeTraits>
	std::unique_ptr<movetk::io::ProbeReader<ProbeTraits>> get_probe_reader(cxxopts::ParseResult &arguments) {
		if (arguments.count("trajectory") == 0) {
			// Use built-in test data if a file is not specified
			return movetk::io::ProbeReaderFactory::create_from_string<ProbeTraits>(testdata::c2d_raw_csv);
		} else {
			// Process trajectories from a (zipped) CSV file (e.g., probe_data_lametro.20180918.wayne.csv.gz)
			return movetk::io::ProbeReaderFactory::create<ProbeTraits>(arguments["trajectory"].as<std::string>());
		}
	}

	// Specializations for the Commit2Data raw probe format
	using TrajectoryTraits = here::c2d::raw::TabularTrajectoryTraits;
	using ProbeTraits = typename TrajectoryTraits::ProbeTraits;

	template <typename Kernel, typename SegmentationTraits, typename OUTPUT_ITERATOR>
	void apply_segmentation(const TrajectoryTraits::trajectory_type &trajectory, OUTPUT_ITERATOR id_iterator) {
		using NT = typename Kernel::NT;
		using Speeds = std::vector<NT>;
		using Headings = std::vector<NT>;
		switch (m_type) {
			case SegmentationType::Heading: {
				typename SegmentationTraits::HeadingSegmentation segment_by_heading(90);
				using SegmentStartReferences = std::vector<Headings::iterator>;
				auto headings = trajectory.get<ProbeTraits::ProbeColumns::HEADING>();

				Headings headings_(headings.cbegin(), headings.cend());

				SegmentStartReferences segIdx;
				segment_by_heading(std::begin(headings_), std::end(headings_), std::back_inserter(segIdx));

				movetk::utils::SegmentIdGenerator make_segment(std::begin(segIdx), std::end(segIdx));
				std::vector<std::size_t> segment_id_col;
				for (auto plit = std::begin(headings_); plit != std::end(headings_); ++plit) {
					*id_iterator = make_segment.getSegmentID(plit);
				}
				break;
			}
			case SegmentationType::MinimumEnclosingBall: {
				typename SegmentationTraits::LocationSegmentation segment_by_meb(10);
				using PolyLine = std::vector<typename SegmentationTraits::Point>;
				PolyLine polyline;
				movetk::geom::MakePoint<Kernel> make_point;
				movetk::geom::to_projected_polyline(make_point,
				                                    trajectory.begin<ProbeTraits::ProbeColumns::LAT>(),
				                                    trajectory.end<ProbeTraits::ProbeColumns::LAT>(),
				                                    trajectory.begin<ProbeTraits::ProbeColumns::LON>(),
				                                    std::back_inserter(polyline));
				using SegmentIdx = std::vector<typename PolyLine::const_iterator>;
				SegmentIdx segIdx;
				segment_by_meb(std::cbegin(polyline), std::cend(polyline), std::back_inserter(segIdx));
				movetk::utils::SegmentIdGenerator make_segment(std::begin(segIdx), std::end(segIdx));
				for (auto plit = std::begin(polyline); plit != std::end(polyline); ++plit) {
					*id_iterator = make_segment.getSegmentID(plit);
				}
				break;
			}
			case SegmentationType::Speed: {
				typename SegmentationTraits::SpeedSegmentation segment_by_speed(10);

				auto speeds = trajectory.template get<ProbeTraits::ProbeColumns::SPEED>();
				Speeds speeds_;
				std::copy(std::cbegin(speeds), std::cend(speeds), std::back_inserter(speeds_));
				using SegmentStartReferences = std::vector<typename Speeds::iterator>;
				SegmentStartReferences segIdx;
				segment_by_speed(std::begin(speeds_), std::end(speeds_), std::back_inserter(segIdx));

				movetk::utils::SegmentIdGenerator make_segment(std::begin(segIdx), std::end(segIdx));

				for (auto plit = std::begin(speeds_); plit != std::end(speeds_); ++plit) {
					*id_iterator = make_segment.getSegmentID(plit);
				}
				break;
			}
			case SegmentationType::Velocity: {
				using SSD = movetk::ds::StartStopDiagram<movetk::ds::SsdType::compressed, Kernel, std::vector<size_t>>;

				typename SegmentationTraits::SpeedSegmentation segment_by_speed(10);
				typename SegmentationTraits::HeadingSegmentation segment_by_heading(90);

				auto headings = trajectory.get<ProbeTraits::ProbeColumns::HEADING>();
				auto speeds = trajectory.get<ProbeTraits::ProbeColumns::SPEED>();
				Headings headings_(std::cbegin(headings), std::cend(headings));

				using SegmentStartReferences = std::vector<Speeds::iterator>;
				SegmentStartReferences segIdx;
				segment_by_heading(std::begin(headings_), std::end(headings_), std::back_inserter(segIdx));

				movetk::utils::SegmentIdGenerator make_segment_heading(std::begin(segIdx), std::end(segIdx));
				SSD heading_ssd;
				heading_ssd(std::begin(headings_), std::end(headings_), make_segment_heading);

				Speeds speeds_(std::cbegin(speeds), std::cend(speeds));
				segIdx.clear();

				segment_by_speed(std::begin(speeds_), std::end(speeds_), std::back_inserter(segIdx));
				movetk::utils::SegmentIdGenerator make_segment_speed(std::begin(segIdx), std::end(segIdx));

				SSD speed_ssd;
				speed_ssd(std::begin(speeds_), std::end(speeds_), make_segment_speed);

				std::cout << "Heading ssd: " << std::endl;
				std::cout << movetk::utils::join(heading_ssd.cbegin(), heading_ssd.cend());
				std::cout << std::endl;
				std::cout << "Speed ssd: " << std::endl;
				std::cout << movetk::utils::join(speed_ssd.cbegin(), speed_ssd.cend());
				std::cout << std::endl;

				SSD conjunction_result = speed_ssd * heading_ssd;

				std::cout << "Conjunction ssd: " << std::endl;
				std::cout << movetk::utils::join(conjunction_result.cbegin(), conjunction_result.cend());
				std::cout << std::endl;

				auto cit = conjunction_result.cbegin();
				auto pit = cit;
				size_t id = 0;
				for (; cit != conjunction_result.cend(); ++cit) {
					if (!movetk::utils::is_sequence(pit, (cit + 1))) {
						id++;
						pit = cit;
					}
					*id_iterator = id;
				}
				break;
			}
		}
	}

	template <typename Kernel>
	void run(cxxopts::ParseResult &arguments) {
		std::ios_base::sync_with_stdio(false);
		std::cout.setf(std::ios::fixed);

		auto mode = arguments["mode"].as<std::string>();
		if (mode == "Heading") {
			m_type = SegmentationType::Heading;
		} else if (mode == "MinimumEnclosingBall") {
			m_type = SegmentationType::MinimumEnclosingBall;
		} else if (mode == "Speed") {
			m_type = SegmentationType::Speed;
		} else if (mode == "Velocity") {
			m_type = SegmentationType::Velocity;
		}
		std::cout << "Applying " << mode << " segmentation\n";


		// Create trajectory reader
		std::unique_ptr<movetk::io::ProbeReader<ProbeTraits>> probe_reader = get_probe_reader<ProbeTraits>(arguments);
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
		std::ofstream ofcsv("output_trajectories_heading.csv");

		// Write the header
		movetk::io::print_tuple(ofcsv, probe_reader->columns());
		ofcsv << ",RAW_TRAJID,HEADING_SEG_ID\n";

		// Write time-sorted trajectories and segment them using  range criteria
		using SegmentationTraits = movetk::segmentation::SegmentationTraits<long double, Kernel, Kernel::dimensions>;
		using NT = typename Kernel::NT;

		std::size_t trajectory_count = 0;
		for (const auto &trajectory : trajectory_reader) {
			std::cout << "New trajectory: \n";

			std::vector<std::size_t> segment_id_col;
			apply_segmentation<Kernel, SegmentationTraits>(trajectory, std::back_inserter(segment_id_col));
			std::copy(segment_id_col.begin(), segment_id_col.end(), std::ostream_iterator<std::size_t>(std::cout, ",\n"));

			// Create the new trajectory id column
			std::vector<std::size_t> trajectory_id_col;
			trajectory_id_col.assign(trajectory.size(), trajectory_count);
			std::cout << "Ids:\n";
			std::copy(trajectory_id_col.begin(),
			          trajectory_id_col.end(),
			          std::ostream_iterator<std::size_t>(std::cout, "\n"));
			// Add new fields to the trajectory
			auto segmented_trajectory = concat_field(trajectory, trajectory_id_col, segment_id_col);
			ofcsv << segmented_trajectory;
			++trajectory_count;
		}

		timer.stop();
		std::cout << "Elapsed time: " << timer << '\n';
		std::cout << "Wrote " << trajectory_count << " trajectories.";
	}
};

int main(int argc, char **argv) {
	return movetk::examples::ExampleRunner().run_example<Example>(argc, argv);
}
