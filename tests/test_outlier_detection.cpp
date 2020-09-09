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
#include <movetk/algo/Segmentation.h>
#include "catch2/catch.hpp"

#if CGAL_BACKEND_ENABLED
#include "movetk/geom/CGALTraits.h"
#else

#include "movetk/geom/BoostGeometryTraits.h"

#endif

#include <stack>
#include "movetk/geom/GeometryInterface.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "movetk/metric/Norm.h"
#include "movetk/TabularTrajectory.h"
#include "movetk/algo/AlgorithmTraits.h"
#include "movetk/algo/OutlierDetection.h"
#include "movetk/algo/OutlierDetectionPredicates.h"
#include "movetk/CartesianProbeTraits.h"




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
    typedef movetk_support::CGALTraits<NT, dimensions> CGAL_GeometryBackend;
    //Using the Geometry Backend define the Movetk Geometry Kernel
    typedef movetk_core::MovetkGeometryKernel<
            typename CGAL_GeometryBackend::Wrapper_CGAL_Geometry> MovetkGeometryKernel;
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
typedef movetk_support::BoostGeometryTraits<long double, dimensions> Boost_Geometry_Backend;
//Using the Geometry Backend define the Movetk Geometry Kernel
typedef movetk_core::MovetkGeometryKernel<typename Boost_Geometry_Backend::Wrapper_Boost_Geometry> MovetkGeometryKernel;
//==============================
#endif

typedef movetk_support::FiniteNorm<MovetkGeometryKernel, 2> Norm;


TEST_CASE("greedy_outlier_detector 1", "[greedy_outlier_detector 1]") {
    std::ios_base::sync_with_stdio(false);
    std::cout.setf(std::ios::fixed);

    typedef movetk_kernel::ProbeTraits<MovetkGeometryKernel> CartesianProbeTraits;

    typedef CartesianProbeTraits::ProbePoint Probe;
    typedef std::vector<CartesianProbeTraits::ProbePoint> Trajectory;

    typedef movetk_algorithms::OutlierDetectionTraits<CartesianProbeTraits,
            MovetkGeometryKernel,
            Norm> OutlierDetectionTraits;
    typedef movetk_algorithms::outlier_detection::TEST<movetk_algorithms::linear_speed_bounded_test_tag,
            movetk_algorithms::cartesian_coordinates_tag,
            OutlierDetectionTraits> Test;
    typedef movetk_algorithms::OutlierDetection<movetk_algorithms::greedy_outlier_detector_tag,
            Test, OutlierDetectionTraits> OutlierDetector;

    movetk_core::MakePoint<MovetkGeometryKernel> make_point;

    Probe p0(make_point({-5, 5}), 0);
    Probe p1(make_point({-6, 3}), 1);
    Probe p2(make_point({-5, 3}), 2);
    Probe p3(make_point({-3, 5}), 3);
    Probe p4(make_point({-2, 3}), 4);
    Probe p5(make_point({-1, 3}), 5);
    Probe p6(make_point({1, 5}), 6);
    Probe p7(make_point({2, 2}), 7);
    Probe p8(make_point({4, 5}), 8);
    Probe p9(make_point({5, 5}), 9);
    Probe p10(make_point({6, 5}), 10);
    Probe p11(make_point({7, 5}), 11);

    Trajectory trajectory = {p0, p1, p2, p3, p4, p5, p6,
                             p7, p8, p9, p10, p11};

    std::cout << "Input Trajectory: \n";
    std::cout << "{";
    for (auto point: trajectory) {
        std::cout << std::get<0>(point);
        std::cout << ';';
    }
    std::cout << "}\n";

    Trajectory expected_trajectory = {p0, p2, p4, p5, p10, p11};

    OutlierDetector outlier_detector(1.5);

    std::vector<Trajectory::const_iterator> result;

    outlier_detector(std::cbegin(trajectory), std::cend(trajectory), movetk_core::movetk_back_insert_iterator(result));

    std::size_t num_outliers = trajectory.size() - result.size();
    std::cout << "Number of Outliers Detected: " << num_outliers;
    std::cout << "\n";

    REQUIRE(num_outliers == 6);

    auto eit = std::cbegin(expected_trajectory);
    std::cout << "Trajectory w/o Outliers: \n";
    std::cout << "{";
    for (auto traj_it: result) {
        std::cout << std::get<0>(*traj_it);
        std::cout << ';';
        MovetkGeometryKernel::MovetkVector v = std::get<0>(*traj_it) - std::get<0>(*eit);
        REQUIRE((v * v) < MOVETK_EPS);
        eit++;
    }
    std::cout << "}\n";
}


TEST_CASE("greedy_outlier_detector 2", "[greedy_outlier_detector 2]") {
    std::ios_base::sync_with_stdio(false);
    std::cout.setf(std::ios::fixed);

    typedef movetk_kernel::ProbeTraits<MovetkGeometryKernel> CartesianProbeTraits;

    typedef CartesianProbeTraits::ProbePoint Probe;
    typedef std::vector<CartesianProbeTraits::ProbePoint> Trajectory;

    typedef movetk_algorithms::OutlierDetectionTraits<CartesianProbeTraits,
            MovetkGeometryKernel,
            Norm> OutlierDetectionTraits;
    typedef movetk_algorithms::outlier_detection::TEST<movetk_algorithms::linear_speed_bounded_test_tag,
            movetk_algorithms::cartesian_coordinates_tag,
            OutlierDetectionTraits> Test;
    typedef movetk_algorithms::OutlierDetection<movetk_algorithms::greedy_outlier_detector_tag,
            Test, OutlierDetectionTraits> OutlierDetector;

    movetk_core::MakePoint<MovetkGeometryKernel> make_point;

    Probe p0(make_point({-5, 5}), 0);
    Probe p1(make_point({-6, 3}), 1);
    Probe p2(make_point({-5, 3}), 2);
    Probe p3(make_point({-3, 5}), 3);
    Probe p4(make_point({-2, 3}), 4);
    Probe p5(make_point({-1, 3}), 5);
    Probe p6(make_point({1, 5}), 6);
    Probe p7(make_point({2, 2}), 7);
    Probe p8(make_point({4, 5}), 8);
    Probe p9(make_point({5, 5}), 9);
    Probe p10(make_point({6, 5}), 10);
    Probe p11(make_point({7, 5}), 11);

    Trajectory trajectory = {p0, p1, p2, p3, p4, p5, p6,
                             p7, p8, p9, p10, p11};

    std::cout << "Input Trajectory: \n";
    std::cout << "{";
    for (auto point: trajectory) {
        std::cout << std::get<0>(point);
        std::cout << ';';
    }
    std::cout << "}\n";

    Trajectory expected_trajectory = {p0};

    OutlierDetector outlier_detector(0);

    std::vector<Trajectory::const_iterator> result;

    outlier_detector(std::cbegin(trajectory), std::cend(trajectory), movetk_core::movetk_back_insert_iterator(result));

    std::size_t num_outliers = trajectory.size() - result.size();
    std::cout << "Number of Outliers Detected: " << num_outliers;
    std::cout << "\n";

    REQUIRE(num_outliers == 11);

    auto traj_it = result[0];
    MovetkGeometryKernel::MovetkVector v = std::get<0>(*traj_it) - std::get<0>(expected_trajectory[0]);
    REQUIRE((v * v) < MOVETK_EPS);

    std::cout << "Trajectory w/o Outliers: \n";
    std::cout << "{";
    std::cout << std::get<0>(*traj_it);
    std::cout << "}\n";
}


TEST_CASE("smart_greedy_outlier_detector 1", "[smart_greedy_outlier_detector 1]") {
    std::ios_base::sync_with_stdio(false);
    std::cout.setf(std::ios::fixed);

    typedef movetk_kernel::ProbeTraits<MovetkGeometryKernel> CartesianProbeTraits;

    typedef CartesianProbeTraits::ProbePoint Probe;

    typedef std::vector<CartesianProbeTraits::ProbePoint> Trajectory;
    typedef std::vector<Trajectory> Trajectories;

    typedef std::vector<Trajectory::const_iterator> Sequence;
    typedef std::vector<Sequence> Sequences;

    typedef movetk_algorithms::OutlierDetectionTraits<CartesianProbeTraits,
            MovetkGeometryKernel,
            Norm> OutlierDetectionTraits;

    typedef movetk_algorithms::outlier_detection::TEST<movetk_algorithms::linear_speed_bounded_test_tag,
            movetk_algorithms::cartesian_coordinates_tag,
            OutlierDetectionTraits> Test;
    typedef movetk_algorithms::OutlierDetection<movetk_algorithms::smart_greedy_outlier_detector_tag,
            Test, OutlierDetectionTraits> OutlierDetector;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;

    Probe p0(make_point({-5, 5}), 0);
    Probe p1(make_point({-6, 3}), 1);
    Probe p2(make_point({-5, 3}), 2);
    Probe p3(make_point({-3, 5}), 3);
    Probe p4(make_point({-2, 3}), 4);
    Probe p5(make_point({-1, 3}), 5);
    Probe p6(make_point({1, 5}), 6);
    Probe p7(make_point({2, 2}), 7);
    Probe p8(make_point({4, 5}), 8);
    Probe p9(make_point({5, 5}), 9);
    Probe p10(make_point({6, 5}), 10);
    Probe p11(make_point({7, 5}), 11);

    Trajectory trajectory = {p0, p1, p2, p3, p4, p5, p6,
                             p7, p8, p9, p10, p11};

    std::cout << "Input Trajectory: \n";
    std::cout << "{";
    for (auto point: trajectory) {
        std::cout << std::get<0>(point);
        std::cout << ';';
    }
    std::cout << "}\n";

    Trajectories trajectories;
    Trajectory expected_trajectory_1 = {p0, p2, p4, p5, p10, p11};
    trajectories.push_back(expected_trajectory_1);
    Trajectory expected_trajectory_2 = {p1, p2, p4, p5, p10, p11};
    trajectories.push_back(expected_trajectory_2);

    OutlierDetector outlier_detector(1.5);


    Sequences sequences;
    auto end_it = outlier_detector(std::cbegin(trajectory), std::cend(trajectory), sequences);

    REQUIRE (std::distance(std::cbegin(sequences), end_it) == 2);

    auto sub_seq = std::cbegin(sequences);
    REQUIRE (sub_seq->size() == (++sub_seq)->size());

    std::size_t num_outliers = trajectory.size() - sub_seq->size();
    std::cout << "Number of Outliers Detected: " << num_outliers;
    std::cout << "\n";

    REQUIRE(num_outliers == 6);

    auto trajs_eit = std::cbegin(trajectories);
    std::cout << "Trajectory w/o Outliers: \n";
    sub_seq = std::cbegin(sequences);
    while (sub_seq != end_it) {
        auto traj_eit = trajs_eit->cbegin();
        std::cout << "{";
        for (auto traj_it: *sub_seq) {
            std::cout << std::get<0>(*traj_it);
            std::cout << ';';
            MovetkGeometryKernel::MovetkVector v = std::get<0>(*traj_it) - std::get<0>(*traj_eit);
            REQUIRE((v * v) < MOVETK_EPS);
            traj_eit++;
        }
        std::cout << "};";
        trajs_eit++;
        sub_seq++;
    }
    std::cout << "\n";
}


TEST_CASE("smart_greedy_outlier_detector 2", "[smart_greedy_outlier_detector 2]") {
    std::ios_base::sync_with_stdio(false);
    std::cout.setf(std::ios::fixed);
    typedef movetk_kernel::ProbeTraits<MovetkGeometryKernel> CartesianProbeTraits;

    typedef CartesianProbeTraits::ProbePoint Probe;

    typedef std::vector<CartesianProbeTraits::ProbePoint> Trajectory;
    typedef std::vector<Trajectory> Trajectories;

    typedef std::vector<Trajectory::const_iterator> Sequence;
    typedef std::vector<Sequence> Sequences;

    typedef movetk_algorithms::OutlierDetectionTraits<CartesianProbeTraits,
            MovetkGeometryKernel,
            Norm> OutlierDetectionTraits;

    typedef movetk_algorithms::outlier_detection::TEST<movetk_algorithms::linear_speed_bounded_test_tag,
            movetk_algorithms::cartesian_coordinates_tag,
            OutlierDetectionTraits> Test;
    typedef movetk_algorithms::OutlierDetection<movetk_algorithms::smart_greedy_outlier_detector_tag,
            Test, OutlierDetectionTraits> OutlierDetector;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;


    Probe p0(make_point({-5, 5}), 0);
    Probe p1(make_point({-6, 3}), 1);
    Probe p2(make_point({-5, 3}), 2);
    Probe p3(make_point({-3, 5}), 3);
    Probe p4(make_point({-2, 3}), 4);
    Probe p5(make_point({-1, 3}), 5);
    Probe p6(make_point({1, 5}), 6);
    Probe p7(make_point({2, 2}), 7);
    Probe p8(make_point({4, 5}), 8);
    Probe p9(make_point({5, 5}), 9);
    Probe p10(make_point({6, 5}), 10);
    Probe p11(make_point({7, 5}), 11);

    Trajectory trajectory = {p0, p1, p2, p3, p4, p5, p6,
                             p7, p8, p9, p10, p11};

    std::cout << "Input Trajectory: \n";
    std::cout << "{";
    for (auto point: trajectory) {
        std::cout << std::get<0>(point);
        std::cout << ';';
    }
    std::cout << "}\n";

    Trajectories trajectories = {{p0}, {p1}, {p2}, {p3}, {p4}, {p5},
                                 {p6}, {p7}, {p8}, {p9}, {p10}, {p11}};

    OutlierDetector outlier_detector(0);

    Sequences sequences;
    auto end_it = outlier_detector(std::cbegin(trajectory), std::cend(trajectory), sequences);

    REQUIRE (std::distance( std::cbegin(sequences), end_it ) == 12);
    auto sub_seq = sequences.cbegin();
    while (sub_seq != (end_it - 1)){
        REQUIRE (sub_seq->size() == (++sub_seq)->size());
    }

    std::size_t num_outliers = trajectory.size() - sub_seq->size();
    std::cout << "Number of Outliers Detected: " << num_outliers;
    std::cout << "\n";

    REQUIRE(num_outliers == 11);

    auto trajs_eit = std::cbegin(trajectories);
    sub_seq = std::cbegin(sequences);
    std::cout << "Trajectory w/o Outliers: \n";
    while ( sub_seq !=  end_it) {
        auto traj_eit = trajs_eit->cbegin();
        std::cout << "{";
        for (auto traj_it: *sub_seq) {
            std::cout << std::get<0>(*traj_it);
            std::cout << ';';
            MovetkGeometryKernel::MovetkVector v = std::get<0>(*traj_it) - std::get<0>(*traj_eit);
            REQUIRE((v * v) < MOVETK_EPS);
            traj_eit++;
        }
        std::cout << "};";
        trajs_eit++;
        sub_seq++;
    }
    std::cout << "\n";
}


TEST_CASE("zheng_greedy_outlier_detector 1", "[zheng_greedy_outlier_detector 1]") {
    std::ios_base::sync_with_stdio(false);
    std::cout.setf(std::ios::fixed);

    typedef movetk_kernel::ProbeTraits<MovetkGeometryKernel> CartesianProbeTraits;

    typedef CartesianProbeTraits::ProbePoint Probe;
    typedef std::vector<CartesianProbeTraits::ProbePoint> Trajectory;
    typedef std::vector<Trajectory> Trajectories;
    typedef movetk_algorithms::OutlierDetectionTraits<CartesianProbeTraits,
            MovetkGeometryKernel,
            Norm> OutlierDetectionTraits;
    typedef movetk_algorithms::outlier_detection::TEST<movetk_algorithms::linear_speed_bounded_test_tag,
            movetk_algorithms::cartesian_coordinates_tag,
            OutlierDetectionTraits> Test;
    typedef movetk_algorithms::OutlierDetection<movetk_algorithms::zheng_outlier_detector_tag,
            Test, OutlierDetectionTraits> OutlierDetector;

    movetk_core::MakePoint<MovetkGeometryKernel> make_point;


    Probe p0(make_point({-5, 5}), 0);
    Probe p1(make_point({-6, 3}), 1);
    Probe p2(make_point({-5, 3}), 2);
    Probe p3(make_point({-3, 5}), 3);
    Probe p4(make_point({-2, 3}), 4);
    Probe p5(make_point({-1, 3}), 5);
    Probe p6(make_point({1, 5}), 6);
    Probe p7(make_point({2, 2}), 7);
    Probe p8(make_point({4, 5}), 8);
    Probe p9(make_point({5, 5}), 9);
    Probe p10(make_point({6, 5}), 10);
    Probe p11(make_point({7, 5}), 11);

    Trajectory trajectory = {p0, p1, p2, p3, p4, p5, p6,
                             p7, p8, p9, p10, p11};

    std::cout << "Input Trajectory: \n";
    std::cout << "{";
    for (auto point: trajectory) {
        std::cout << std::get<0>(point);
        std::cout << ';';
    }
    std::cout << "}\n";

    Trajectory expected_trajectory = { p8, p9, p10, p11};

    OutlierDetector outlier_detector(1.5, 3);

    std::vector<Trajectory::const_iterator> result;
    outlier_detector(std::cbegin(trajectory), std::cend(trajectory), movetk_core::movetk_back_insert_iterator(result));

    std::size_t num_outliers = trajectory.size() - result.size();
    std::cout << "Number of Outliers Detected: " << num_outliers;
    std::cout << "\n";

    REQUIRE(num_outliers == 8);

    auto eit = std::cbegin(expected_trajectory);
    std::cout << "Trajectory w/o Outliers: \n";
    std::cout << "{";
    for (auto traj_it: result) {
        std::cout << std::get<0>(*traj_it);
        std::cout << ';';
        MovetkGeometryKernel::MovetkVector v = std::get<0>(*traj_it) - std::get<0>(*eit);
        REQUIRE((v * v) < MOVETK_EPS);
        eit++;
    }
    std::cout << "}\n";
}

TEST_CASE("zheng_greedy_outlier_detector 2", "[zheng_greedy_outlier_detector 2]") {
    std::ios_base::sync_with_stdio(false);
    std::cout.setf(std::ios::fixed);

    typedef movetk_kernel::ProbeTraits<MovetkGeometryKernel> CartesianProbeTraits;

    typedef CartesianProbeTraits::ProbePoint Probe;
    typedef std::vector<CartesianProbeTraits::ProbePoint> Trajectory;
    typedef std::vector<Trajectory> Trajectories;
    typedef movetk_algorithms::OutlierDetectionTraits<CartesianProbeTraits,
            MovetkGeometryKernel,
            Norm> OutlierDetectionTraits;
    typedef movetk_algorithms::outlier_detection::TEST<movetk_algorithms::linear_speed_bounded_test_tag,
            movetk_algorithms::cartesian_coordinates_tag,
            OutlierDetectionTraits> Test;
    typedef movetk_algorithms::OutlierDetection<movetk_algorithms::zheng_outlier_detector_tag,
            Test, OutlierDetectionTraits> OutlierDetector;

    movetk_core::MakePoint<MovetkGeometryKernel> make_point;

    Probe p0(make_point({-5, 5}), 0);
    Probe p1(make_point({-6, 3}), 1);
    Probe p2(make_point({-5, 3}), 2);
    Probe p3(make_point({-3, 5}), 3);
    Probe p4(make_point({-2, 3}), 4);
    Probe p5(make_point({-1, 3}), 5);
    Probe p6(make_point({1, 5}), 6);
    Probe p7(make_point({2, 2}), 7);
    Probe p8(make_point({4, 5}), 8);
    Probe p9(make_point({5, 5}), 9);
    Probe p10(make_point({6, 5}), 10);
    Probe p11(make_point({7, 5}), 11);

    Trajectory trajectory = {p0, p1, p2, p3, p4, p5, p6,
                             p7, p8, p9, p10, p11};

    std::cout << "Input Trajectory: \n";
    std::cout << "{";
    for (auto point: trajectory) {
        std::cout << std::get<0>(point);
        std::cout << ';';
    }
    std::cout << "}\n";

    Trajectory expected_trajectory = {p0};

    OutlierDetector outlier_detector(0,3);

    std::vector<Trajectory::const_iterator> result;

    outlier_detector(std::cbegin(trajectory), std::cend(trajectory), movetk_core::movetk_back_insert_iterator(result));

    std::size_t num_outliers = trajectory.size() - result.size();
    std::cout << "Number of Outliers Detected: " << num_outliers;
    std::cout << "\n";

    REQUIRE(num_outliers == 12);

}

TEST_CASE("output_sensitive_outlier_detector 1", "[output_sensitive_outlier_detector 1]") {
    std::ios_base::sync_with_stdio(false);
    std::cout.setf(std::ios::fixed);

    typedef movetk_kernel::ProbeTraits<MovetkGeometryKernel> CartesianProbeTraits;

    typedef CartesianProbeTraits::ProbePoint Probe;
    typedef std::vector<CartesianProbeTraits::ProbePoint> Trajectory;
    typedef std::vector<Trajectory> Trajectories;
    typedef movetk_algorithms::OutlierDetectionTraits<CartesianProbeTraits,
            MovetkGeometryKernel,
            Norm> OutlierDetectionTraits;
    typedef movetk_algorithms::outlier_detection::TEST<movetk_algorithms::linear_speed_bounded_test_tag,
            movetk_algorithms::cartesian_coordinates_tag,
            OutlierDetectionTraits> Test;
    typedef movetk_algorithms::OutlierDetection<movetk_algorithms::output_sensitive_outlier_detector_tag,
            Test, OutlierDetectionTraits> OutlierDetector;

    movetk_core::MakePoint<MovetkGeometryKernel> make_point;


    Probe p0(make_point({-5, 5}), 0);
    Probe p1(make_point({-6, 3}), 1);
    Probe p2(make_point({-5, 3}), 2);
    Probe p3(make_point({-3, 5}), 3);
    Probe p4(make_point({-2, 3}), 4);
    Probe p5(make_point({-1, 3}), 5);
    Probe p6(make_point({1, 5}), 6);
    Probe p7(make_point({2, 2}), 7);
    Probe p8(make_point({4, 5}), 8);
    Probe p9(make_point({5, 5}), 9);
    Probe p10(make_point({6, 5}), 10);
    Probe p11(make_point({7, 5}), 11);

    Trajectory trajectory = {p0, p1, p2, p3, p4, p5, p6,
                             p7, p8, p9, p10, p11};

    std::cout << "Input Trajectory: \n";
    std::cout << "{";
    for (auto point: trajectory) {
        std::cout << std::get<0>(point);
        std::cout << ';';
    }
    std::cout << "}\n";

    Trajectory expected_trajectory = { p0, p3,  p6, p8, p9, p10, p11};

    OutlierDetector outlier_detector(1.5);

    std::vector<Trajectory::const_iterator> result;
    outlier_detector(std::cbegin(trajectory), std::cend(trajectory), movetk_core::movetk_back_insert_iterator(result));
    std::reverse(std::begin(result),std::end(result));
    std::size_t num_outliers = trajectory.size() - result.size();
    std::cout << "Number of Outliers Detected: " << num_outliers;
    std::cout << "\n";

    REQUIRE(num_outliers == 5);

    auto eit = std::cbegin(expected_trajectory);
    std::cout << "Trajectory w/o Outliers: \n";
    std::cout << "{";
    for (auto traj_it: result) {
        std::cout << std::get<0>(*traj_it);
        std::cout << ';';
        MovetkGeometryKernel::MovetkVector v = std::get<0>(*traj_it) - std::get<0>(*eit);
        REQUIRE((v * v) < MOVETK_EPS);
        eit++;
    }
    std::cout << "}\n";
}


TEST_CASE("output_sensitive_outlier_detector 2", "[output_sensitive_outlier_detector 2]") {
    std::ios_base::sync_with_stdio(false);
    std::cout.setf(std::ios::fixed);

    typedef movetk_kernel::ProbeTraits<MovetkGeometryKernel> CartesianProbeTraits;

    typedef CartesianProbeTraits::ProbePoint Probe;
    typedef std::vector<CartesianProbeTraits::ProbePoint> Trajectory;
    typedef std::vector<Trajectory> Trajectories;
    typedef movetk_algorithms::OutlierDetectionTraits<CartesianProbeTraits,
            MovetkGeometryKernel,
            Norm> OutlierDetectionTraits;
    typedef movetk_algorithms::outlier_detection::TEST<movetk_algorithms::linear_speed_bounded_test_tag,
            movetk_algorithms::cartesian_coordinates_tag,
            OutlierDetectionTraits> Test;
    typedef movetk_algorithms::OutlierDetection<movetk_algorithms::output_sensitive_outlier_detector_tag,
            Test, OutlierDetectionTraits> OutlierDetector;

    movetk_core::MakePoint<MovetkGeometryKernel> make_point;


    Probe p0(make_point({-5, 5}), 0);
    Probe p1(make_point({-6, 3}), 1);
    Probe p2(make_point({-5, 3}), 2);
    Probe p3(make_point({-3, 5}), 3);
    Probe p4(make_point({-2, 3}), 4);
    Probe p5(make_point({-1, 3}), 5);
    Probe p6(make_point({1, 5}), 6);
    Probe p7(make_point({2, 2}), 7);
    Probe p8(make_point({4, 5}), 8);
    Probe p9(make_point({5, 5}), 9);
    Probe p10(make_point({6, 5}), 10);
    Probe p11(make_point({7, 5}), 11);

    Trajectory trajectory = {p0, p1, p2, p3, p4, p5, p6,
                             p7, p8, p9, p10, p11};

    std::cout << "Input Trajectory: \n";
    std::cout << "{";
    for (auto point: trajectory) {
        std::cout << std::get<0>(point);
        std::cout << ';';
    }
    std::cout << "}\n";

    Trajectory expected_trajectory = { p0, p3,  p6, p8, p9, p10, p11};

    OutlierDetector outlier_detector(0);

    std::vector<Trajectory::const_iterator> result;
    outlier_detector(std::cbegin(trajectory), std::cend(trajectory), movetk_core::movetk_back_insert_iterator(result));
    std::reverse(std::begin(result),std::end(result));
    std::size_t num_outliers = trajectory.size() - result.size();
    std::cout << "Number of Outliers Detected: " << num_outliers;
    std::cout << "\n";

    REQUIRE(num_outliers == 11);

}



