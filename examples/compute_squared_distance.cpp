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
// Created by Mitra, Aniket on 2019-02-19.
//

#include "movetk/utils/GeometryBackendTraits.h"
#include "movetk/metric/DistanceInterface.h"

int main(int argc, char **argv)
{
#if CGAL_BACKEND_ENABLED
    std::cerr << "Using CGAL Backend for Geometry\n";
#else
    std::cerr << "Using Boost Backend for Geometry\n";
#endif

    movetk::geom::MakePoint<GeometryKernel::MovetkGeometryKernel> make_point;
    GeometryKernel::MovetkGeometryKernel::MovetkPoint pt1 = make_point({3, 3});

    GeometryKernel::MovetkGeometryKernel::MovetkPoint pt2 = make_point({3, 1});
    GeometryKernel::MovetkGeometryKernel::MovetkPoint pt3 = make_point({5, 3});
    movetk::utils::MakeSegment<GeometryKernel::MovetkGeometryKernel> make_segment;
    GeometryKernel::MovetkGeometryKernel::MovetkSegment seg = make_segment(pt2, pt3);

    movetk::utils::ComputeSquaredDistance<GeometryKernel::MovetkGeometryKernel,
                                        GeometryKernel::Norm>
        squared_dist;
    std::cout << "Squared Distance from Point to Segment: " << squared_dist(pt1, seg) << std::endl;
    return 0;
}
