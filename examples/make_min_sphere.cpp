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
#include "movetk/utils/Iterators.h"

int main(int argc, char **argv) {
#if CGAL_BACKEND_ENABLED
    std::cerr<<"Using CGAL Backend for Geometry\n";
#else
    std::cerr<<"Using Boost Backend for Geometry\n";
#endif

    std::cout.setf(std::ios::fixed);

    //==============================
    // Declare the data structure that will hold a set of points
    // One can use any stl container that supports a random access
    // iterator
    std::vector<typename GeometryKernel::MovetkGeometryKernel::MovetkPoint> points;
    std::vector<typename GeometryKernel::MovetkGeometryKernel::NT> CenterMinSphere;
    //===================================
    // Create a  Movetk min sphere and print it's center and radius
    movetk::geom::MakePoint<GeometryKernel::MovetkGeometryKernel> make_point;


    points.push_back(make_point({1, 1}));

    std::cout<<"Point 1:"<<std::endl;
    std::cout << points.back();
    std::cout << std::endl;

    points.push_back(make_point({2, 2}));

    std::cout<<"Point 2:"<<std::endl;
    std::cout << points.back();
    std::cout << std::endl;

    points.push_back(make_point({3, 3}));

    std::cout<<"Point 3: "<<std::endl;
    std::cout << points.back();
    std::cout << std::endl;

    movetk::geom::MakeMinSphere<GeometryKernel::MovetkGeometryKernel> min_sphere;

    GeometryKernel::MovetkGeometryKernel::NT radius = min_sphere(std::cbegin(points), std::cend(points),
                                                                 std::back_inserter(
                                                                         CenterMinSphere));

    std::cout<<"Radius: "<<std::endl;
    std::cout<<radius<<std::endl;

    std::cout<<"Center: "<<std::endl;
    auto it = std::cbegin(CenterMinSphere);
    while ( it != std::cend(CenterMinSphere) ){
        std::cout<<*it;
        std::cout<<" ";
        it++;
    }
    //====================================
    std::cout<<"\n";
    return 0;
}
