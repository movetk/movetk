/*
 * Copyright (C) 2018-2020
 * HERE Europe B.V.
 * Utrecht University (The Netherlands).
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
// Created by Wouter Jongeling (wouter.jongeling@gmail.com)
//

#ifndef MOVETK_GEOJSONUTILS_H
#define MOVETK_GEOJSONUTILS_H

#include <vector>
#include <string>
#include <any>
#include "movetk/TuplePrinter.h"

namespace movetk_io {

// range [first, last] (both inclusive!)
template <class LatIterator, class LonIterator>
void trajectory_to_geojson_linestring_coloured(std::ostream& os, LatIterator first_lat_it, LatIterator last_lat_it,
        LonIterator first_lon_it, std::string colour, int width)
{
    os << "{\n"
          "            \"geometry\": {\n"
          "                \"coordinates\": [\n";

    while (first_lat_it != last_lat_it) {
        variadic_print(os, *first_lon_it++, *first_lat_it++);
        os << ",\n";
    }
    variadic_print(os, *first_lon_it, *first_lat_it);

    os << "\n               ],\n";
    os << "                \"type\": \"LineString\"\n"
          "            },\n"
          "            \"properties\": {\n"
          "                \"stroke\": \"" + colour +"\",\n"
          "                \"stroke-width\": " + std::to_string(width) +"\n";
    os << "            },\n"
          "            \"type\": \"Feature\"\n"
          "}";
}

/**
 * Write shapes in the range [first, last] (both inclusive!) as geojson.
 * @tparam TrajectoriesIterator
 * @param start
 * @param last
 * @return
 */
template <class TrajectoriesIterator, int LatIdx, int LonIdx>
void trajectories_to_geojson_coloured(std::ostream& os, TrajectoriesIterator first, TrajectoriesIterator last)
{
    os.setf(std::ios::fixed);

    // Write the GeoJSON header
    os << "{\n"
              "    \"features\": [\n";

    bool colourEven = true;
    // Write trajectories
    while(first != std::next(last)) {

        auto lats = first->template get<LatIdx>();
        auto lons = first->template get<LonIdx>();
        trajectory_to_geojson_linestring_coloured(os, lats.begin(), std::prev(lats.end()), lons.begin(), colourEven ? "blue" : "red", 4);

        if (first != last)
            os << ",\n";
        ++first;
        colourEven = !colourEven;
    }

    // Write GeoJSON footer
    os << "],\n"
              "    \"type\": \"FeatureCollection\"\n"
              "}\n";

}


} // /namespace

#endif //MOVETK_GEOJSONUTILS_H
