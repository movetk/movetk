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
 *   Use monotonic segmentation with  min_heading <= heading <= min_heading + threshold criterion.
 *   Write segmented trajectories to a CSV file.
 *
 *   Authors: Aniket Mitra, Onur Derin
 */

#include <chrono>
#include <vector>

#ifdef _GLIBCXX_PARALLEL
#include <parallel/algorithm>
#endif

#include "movetk/logging.h"
#include "movetk/test_data.h"
#include "movetk/utils/HereTrajectoryTraits.h"
#include "movetk/io/ProbeReader.h"
#include "movetk/SortedProbeReader.h"
#include "movetk/TrajectoryReader.h"
#include "movetk/geom/trajectory_to_interface.h"
#include "movetk/algo/SegmentationTraits.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/GeometryBackendTraits.h"

/**
 * Example: Create trajectories from raw probe points by
 *          - buffering probe points in memory
 *          - sorting them by PROBE_ID
 *          - splitting by PROBE_ID
 *          - sorting each trajectoy points by SAMPLE_DATE
 *          - segment the trajectories by heading 
 *              - please see https://doi.org/10.1145/1869790.1869821 for a description of the algorihtm
 *          - writing trajectories to a CSV file.
 */
int main(int argc, char **argv)
{
    std::ios_base::sync_with_stdio(false);
    std::cout.setf(std::ios::fixed);
    init_logging(logging::trivial::trace);
    BOOST_LOG_TRIVIAL(info) << "Started";
#ifdef _GLIBCXX_PARALLEL
    BOOST_LOG_TRIVIAL(info) << "Using parallel STL";
#endif

#if CGAL_BACKEND_ENABLED
    BOOST_LOG_TRIVIAL(info) << "Using CGAL Backend for Geometry";
#else
    BOOST_LOG_TRIVIAL(info) << "Using Boost Backend for Geometry";
#endif

    // Specializations for the Commit2Data raw probe format
    using TrajectoryTraits = here::c2d::raw::TabularTrajectoryTraits;
    using ProbeTraits = typename TrajectoryTraits::ProbeTraits;

    // Create trajectory reader
    std::unique_ptr<ProbeReader<ProbeTraits>> probe_reader;
    if (argc < 2)
    {
        // Use built-in test data if a file is not specified
        probe_reader = ProbeReaderFactory::create_from_string<ProbeTraits>(testdata::c2d_raw_csv);
    }
    else
    {
        // Process trajectories from a (zipped) CSV file (e.g., probe_data_lametro.20180918.wayne.csv.gz)
        probe_reader = ProbeReaderFactory::create<ProbeTraits>(argv[1]);
    }
    using ProbeInputIterator = decltype(probe_reader->begin());

    constexpr int PROBE_ID = ProbeTraits::ProbeColumns::PROBE_ID;
    SortedProbeReader<ProbeInputIterator, PROBE_ID> sorted_probe_reader(probe_reader->begin(), probe_reader->end());
    using SortedProbeInputIterator = decltype(sorted_probe_reader.begin());
    auto trajectory_reader = TrajectoryReader<TrajectoryTraits, SortedProbeInputIterator>(sorted_probe_reader.begin(),
                                                                                          sorted_probe_reader.end());

    auto t_start = std::chrono::high_resolution_clock::now();

    // Create an output csv file
    std::ofstream ofcsv("output_trajectories_heading.csv");

    // Write the header
    print_tuple(ofcsv, probe_reader->columns());
    ofcsv << ",RAW_TRAJID,HEADING_SEG_ID\n";

    // Write time-sorted trajectories and segment them using  range criteria
    typedef movetk_algorithms::SegmentationTraits<long double,
                                                  typename GeometryKernel::MovetkGeometryKernel, GeometryKernel::dimensions>
        SegmentationTraits;
    typedef GeometryKernel::MovetkGeometryKernel::NT NT;
    typedef std::vector<NT> Headings;
    typedef std::vector<Headings::iterator> SegmentStartReferences;
    SegmentationTraits::HeadingSegmentation segment_by_heading(90);
   

    std::size_t trajectory_count = 0;
    for (auto trajectory : trajectory_reader)
    {
        BOOST_LOG_TRIVIAL(trace) << "New trajectory: \n";

       
        auto headings = trajectory.get<ProbeTraits::ProbeColumns::HEADING>();

        Headings headings_;

        for (auto sit = std::cbegin(headings); sit != std::cend(headings); sit++)
        {
            cout << *sit << endl;
            headings_.push_back(*sit);
        }

        SegmentStartReferences segIdx;
        segment_by_heading(std::begin(headings_), std::end(headings_),
                           movetk_core::movetk_back_insert_iterator(segIdx));
        BOOST_LOG_TRIVIAL(trace) << "Number of Segments: " << segIdx.size();

        movetk_core::SegmentIdGenerator make_segment(std::begin(segIdx), std::end(segIdx));

        std::vector<std::size_t> segment_id_col;
        for (auto plit = std::begin(headings_); plit != std::end(headings_); ++plit)
        {
            auto id = make_segment.getSegmentID(plit);
            segment_id_col.push_back(id);
            BOOST_LOG_TRIVIAL(trace) << "Segment Ids: " << id;
        }

        // Create the new trajectory id column
        std::vector<std::size_t> trajectory_id_col;
        trajectory_id_col.assign(trajectory.size(), trajectory_count);
        // Add new fields to the trajectory
        auto segmented_trajectory = concat_field(trajectory, trajectory_id_col, segment_id_col);

        
        ofcsv << segmented_trajectory;
        ++trajectory_count;
    }

    auto t_end = std::chrono::high_resolution_clock::now();
    display("rest", t_start, t_end);
    BOOST_LOG_TRIVIAL(info) << "Wrote " << trajectory_count << " trajectories.";

    return 0;
}
