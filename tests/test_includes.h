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
// Created by Custers, Bram on 2020-02-08.
//

#ifndef TEST_INCLUDES_H
#define TEST_INCLUDES_H

#include "catch2/catch.hpp"
#if CGAL_BACKEND_ENABLED
#include "movetk/geom/CGALTraits.h"
#else
#include "movetk/geom/BoostGeometryTraits.h"
#endif

#include "movetk/geom/GeometryInterface.h"

using namespace std;

#if CGAL_BACKEND_ENABLED
//==============================
    // Define the Number Type
    // For the CGAL backend,
    // One can choose from the
    // following number types
    typedef long double NT;
    //typedef CGAL::Mpzf NT;
    //typedef CGAL::Gmpfr NT;
    //typedef CGAL::Gmpq NT;
    //==============================

    //==============================
    // Define the Dimensions
    const size_t dimensions = 2;
    //==============================

    //==============================
    // Define the Geometry Backend
    typedef movetk_support::CGALTraits<NT, dimensions> GeometryBackend;
    //Using the Geometry Backend define the Movetk Geometry Kernel
    typedef movetk_core::MovetkGeometryKernel<
            typename GeometryBackend::Wrapper_CGAL_Geometry> MovetkGeometryKernel;
    //==============================
#else
//==============================
// Define the Number Type
// For the Boost Backend
typedef long double NT;
//==============================

//==============================
// Define the Dimensions
// It is possible to choose
// a higher dimension
// Note: Boost Geometry Adapter supports geometry in two
// dimensions only
const static size_t dimensions = 2;
//==============================

//==============================
// Define the Geometry Backend
typedef movetk_support::BoostGeometryTraits<long double, dimensions> GeometryBackend;
//Using the Geometry Backend define the Movetk Geometry Kernel
typedef movetk_core::MovetkGeometryKernel<typename GeometryBackend::Wrapper_Boost_Geometry> MovetkGeometryKernel;
//==============================
#endif

namespace test_helpers
{
    template<typename FieldsTuple, int XIdx, int YIdx, int TimeIdx>
    std::vector<FieldsTuple> buildData(const std::vector<double>& xs, const std::vector<double>& ys, std::time_t timeIncrement,FieldsTuple defaultTuple)
    {
        assert(xs.size() == ys.size());
        std::vector<FieldsTuple> out(xs.size(), defaultTuple);
        std::time_t curr = std::get<TimeIdx>(defaultTuple);
        for(int i = 0; i < xs.size(); ++i)
        {
            if(i != 0)
            {
                std::get<TimeIdx>(out[i]) = curr + timeIncrement;
                curr += timeIncrement;
            }
            std::get<XIdx>(out[i]) = xs[i];
            std::get<YIdx>(out[i]) = ys[i];
        }
        return out;
    }
    template<typename FieldsTuple, int XIdx, int YIdx, int TimeIdx>
    std::vector<FieldsTuple> buildData(const std::vector<double>& xs, const std::vector<double>& ys, const std::vector<std::time_t>& times, FieldsTuple defaultTuple)
    {
        assert(xs.size() == ys.size());
        assert(xs.size() == times.size());
        std::vector<FieldsTuple> out(xs.size(), defaultTuple);
        for (int i = 0; i < xs.size(); ++i)
        {
            std::get<TimeIdx>(out[i]) = times[i];
            std::get<XIdx>(out[i]) = xs[i];
            std::get<YIdx>(out[i]) = ys[i];
        }
        return out;
    }

    inline bool approximatelyZero(double val, double tolerance = 0.00001)
    {
        return std::abs(val) < tolerance;
    }
    inline bool approximatelyEqual(double v0, double v1, double tolerance = 0.00001)
    {
        return std::abs(v0-v1) < tolerance;
    }
}
#endif