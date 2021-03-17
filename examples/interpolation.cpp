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

#include "movetk/logging.h"
#include "movetk/io/csv/ParseDate.h"
#include "movetk/io/csv/csv.h"
#include "movetk/ProbeTraits.h"
#include "movetk/TrajectoryTraits.h"
#include "movetk/TabularTrajectory.h"
#include "movetk/HighFrequencyTrajectoryTraits.h"
#include "movetk/HighFrequencyTrajectoryReader.h"
#include "movetk/SplitByField.h"
#include "movetk/SortByField.h"
#include "movetk/SplitByDifferenceThreshold.h"
#include "movetk/SplitByDistanceThreshold.h"
#include "movetk/geo/geo.h"
#include "movetk/utils/Iterators.h"
#include "movetk/algo/SegmentationTraits.h"
#include "movetk/algo/Interpolation.h"

#include "movetk/utils/GeometryBackendTraits.h"

/* Reads in an input file  of gps data , construct trajectories  
*  and interpolates the trajectories using a kinematic interpolator
*/

// namespace for defining the input types
namespace dl
{

    // namespace for the input
    namespace raw
    {
        //alias for the input columns
        enum InputColumns
        {
            _ORIGFILEROW,
            _RAW_SOURCE,
            _RAW_DEVICEID,
            _RAW_GPSTIME,
            _RAW_LAT,
            _RAW_LON,
            _RAW_HEADING,
            _RAW_SPEED
        };

        // ordering of the input columns
        enum ProbeColumns
        {
            ORIGFILEROW,
            RAW_SOURCE,
            RAW_DEVICEID,
            RAW_GPSTIME,
            RAW_LAT,
            RAW_LON,
            RAW_HEADING,
            RAW_SPEED
        };

        // define the input type as comma seaparted values
        typedef csv<std::tuple<long, string, string, std::time_t,
                               float, float, float, float>,
                    _ORIGFILEROW, _RAW_SOURCE, _RAW_DEVICEID, _RAW_GPSTIME,
                    _RAW_LAT, _RAW_LON, _RAW_HEADING, _RAW_SPEED>
            ProbeCsv;

        // the std::tuple passed as a template parameter to the csv class
        typedef typename ProbeCsv::value_type ProbePoint;
        // the datetime format is assumed to be be a unix timestamp
        // so no class for parsing the input datetime is defined
        typedef int Dummy;
        /* define the probe traits class 
        * the template parameters are:
        *   -  ProbeColumns: the enum that specifies the ordering of the input columns
        *   -  Dummy: the type for datetime column
        *   -  ProbeCsv: the csv type defined above
        *   -  ProbePoint: the std::tuple of each attribute / column type in the input
        */
        typedef _ProbeTraits<ProbeColumns, Dummy, ProbeCsv, ProbePoint> ProbeTraits;

        // alias for the column that will be used to split the input
        constexpr static int SplitByFieldIdx = ProbeTraits::ProbeColumns::RAW_DEVICEID;
        // alias for the column that will be used to sort the input
        constexpr static int SortByFieldIdx = ProbeTraits::ProbeColumns::RAW_GPSTIME;

        // alias for TabularTrajectory where template parameters are the types for each column
        using trajectory_type = TabularTrajectory<long, string, string, std::time_t,
                                                  float, float, float, float>;
        // alias for the Trajectory traits class
        using TrajectoryTraits = _TrajectoryTraits<ProbeTraits, SplitByFieldIdx, SortByFieldIdx, trajectory_type>;

        // alias for the HighFrequencyTrajectoryTraits class
        using HighFrequencyTrajectoryTraits = _HighFrequencyTrajectoryTraits<
            TrajectoryTraits, ProbeTraits::ProbeColumns::RAW_GPSTIME,
            ProbeTraits::ProbeColumns::RAW_LAT, ProbeTraits::ProbeColumns::RAW_LON>;

    } // namespace raw
} // namespace dl

using MovetkGeometryKernel = typename GeometryKernel::MovetkGeometryKernel;

using namespace std;

// namespace for the traits classes for the Interpolation algorithm
namespace Interpolation
{
    // Probe traits required for the interpolation algorithm
    struct ProbeTraits
    {
        enum ProbeColumns
        {
            SAMPLE_DATE,
            LAT,
            LON,
            HEADING,
            SPEED
        };
        typedef MovetkGeometryKernel::NT NT;
        typedef std::tuple<std::time_t, NT, NT, NT, NT> ProbePoint;
    };
    // Norm for computing distances
    using Norm = typename GeometryKernel::Norm;
    // Define the type for projection of geo-coordinates to cartesian coordinates
    typedef LocalCoordinateReference<typename MovetkGeometryKernel::NT> Projection;
    // Interpolation traits class as a composition of various types necessary for trajectory interpolation
    typedef movetk_algorithms::InterpolationTraits<MovetkGeometryKernel, Projection, ProbeTraits, Norm> InterpolationTraits;
    /* The interpolation algorithm class  is parameterized with the following:
    *     - the tag that defines the type of interpolation e.g. linear, kinematic, etc..
    *     - Interpolation traits class that contains all types e.g. projection, norm etc.. 
    *     - the index of the relevant columns
    */
    typedef movetk_algorithms::Interpolator<movetk_algorithms::kinematic_interpolator_tag,
                                            InterpolationTraits, ProbeTraits::ProbeColumns::LAT,
                                            ProbeTraits::ProbeColumns::LON, ProbeTraits::ProbeColumns::SAMPLE_DATE,
                                            ProbeTraits::ProbeColumns::SPEED, ProbeTraits::ProbeColumns::HEADING>
        Interpolator;
} // namespace Interpolation

int main(int argc, char **argv)
{

    // ALTERNATIVE 2: Read already grouped trajectories with high frequency filtering
    //                HighFrequencyTrajectoryReader<HighFrequencyTrajectoryTraits, trajectories_are_grouped> tr(hifreq_time_diff_thr_s, hifreq_dist_diff_thr_m, hifreq_min_num_pts);
    //                tr.init(*file_it);

    std::ios_base::sync_with_stdio(false);
    std::cout.setf(std::ios::fixed);
    init_logging(logging::trivial::trace);
    BOOST_LOG_TRIVIAL(info) << "Started";
#ifdef _GLIBCXX_PARALLEL
    BOOST_LOG_TRIVIAL(info) << "Using parallel STL";
#endif

    // stop if the input filename is not passed to the executable as an argument
    if (argc < 2)
    {
        return 1;
    }

    // threshold for the maximum allowed time gaps between two consecutive points in a trajectory
    double hifreq_time_diff_thr_s = 1.1;
    // threshold for the maximum allowed distance between two consecutve points in a trajectory
    double hifreq_dist_diff_thr_m = 10000.0;
    // threshold on the minimum number of points in a trajectory
    std::size_t hifreq_min_num_pts = 6;

    // instantiate the trajectory reader
    HighFrequencyTrajectoryReader<dl::raw::HighFrequencyTrajectoryTraits, false> tr(hifreq_time_diff_thr_s,
                                                                                    hifreq_dist_diff_thr_m, hifreq_min_num_pts);
    // intitialize the trajectory reader with the input file name
    tr.init(argv[1]);

    //BOOST_LOG_TRIVIAL(trace) <<"Number of filtered trajectories: "<<std::distance(tr.begin(),tr.end());

    // Create an output csv file
    std::ofstream ofcsv_traj("output_trajectories.csv");

    // Write the header
    print_tuple(ofcsv_traj, tr.columns());
    ofcsv_traj << ",RAW_TRAJID,RAW_SEGMENTID,INTERPOLATION_SAMPLE_DATE, INTERPOLATION_LAT, INTERPOLATION_LON, INTERPOLATION_HEADING, INTERPOLATION_SPEED\n";

    // Write time-sorted trajectories and segment them using Monotone Diff Criteria
    typedef movetk_algorithms::SegmentationTraits<long double, MovetkGeometryKernel, GeometryKernel::dimensions> SegmentationTraits;
    typedef MovetkGeometryKernel::NT NT;
    // the time diff criteria is set to 6 i.e each segment will have a time range < 6 seconds
    SegmentationTraits::TSSegmentation segment_by_tdiff(6);

    typename Interpolation::Norm norm;

    // alias for various input fields
    constexpr int IN_FILEROW = dl::raw::ProbeColumns::ORIGFILEROW;
    constexpr int IN_SOURCE = dl::raw::ProbeColumns::RAW_SOURCE;
    constexpr int IN_DEVICEID = dl::raw::ProbeColumns::RAW_DEVICEID;
    constexpr int IN_SAMPLE_DATE = dl::raw::ProbeColumns::RAW_GPSTIME;
    constexpr int IN_LAT = dl::raw::ProbeColumns::RAW_LAT;
    constexpr int IN_LON = dl::raw::ProbeColumns::RAW_LON;
    constexpr int IN_HEADING = dl::raw::ProbeColumns::RAW_HEADING;
    constexpr int IN_SPEED = dl::raw::ProbeColumns::RAW_SPEED;

    // alias for various output fields
    constexpr int OUT_SAMPLE_DATE = Interpolation::ProbeTraits::ProbeColumns::SAMPLE_DATE;
    constexpr int OUT_LAT = Interpolation::ProbeTraits::ProbeColumns::LAT;
    constexpr int OUT_LON = Interpolation::ProbeTraits::ProbeColumns::LON;
    constexpr int OUT_HEADING = Interpolation::ProbeTraits::ProbeColumns::HEADING;
    constexpr int OUT_SPEED = Interpolation::ProbeTraits::ProbeColumns::SPEED;

    // the probe point defined as a std::tuple
    using ProbePoint = Interpolation::ProbeTraits::ProbePoint;

    std::size_t trajectory_count = 0;

    // iterate over the trajectories
    for (auto trajectory : tr)
    {
        // get all timestamps for the current trajectory
        auto timestamps = trajectory.get<IN_SAMPLE_DATE>();

        // segment each trajectory by tdiff criteria
        std::vector<std::size_t> ts;
        std::copy(timestamps.begin(), timestamps.end(), std::back_insert_iterator(ts));

        // container to store references to the start of a new segment
        std::vector<decltype(ts)::const_iterator> segIdx;

        /* run the segmentation algorithm 
        * the first and second arguments are iterators over the timestamps
        * the last argument is an insert iterator over segIdx which will
        *  append to segIdx for every new segment
        */
        segment_by_tdiff(std::cbegin(ts), std::cend(ts), movetk_core::movetk_back_insert_iterator(segIdx));

        /* convert the references to ts contain stored in segIdx to a sequence 
        * of segment ids 0,1,2... wherein a collection of values in ts maps to a 
        *  segment id
        */
        movetk_core::SegmentIdGenerator make_segment(std::begin(segIdx), std::end(segIdx));

        // store the segment ids in the segment_id_col
        std::vector<std::size_t> segment_id_col;
        for (auto plit = std::begin(ts); plit != std::end(ts); ++plit)
        {
            auto id = make_segment.getSegmentID(plit);
            segment_id_col.push_back(id);
        }
        // Create the new trajectory id column
        std::vector<std::size_t> trajectory_id_col;
        trajectory_id_col.assign(trajectory.size(), trajectory_count);

        // Add new fields to the trajectory
        auto segmented_trajectory = concat_field(trajectory, trajectory_id_col, segment_id_col);

        // split the trajectory by segment id
        using SegmentIdType = typename decltype(segmented_trajectory)::value_type;
        /* SplitByField accepts two tgemplate parameters:
        *   - the position of the field to be used for splitting the trajectory
        *   - the trajectory type
        */
        using SplitBySegmentId = SplitByField<9, SegmentIdType>;
        /* Splitter class splits the trajectory. It has the following template parameters
        *   -   SplitBySegmentId defines the type of the attribute that will be used to split  
        *   -   type of the input trajectory
        *  Moreover constructor accepts an iterator ove the trajectory
        */
        Splitter<SplitBySegmentId, decltype(segmented_trajectory.begin())> segment_splitter(segmented_trajectory.begin(), segmented_trajectory.end());

        /* iterator over the first segment
        *  this is being used to get the geo-coordinates from the 
        *  first row of the first segment and use that to initialize the interpolator 
        */
        auto ref = (segment_splitter.begin())->begin();
        MovetkGeometryKernel::NT lat0 = std::get<IN_LAT>(*ref);
        MovetkGeometryKernel::NT lon0 = std::get<IN_LON>(*ref);
        typename Interpolation::Interpolator interpolator(lat0, lon0);

        // iterate over each row of the segment
        for (const auto &rows : segment_splitter)
        {

            //filter out all segments where the tdiff: max_time - min_tim < 6
            if (std::distance(rows.begin(), rows.end()) != 6)
                continue;

            // reference to the first and last row of the segment
            auto first = rows.begin();
            auto last = rows.end() - 1;
            // container to hold the results of the interpolator
            std::vector<ProbePoint> interpolated_pts;
            std::vector<std::time_t> ts;
            // define the end points in the same format as Interpolation::ProbeTraits
            // multiply by 0.277778 to convert speed from km/h to m/s
            ProbePoint p_u = std::make_tuple(
                std::get<IN_SAMPLE_DATE>(*first), std::get<IN_LAT>(*first),
                std::get<IN_LON>(*first), std::get<IN_HEADING>(*first),
                std::get<IN_SPEED>(*first) * 0.277778);

            ProbePoint p_v = std::make_tuple(
                std::get<IN_SAMPLE_DATE>(*last), std::get<IN_LAT>(*last),
                std::get<IN_LON>(*last), std::get<IN_HEADING>(*last),
                std::get<IN_SPEED>(*last) * 0.277778);

            auto it = first + 1;
            ts.push_back(std::get<IN_SAMPLE_DATE>(*first));
            while (it != last)
            {
                ts.push_back(std::get<IN_SAMPLE_DATE>(*it));
                it++;
            }
            ts.push_back(std::get<IN_SAMPLE_DATE>(*last));

            movetk_core::movetk_back_insert_iterator result(interpolated_pts);
            /* run the interpolator 
            * the first two arguments are the endpoint between which interpolation needs
            *  to be done, the next two arguments are the timestamps at which the points
            *  will be interpolated. the last argument is an insert iterator that will 
            *  collect the results of the interpolated points.
            */
            interpolator(p_u, p_v, std::begin(ts), std::end(ts), result);

            // print the interpolated values along with the original values
            // multiplication by 3.6 is to convert speed from m/s to km/h
            auto rit = std::begin(interpolated_pts);
            for (const auto &row : rows)
            {
                std::get<OUT_SPEED>(*rit) = std::get<OUT_SPEED>(*rit) * 3.6;
                auto t = std::tuple_cat(row, *rit);
                print_tuple(ofcsv_traj, t);
                ofcsv_traj << "\n";
                rit++;
            }
        }

        ++trajectory_count;
    }
}
