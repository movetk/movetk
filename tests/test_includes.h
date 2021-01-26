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
    typename GeometryBackend::Wrapper_CGAL_Geometry>
    MovetkGeometryKernel;
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
    namespace detail
    {
        inline bool startsWith(const std::string &str, int offset, const std::string &toSearch, bool ignoreWs)
        {
            if (offset + toSearch.size() >= str.size())
                return false;
            int j = 0;
            for (int i = offset; i < str.size(); ++i)
            {
                if (ignoreWs && std::isspace(str[i]) && j == 0)
                    continue;
                if (str[i] != toSearch[j])
                    return false;
                ++j;
                if (j == toSearch.size())
                    break;
            }
            return j == toSearch.size();
        }
    } // namespace detail

    /**
     * \brief Reads the first <path> element in an ipe string. The path is assumed to only contain 'm' and 'l' operators,
     * thus defining a polyline
     * \param pathData IPE string
     * \param points Output movetk points.
     */
    inline void parseIpePath(const std::string &pathData, std::vector<MovetkGeometryKernel::MovetkPoint> &points)
    {
        movetk_core::MakePoint<MovetkGeometryKernel> make_point;
        int i = 0;
        for (; i < pathData.size(); ++i)
        {
            if (pathData[i] == '<')
            {
                if (!detail::startsWith(pathData, i + 1, "path", true))
                {
                    for (; i < pathData.size(); ++i)
                    {
                        if (pathData[i] == '>')
                            break;
                    }
                }
                else
                {
                    for (; i < pathData.size(); ++i)
                    {
                        if (pathData[i] == '>')
                            break;
                    }
                    // One past the last '>'
                    ++i;
                    break;
                }
            }
        }

        for (; i < pathData.size(); ++i)
        {
            // Read <path> content
            std::vector<NT> buff;

            int prevStart = i;
            for (; i < pathData.size(); ++i)
            {
                if (std::isspace(pathData[i]))
                {
                    if ((i > 0 && std::isspace(pathData[i - 1])) || (i == prevStart))
                    {
                        prevStart = i + 1;
                    }
                    else
                    {
                        std::stringstream ss(pathData.substr(prevStart, i - prevStart));
                        NT datum;
                        ss >> datum;
                        buff.push_back(datum);
                        prevStart = i + 1;
                    }
                }
                else if (pathData[i] == 'm' || pathData[i] == 'l')
                {
                    assert(buff.size() == 2);
                    points.push_back(make_point({buff[0], buff[1]}));
                    buff.clear();
                    prevStart = i + 1;
                }
                // Start of endtag
                else if (pathData[i] == '<')
                {
                    return;
                }
            }
        }
    }

    template <typename FieldsTuple, int XIdx, int YIdx, int TimeIdx>
    std::vector<FieldsTuple> buildData(const std::vector<double> &xs, const std::vector<double> &ys, std::time_t timeIncrement, FieldsTuple defaultTuple)
    {
        assert(xs.size() == ys.size());
        std::vector<FieldsTuple> out(xs.size(), defaultTuple);
        std::time_t curr = std::get<TimeIdx>(defaultTuple);
        for (int i = 0; i < xs.size(); ++i)
        {
            if (i != 0)
            {
                std::get<TimeIdx>(out[i]) = curr + timeIncrement;
                curr += timeIncrement;
            }
            std::get<XIdx>(out[i]) = xs[i];
            std::get<YIdx>(out[i]) = ys[i];
        }
        return out;
    }
    template <typename FieldsTuple, int XIdx, int YIdx, int TimeIdx>
    std::vector<FieldsTuple> buildData(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<std::time_t> &times, FieldsTuple defaultTuple)
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
        return std::abs(v0 - v1) < tolerance;
    }
} // namespace test_helpers
#endif