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
// Created by onur on 11/8/18.
//
#include "movetk/geo/geo.h"

#include <iostream>
#include <cmath>
//#include <iomanip>
#include <GeographicLib/Geocentric.hpp>
#include <GeographicLib/LocalCartesian.hpp>
#include <GeographicLib/Geodesic.hpp>
using namespace GeographicLib;

namespace movetk::geo {
double distance_exact(double lat0, double lon0, double lat1, double lon1) {
    const Geodesic& geod = Geodesic::WGS84();
    double s12;
    geod.Inverse(lat0, lon0, lat1, lon1, s12);
    return s12;
}

void destination_exact(double lat0, double lon0, double x, double y, double& lat, double& lon) {
    const Geocentric& earth = Geocentric::WGS84();
    LocalCartesian proj(lat0, lon0, 0, earth);
    double h;  // unused
    proj.Reverse(x, y, 0, lat, lon, h);
}

void destination_by_bearing_exact(double lat0, double lon0, double bearing, double distance, double& lat1, double& lon1) {
    const Geodesic& geod = Geodesic::WGS84();
    geod.Direct(lat0, lon0, bearing, distance, lat1, lon1);
}

double bearing_exact(double lat0, double lon0, double lat1, double lon1) {
    const Geodesic& geod = Geodesic::WGS84();
    double s12, azi1, azi2;
    geod.Inverse(lat0, lon0, lat1, lon1, s12, azi1, azi2);
    return azi1;
}

void meters_per_degree(double lat0, double lon0, double& meters_per_lat_degree, double& meters_per_lon_degree) {
    double lat, lon;
    destination_exact(lat0, lon0, 1, 1, lat, lon);
    meters_per_lat_degree = 1.0/(lat - lat0);
    meters_per_lon_degree = 1.0/(lon - lon0);
}

double euclidean_distance(double x0, double y0, double x1, double y1) {
    auto dx = x1-x0;
    auto dy = y1-y0;
    return sqrt(dx*dx + dy*dy);
}

double euclidean_distance_3d(double x0, double y0, double z0, double x1, double y1, double z1) {
    auto dx = x1-x0;
    auto dy = y1-y0;
    auto dz = z1-z0;
    return sqrt(dx*dx + dy*dy + dz*dz);
}
}