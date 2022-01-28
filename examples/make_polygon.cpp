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
// Created by Mitra, Aniket on 08/01/2019.
//
#include "movetk/utils/GeometryBackendTraits.h"

int main(int argc, char **argv)
{
#if CGAL_BACKEND_ENABLED
    std::cerr << "Using CGAL Backend for Geometry\n";
#else
    std::cerr << "Using Boost Backend for Geometry\n";
#endif

    //==============================
    // Declare the data structure that will hold a set of points
    // One can use any stl container that supports a random access
    // iterator
    std::vector<typename GeometryKernel::MovetkGeometryKernel::MovetkPoint> points;

    //===================================
    // Create a  Movetk polygon and print its vertices
    movetk::geom::MakePoint<GeometryKernel::MovetkGeometryKernel> make_point;

    points.push_back(make_point({1, 1}));

    std::cout << "Point 1:" << std::endl;
    std::cout << points.back();
    std::cout << std::endl;

    points.push_back(make_point({2, 2}));

    std::cout << "Point 2:" << std::endl;
    std::cout << points.back();
    std::cout << std::endl;

    points.push_back(make_point({3, 3}));

    std::cout << "Point 3: " << std::endl;
    std::cout << points.back();
    std::cout << std::endl;

    movetk::geom::MakePolygon<GeometryKernel::MovetkGeometryKernel> make_polygon;

    GeometryKernel::MovetkGeometryKernel::MovetkPolygon polygon = make_polygon(std::cbegin(points),
                                                                               std::cend(points));

    std::cout << "Polygon: " << std::endl;
    std::cout << polygon;
    std::cout << "\n";
    //====================================

    return 0;
}
