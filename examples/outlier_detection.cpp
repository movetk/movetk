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
// Created by Mitra, Aniket on 2019-08-14.
//

#include <cassert>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include "movetk/utils/GeometryBackendTraits.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "movetk/metric/Norm.h"
#include "movetk/TabularTrajectory.h"
#include "movetk/algo/AlgorithmTraits.h"
#include "movetk/algo/OutlierDetection.h"
#include "movetk/algo/OutlierDetectionPredicates.h"
#include "movetk/CartesianProbeTraits.h"

using namespace std;

/*
* Example to read in a time sorted polyline as an input stream
* and identify outliers with the output sensitive outlier detection
* algorithm. For a description of the algorithm please refer to
* https://doi.org/10.1145/3347146.3359363
*/

class ParseInput
{

public:
    static void show_usage(std::string &name)
    {
        std::cerr << "Usage: " << name << " <option(s)>\n"
                  << "Description: to be filled \n"
                  << "Input Format: X,Y,TS\n"
                  << "Pre-Conditions:\n"
                  << "\t - Input sorted by Timestamp\n"
                  << "Options:\n"
                  << "\t-h,--help\t\tShow this help message\n"
                  << "\t-tr,--trajectory\t\tTrajectories file\n"
                  << "\t--head\t\t\tSpecify this when the input file has header\n"
                  << "\t-idx,--indexes\t\tPosition of columns in the input\n"
                  << "\t-eps,--epsilon\t\tThreshold\n";
    }

    bool operator()(int argc, char **argv)
    {
        std::string executable = argv[0];
        if ((argc >= MinArgs) && (argc <= MaxArgs))
        {
            auto it = argv;
            it++;
            while (it != (argv + argc))
            {
                bool Matched = false;
                auto eit = eargs.cbegin();
                while (eit != eargs.cend())
                {
                    if ((std::get<0>(*eit) == *it) ||
                        (std::get<1>(*eit) == *it))
                    {
                        Matched = true;
                        break;
                    }
                    eit++;
                }
                if (Matched)
                {
                    if (std::get<2>(*eit))
                    {
                        params[std::get<0>(*eit)] = *(it + 1);
                        it = it + 2;
                    }
                    else
                        it++;
                    set_flags(std::get<0>(*eit));
                    eargs.erase(eit);
                }
                else
                {
                    show_usage(executable);
                    return false;
                }
            }
            return true;
        }
        show_usage(executable);
        return false;
    }

    std::string &get_parameter(std::string &key)
    {
        return params[key];
    }

    bool has_header()
    {
        return header;
    }

    bool is_stream()
    {
        return stream;
    }

private:
    static const int MinArgs = 4;
    static const int MaxArgs = 9;
    bool header = false, stream = true;
    typedef std::tuple<std::string, std::string, bool> earg;
    std::vector<earg> eargs{
        std::make_tuple("--head", "--head", false),
        std::make_tuple("-tr", "--trajectory", true),
        std::make_tuple("-idx", "--indexes", true),
        std::make_tuple("-eps", "--epsilon", true)};

    std::map<std::string, std::string> params{
        {"-tr", ""},
        {"-eps", ""},
        {"-idx", ""},
        {"--head", ""}};

    void set_flags(std::string arg)
    {
        if (arg == "--head")
            header = true;
        if (arg == "-tr" || (arg == "--trajectory"))
            stream = false;
    }
};

using MovetkGeometryKernel = typename GeometryKernel::MovetkGeometryKernel;
using Norm = typename GeometryKernel::Norm;

int main(int argc, char **argv)
{
#if CGAL_BACKEND_ENABLED
    std::cerr << "Using CGAL Backend for Geometry\n";
#else
    std::cerr << "Using Boost Backend for Geometry\n";
#endif

    std::ios_base::sync_with_stdio(false);
    std::cout.setf(std::ios::fixed);

    ParseInput parse;
    if (!parse(argc, argv))
        return 0;

    std::cerr << "Has Header?: " << parse.has_header() << "\n";

    std::ios_base::sync_with_stdio(false);
    std::cout.setf(std::ios::fixed);

    std::vector<string> tokens;
    std::string key = "-idx";
    string line;
    line = parse.get_parameter(key);
    movetk_support::split(line, movetk_core::movetk_back_insert_iterator(tokens));
    assert(tokens.size() == 3);

    std::vector<std::size_t> col_idx;
    std::transform(std::cbegin(tokens), std::cend(tokens), std::back_insert_iterator(col_idx),
                   [](auto i) {
                       return static_cast<std::size_t>(std::stoul(i)) - 1;
                   });

    key = "-eps";
    MovetkGeometryKernel::NT threshold = std::stold(parse.get_parameter(key));

    std::cerr << "Threshold: " << threshold << "\n";

    typedef movetk_kernel::ProbeTraits<MovetkGeometryKernel> CartesianProbeTraits;

    typedef CartesianProbeTraits::ProbePoint Probe;
    typedef std::vector<CartesianProbeTraits::ProbePoint> Trajectory;
    typedef std::vector<Trajectory> Trajectories;
    typedef movetk_algorithms::OutlierDetectionTraits<CartesianProbeTraits,
                                                      MovetkGeometryKernel,
                                                      Norm>
        OutlierDetectionTraits;
    typedef movetk_algorithms::outlier_detection::TEST<movetk_algorithms::linear_speed_bounded_test_tag,
                                                       movetk_algorithms::cartesian_coordinates_tag,
                                                       OutlierDetectionTraits>
        Test;
    typedef movetk_algorithms::OutlierDetection<movetk_algorithms::output_sensitive_outlier_detector_tag,
                                                Test, OutlierDetectionTraits>
        OutlierDetector;

    movetk_core::MakePoint<MovetkGeometryKernel> make_point;

    MovetkGeometryKernel::NT ts, x, y;
    Trajectory trajectory;
    std::size_t line_count = 0;

    std::vector<std::string> input;
    if (parse.is_stream())
    {
        while (getline(cin, line))
        {
            tokens.clear();
            if (line_count == 0)
            {
                if (parse.has_header())
                {
                    line_count++;
                    continue;
                }
            }
            input.push_back(line);
            movetk_support::split(line,
                                  movetk_core::movetk_back_insert_iterator(tokens));
            x = std::stold(tokens[col_idx[0]]);
            y = std::stold(tokens[col_idx[1]]);
            ts = static_cast<std::size_t>(std::stoul(tokens[col_idx[2]]));
            trajectory.push_back(make_tuple(make_point({x, y}), ts));
            line_count++;
        }
    }
    else
    {
        key = "-tr";
        std::string trajfile = parse.get_parameter(key);
        ifstream infile;
        infile.open(trajfile);
        while (getline(infile, line))
        {
            tokens.clear();
            if (line_count == 0)
            {
                if (parse.has_header())
                {
                    line_count++;
                    continue;
                }
            }
            input.push_back(line);
            movetk_support::split(line,
                                  movetk_core::movetk_back_insert_iterator(tokens));
            x = std::stold(tokens[col_idx[0]]);
            y = std::stold(tokens[col_idx[1]]);
            ts = static_cast<std::size_t>(std::stoul(tokens[col_idx[2]]));
            trajectory.push_back(make_tuple(make_point({x, y}), ts));
            line_count++;
        }
    }

    std::vector<Trajectory::const_iterator> result;
    OutlierDetector outlier_detector(threshold);
    outlier_detector(std::cbegin(trajectory), std::cend(trajectory), movetk_core::movetk_back_insert_iterator(result));
    std::reverse(std::begin(result), std::end(result));
    std::size_t num_outliers = trajectory.size() - result.size();
    std::cerr << "Number of Outliers Detected: " << num_outliers;
    std::cerr << "\n";
    auto cit = std::cbegin(input);
    auto prev = *std::cbegin(result);
    for (auto traj_it : result)
    {
        std::size_t dist = std::distance(traj_it, std::cbegin(trajectory));
        if (!(dist == 0))
        {
            dist = std::distance(prev, traj_it);
        }
        cit = cit + dist;
        prev = traj_it;
        std::cout << *cit << std::endl;
    }
}
