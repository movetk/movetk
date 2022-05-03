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

#include <array>
#include <catch2/catch.hpp>
#include <iostream>
#include <vector>

#include "helpers/CustomCatchTemplate.h"
#include "movetk/Interpolation.h"
#include "movetk/ds/TabularTrajectory.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/io/TuplePrinter.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"


template <typename Backend>
struct InterpolationTests {
	using MovetkGeometryKernel = test_helpers::MovetkKernelFromBackend<Backend>;
	using NT = test_helpers::NTFromBackend<Backend>;
	using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;

	// Used for kinematic interpolators
	struct ProbeTraits {
		enum ProbeColumns { SAMPLE_DATE, LAT, LON, HEADING, SPEED };
		using ProbePoint = std::tuple<NT, NT, NT, NT, NT>;
	};

	using Projection = movetk::geo::LocalCoordinateReference<NT>;
	using InterpolationTraits =
	    movetk::interpolation::InterpolationTraits<MovetkGeometryKernel, Projection, ProbeTraits, Norm>;
	using KinematicInterpolator = movetk::interpolation::Interpolator<movetk::interpolation::kinematic_interpolator_tag,
	                                                                  InterpolationTraits,
	                                                                  ProbeTraits::ProbeColumns::LAT,
	                                                                  ProbeTraits::ProbeColumns::LON,
	                                                                  ProbeTraits::ProbeColumns::SAMPLE_DATE,
	                                                                  ProbeTraits::ProbeColumns::SPEED,
	                                                                  ProbeTraits::ProbeColumns::HEADING>;
};


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(InterpolationTests, "trajectory interpolation", "[trajectory_interpolation]") {
	// Create three points with know offsets
	double lat0 = 33.439361, lon0 = -112.084793;  // origin
	using ProbePoint = std::tuple<float, float, std::string>;
	std::vector<ProbePoint> data{ProbePoint{lat0, lon0, "0"}};

	std::vector<std::pair<double, double>> true_xys{{30, 40}, {30, 40}};
	std::vector<double> true_offsets{0, 50, 100};  // length of each segment: {50, 50};

	int i = 1;
	for (auto& [x, y] : true_xys) {
		double lat1, lon1;
		movetk::geo::destination_exact(lat0, lon0, x, y, lat1, lon1);
		data.push_back(ProbePoint{lat1, lon1, std::to_string(i)});
		i++;
	}
	std::get<2>(data[1]) = "2";  // other fields of interpolated are taken from end point

	movetk::interpolation::DummyOffsetInterpolator_2<ProbePoint, 0, 1> interpolator;
	ProbePoint p = interpolator(data[0], data[2], 50);

	REQUIRE(std::get<0>(p) == Approx(std::get<0>(data[1])).epsilon(0.0001));
	REQUIRE(std::get<1>(p) == Approx(std::get<1>(data[1])).epsilon(0.0001));
	REQUIRE(std::get<2>(p) == std::get<2>(data[1]));
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(InterpolationTests,
                                      "kinematic trajectory interpolations",
                                      "[kinematic_trajectory_interpolation]") {
	using FixtureType = InterpolationTests<TestType>;
	using ProbeTraits = typename InterpolationTests<TestType>::ProbeTraits;
	using NT = typename InterpolationTests<TestType>::NT;
	using ProbePoint = typename ProbeTraits::ProbePoint;
	std::vector<std::pair<ProbePoint, ProbePoint>> tests{
	    std::make_pair(ProbePoint{1461862301, 40.84830093, -73.8443985, 20, 5.2778},
	                   ProbePoint{1461862305, 40.84841919, -73.84434509, 20, 3.05556}),
	    std::make_pair(ProbePoint{0, 40.84812546, -73.84451294, 0, 1}, ProbePoint{1, 40.8481636, -73.84448242, 0, 1})};
	std::vector<std::vector<NT>> timestamps{{1461862301, 1461862302, 1461862303, 1461862304, 1461862305},
	                                        {0, 0.25, 0.5, 0.75, 1}};

	std::vector<std::pair<NT, NT>> interpolator_refs{std::make_pair<NT>(40.84812546, -73.84451294),
	                                                 std::make_pair<NT>(40.84812546, -73.84451294)};

	typename FixtureType::Norm norm;
	for (std::size_t i = 0; i < tests.size(); ++i) {
		SECTION(std::string{"test"} + std::to_string(i)) {
			typename FixtureType::KinematicInterpolator interpolator(interpolator_refs[i].first, interpolator_refs[i].second);
			std::vector<typename ProbeTraits::ProbePoint> interpolated_pts;

			interpolator(tests[i].first,
			             tests[i].second,
			             std::cbegin(timestamps[i]),
			             std::cend(timestamps[i]),
			             std::back_inserter(interpolated_pts));
			auto pit = std::begin(interpolated_pts);
		}
	}
}
