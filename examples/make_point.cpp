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


int main(int argc, char **argv) {
#if CGAL_BACKEND_ENABLED
    std::cerr<<"Using CGAL Backend for Geometry\n";
#else
    std::cerr<<"Using Boost Backend for Geometry\n";
#endif

    //==============================

    //==============================
    // Declare the data structure that  contains a point.
    // This is a temporary store. Can be overwritten
    // every time a new point needs to be created
    // One can use any stl container that supports a random access
    // iterator
    std::array<typename GeometryKernel::MovetkGeometryKernel::NT, GeometryKernel::dimensions> p;
    p = {3, 2};
    //p = {1,1,1,1,1,1,1};

    //===================================
    // Create a  Movetk point and print it
    movetk::geom::MakePoint<GeometryKernel::MovetkGeometryKernel> make_point;
    GeometryKernel::MovetkGeometryKernel::MovetkPoint pt = make_point(std::cbegin(p),
                                                     std::cend(p));
    std::cout<<"Point\n";
    std::cout << pt;
    //====================================
    std::cout<<"\n";

    return 0;
}
