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
// Created by Mitra, Aniket on 2019-02-21.
//

#include "movetk/utils/GeometryBackendTraits.h"
#include "movetk/algo/Simplification.h"
#include "movetk/utils/Iterators.h"

/*
* Example for simplyfying a polyline
* using the Douglas-Peucker simplification algorithm
*/

int main(int argc, char **argv)
{
#if CGAL_BACKEND_ENABLED
    std::cerr << "Using CGAL Backend for Geometry\n";
#else
    std::cerr << "Using Boost Backend for Geometry\n";
#endif

    typedef std::vector<GeometryKernel::MovetkGeometryKernel::MovetkPoint> PolyLine;
    typedef movetk_algorithms::FindFarthest<GeometryKernel::MovetkGeometryKernel, GeometryKernel::Norm> FindFarthest;

    // functor to create a point
    movetk_core::MakePoint<GeometryKernel::MovetkGeometryKernel> make_point;
    // create a polyline
    PolyLine polyline({make_point({-6.19, -3.46}), make_point({-4.99, 1.16}),
                       make_point({-2.79, -2.22}), make_point({-1.87, 0.58}),
                       make_point({0.77, 0.22}), make_point({-1.15, 3.06}),
                       make_point({5.33, -1.12})});

    // print the polyline
    std::cout << "Polyline to be simplified: ";
    std::cout << "{";
    for (auto vertex : polyline)
    {
        cout << vertex;
        std::cout << ";";
    }
    std::cout << "}\n";

    /* container for storing the simplified polyline
    * the entries do not store the values itself but
    * reference to the points in the polyline
    */
    std::vector<PolyLine::const_iterator> result;

    // Initialize the Douglas-Peucker algorithm
    movetk_algorithms::DouglasPeucker<GeometryKernel::MovetkGeometryKernel, FindFarthest> DouglasPeucker(10);

    /* Run the DP algorithm, the first two arguments are iterators over the polyline and the 
    *  last argument is a insert iterator that stores the reference to the points in the 
    *  polyline that will be retained after simplification. The result container is updated 
    *  with an append operation.
    */
    DouglasPeucker(std::begin(polyline), std::end(polyline), movetk_core::movetk_back_insert_iterator(result));

    // print the simplified polyline
    std::cout << "Simplified polyline has: " << result.size() << " vertices\n";
    std::cout << "Simplified Polyline: ";
    std::cout << "{";
    for (auto reference : result)
    {
        cout << *reference;
        std::cout << ";";
    }
    std::cout << "}\n";
    return 0;
}
