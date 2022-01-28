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
// Created by onur on 10/11/18.
//

#ifndef MOVETK_TRAJECTORY_GDAL_H
#define MOVETK_TRAJECTORY_GDAL_H

#include <ogr_geometry.h>

#include <iostream>
#include <memory>

namespace movetk::geom {

template <class LatIterator, class LonIterator>
std::unique_ptr<OGRMultiPoint> to_multipoint(LatIterator first_lat, LatIterator last_lat, LonIterator first_lon) {
	auto mp = std::make_unique<OGRMultiPoint>();
	while (first_lat != last_lat) {
		mp->addGeometryDirectly(new OGRPoint(*first_lon++, *first_lat++));
	}
	return mp;
}

template <class LatIterator, class LonIterator>
std::unique_ptr<OGRPoint> to_point(LatIterator first_lat, LonIterator first_lon) {
	return std::make_unique<OGRPoint>(*first_lon, *first_lat);
}

template <class LatIterator, class LonIterator>
std::unique_ptr<OGRLineString> to_linestring(LatIterator first_lat, LatIterator last_lat, LonIterator first_lon) {
	auto mp = std::make_unique<OGRLineString>();
	while (first_lat != last_lat) {
		mp->addPoint(*first_lon++, *first_lat++);
	}
	return mp;
}

bool intersects(const OGRGeometry* p1, const OGRGeometry* p2) {
	return p1->Intersects(p2);
}

}  // namespace movetk::geom
#endif  // MOVETK_TRAJECTORY_GDAL_H
