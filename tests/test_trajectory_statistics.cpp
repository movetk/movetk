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

#include "movetk/ColumnarTrajectory.h"
#include "movetk/TabularTrajectory.h"
#include "movetk/TuplePrinter.h"

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

TEST_CASE( "Trajectory length", "[trajectory_length]" ) {

    using ProbePoint = std::tuple<string, double, double, std::time_t>;
    // Construct data
    ProbePoint base{ "test",0,0,0 };
    std::vector<double> xs = { 0,0,2,-3 };
    std::vector<double> ys = { 2,4,6,5 };
    std::vector<ProbePoint> data = test_helpers::buildData<ProbePoint, 1, 2, 3>(xs, ys, static_cast<std::time_t>(5),base);

    using Trajectory = TabularTrajectory<string, double, double, std::time_t>;
    Trajectory t{ data };

    // Setup algorithm
    Dist d;
    movetk_algorithms::TrajectoryLength<Trajectory, MovetkGeometryKernel, Dist, 1, 2> lengthCalc(d);

    auto dist = lengthCalc(t);
    
    REQUIRE(dist == Approx(2.0 + std::sqrt(8.) + std::sqrt(26.)) );
}

TEST_CASE("Trajectory duration", "[trajectory_duration]") {
    using ProbePoint = std::tuple<string, double, double, std::time_t>;
    // Construct data
    ProbePoint base{ "test",0,0,0 };
    std::vector<double> xs = { 0, 0, 2,-3, 2.0, 3.0, -2.3, 1.5 };
    std::vector<double> ys = { 2, 4, 6, 5, 1.2, 3.3, 2.25, -1.0 };
    std::vector<std::time_t> times = { 3, 5, 2, 21, 13, 15, 2, 6 };
    std::vector<ProbePoint> data = test_helpers::buildData<ProbePoint, 1, 2, 3>(xs, ys, times, base);

    using Trajectory = TabularTrajectory<string, double, double, std::time_t>;
    movetk_algorithms::TrajectoryDuration<Trajectory, 3> durationCalc;
    Trajectory t{ data };

    //Approx here since duration is implementation defined, may be double
    REQUIRE( durationCalc(t) == Approx(19) ); 
}

TEST_CASE("Trajectory speed statistics", "[trajectory_speed_statistics]") {
    using ProbePoint = std::tuple<string, double, double, std::time_t>;
    // Construct data
    ProbePoint base{ "test",0,0,0 };
    // TODO: maybe a more interesting trajectory.
    std::vector<double> xs = { 0,0,2,-3 };
    std::vector<double> ys = { 2,4,6,5 };
    std::time_t timeStep = 5;
    std::vector<ProbePoint> data = test_helpers::buildData<ProbePoint, 1, 2, 3>(xs, ys, timeStep, base);

    using Trajectory = TabularTrajectory<string, double, double, std::time_t>;
    Dist d;
    movetk_algorithms::TrajectorySpeedStatistic<Trajectory, MovetkGeometryKernel, Dist, 1, 2, 3> speedStat(d);
    using SpeedStat = decltype(speedStat);
    Trajectory t{ data };
    using Stat = typename decltype(speedStat)::Statistic;
    //Approx here since duration is implementation defined, may be double
    SECTION("The mean speed is properly calculated")
    {
        auto mean = speedStat(t,Stat::Mean);
        auto meanComputed = (2.0 / 5.0 + std::sqrt(8) / 5.0 + std::sqrt(26.0) / 5.0) / 3.0;
        REQUIRE(mean == Approx(meanComputed));
    }
    SECTION("The median speed is properly calculated")
    {
        auto median = speedStat(t, Stat::Median);
        auto medianComputed = std::sqrt(8) / 5.0;
        REQUIRE(median == Approx(medianComputed));
    }
    SECTION("The min and max speeds are properly calculated")
    {
        auto minS = speedStat(t, Stat::Min);
        auto maxS = speedStat(t, Stat::Max);
        REQUIRE(minS == Approx(2.0/5.0));
        REQUIRE(maxS == Approx(std::sqrt(26.0)/ 5.0));
    }
    SECTION("The variance of the speeds is properly calculated")
    {
        auto sqDiff = [](SpeedStat::Speed_t s1, SpeedStat::Speed_t s2)
        {
            auto diff = s1 - s2;
            return diff * diff;
        };
        auto mean = speedStat(t, Stat::Mean);
        auto var = speedStat(t, Stat::Variance);
        auto varComputed = (sqDiff(mean, 2.0/5.0) + sqDiff(mean, std::sqrt(26.)/5.0) + sqDiff(mean,std::sqrt(8.)/5.0)) / 3.0;
        REQUIRE(var == Approx(varComputed));
    }
}

TEST_CASE("Trajectory dominant time mode statistic", "[trajectory_time_mode_statistic]")
{
    using ProbePoint = std::tuple<string, double, double, std::time_t>;
    // Construct data
    ProbePoint base{ "test",0,0,0 };
    std::vector<double> xs = { 0, 0, 2,-3, 2.0, 3.0, -2.3, 1.5 };
    std::vector<double> ys = { 2, 4, 6, 5, 1.2, 3.3, 2.25, -1.0 };
    std::vector<std::time_t> times = { 2,3,4,5,6,13,15,21 };
    std::vector<ProbePoint> data = test_helpers::buildData<ProbePoint, 1, 2, 3>(xs, ys, times, base);

    using Trajectory = TabularTrajectory<string, double, double, std::time_t>;
    movetk_algorithms::TrajectoryTimeIntervalMode<Trajectory, 3> timeIntervalModeCalc(4);
    Trajectory t{ data };
    //std::vector<std::time_t> timeDiff = { 1,1,1,1,6,2,7 };

    auto dominant = timeIntervalModeCalc(t);
    using Duration_t = decltype(timeIntervalModeCalc)::Duration_t;
    if constexpr(std::is_integral_v<Duration_t>){
        REQUIRE(dominant == 1);
    }
    else
    {
        auto binSize = (7 - 4) / static_cast<Duration_t>(3);
        auto center = static_cast<Duration_t>(1) - 0.5 * binSize;
        REQUIRE(dominant == Approx(center));
    }
}