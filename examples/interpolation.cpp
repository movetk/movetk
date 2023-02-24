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
// Created by Mitra, Aniket on 11/11/2019.
//

#include <chrono>
#include <vector>

#ifdef _GLIBCXX_PARALLEL
#include <parallel/algorithm>
#endif

#include "movetk/Interpolation.h"
#include "movetk/ds/TabularTrajectory.h"
#include "movetk/geo/geo.h"
#include "movetk/io/HighFrequencyTrajectoryReader.h"
#include "movetk/io/HighFrequencyTrajectoryTraits.h"
#include "movetk/io/ParseDate.h"
#include "movetk/io/ProbeTraits.h"
#include "movetk/io/SortByField.h"
#include "movetk/io/SplitByDifferenceThreshold.h"
#include "movetk/io/SplitByDistanceThreshold.h"
#include "movetk/io/SplitByField.h"
#include "movetk/io/TrajectoryTraits.h"
#include "movetk/io/csv/csv.h"
#include "movetk/segmentation/SegmentationTraits.h"
#include "movetk/utils/GeometryBackendTraits.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/Meta.h"

namespace dl {

namespace raw {
enum InputColumns {
	_ORIGFILEROW,
	_RAW_SOURCE,
	_RAW_DEVICEID,
	_RAW_GPSTIME,
	_RAW_LAT,
	_RAW_LON,
	_RAW_HEADING,
	_RAW_SPEED
};

enum ProbeColumns { ORIGFILEROW, RAW_SOURCE, RAW_DEVICEID, RAW_GPSTIME, RAW_LAT, RAW_LON, RAW_HEADING, RAW_SPEED };

using data_tuple = std::tuple<long, std::string, std::string, std::time_t, float, float, float, float>;

using ProbeCsv = movetk::io::csv::csv<data_tuple,
                                      _ORIGFILEROW,
                                      _RAW_SOURCE,
                                      _RAW_DEVICEID,
                                      _RAW_GPSTIME,
                                      _RAW_LAT,
                                      _RAW_LON,
                                      _RAW_HEADING,
                                      _RAW_SPEED>;
typedef typename ProbeCsv::value_type ProbePoint;
typedef int Dummy;
using ProbeTraits = movetk::io::_ProbeTraits<ProbeColumns, ProbeCsv, ProbePoint>;

constexpr static int SplitByFieldIdx = ProbeTraits::ProbeColumns::RAW_DEVICEID;
constexpr static int SortByFieldIdx = ProbeTraits::ProbeColumns::RAW_GPSTIME;
using trajectory_type = movetk::utils::transfer_types<data_tuple, movetk::ds::TabularTrajectory>;

using TrajectoryTraits = movetk::io::_TrajectoryTraits<ProbeTraits, SplitByFieldIdx, SortByFieldIdx, trajectory_type>;

using HighFrequencyTrajectoryTraits = movetk::io::_HighFrequencyTrajectoryTraits<TrajectoryTraits,
                                                                                 ProbeTraits::ProbeColumns::RAW_GPSTIME,
                                                                                 ProbeTraits::ProbeColumns::RAW_LAT,
                                                                                 ProbeTraits::ProbeColumns::RAW_LON>;

}  // namespace raw
}  // namespace dl

using MovetkGeometryKernel = typename GeometryKernel::MovetkGeometryKernel;

using namespace std;

namespace Interpolation {
struct ProbeTraits {
	enum ProbeColumns { SAMPLE_DATE, LAT, LON, HEADING, SPEED };
	typedef MovetkGeometryKernel::NT NT;
	typedef std::tuple<std::time_t, NT, NT, NT, NT> ProbePoint;
};
using Norm = typename GeometryKernel::Norm;
using Projection = movetk::geo::LocalCoordinateReference<typename MovetkGeometryKernel::NT>;
using InterpolationTraits =
    movetk::interpolation::InterpolationTraits<MovetkGeometryKernel, Projection, ProbeTraits, Norm>;
using Interpolator = movetk::interpolation::Interpolator<movetk::interpolation::kinematic_interpolator_tag,
                                                         InterpolationTraits,
                                                         ProbeTraits::ProbeColumns::LAT,
                                                         ProbeTraits::ProbeColumns::LON,
                                                         ProbeTraits::ProbeColumns::SAMPLE_DATE,
                                                         ProbeTraits::ProbeColumns::SPEED,
                                                         ProbeTraits::ProbeColumns::HEADING>;
}  // namespace Interpolation

int main(int argc, char **argv) {
	// ALTERNATIVE 2: Read already grouped trajectories with high frequency filtering
	//                HighFrequencyTrajectoryReader<HighFrequencyTrajectoryTraits, trajectories_are_grouped>
	//                tr(hifreq_time_diff_thr_s, hifreq_dist_diff_thr_m, hifreq_min_num_pts); tr.init(*file_it);

	std::ios_base::sync_with_stdio(false);
	std::cout.setf(std::ios::fixed);
	std::cout << "Started";
#ifdef _GLIBCXX_PARALLEL
	std::cout << "Using parallel STL";
#endif

	if (argc < 2) {
		// Use built-in test data if a file is not specified
		return 1;
	}

	double hifreq_time_diff_thr_s = 1.1;
	double hifreq_dist_diff_thr_m = 10000.0;
	std::size_t hifreq_min_num_pts = 6;

	movetk::io::HighFrequencyTrajectoryReader<dl::raw::HighFrequencyTrajectoryTraits, false> tr(hifreq_time_diff_thr_s,
	                                                                                            hifreq_dist_diff_thr_m,
	                                                                                            hifreq_min_num_pts);
	tr.init(argv[1]);

	// std::cout <<"Number of filtered trajectories: "<<std::distance(tr.begin(),tr.end());

	// Create an output csv file
	std::ofstream ofcsv_traj("output_trajectories.csv");

	// Write the header
	movetk::io::print_tuple(ofcsv_traj, tr.columns());
	ofcsv_traj << ",RAW_TRAJID,RAW_SEGMENTID,INTERPOLATION_SAMPLE_DATE, INTERPOLATION_LAT, INTERPOLATION_LON, "
	              "INTERPOLATION_HEADING, INTERPOLATION_SPEED\n";

	// Write time-sorted trajectories and segment them using Monotone Diff Criteria
	using SegmentationTraits =
	    movetk::segmentation::SegmentationTraits<long double, MovetkGeometryKernel, GeometryKernel::dimensions>;
	using NT = MovetkGeometryKernel::NT;
	SegmentationTraits::TSSegmentation segment_by_tdiff(6);

	typename Interpolation::Norm norm;
	constexpr int IN_FILEROW = dl::raw::ProbeColumns::ORIGFILEROW;
	constexpr int IN_SOURCE = dl::raw::ProbeColumns::RAW_SOURCE;
	constexpr int IN_DEVICEID = dl::raw::ProbeColumns::RAW_DEVICEID;
	constexpr int IN_SAMPLE_DATE = dl::raw::ProbeColumns::RAW_GPSTIME;
	constexpr int IN_LAT = dl::raw::ProbeColumns::RAW_LAT;
	constexpr int IN_LON = dl::raw::ProbeColumns::RAW_LON;
	constexpr int IN_HEADING = dl::raw::ProbeColumns::RAW_HEADING;
	constexpr int IN_SPEED = dl::raw::ProbeColumns::RAW_SPEED;
	using ProbePoint = Interpolation::ProbeTraits::ProbePoint;

	constexpr int OUT_SAMPLE_DATE = Interpolation::ProbeTraits::ProbeColumns::SAMPLE_DATE;
	constexpr int OUT_LAT = Interpolation::ProbeTraits::ProbeColumns::LAT;
	constexpr int OUT_LON = Interpolation::ProbeTraits::ProbeColumns::LON;
	constexpr int OUT_HEADING = Interpolation::ProbeTraits::ProbeColumns::HEADING;
	constexpr int OUT_SPEED = Interpolation::ProbeTraits::ProbeColumns::SPEED;

	std::size_t trajectory_count = 0;
	for (auto trajectory : tr) {
		auto timestamps = trajectory.get<IN_SAMPLE_DATE>();

		std::vector<std::size_t> ts;
		std::copy(timestamps.begin(), timestamps.end(), std::back_insert_iterator(ts));
		std::vector<decltype(ts)::const_iterator> segIdx;
		segment_by_tdiff(std::cbegin(ts), std::cend(ts), std::back_inserter(segIdx));
		movetk::utils::SegmentIdGenerator make_segment(std::begin(segIdx), std::end(segIdx));

		std::vector<std::size_t> segment_id_col;
		for (auto plit = std::begin(ts); plit != std::end(ts); ++plit) {
			auto id = make_segment.getSegmentID(plit);
			segment_id_col.push_back(id);
		}
		// Create the new trajectory id column
		std::vector<std::size_t> trajectory_id_col;
		trajectory_id_col.assign(trajectory.size(), trajectory_count);
		// Add new fields to the trajectory
		auto segmented_trajectory = movetk::ds::concat_field(trajectory, trajectory_id_col, segment_id_col);

		using SegmentIdType = typename decltype(segmented_trajectory)::value_type;
		using SplitBySegmentId = movetk::io::SplitByField<9, SegmentIdType>;
		movetk::io::Splitter<SplitBySegmentId, decltype(segmented_trajectory.begin())> segment_splitter(
		    segmented_trajectory.begin(),
		    segmented_trajectory.end());
		auto ref = (segment_splitter.begin())->begin();
		MovetkGeometryKernel::NT lat0 = std::get<IN_LAT>(*ref);
		MovetkGeometryKernel::NT lon0 = std::get<IN_LON>(*ref);
		typename Interpolation::Interpolator interpolator(lat0, lon0);
		for (const auto &rows : segment_splitter) {
			if (std::distance(rows.begin(), rows.end()) != 6)
				continue;

			auto first = rows.begin();
			auto last = rows.end() - 1;
			std::vector<ProbePoint> interpolated_pts;
			std::vector<std::time_t> ts;
			ProbePoint p_u = std::make_tuple(std::get<IN_SAMPLE_DATE>(*first),
			                                 std::get<IN_LAT>(*first),
			                                 std::get<IN_LON>(*first),
			                                 std::get<IN_HEADING>(*first),
			                                 std::get<IN_SPEED>(*first) * 0.277778);

			ProbePoint p_v = std::make_tuple(std::get<IN_SAMPLE_DATE>(*last),
			                                 std::get<IN_LAT>(*last),
			                                 std::get<IN_LON>(*last),
			                                 std::get<IN_HEADING>(*last),
			                                 std::get<IN_SPEED>(*last) * 0.277778);
			auto it = first + 1;
			ts.push_back(std::get<IN_SAMPLE_DATE>(*first));
			while (it != last) {
				ts.push_back(std::get<IN_SAMPLE_DATE>(*it));
				it++;
			}
			ts.push_back(std::get<IN_SAMPLE_DATE>(*last));
			auto result = std::back_inserter(interpolated_pts);
			// result = p_u;
			interpolator(p_u, p_v, std::begin(ts), std::end(ts), result);
			// result = p_v;

			auto rit = std::begin(interpolated_pts);
			for (const auto &row : rows) {
				std::get<OUT_SPEED>(*rit) = std::get<OUT_SPEED>(*rit) * 3.6;
				auto t = std::tuple_cat(row, *rit);
				movetk::io::print_tuple(ofcsv_traj, t);
				ofcsv_traj << "\n";
				rit++;
			}
		}

		++trajectory_count;
	}
}
