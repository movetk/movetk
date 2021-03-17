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

#include "movetk/logging.h"
#include "movetk/test_data.h"
#include "movetk/utils/GeolifeTrajectoryTraits.h"
#include "movetk/geom/trajectory_to_interface.h"
#include "movetk/io/ProbeReader.h"
#include "movetk/TrajectoryReader.h"
#include "movetk/utils/GeometryBackendTraits.h"

int main(int argc, char **argv)
{
    std::ios_base::sync_with_stdio(false);
    init_logging(logging::trivial::trace);

    // Specializations for the Geolife raw probe format
    using TrajectoryTraits = geolife::c2d::raw::TabularTrajectoryTraits;
    using ProbeTraits = typename TrajectoryTraits::ProbeTraits;

    // Create trajectory reader
    std::unique_ptr<ProbeReader<ProbeTraits>> probe_reader;
    if (argc < 2)
    {
        // Use built-in test data if a file is not specified
        probe_reader = ProbeReaderFactory::create_from_string<ProbeTraits>(testdata::geolife_raw_csv);
    }
    else
    {
        // Example: Process trajectories from a (zipped) CSV file (e.g., probe_data_lametro.20180918.wayne.csv.gz)
        probe_reader = ProbeReaderFactory::create<ProbeTraits>(argv[1]);
    }

    // iterator over the probe_reader
    using ProbeInputIterator = decltype(probe_reader->begin());

    /* iterator over the trajectory reader, 
    *  the constructor over the TrajectoryReader class takes iterator
    *  over the probe reader as input 
    */
    auto trajectory_reader = TrajectoryReader<TrajectoryTraits, ProbeInputIterator>(probe_reader->begin(), probe_reader->end());

    // iterate over the trajectories
    for (auto trajectory : trajectory_reader)
    {
        double prev_lat, prev_lon;
        bool first = true;

        // alias for the LAT and LON index in the input
        constexpr int LAT = ProbeTraits::ProbeColumns::LAT;
        constexpr int LON = ProbeTraits::ProbeColumns::LON;

        // iterate over each row of the trajectory
        for (auto probe : trajectory)
        {
            // if first row of the trajectory
            if (first)
            {
                // get LAT value from probe and
                // set it as previous LAT
                prev_lat = get<LAT>(probe);
                // get LON value from probe and
                // set it as previous LON
                prev_lon = get<LON>(probe);
                first = false;
            }
            else
            {
                // get LAT value from probe and set it as current lat
                double curr_lat = get<LAT>(probe);
                // get LON value from probe and set it as current lon
                double curr_lon = get<LON>(probe);
                // calcuclate distance between the two consecutive points in the trajectory
                double d = distance_exact(prev_lat, prev_lon, curr_lat, curr_lon);
                // calculate headincg between two consecutive points in the trajectory
                double b = bearing_exact(prev_lat, prev_lon, curr_lat, curr_lon);
                //Log  output to console
                BOOST_LOG_TRIVIAL(trace) << d << " " << b;
                // reset the current row to the previous row
                prev_lat = curr_lat;
                prev_lon = curr_lon;
            }
        }
    }
}