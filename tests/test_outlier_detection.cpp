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
 // Created by Mees van de Kerkhof (m.a.vandekerkhof@uu.nl)
 // Modified by Mitra, Aniket on 2019-07-23.
 //



#include <array>
#include <iostream>
#include <stack>

#include <catch2/catch.hpp>


#include "movetk/algo/Segmentation.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "movetk/metric/Norm.h"
#include "movetk/ds/TabularTrajectory.h"
#include "movetk/algo/AlgorithmTraits.h"
#include "movetk/algo/OutlierDetection.h"
#include "movetk/algo/OutlierDetectionPredicates.h"
#include "movetk/io/CartesianProbeTraits.h"

#include "test_includes.h"

template<typename Backend>
struct OutlierDetectionTests : public test_helpers::BaseTestFixture<Backend> {
    using CartesianProbeTraits = movetk_kernel::ProbeTraits<MovetkGeometryKernel>;
    using Probe = typename CartesianProbeTraits::ProbePoint;
    using Trajectory = std::vector<Probe>;
    using OutlierDetectionTraits = movetk_algorithms::OutlierDetectionTraits<CartesianProbeTraits,
        MovetkGeometryKernel,
        Norm>;
    using LinearSpeedboundedTest = movetk_algorithms::outlier_detection::TEST<movetk_algorithms::linear_speed_bounded_test_tag,
        movetk_algorithms::cartesian_coordinates_tag,
        OutlierDetectionTraits>;
    using Trajectories = std::vector<Trajectory>;
    using Sequence = std::vector<typename Trajectory::const_iterator>;
    using Sequences = std::vector<Sequence>;

    movetk_core::MakePoint<MovetkGeometryKernel> make_point;

    static Trajectory sub_trajectory(const Trajectory& base_trajectory, const std::vector<std::size_t>& indices) {
        Trajectory trajectory;
        trajectory.reserve(indices.size());
        for (auto index : indices) {
            trajectory.push_back(base_trajectory[index]);
        }
        return trajectory;
    }
};


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(OutlierDetectionTests, "greedy_outlier_detector 1", "[greedy_outlier_detector 1]") {
    using OutlierDetector = movetk_algorithms::OutlierDetection<movetk_algorithms::greedy_outlier_detector_tag,
        LinearSpeedboundedTest, OutlierDetectionTraits>;

    Trajectory trajectory = {
        {make_point({ -5, 5 }), 0},
        {make_point({ -6, 3 }), 1 },
        {make_point({ -5, 3 }), 2},
        {make_point({ -3, 5 }), 3},
        {make_point({ -2, 3 }), 4},
        {make_point({ -1, 3 }), 5},
        {make_point({ 1, 5 }), 6},
        {make_point({ 2, 2 }), 7},
        {make_point({ 4, 5 }), 8},
        {make_point({ 5, 5 }), 9},
        {make_point({ 6, 5 }), 10},
        {make_point({ 7, 5 }), 11}
    };

    const auto trajectory_complexity = trajectory.size();

    Trajectory expected_trajectory = sub_trajectory(trajectory, { 0,2,4,5,10,11 });

    OutlierDetector outlier_detector(1.5);

    std::vector<Trajectory::const_iterator> result;

    outlier_detector(std::cbegin(trajectory), std::cend(trajectory), movetk_core::movetk_back_insert_iterator(result));

    const auto num_outliers = trajectory_complexity - result.size();
    const auto expected_outliers = trajectory_complexity - expected_trajectory.size();
    REQUIRE(num_outliers == expected_outliers);

    auto eit = std::cbegin(expected_trajectory);
    for (auto traj_it : result) {
        auto v = std::get<0>(*traj_it) - std::get<0>(*eit);
        REQUIRE((v * v) < MOVETK_EPS);
        eit++;
    }
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(OutlierDetectionTests, "greedy_outlier_detector 2", "[greedy_outlier_detector 2]") {
    using OutlierDetector = movetk_algorithms::OutlierDetection<movetk_algorithms::greedy_outlier_detector_tag,
        LinearSpeedboundedTest, OutlierDetectionTraits>;

    Trajectory trajectory = {
        {make_point({ -5, 5 }), 0},
        {make_point({ -6, 3 }), 1 },
        {make_point({ -5, 3 }), 2},
        {make_point({ -3, 5 }), 3},
        {make_point({ -2, 3 }), 4},
        {make_point({ -1, 3 }), 5},
        {make_point({ 1, 5 }), 6},
        {make_point({ 2, 2 }), 7},
        {make_point({ 4, 5 }), 8},
        {make_point({ 5, 5 }), 9},
        {make_point({ 6, 5 }), 10},
        {make_point({ 7, 5 }), 11}
    };

    const auto trajectory_complexity = trajectory.size();

    Trajectory expected_trajectory = { trajectory[0] };

    OutlierDetector outlier_detector(0);

    std::vector<Trajectory::const_iterator> result;

    outlier_detector(std::cbegin(trajectory), std::cend(trajectory), movetk_core::movetk_back_insert_iterator(result));

    const auto num_outliers = trajectory_complexity - result.size();
    const auto expected_num_outliers = trajectory_complexity - expected_trajectory.size();

    REQUIRE(num_outliers == expected_num_outliers);

    auto traj_it = result[0];
    auto v = std::get<0>(*traj_it) - std::get<0>(expected_trajectory[0]);
    REQUIRE((v * v) < MOVETK_EPS);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(OutlierDetectionTests, "smart_greedy_outlier_detector 1", "[smart_greedy_outlier_detector 1]") {
    using OutlierDetector = movetk_algorithms::OutlierDetection<movetk_algorithms::smart_greedy_outlier_detector_tag,
        LinearSpeedboundedTest, OutlierDetectionTraits>;
    Trajectory trajectory{
    {make_point({ -5, 5 }), 0},
    {make_point({ -6, 3 }), 1},
    {make_point({ -5, 3 }), 2},
    {make_point({ -3, 5 }), 3},
    {make_point({ -2, 3 }), 4},
    {make_point({ -1, 3 }), 5},
    {make_point({ 1, 5 }), 6},
    {make_point({ 2, 2 }), 7},
    {make_point({ 4, 5 }), 8},
    {make_point({ 5, 5 }), 9},
    {make_point({ 6, 5 }), 10},
    {make_point({ 7, 5 }), 11}
    };

    Trajectories trajectories;
    Trajectory expected_trajectory_1 = sub_trajectory(trajectory, { 0,2,4,5,10,11 });
    trajectories.push_back(expected_trajectory_1);
    Trajectory expected_trajectory_2 = sub_trajectory(trajectory, { 1,2,4,5,10,11 });
    trajectories.push_back(expected_trajectory_2);

    OutlierDetector outlier_detector(1.5);

    Sequences sequences;
    auto end_it = outlier_detector(std::cbegin(trajectory), std::cend(trajectory), sequences);

    REQUIRE(std::distance(std::cbegin(sequences), end_it) == 2);

    auto sub_seq = std::cbegin(sequences);
    REQUIRE(sub_seq->size() == (++sub_seq)->size());

    std::size_t num_outliers = trajectory.size() - sub_seq->size();
    REQUIRE(num_outliers == 6);

    auto trajs_eit = std::cbegin(trajectories);
    sub_seq = std::cbegin(sequences);
    for (; sub_seq != end_it; ++sub_seq) {
        auto traj_eit = trajs_eit->cbegin();
        for (auto traj_it : *sub_seq) {
            auto v = std::get<0>(*traj_it) - std::get<0>(*traj_eit);
            REQUIRE((v * v) < MOVETK_EPS);
            traj_eit++;
        }
        trajs_eit++;
    }
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(OutlierDetectionTests, "smart_greedy_outlier_detector 2", "[smart_greedy_outlier_detector 2]") {
    using OutlierDetector = movetk_algorithms::OutlierDetection<movetk_algorithms::smart_greedy_outlier_detector_tag,
        LinearSpeedboundedTest, OutlierDetectionTraits>;
    Trajectory trajectory = {
    {make_point({ -5, 5 }), 0},
    { make_point({ -6, 3 }), 1 },
    { make_point({ -5, 3 }), 2 },
    { make_point({ -3, 5 }), 3 },
    { make_point({ -2, 3 }), 4 },
    { make_point({ -1, 3 }), 5 },
    { make_point({ 1, 5 }), 6 },
    { make_point({ 2, 2 }), 7 },
    { make_point({ 4, 5 }), 8 },
    { make_point({ 5, 5 }), 9 },
    { make_point({ 6, 5 }), 10 },
    { make_point({ 7, 5 }), 11 }
    };
    Trajectories trajectories = { {trajectory[0]}, {trajectory[1]}, {trajectory[2]}, {trajectory[3]}, {trajectory[4]}, {trajectory[5]},
                                 {trajectory[6]}, {trajectory[7]}, {trajectory[8]}, {trajectory[9]}, {trajectory[10]}, {trajectory[11]} };

    OutlierDetector outlier_detector(0);

    Sequences sequences;
    auto end_it = outlier_detector(std::cbegin(trajectory), std::cend(trajectory), sequences);

    REQUIRE(std::distance(std::cbegin(sequences), end_it) == 12);
    auto sub_seq = sequences.cbegin();
    while (sub_seq != (end_it - 1)) {
        REQUIRE(sub_seq->size() == (++sub_seq)->size());
    }

    std::size_t num_outliers = trajectory.size() - sub_seq->size();
    REQUIRE(num_outliers == 11);

    auto trajs_eit = std::cbegin(trajectories);
    sub_seq = std::cbegin(sequences);
    for (; sub_seq != end_it; ++sub_seq, ++trajs_eit) {
        auto traj_eit = trajs_eit->cbegin();
        for (auto traj_it : *sub_seq) {
            auto v = std::get<0>(*traj_it) - std::get<0>(*traj_eit);
            REQUIRE((v * v) < MOVETK_EPS);
            traj_eit++;
        }
    }
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(OutlierDetectionTests, "zheng_greedy_outlier_detector 1", "[zheng_greedy_outlier_detector 1]") {
    using OutlierDetector = movetk_algorithms::OutlierDetection<movetk_algorithms::zheng_outlier_detector_tag,
        LinearSpeedboundedTest, OutlierDetectionTraits>;
    Trajectory trajectory{
    {make_point({ -5, 5 }), 0},
    {make_point({ -6, 3 }), 1},
    {make_point({ -5, 3 }), 2},
    {make_point({ -3, 5 }), 3},
    {make_point({ -2, 3 }), 4},
    {make_point({ -1, 3 }), 5},
    {make_point({ 1, 5 }), 6},
    {make_point({ 2, 2 }), 7},
    {make_point({ 4, 5 }), 8},
    {make_point({ 5, 5 }), 9},
    {make_point({ 6, 5 }), 10},
    {make_point({ 7, 5 }), 11}
    };

    Trajectory expected_trajectory = sub_trajectory(trajectory, { 8,9,10,11 });

    OutlierDetector outlier_detector(1.5, 3);

    std::vector<Trajectory::const_iterator> result;
    outlier_detector(std::cbegin(trajectory), std::cend(trajectory), movetk_core::movetk_back_insert_iterator(result));

    std::size_t num_outliers = trajectory.size() - result.size();
    REQUIRE(num_outliers == 8);

    auto eit = std::cbegin(expected_trajectory);
    for (auto traj_it : result) {
        auto v = std::get<0>(*traj_it) - std::get<0>(*eit);
        REQUIRE((v * v) < MOVETK_EPS);
        eit++;
    }
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(OutlierDetectionTests, "zheng_greedy_outlier_detector 2", "[zheng_greedy_outlier_detector 2]") {
    using OutlierDetector = movetk_algorithms::OutlierDetection<movetk_algorithms::zheng_outlier_detector_tag,
        LinearSpeedboundedTest, OutlierDetectionTraits>;

    Trajectory trajectory{ {make_point({ -5, 5 }), 0},
    {make_point({ -6, 3 }), 1},
    {make_point({ -5, 3 }), 2},
    {make_point({ -3, 5 }), 3},
    {make_point({ -2, 3 }), 4},
    {make_point({ -1, 3 }), 5},
    {make_point({ 1, 5 }), 6},
    {make_point({ 2, 2 }), 7},
    {make_point({ 4, 5 }), 8},
    {make_point({ 5, 5 }), 9},
    {make_point({ 6, 5 }), 10},
    {make_point({ 7, 5 }), 11}
    };

    Trajectory expected_trajectory = { trajectory[0] };

    OutlierDetector outlier_detector(0, 3);

    std::vector<Trajectory::const_iterator> result;

    outlier_detector(std::cbegin(trajectory), std::cend(trajectory), movetk_core::movetk_back_insert_iterator(result));

    std::size_t num_outliers = trajectory.size() - result.size();
    REQUIRE(num_outliers == 12);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(OutlierDetectionTests, "output_sensitive_outlier_detector 1", "[output_sensitive_outlier_detector 1]") {
    using OutlierDetector = movetk_algorithms::OutlierDetection<movetk_algorithms::output_sensitive_outlier_detector_tag,
        LinearSpeedboundedTest, OutlierDetectionTraits>;
    Trajectory trajectory{
    {make_point({ -5, 5 }), 0},
    { make_point({ -6, 3 }), 1 },
    { make_point({ -5, 3 }), 2 },
    { make_point({ -3, 5 }), 3 },
    { make_point({ -2, 3 }), 4 },
    { make_point({ -1, 3 }), 5 },
    { make_point({ 1, 5 }), 6 },
    { make_point({ 2, 2 }), 7 },
    { make_point({ 4, 5 }), 8 },
    { make_point({ 5, 5 }), 9 },
    { make_point({ 6, 5 }), 10 },
    { make_point({ 7, 5 }), 11 }
    };

    Trajectory expected_trajectory = sub_trajectory(trajectory, { 0, 3,  6, 8, 9, 10, 11 });

    OutlierDetector outlier_detector(1.5);

    std::vector<Trajectory::const_iterator> result;
    outlier_detector(std::cbegin(trajectory), std::cend(trajectory), movetk_core::movetk_back_insert_iterator(result));
    std::reverse(std::begin(result), std::end(result));
    std::size_t num_outliers = trajectory.size() - result.size();
    REQUIRE(num_outliers == 5);

    auto eit = std::cbegin(expected_trajectory);
    for (auto traj_it : result) {
        MovetkGeometryKernel::MovetkVector v = std::get<0>(*traj_it) - std::get<0>(*eit);
        REQUIRE((v * v) < MOVETK_EPS);
        eit++;
    }
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(OutlierDetectionTests, "output_sensitive_outlier_detector 2", "[output_sensitive_outlier_detector 2]") {
    using OutlierDetector = movetk_algorithms::OutlierDetection<movetk_algorithms::output_sensitive_outlier_detector_tag,
        LinearSpeedboundedTest, OutlierDetectionTraits>;
    Trajectory trajectory{
    {make_point({ -5, 5 }), 0},
    { make_point({ -6, 3 }), 1 },
    { make_point({ -5, 3 }), 2 },
    { make_point({ -3, 5 }), 3 },
    { make_point({ -2, 3 }), 4 },
    { make_point({ -1, 3 }), 5 },
    { make_point({ 1, 5 }), 6 },
    { make_point({ 2, 2 }), 7 },
    { make_point({ 4, 5 }), 8 },
    { make_point({ 5, 5 }), 9 },
    { make_point({ 6, 5 }), 10 },
    { make_point({ 7, 5 }), 11 }
    };

    Trajectory expected_trajectory = sub_trajectory(trajectory, { 0, 3, 6, 8, 9, 10, 11 });

    OutlierDetector outlier_detector(0);

    std::vector<Trajectory::const_iterator> result;
    outlier_detector(std::cbegin(trajectory), std::cend(trajectory), movetk_core::movetk_back_insert_iterator(result));
    std::reverse(std::begin(result), std::end(result));
    std::size_t num_outliers = trajectory.size() - result.size();

    REQUIRE(num_outliers == 11);
}