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
// Created by Mitra, Aniket on 30/01/2019.
//
#include "movetk/utils/GeometryBackendTraits.h"



int main(int argc, char **argv){
#if CGAL_BACKEND_ENABLED
    std::cerr<<"Using CGAL Backend for Geometry\n";
#else
    std::cerr<<"Using Boost Backend for Geometry\n";
#endif

    movetk::geom::MakePoint<GeometryKernel::MovetkGeometryKernel> make_point;
    auto pt1 = make_point({1, 1});
    auto pt2 = make_point({2, 2});
    movetk::geom::MakeSegment<GeometryKernel::MovetkGeometryKernel> make_segment;
    auto seg = make_segment(pt1, pt2);

    movetk::geom::ComputeLength<GeometryKernel::MovetkGeometryKernel> compute_length;
    std::cout<<"Length of the Segment: "<<compute_length(seg)<<std::endl;
    return 0;
}
