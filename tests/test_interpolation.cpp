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

#include "movetk/algo/Interpolation.h"
#include "movetk/ds/TabularTrajectory.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/io/TuplePrinter.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "test_includes.h"


template <typename Backend>
struct InterpolationTests : public test_helpers::BaseTestFixture<Backend> {
	using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;

	// Used for kinematic interpolators
	struct ProbeTraits {
		enum ProbeColumns { SAMPLE_DATE, LAT, LON, HEADING, SPEED };
		using NT = typename MovetkGeometryKernel::NT;
		using ProbePoint = std::tuple<NT, NT, NT, NT, NT>;
	};

	using Projection = movetk::geo::LocalCoordinateReference<NT>;
	using InterpolationTraits = movetk::algo::InterpolationTraits<MovetkGeometryKernel, Projection, ProbeTraits, Norm>;
	using KinematicInterpolator = movetk::algo::Interpolator<movetk::algo::kinematic_interpolator_tag,
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

	movetk::algo::DummyOffsetInterpolator_2<ProbePoint, 0, 1> interpolator;
	ProbePoint p = interpolator(data[0], data[2], 50);

	REQUIRE(std::get<0>(p) == Approx(std::get<0>(data[1])).epsilon(0.0001));
	REQUIRE(std::get<1>(p) == Approx(std::get<1>(data[1])).epsilon(0.0001));
	REQUIRE(std::get<2>(p) == std::get<2>(data[1]));
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(InterpolationTests,
                                      "kinematic trajectory interpolation 1",
                                      "[kinematic_trajectory_interpolation_1]") {
	struct ProbeTraits {
		enum ProbeColumns { SAMPLE_DATE, LAT, LON, HEADING, SPEED };
		typedef MovetkGeometryKernel::NT NT;
		typedef std::tuple<NT, NT, NT, NT, NT> ProbePoint;
	};

	ProbeTraits::ProbePoint p1 = {0, 40.84812546, -73.84451294, 0, 1};
	ProbeTraits::ProbePoint p2 = {1, 40.8481636, -73.84448242, 0, 1};

	std::vector<typename ProbeTraits::ProbePoint> data = {p1, p2};
	Norm norm;

	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;

	KinematicInterpolator interpolator(40.84812546, -73.84451294);
	std::vector<typename MovetkGeometryKernel::NT> ts{0, 0.25, 0.5, 0.75, 1};
	std::vector<typename ProbeTraits::ProbePoint> interpolated_pts;

	movetk::utils::movetk_back_insert_iterator result(interpolated_pts);
	// result = p1;
	interpolator(p1, p2, std::cbegin(ts), std::cend(ts), result);
	// result = p2;

	// TODO: checks?!
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(InterpolationTests,
                                      "kinematic trajectory interpolation 2",
                                      "[kinematic_trajectory_interpolation_2]") {
	ProbeTraits::ProbePoint p1 = {1461862301, 40.84830093, -73.8443985, 20, 5.2778};
	ProbeTraits::ProbePoint p2 = {1461862305, 40.84841919, -73.84434509, 20, 3.05556};

	std::vector<typename ProbeTraits::ProbePoint> data = {p1, p2};
	Norm norm;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	KinematicInterpolator interpolator(40.84812546, -73.84451294);
	std::vector<std::size_t> ts{1461862301, 1461862302, 1461862303, 1461862304, 1461862305};
	std::vector<typename ProbeTraits::ProbePoint> interpolated_pts;

	movetk::utils::movetk_back_insert_iterator result(interpolated_pts);
	// result = p1;
	interpolator(p1, p2, std::cbegin(ts), std::cend(ts), result);
	// result = p2;
	auto pit = std::begin(interpolated_pts);
	/*std::cout << "SAMPLE_DATE, LAT, LON, HEADING, SPEED\n";
	while (pit != std::end(interpolated_pts)) {
	    std::cout << std::get<ProbeTraits::ProbeColumns::SAMPLE_DATE>(*pit) << ",";
	    std::cout << std::get<ProbeTraits::ProbeColumns::LAT>(*pit) << ",";
	    std::cout << std::get<ProbeTraits::ProbeColumns::LON>(*pit) << ",";
	    std::cout << std::get<ProbeTraits::ProbeColumns::HEADING>(*pit) << ",";
	    std::cout << std::get<ProbeTraits::ProbeColumns::SPEED>(*pit) << "\n";
	    pit++;
	}*/
	// TODO: checks ?!
}
