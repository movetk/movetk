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
#include <catch2/catch.hpp>
#include <iostream>
#include <stack>

#include "helpers/CustomCatchTemplate.h"
#include "movetk/AlgorithmTraits.h"
#include "movetk/OutlierDetection.h"
#include "movetk/ds/TabularTrajectory.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/io/CartesianProbeTraits.h"
#include "movetk/metric/Norm.h"
#include "movetk/outlierdetection/OutlierDetectionPredicates.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"

template <typename Backend>
struct OutlierDetectionTests : public test_helpers::BaseTestFixture<Backend> {
	using Base = test_helpers::BaseTestFixture<Backend>;
	using CartesianProbeTraits = movetk::io::ProbeTraits<typename Base::MovetkGeometryKernel>;
	using Probe = typename CartesianProbeTraits::ProbePoint;
	using Trajectory = std::vector<Probe>;
	using OutlierDetectionTraits = movetk::outlierdetection::
	    OutlierDetectionTraits<CartesianProbeTraits, typename Base::MovetkGeometryKernel, typename Base::Norm>;
	using LinearSpeedboundedTest = movetk::outlierdetection::TEST<movetk::outlierdetection::linear_speed_bounded_test_tag,
	                                                              movetk::algo::cartesian_coordinates_tag,
	                                                              OutlierDetectionTraits>;

	struct OutlierDetectionFactory {
		template <typename Tag>
		using outlierdetector_for_tag =
		    movetk::outlierdetection::OutlierDetection<typename Base::MovetkGeometryKernel, LinearSpeedboundedTest, Tag>;
	};

	using Trajectories = std::vector<Trajectory>;
	using Sequence = std::vector<typename Trajectory::const_iterator>;
	using Sequences = std::vector<Sequence>;

	movetk::geom::MakePoint<typename Base::MovetkGeometryKernel> make_point;

	static Sequence create_expected_subtrajectory(const Trajectory& base_trajectory,
	                                              const std::vector<std::size_t>& indices) {
		Sequence sequence;
		sequence.reserve(indices.size());
		for (auto index : indices) {
			sequence.push_back(base_trajectory.cbegin() + index);
		}
		return sequence;
	}
	static Trajectory create_trajectory(std::initializer_list<Probe> initializer) {
		return Trajectory(std::move(initializer));
	}

	template <typename OutlierDetector>
	static void verify_outlier_detector_output(OutlierDetector& detector,
	                                           const Trajectory& trajectory,
	                                           const Sequence& expected_filtered_trajectory) {
		std::vector<typename Trajectory::const_iterator> result;
		detector(std::cbegin(trajectory), std::cend(trajectory), std::back_inserter(result));

		REQUIRE(result.size() == expected_filtered_trajectory.size());
		// Check individual elements
		REQUIRE(std::equal(result.begin(), result.end(), expected_filtered_trajectory.begin()));
	}
	template <typename OutlierDetector>
	static void verify_outlier_detector_multi_output(OutlierDetector& detector,
	                                                 const Trajectory& trajectory,
	                                                 const Sequences& expected_filtered_trajectories) {
		// Sanity check
		for (std::size_t i = 0; i < expected_filtered_trajectories.size() - 1; ++i) {
			REQUIRE(expected_filtered_trajectories[i].size() == expected_filtered_trajectories[i + 1].size());
		}

		// Find the list of sequences that are maximal after oulier detection.
		Sequences sequences;
		auto end_it = detector(std::cbegin(trajectory), std::cend(trajectory), sequences);

		REQUIRE(std::distance(std::cbegin(sequences), end_it) == expected_filtered_trajectories.size());

		// Checked for empty, so done.
		if (sequences.size() == 0) {
			return;
		}

		// All sequences should be of the same size
		for (std::size_t i = 0; i < sequences.size() - 1; ++i) {
			REQUIRE(sequences[i].size() == sequences[i + 1].size());
		}
		REQUIRE(sequences[0].size() == expected_filtered_trajectories[0].size());

		for (std::size_t i = 0; i < sequences.size(); ++i) {
			REQUIRE(std::equal(sequences[i].begin(), sequences[i].end(), expected_filtered_trajectories[i].begin()));
		}
	}
};


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(OutlierDetectionTests,
                                      "greedy_outlier_detector 1",
                                      "[greedy_outlier_detector 1]") {
	using Fixture = OutlierDetectionTests<TestType>;
	using OutlierDetector = typename Fixture::OutlierDetectionFactory::template outlierdetector_for_tag<
	    movetk::outlierdetection::greedy_outlier_detector_tag>;
	auto make_point = Fixture::make_point;
	auto trajectory = Fixture::create_trajectory({{make_point({-5, 5}), 0},
	                                              {make_point({-6, 3}), 1},
	                                              {make_point({-5, 3}), 2},
	                                              {make_point({-3, 5}), 3},
	                                              {make_point({-2, 3}), 4},
	                                              {make_point({-1, 3}), 5},
	                                              {make_point({1, 5}), 6},
	                                              {make_point({2, 2}), 7},
	                                              {make_point({4, 5}), 8},
	                                              {make_point({5, 5}), 9},
	                                              {make_point({6, 5}), 10},
	                                              {make_point({7, 5}), 11}});

	const auto trajectory_complexity = trajectory.size();
	const auto expected_trajectory = Fixture::create_expected_subtrajectory(trajectory, {0, 2, 4, 5, 10, 11});
	OutlierDetector outlier_detector(1.5);

	Fixture::verify_outlier_detector_output(outlier_detector, trajectory, expected_trajectory);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(OutlierDetectionTests,
                                      "greedy_outlier_detector 2",
                                      "[greedy_outlier_detector 2]") {
	using Fixture = OutlierDetectionTests<TestType>;
	auto make_point = Fixture::make_point;
	using OutlierDetector = typename Fixture::OutlierDetectionFactory::template outlierdetector_for_tag<
	    movetk::outlierdetection::greedy_outlier_detector_tag>;
	typename Fixture::Trajectory trajectory = {{make_point({-5, 5}), 0},
	                                           {make_point({-6, 3}), 1},
	                                           {make_point({-5, 3}), 2},
	                                           {make_point({-3, 5}), 3},
	                                           {make_point({-2, 3}), 4},
	                                           {make_point({-1, 3}), 5},
	                                           {make_point({1, 5}), 6},
	                                           {make_point({2, 2}), 7},
	                                           {make_point({4, 5}), 8},
	                                           {make_point({5, 5}), 9},
	                                           {make_point({6, 5}), 10},
	                                           {make_point({7, 5}), 11}};

	const auto trajectory_complexity = trajectory.size();

	const auto expected_trajectory = Fixture::create_expected_subtrajectory(trajectory, {0});

	OutlierDetector outlier_detector(0);

	Fixture::verify_outlier_detector_output(outlier_detector, trajectory, expected_trajectory);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(OutlierDetectionTests,
                                      "smart_greedy_outlier_detector 1",
                                      "[smart_greedy_outlier_detector 1]") {
	using Fixture = OutlierDetectionTests<TestType>;
	auto make_point = Fixture::make_point;
	using OutlierDetector = typename Fixture::OutlierDetectionFactory::template outlierdetector_for_tag<
	    movetk::outlierdetection::smart_greedy_outlier_detector_tag>;
	auto trajectory = Fixture::create_trajectory({{make_point({-5, 5}), 0},
	                                              {make_point({-6, 3}), 1},
	                                              {make_point({-5, 3}), 2},
	                                              {make_point({-3, 5}), 3},
	                                              {make_point({-2, 3}), 4},
	                                              {make_point({-1, 3}), 5},
	                                              {make_point({1, 5}), 6},
	                                              {make_point({2, 2}), 7},
	                                              {make_point({4, 5}), 8},
	                                              {make_point({5, 5}), 9},
	                                              {make_point({6, 5}), 10},
	                                              {make_point({7, 5}), 11}});

	typename Fixture::Sequences expected_trajectories{
	    Fixture::create_expected_subtrajectory(trajectory, {0, 2, 4, 5, 10, 11}),
	    Fixture::create_expected_subtrajectory(trajectory, {1, 2, 4, 5, 10, 11})};

	OutlierDetector outlier_detector(1.5);
	Fixture::verify_outlier_detector_multi_output(outlier_detector, trajectory, expected_trajectories);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(OutlierDetectionTests,
                                      "smart_greedy_outlier_detector 2",
                                      "[smart_greedy_outlier_detector 2]") {
	using Fixture = OutlierDetectionTests<TestType>;
	auto make_point = Fixture::make_point;
	using OutlierDetector = typename Fixture::OutlierDetectionFactory::template outlierdetector_for_tag<
	    movetk::outlierdetection::smart_greedy_outlier_detector_tag>;
	typename Fixture::Trajectory trajectory = {{make_point({-5, 5}), 0},
	                                           {make_point({-6, 3}), 1},
	                                           {make_point({-5, 3}), 2},
	                                           {make_point({-3, 5}), 3},
	                                           {make_point({-2, 3}), 4},
	                                           {make_point({-1, 3}), 5},
	                                           {make_point({1, 5}), 6},
	                                           {make_point({2, 2}), 7},
	                                           {make_point({4, 5}), 8},
	                                           {make_point({5, 5}), 9},
	                                           {make_point({6, 5}), 10},
	                                           {make_point({7, 5}), 11}};
	typename Fixture::Sequences expected_trajectories;
	for (std::size_t i = 0; i < trajectory.size(); ++i) {
		expected_trajectories.push_back(Fixture::create_expected_subtrajectory(trajectory, {i}));
	}

	OutlierDetector outlier_detector(0);

	Fixture::verify_outlier_detector_multi_output(outlier_detector, trajectory, expected_trajectories);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(OutlierDetectionTests,
                                      "zheng_greedy_outlier_detector 1",
                                      "[zheng_greedy_outlier_detector 1]") {
	using Fixture = OutlierDetectionTests<TestType>;
	auto make_point = Fixture::make_point;
	using OutlierDetector = typename Fixture::OutlierDetectionFactory::template outlierdetector_for_tag<
	    movetk::outlierdetection::zheng_outlier_detector_tag>;
	typename Fixture::Trajectory trajectory{{make_point({-5, 5}), 0},
	                                        {make_point({-6, 3}), 1},
	                                        {make_point({-5, 3}), 2},
	                                        {make_point({-3, 5}), 3},
	                                        {make_point({-2, 3}), 4},
	                                        {make_point({-1, 3}), 5},
	                                        {make_point({1, 5}), 6},
	                                        {make_point({2, 2}), 7},
	                                        {make_point({4, 5}), 8},
	                                        {make_point({5, 5}), 9},
	                                        {make_point({6, 5}), 10},
	                                        {make_point({7, 5}), 11}};

	const auto expected_trajectory = Fixture::create_expected_subtrajectory(trajectory, {8, 9, 10, 11});

	typename Fixture::LinearSpeedboundedTest tester(0.05);
	OutlierDetector outlier_detector(1.5, 3);

	Fixture::verify_outlier_detector_output(outlier_detector, trajectory, expected_trajectory);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(OutlierDetectionTests,
                                      "zheng_greedy_outlier_detector 2",
                                      "[zheng_greedy_outlier_detector 2]") {
	using Fixture = OutlierDetectionTests<TestType>;
	auto make_point = Fixture::make_point;
	using OutlierDetector = typename Fixture::OutlierDetectionFactory::template outlierdetector_for_tag<
	    movetk::outlierdetection::zheng_outlier_detector_tag>;


	typename Fixture::Trajectory trajectory{{make_point({-5, 5}), 0},
	                                        {make_point({-6, 3}), 1},
	                                        {make_point({-5, 3}), 2},
	                                        {make_point({-3, 5}), 3},
	                                        {make_point({-2, 3}), 4},
	                                        {make_point({-1, 3}), 5},
	                                        {make_point({1, 5}), 6},
	                                        {make_point({2, 2}), 7},
	                                        {make_point({4, 5}), 8},
	                                        {make_point({5, 5}), 9},
	                                        {make_point({6, 5}), 10},
	                                        {make_point({7, 5}), 11}};

	OutlierDetector outlier_detector(0, 3);

	const auto expected_trajectory = Fixture::create_expected_subtrajectory(trajectory, {0});
	Fixture::verify_outlier_detector_output(outlier_detector, trajectory, expected_trajectory);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(OutlierDetectionTests,
                                      "output_sensitive_outlier_detector 1",
                                      "[output_sensitive_outlier_detector 1]") {
	using Fixture = OutlierDetectionTests<TestType>;
	auto make_point = Fixture::make_point;
	using OutlierDetector = typename Fixture::OutlierDetectionFactory::template outlierdetector_for_tag<
	    movetk::outlierdetection::output_sensitive_outlier_detector_tag>;
	typename Fixture::Trajectory trajectory{{make_point({-5, 5}), 0},
	                                        {make_point({-6, 3}), 1},
	                                        {make_point({-5, 3}), 2},
	                                        {make_point({-3, 5}), 3},
	                                        {make_point({-2, 3}), 4},
	                                        {make_point({-1, 3}), 5},
	                                        {make_point({1, 5}), 6},
	                                        {make_point({2, 2}), 7},
	                                        {make_point({4, 5}), 8},
	                                        {make_point({5, 5}), 9},
	                                        {make_point({6, 5}), 10},
	                                        {make_point({7, 5}), 11}};

	OutlierDetector outlier_detector(1.5);

	const auto expected_trajectory = Fixture::create_expected_subtrajectory(trajectory, {0, 3, 6, 8, 9, 10, 11});
	Fixture::verify_outlier_detector_output(outlier_detector, trajectory, expected_trajectory);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(OutlierDetectionTests,
                                      "output_sensitive_outlier_detector 2",
                                      "[output_sensitive_outlier_detector 2]") {
	using Fixture = OutlierDetectionTests<TestType>;
	auto make_point = Fixture::make_point;
	using OutlierDetector = typename Fixture::OutlierDetectionFactory::template outlierdetector_for_tag<
	    movetk::outlierdetection::output_sensitive_outlier_detector_tag>;
	const auto trajectory = Fixture::create_trajectory({{make_point({-5, 5}), 0},
	                                                    {make_point({-6, 3}), 1},
	                                                    {make_point({-5, 3}), 2},
	                                                    {make_point({-3, 5}), 3},
	                                                    {make_point({-2, 3}), 4},
	                                                    {make_point({-1, 3}), 5},
	                                                    {make_point({1, 5}), 6},
	                                                    {make_point({2, 2}), 7},
	                                                    {make_point({4, 5}), 8},
	                                                    {make_point({5, 5}), 9},
	                                                    {make_point({6, 5}), 10},
	                                                    {make_point({7, 5}), 11}});

	OutlierDetector outlier_detector(0);
	const auto expected_trajectory = Fixture::create_expected_subtrajectory(trajectory, {0, 3, 6, 8, 9, 10, 11});
	Fixture::verify_outlier_detector_output(outlier_detector, trajectory, expected_trajectory);
}
