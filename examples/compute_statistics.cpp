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

#include "movetk/logging.h"
#include "movetk/test_data.h"
#include "movetk/utils/HereTrajectoryTraits.h"
#include "movetk/io/ProbeReader.h"
#include "movetk/io/TrajectoryReader.h"
#include "movetk/algo/Statistics.h"
#include "movetk/geo/geo.h"
#include "movetk/utils/GeometryBackendTraits.h"

 /**
  * Example: Process a stream of probe points to create a trajectory. Then,
  * display statistics of the trajectory.
  */
template<typename TrajectoryTraits>
void run(int argc, char** argv)
{
    using ProbeTraits = typename TrajectoryTraits::ProbeTraits;

    // Create trajectory reader
    std::unique_ptr<ProbeReader<ProbeTraits>> probe_reader;
    if (argc < 2) {
        // Use built-in test data if a file is not specified
        probe_reader = ProbeReaderFactory::create_from_string<ProbeTraits>(testdata::c2d_format_geolife_csv);
    }
    else {
        // Example: Process trajectories from a (zipped) CSV file (e.g., probe_data_lametro.20180918.wayne.csv.gz)
        probe_reader = ProbeReaderFactory::create<ProbeTraits>(argv[1]);
    }
    using ProbeInputIterator = decltype(probe_reader->begin());
    auto trajectory_reader = TrajectoryReader<TrajectoryTraits, ProbeInputIterator>(probe_reader->begin(), probe_reader->end());

    /**
     * The points in the trajectory are assumed to be lat-lon coordinates. So, use the appropriate distance between the coordinates.
     */
    struct Distance
    {
        GeometryKernel::NT operator()(GeometryKernel::MovetkGeometryKernel::MovetkPoint p0, GeometryKernel::MovetkGeometryKernel::MovetkPoint p1)
        {
            std::vector<GeometryKernel::NT> ll0;
            std::copy(p0.begin(), p0.end(), std::back_inserter(ll0));
            std::vector<GeometryKernel::NT> ll1;
            std::copy(p1.begin(), p1.end(), std::back_inserter(ll1));
            return distance_exact(ll0[1], ll0[0], ll1[1], ll1[0]);
        }
    };

    // Process trajectories in a streaming fashion
    std::size_t count = 0;
    // Shorthands for fields
    constexpr int LON_Idx = ProbeTraits::ProbeColumns::LON;
    constexpr int LAT_Idx = ProbeTraits::ProbeColumns::LAT;
    constexpr int TS_Idx = ProbeTraits::ProbeColumns::SAMPLE_DATE;
    constexpr int PROBE_ID = ProbeTraits::ProbeColumns::PROBE_ID;

    for (auto trajectory : trajectory_reader) {
        
        BOOST_LOG_TRIVIAL(trace) << "New trajectory: " << trajectory.template get<PROBE_ID>()[0];
        BOOST_LOG_TRIVIAL(info) << "Size:" << trajectory.size() << std::endl;
        using Trajectory_t = decltype(trajectory);
        using GeomKernel = GeometryKernel::MovetkGeometryKernel;

        Distance distanceFunc;

        // Get iterators for data
        auto lons = trajectory.template get<LON_Idx>();
        using CoordIt = decltype(lons.begin());
        auto lats = trajectory.template get<LAT_Idx>();
        auto ts = trajectory.template get<TS_Idx>();
        auto pointIterators = movetk::utils::point_iterators_from_coordinates<GeomKernel>(std::array<std::pair<CoordIt, CoordIt>,2>{
            std::make_pair(lons.begin(), lons.end()),
            std::make_pair(lats.begin(), lats.end())
        });

        movetk::algo::TrajectoryLength<GeomKernel, Distance> lenCalc;
        BOOST_LOG_TRIVIAL(info) << "Trajectory length:" << lenCalc(lons.begin(), lons.end(), lats.begin(), lats.end()) << std::endl;
        movetk::algo::TrajectoryDuration duration;
        BOOST_LOG_TRIVIAL(info) << "Trajectory duration:" << duration(ts.begin(), ts.end()) << std::endl;

        // Show speed statistics:
        using SpeedStat = movetk::algo::TrajectorySpeedStatistic<GeomKernel, Distance>;
        SpeedStat speedStat;
        using Stat = typename SpeedStat::Statistic;
        BOOST_LOG_TRIVIAL(info) << "Trajectory average speed:" << speedStat(pointIterators.first, pointIterators.second, ts.begin(), ts.end(),Stat::Mean) << std::endl;
        BOOST_LOG_TRIVIAL(info) << "Trajectory median speed:" << speedStat(pointIterators.first, pointIterators.second, ts.begin(), ts.end(), Stat::Median) << std::endl;
        BOOST_LOG_TRIVIAL(info) << "Trajectory min speed:" << speedStat(pointIterators.first, pointIterators.second, ts.begin(), ts.end(), Stat::Min) << std::endl;
        BOOST_LOG_TRIVIAL(info) << "Trajectory max speed:" << speedStat(pointIterators.first, pointIterators.second, ts.begin(), ts.end(), Stat::Max) << std::endl;
        BOOST_LOG_TRIVIAL(info) << "Trajectory variance of speed:" << speedStat(pointIterators.first, pointIterators.second, ts.begin(), ts.end(), Stat::Variance) << std::endl;

        // Show time mode
        movetk::algo::ComputeDominantDifference timeMode;
        BOOST_LOG_TRIVIAL(info) << "Most common time interval: " << timeMode(ts.begin(), ts.end(), 0);

        count++;
    }
    BOOST_LOG_TRIVIAL(info) << "Processed " << count << " trajectories" << std::endl;
}

int main(int argc, char** argv) {
    std::ios_base::sync_with_stdio(false);
    init_logging(logging::trivial::trace);

    // Run for columnar
    BOOST_LOG_TRIVIAL(info) << "------------ Columnar";
    run<here::c2d::raw::ColumnarTrajectoryTraits>(argc, argv);
    BOOST_LOG_TRIVIAL(info) << "------------ Tabular";
    run<here::c2d::raw::TabularTrajectoryTraits>(argc, argv);
    return 0;
}
