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

#include "movetk/logging.h"
#include "movetk/test_data.h"
#include "movetk/utils/HereTrajectoryTraits.h"
#include "movetk/io/ProbeReader.h"
#include "movetk/TrajectoryReader.h"
#include "movetk/geom/trajectory_gdal.h"
#include "cpl_string.h"

/**
 * Example: Process a stream of probe points by
 *          - creating trajectories from input by
 *            - splitting by PROBE_ID
 *            - sorting by SAMPLE_DATE
 *          - writing processed trajectories to a WKT file.
 */
int main(int argc, char** argv) {
    std::ios_base::sync_with_stdio(false);
    init_logging(logging::trivial::trace);

    // Specializations for the Commit2Data raw probe format
    using TrajectoryTraits = here::c2d::raw::ColumnarTrajectoryTraits;
    // using TrajectoryTraits = here::c2d::raw::TabularTrajectoryTraits; // should also work in this case
    using ProbeTraits = typename TrajectoryTraits::ProbeTraits;

    // Create trajectory reader
    std::unique_ptr<ProbeReader<ProbeTraits>> probe_reader;
    if(argc < 2) {
        // Use built-in test data if a file is not specified
        probe_reader = ProbeReaderFactory::create_from_string<ProbeTraits>(testdata::c2d_raw_csv);
    }
    else {
        // Example: Process trajectories from a (zipped) CSV file (e.g., probe_data_lametro.20180918.wayne.csv.gz)
        probe_reader = ProbeReaderFactory::create<ProbeTraits>(argv[1]);
    }
    using ProbeInputIterator = decltype(probe_reader->begin());
    auto trajectory_reader = TrajectoryReader<TrajectoryTraits, ProbeInputIterator>(probe_reader->begin(), probe_reader->end());

    // Create a wkt file
    std::ofstream ofwkt("output_trajectories.wkt");

    // Process trajectories in a streaming fashion
    std::size_t count = 0;
    for (auto trajectory: trajectory_reader) {
		constexpr int PROBE_ID = ProbeTraits::ProbeColumns::PROBE_ID;
        BOOST_LOG_TRIVIAL(trace) << "New trajectory: " << trajectory.get<PROBE_ID>()[0];

        auto lons = trajectory.get<ProbeTraits::ProbeColumns::LON>();
        auto lats = trajectory.get<ProbeTraits::ProbeColumns::LAT>();
        std::unique_ptr<OGRGeometry> trajectory_geom;
        if(trajectory.size() == 1) {
            trajectory_geom = to_point(std::begin(lats), std::begin(lons));
        }
        else {
            trajectory_geom = to_linestring(std::begin(lats), std::end(lats), std::begin(lons));
        }
        // Write trajectory as WKT
        char* wkt_str;
        trajectory_geom->exportToWkt(&wkt_str);
        ofwkt << wkt_str << std::endl;
        CPLFree(wkt_str);

        count++;
    }
    BOOST_LOG_TRIVIAL(info) << "Processed " << count << " trajectories" << std::endl;
    return 0;
}
