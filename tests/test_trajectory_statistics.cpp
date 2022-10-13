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


// Created by Custers, Bram on 2020-02-08.

#include "test_includes.h"

#include "movetk/ds/ColumnarTrajectory.h"
#include "movetk/ds/TabularTrajectory.h"
#include "movetk/io/TuplePrinter.h"

#include "movetk/geom/BoostGeometryTraits.h"
#include "movetk/geom/BoostGeometryWrapper.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/Norm.h"
#include "movetk/algo/Statistics.h"

struct Dist
{
    movetk_support::FiniteNorm<MovetkGeometryKernel, 2> norm;

    NT operator()(MovetkGeometryKernel::MovetkPoint v0, MovetkGeometryKernel::MovetkPoint v1){
        auto  vec = v1 - v0;
        return std::sqrt(norm(vec));
    }
};

TEST_CASE( "Trajectory length statistic", "[trajectory_length][trajectory_statistics]" ) {

    using Trajectory = TabularTrajectory<string, double, double, std::time_t>;
    using ProbePoint = std::tuple<string, double, double, std::time_t>;
    // Setup algorithm
    movetk_algorithms::TrajectoryLength<MovetkGeometryKernel> lengthCalc;

    SECTION("Simple trajectory")
    {
        // Construct data
        ProbePoint base{ "test",0,0,0 };
        std::vector<double> xs = { 0,0,2,-3 };
        std::vector<double> ys = { 2,4,6,5 };
        std::vector<ProbePoint> data = test_helpers::buildData<ProbePoint, 1, 2, 3>(xs, ys, static_cast<std::time_t>(5), base);

        Trajectory t{ data };

        auto dist = lengthCalc(xs.begin(), xs.end(), ys.begin(), ys.end());
        REQUIRE(dist == Approx(2.0 + std::sqrt(8.) + std::sqrt(26.)));
    }
    SECTION("Single segment trajectory")
    {
        // Construct data
        ProbePoint base{ "test",0,0,0 };
        std::vector<double> xs = { 0,2};
        std::vector<double> ys = { 2,6};
        std::vector<ProbePoint> data = test_helpers::buildData<ProbePoint, 1, 2, 3>(xs, ys, static_cast<std::time_t>(5), base);

        Trajectory t{ data };

        auto dist = lengthCalc(xs.begin(), xs.end(), ys.begin(), ys.end());
        REQUIRE(dist == Approx(std::sqrt(2.0*2.0 + 4.0 * 4.0)));
    }
    SECTION("Degenerate point trajectory case")
    {
        // Construct data
        ProbePoint base{ "test",0,0,0 };
        std::vector<double> xs = { 0 };
        std::vector<double> ys = { 2 };
        std::vector<ProbePoint> data = test_helpers::buildData<ProbePoint, 1, 2, 3>(xs, ys, static_cast<std::time_t>(5), base);

        Trajectory t{ data };

        auto dist = lengthCalc(xs.begin(), xs.end(), ys.begin(), ys.end());
        REQUIRE(dist == Approx(0));
    }
    SECTION("Empty trajectory case")
    {
        Trajectory t;
        std::vector<double> xs;
        std::vector<double> ys;

        auto dist = lengthCalc(xs.begin(), xs.end(), ys.begin(), ys.end());
        REQUIRE(dist == Approx(0));
    }
}

TEST_CASE("Trajectory duration", "[trajectory_duration][trajectory_statistics]") {
    using Trajectory = TabularTrajectory<string, double, double, std::time_t>;
    using ProbePoint = std::tuple<string, double, double, std::time_t>;
    movetk_algorithms::TrajectoryDuration durationCalc;
    SECTION("Simple trajectory")
    {
        // Construct data
        ProbePoint base{ "test",0,0,0 };
        std::vector<double> xs = { 0, 0, 2,-3, 2.0, 3.0, -2.3, 1.5 };
        std::vector<double> ys = { 2, 4, 6, 5, 1.2, 3.3, 2.25, -1.0 };
        std::vector<std::time_t> times = { 3, 5, 2, 21, 13, 15, 2, 6 };
        std::vector<ProbePoint> data = test_helpers::buildData<ProbePoint, 1, 2, 3>(xs, ys, times, base);

        Trajectory t{ data };

        //Approx here since duration is implementation defined, may be double
        REQUIRE( durationCalc(times.begin(), times.end()) == Approx(19) );
    }
    SECTION("Degenerate point trajectory case")
    {
        // Construct data
        ProbePoint base{ "test",0,0,0 };
        std::vector<double> xs = { 0 };
        std::vector<double> ys = { 2 };
        std::vector<std::time_t> times = { 3};
        std::vector<ProbePoint> data = test_helpers::buildData<ProbePoint, 1, 2, 3>(xs, ys, static_cast<std::time_t>(5), base);

        Trajectory t { data };

        REQUIRE(durationCalc(times.begin(), times.end()) == Approx(0));
    }
    SECTION("Empty trajectory case")
    {
        Trajectory t;
        std::vector<std::time_t> times;

        REQUIRE(durationCalc(times.begin(), times.end()) == Approx(0));
    }
}

TEST_CASE("Trajectory speed statistics", "[trajectory_speed_statistics][trajectory_statistics]") {
    using ProbePoint = std::tuple<string, double, double, std::time_t>;
    // Construct data
    ProbePoint base{ "test",0,0,0 };

    movetk_algorithms::TrajectorySpeedStatistic<MovetkGeometryKernel> speedStat;
    using SpeedStat = decltype(speedStat);
    using Stat = typename decltype(speedStat)::Statistic;
    //Approx here since duration is implementation defined, may be double
    SECTION("Simple trajectory")
    {
        // TODO: maybe a more interesting trajectory.
        std::vector<double> xs = { 0,0,2,-3 };
        std::vector<double> ys = { 2,4,6,5 };
        std::vector<std::time_t> times = { 0, 5, 10, 15 };
        std::vector<ProbePoint> data = test_helpers::buildData<ProbePoint, 1, 2, 3>(xs, ys, times, base);
        
        auto xItPair = std::make_pair(xs.begin(), xs.end());
        auto yItPair = std::make_pair(ys.begin(), ys.end());
        auto pointItPair = movetk_core::point_iterators_from_coordinates<MovetkGeometryKernel, decltype(xs.begin())>(std::array<decltype(xItPair), 2>{xItPair, yItPair});

        SECTION("The mean speed is properly calculated")
        {
            Stat targetStat = Stat::Mean;
            auto stat = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), targetStat);
            auto statComputed = (2.0 / 5.0 + std::sqrt(8) / 5.0 + std::sqrt(26.0) / 5.0) / 3.0;
            REQUIRE(stat == Approx(statComputed));
        }
        SECTION("The median speed is properly calculated")
        {
            Stat targetStat = Stat::Median;
            auto stat = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), targetStat);
            auto statComputed = std::sqrt(8) / 5.0;
            REQUIRE(stat == Approx(statComputed));
        }
        SECTION("The min and max speeds are properly calculated")
        {
            auto minS = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), Stat::Min);
            auto maxS = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), Stat::Max);
            REQUIRE(minS == Approx(2.0 / 5.0));
            REQUIRE(maxS == Approx(std::sqrt(26.0) / 5.0));
        }
        SECTION("The variance of the speeds is properly calculated")
        {
            auto sqDiff = [](auto s1, auto s2)
            {
                auto diff = s1 - s2;
                return diff * diff;
            };
            auto mean = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), Stat::Mean);
            auto var = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), Stat::Variance);
            auto varComputed = (sqDiff(mean, 2.0 / 5.0) + sqDiff(mean, std::sqrt(26.) / 5.0) + sqDiff(mean, std::sqrt(8.) / 5.0)) / 3.0;
            REQUIRE(var == Approx(varComputed));
        }
    }
    
    SECTION("Single segment trajectory")
    {
        // Construct data
        ProbePoint base{ "test",0,0,0 };
        std::vector<double> xs = { 0,2 };
        std::vector<double> ys = { 2,6 };
        std::vector<std::time_t> times = { 0, 5};

        auto xItPair = std::make_pair(xs.begin(), xs.end());
        auto yItPair = std::make_pair(ys.begin(), ys.end());
        auto pointItPair = movetk_core::point_iterators_from_coordinates<MovetkGeometryKernel, decltype(xs.begin())>(std::array<decltype(xItPair), 2>{xItPair, yItPair});

        auto v = std::sqrt(2.0*2.0 + 4.0 * 4.0) / 5.0;


        auto mean = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), Stat::Mean);
        REQUIRE(mean== Approx(v));
        auto median= speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), Stat::Median);
        REQUIRE(median == Approx(v));
        auto minS = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), Stat::Min);
        REQUIRE(minS == Approx(v));
        auto maxS = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), Stat::Max);
        REQUIRE(maxS == Approx(v));
        auto var = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), Stat::Variance);
        REQUIRE(var == Approx(0));
    }
    SECTION("Degenerate point trajectory case")
    {
        // Construct data
        ProbePoint base{ "test",0,0,0 };
        std::vector<double> xs = { 0 };
        std::vector<double> ys = { 2 };
        std::vector<std::time_t> times = { 0};
        std::vector<ProbePoint> data = test_helpers::buildData<ProbePoint, 1, 2, 3>(xs, ys, static_cast<std::time_t>(5), base);

        auto xItPair = std::make_pair(xs.begin(), xs.end());
        auto yItPair = std::make_pair(ys.begin(), ys.end());
        auto pointItPair = movetk_core::point_iterators_from_coordinates<MovetkGeometryKernel, decltype(xs.begin())>(std::array<decltype(xItPair), 2>{xItPair, yItPair});

        auto v = 0;

        auto mean = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), Stat::Mean);
        REQUIRE(mean == Approx(v));
        auto median = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), Stat::Median);
        REQUIRE(median == Approx(v));
        auto minS = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), Stat::Min);
        REQUIRE(minS == Approx(v));
        auto maxS = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), Stat::Max);
        REQUIRE(maxS == Approx(v));
        auto var = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), Stat::Variance);
        REQUIRE(var == Approx(0));
    }
    SECTION("Empty trajectory case")
    {
        std::vector<double> xs;
        std::vector<double> ys;
        std::vector<std::time_t> times;
        auto xItPair = std::make_pair(xs.begin(), xs.end());
        auto yItPair = std::make_pair(ys.begin(), ys.end());
        auto pointItPair = movetk_core::point_iterators_from_coordinates<MovetkGeometryKernel, decltype(xs.begin())>(std::array<decltype(xItPair), 2>{xItPair, yItPair});

        auto v = 0;
        auto mean = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), Stat::Mean);
        REQUIRE(mean == Approx(v));
        auto median = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), Stat::Median);
        REQUIRE(median == Approx(v));
        auto minS = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), Stat::Min);
        REQUIRE(minS == Approx(v));
        auto maxS = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), Stat::Max);
        REQUIRE(maxS == Approx(v));
        auto var = speedStat(pointItPair.first, pointItPair.second, times.begin(), times.end(), Stat::Variance);
        REQUIRE(var == Approx(0));
    }
}

TEST_CASE("Trajectory dominant time mode statistic", "[trajectory_time_mode_statistic][trajectory_statistics]")
{
    movetk_algorithms::ComputeDominantDifference timeIntervalModeCalc;

    SECTION("Integer times")
    {
        // Construct data
        std::vector<std::size_t> times = { 2,3,4,5,6,13,15,21 };

        using DiffTime_t = decltype(std::declval<std::time_t>() - std::declval<std::time_t>());
        DiffTime_t threshold = 0;
        auto dominant = timeIntervalModeCalc(times.begin(), times.end(), threshold);
        REQUIRE(dominant == 1);
    }

    SECTION("Floating point times")
    {
        SECTION("Double casted integers case")
        {
            // Construct data
            std::vector<double> times = { 2,3,4,5,6,13,15,21 };

            double threshold = 1e-5;
            auto dominant = timeIntervalModeCalc(times.begin(), times.end(), threshold);
            REQUIRE(dominant == Approx(1.0));
        }
        SECTION("Doubles")
        {
            // Construct data
            std::vector<double> targets = { 1.0, 2.1, 2.25,2.5,2.6,2.8, 3.1, 4.5 }; // Target time differences
            std::vector<double> times(targets.size()+1,0);
            for(std::size_t i = 0; i < targets.size();++i)
            {
                times[i + 1] = times[i] + targets[i];
            }
            // Set threshold such that 2.5 captures 2.1, 2.25, 2.5, 2.6 and 2.8.
            const double threshold = 0.5;
            const auto dominant = timeIntervalModeCalc(times.begin(), times.end(), threshold);
            REQUIRE(dominant == Approx(2.5));
        }
    }
}