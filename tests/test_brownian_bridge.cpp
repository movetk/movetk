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
// Created by Wouter Jongeling (wouter.jongeling@gmail.com)
// Created by Mitra, Aniket on 2019-06-20.
//


#include <array>
#include <catch2/catch.hpp>
#include <vector>

#include "helpers/CustomCatchTemplate.h"
#include "movetk/ds/TabularTrajectory.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/Norm.h"
#include "movetk/segmentation/BrownianBridge.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"

template <typename Backend>
struct BrownianBridgeTests {
	using MovetkGeometryKernel = typename Backend::MovetkGeometryKernel;
	using NT = typename Backend::NT;
	using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;
	static constexpr std::size_t MAX_ITERATIONS = 1000;
	template <typename... TrajectoryArgs>
	struct TrajectoryTs {
		using Trajectory = movetk::ds::TabularTrajectory<TrajectoryArgs...>;
		using ParameterTraits =
		    movetk::segmentation::brownian_bridge::ParameterTraits<MovetkGeometryKernel,
		                                                           decltype(std::declval<Trajectory>().begin())>;
		using Parameters = typename ParameterTraits::Parameters;
		using BridgeIterator = typename std::vector<Parameters>::const_iterator;
		using ParameterColumns = typename ParameterTraits::ParameterColumns;
		using MLE = movetk::segmentation::brownian_bridge::
		    MLE<MovetkGeometryKernel, ParameterTraits, Norm, BridgeIterator, MAX_ITERATIONS>;
	};
	using EmptyTrajectoryTs = TrajectoryTs<>;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;

	// When we are using lat-lon:
	using Projection = movetk::geo::LocalCoordinateReference<NT>;
};

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(BrownianBridgeTests, "brownian bridge mle 1", "[test brownian bridge mle 1]") {
	using Fixture = BrownianBridgeTests<TestType>;
	typename Fixture::Norm norm;
	using Ts = typename Fixture::EmptyTrajectoryTs;
	auto make_point = Fixture::make_point;

	typename Ts::Trajectory t = {};
	std::vector<typename Ts::Parameters> bridges{{make_point({0, 1}), make_point({0, 0}), 0, t.begin(), t.begin()}};
	const auto v = std::get<Ts::ParameterColumns::POINT>(bridges[0]) - std::get<Ts::ParameterColumns::MU>(bridges[0]);
	const auto squared_length = norm(v);
	const auto initial_estimate = squared_length / 2;
	const auto upper_bound = squared_length;
	typename Ts::MLE mle(std::cbegin(bridges), std::cend(bridges), initial_estimate, MOVETK_EPS, upper_bound, MOVETK_EPS);
	std::cout << "Maximum Likelihood Estimate: " << mle() << "\n";
	REQUIRE(std::abs(mle() - 0.5) < MOVETK_EPS);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(BrownianBridgeTests, "brownian bridge mle 2", "[test brownian bridge mle 2]") {
	using Fixture = BrownianBridgeTests<TestType>;
	typename Fixture::Norm norm;
	using Ts = typename Fixture::EmptyTrajectoryTs;
	auto make_point = Fixture::make_point;

	typename Ts::Trajectory t = {};
	std::vector<typename Ts::Parameters> bridges{{make_point({10, 20}), make_point({20, 10}), 0, t.begin(), t.begin()}};
	auto v = std::get<Ts::ParameterColumns::POINT>(bridges[0]) - std::get<Ts::ParameterColumns::MU>(bridges[0]);
	const auto squared_length = norm(v);
	auto initial_estimate = squared_length / 2;
	auto upper_bound = squared_length;
	typename Ts::MLE mle(std::cbegin(bridges), std::cend(bridges), initial_estimate, MOVETK_EPS, upper_bound, MOVETK_EPS);
	std::cout << "Maximum Likelihood Estimate: " << mle() << "\n";
	REQUIRE(std::abs(mle() - 100) < MOVETK_EPS);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(BrownianBridgeTests, "brownian bridge mle 3", "[test brownian bridge mle 3]") {
	using Fixture = BrownianBridgeTests<TestType>;
	typename Fixture::Norm norm;
	using Ts = typename Fixture::EmptyTrajectoryTs;
	auto make_point = Fixture::make_point;

	typename Ts::Trajectory t = {};
	typename Fixture::NT sigma = 1.0;
	std::vector<typename Ts::Parameters> bridges{
	    {make_point({0, 1}), make_point({0, 0}), sigma, t.begin(), t.begin()},
	    {make_point({10, 20}), make_point({20, 10}), sigma, t.begin(), t.begin()}};
	auto v = std::get<Ts::ParameterColumns::POINT>(bridges[0]) - std::get<Ts::ParameterColumns::MU>(bridges[0]);
	auto squared_length = norm(v);
	auto upper_bound = squared_length;
	v = std::get<Ts::ParameterColumns::POINT>(bridges[1]) - std::get<Ts::ParameterColumns::MU>(bridges[1]);
	auto l = norm(v);
	squared_length += l;
	if (l > upper_bound) {
		upper_bound = l;
	}
	auto initial_estimate = squared_length / (2 * bridges.size());
	typename Ts::MLE mle(std::cbegin(bridges), std::cend(bridges), initial_estimate, MOVETK_EPS, upper_bound, MOVETK_EPS);
	std::cout << "Maximum Likelihood Estimate: " << mle() << "\n";
	REQUIRE(std::abs(mle() - 50.25) < MOVETK_EPS);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(BrownianBridgeTests,
                                      "brownian bridge model 1",
                                      "[test brownian bridge model 1]") {
	using NT = long double;
	using Fixture = BrownianBridgeTests<TestType>;
	struct ProbeTraits {
		enum ProbeColumns { LAT, LON, SAMPLE_DATE };
	};
	using ProbePoint = std::tuple<long double, long double, std::size_t>;

	ProbePoint p1 = {51.443780998, 5.47970146, 1001};
	ProbePoint p2 = {51.44394316, 5.479985774, 1002};
	ProbePoint p3 = {51.444081916, 5.480219126, 1003};

	std::vector<ProbePoint> data = {p1, p2, p3};
	movetk::ds::TabularTrajectory<long double, long double, std::size_t> t{data};

	using Ts = typename Fixture::template TrajectoryTs<long double, long double, std::size_t>;

	typename Fixture::Norm norm;
	std::vector<typename Ts::Parameters> bridges;

	typedef movetk::segmentation::brownian_bridge::Model<typename Fixture::MovetkGeometryKernel,
	                                             ProbeTraits,
	                                             typename Ts::ParameterTraits,
	                                             typename Fixture::Norm,
	                                             typename Fixture::Projection>
	    BBMM;

	BBMM bb(t.begin(), t.end(), std::back_inserter(bridges));

	NT upper_bound = 0;
	NT squared_length = 0;
	for (auto bridge : bridges) {
		auto v = std::get<Ts::ParameterColumns::POINT>(bridge) - std::get<Ts::ParameterColumns::MU>(bridge);
		auto l = norm(v);
		squared_length += l;
		if (l > upper_bound)
			upper_bound = l;
	}
	auto initial_estimate = squared_length / (2 * bridges.size());
	movetk::segmentation::brownian_bridge::MLE<typename Fixture::MovetkGeometryKernel,
	                                   typename Ts::ParameterTraits,
	                                   typename Fixture::Norm,
	                                   typename Ts::BridgeIterator,
	                                   Fixture::MAX_ITERATIONS>
	    mle(std::cbegin(bridges), std::cend(bridges), initial_estimate, MOVETK_EPS, upper_bound, MOVETK_EPS);
	REQUIRE(std::abs(mle() - 2.416917) < MOVETK_EPS);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(BrownianBridgeTests,
                                      "brownian bridge model 2",
                                      "[test brownian bridge model 2]") {
	using Fixture = BrownianBridgeTests<TestType>;
	struct ProbeTraits {
		enum ProbeColumns { LAT, LON, SAMPLE_DATE };
	};


	using ProbePoint = std::tuple<long double, long double, std::size_t>;

	ProbePoint p1 = {51.443780998, 5.47970146, 1001};
	ProbePoint p2 = {51.44394316, 5.479985774, 1002};
	ProbePoint p3 = {51.444081916, 5.480219126, 1003};
	ProbePoint p4 = {51.444402058, 5.480742157, 1004};
	ProbePoint p5 = {51.444943702, 5.481090844, 1005};
	ProbePoint p6 = {51.445498713, 5.481675566, 1006};

	std::vector<ProbePoint> data = {p1, p2, p3, p4, p5, p6};
	movetk::ds::TabularTrajectory<long double, long double, std::size_t> t{data};

	using Ts = typename Fixture::template TrajectoryTs<long double, long double, std::size_t>;

	typename Fixture::Norm norm;
	std::vector<typename Ts::Parameters> bridges;

	using BBMM = movetk::segmentation::brownian_bridge::Model<typename Fixture::MovetkGeometryKernel,
	                                                  ProbeTraits,
	                                                  typename Ts::ParameterTraits,
	                                                  typename Fixture::Norm,
	                                                  typename Fixture::Projection>;

	BBMM bb(t.begin(), t.end(), std::back_inserter(bridges));

	typename Fixture::NT upper_bound = 0;
	typename Fixture::NT squared_length = 0;
	for (auto bridge : bridges) {
		auto v = std::get<Ts::ParameterColumns::POINT>(bridge) - std::get<Ts::ParameterColumns::MU>(bridge);
		auto l = norm(v);
		squared_length += l;
		if (l > upper_bound)
			upper_bound = l;
	}

	auto initial_estimate = squared_length / (2 * bridges.size());
	movetk::segmentation::brownian_bridge::MLE<typename Fixture::MovetkGeometryKernel,
	                                   typename Ts::ParameterTraits,
	                                   typename Fixture::Norm,
	                                   typename Ts::BridgeIterator,
	                                   Fixture::MAX_ITERATIONS>
	    mle(std::cbegin(bridges), std::cend(bridges), initial_estimate, MOVETK_EPS, upper_bound, MOVETK_EPS);
	REQUIRE(std::abs(mle() - 32.2321283) < MOVETK_EPS);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(BrownianBridgeTests,
                                      "brownian bridge parameter selection 1",
                                      "[test brownian bridge parameter selection 1]") {
	using Fixture = BrownianBridgeTests<TestType>;
	struct ProbeTraits {
		enum ProbeColumns { LAT, LON, SAMPLE_DATE };
	};

	using ProbePoint = std::tuple<long double, long double, std::size_t>;

	std::vector<ProbePoint> data = {ProbePoint(51.4443930124353, 5.47950342106722, 1550866709),
	                                ProbePoint(51.444393336164, 5.47950008089042, 1550866719),
	                                ProbePoint(51.4443942186351, 5.47950781233749, 1550866729),
	                                ProbePoint(51.4443943960143, 5.47950784270154, 1550866739),
	                                ProbePoint(51.4443944277322, 5.47947849377084, 1550866749),
	                                ProbePoint(51.4443944586718, 5.47950714413615, 1550866759),
	                                ProbePoint(51.4443944790561, 5.47948123937032, 1550866769),
	                                ProbePoint(51.4443945042884, 5.479478464752472, 1550866779),
	                                ProbePoint(51.4443947175562, 5.47950683174872, 1550866789),
	                                ProbePoint(51.4443949234094, 5.47948138337028, 1550866799),
	                                ProbePoint(51.4443949362765, 5.47947914874741, 1550866809),
	                                ProbePoint(51.4443949503156, 5.47950642352529, 1550866819),
	                                ProbePoint(51.4443951830696, 5.47950328314741, 1550866829),
	                                ProbePoint(51.4443954255127, 5.47948166958525, 1550866839),
	                                ProbePoint(51.4443954402577, 5.47948047240938, 1550866849),
	                                ProbePoint(51.4443955694185, 5.47950080933134, 1550866859),
	                                ProbePoint(51.4443956476577, 5.4794815471467, 1550866869),
	                                ProbePoint(51.4443957506309, 5.47948009708671, 1550866879)};
	movetk::ds::TabularTrajectory<long double, long double, std::size_t> t{data};
	using Ts = typename Fixture::template TrajectoryTs<long double, long double, std::size_t>;
	typename Fixture::Norm norm;
	std::vector<typename Ts::Parameters> bridges;

	using BBMM = movetk::segmentation::brownian_bridge::Model<typename Fixture::MovetkGeometryKernel,
	                                                  ProbeTraits,
	                                                  typename Ts::ParameterTraits,
	                                                  typename Fixture::Norm,
	                                                  typename Fixture::Projection>;

	BBMM bb(t.begin(), t.end(), std::back_inserter(bridges));

	std::vector<typename Fixture::NT> selected_coeffs;

	auto bit = begin(bridges);
	while (bit != end(bridges)) {
		movetk::segmentation::brownian_bridge::MLE<typename Fixture::MovetkGeometryKernel,
		                                   typename Ts::ParameterTraits,
		                                   typename Fixture::Norm,
		                                   typename Ts::BridgeIterator,
		                                   Fixture::MAX_ITERATIONS>
		    mle(bit, bit + 1);
		std::get<Ts::ParameterColumns::SIGMA_SQUARED>(*bit) = mle();
		bit++;
	}

	movetk::segmentation::brownian_bridge::ParameterSelector<typename Fixture::MovetkGeometryKernel, typename Ts::ParameterTraits>
	    selector(4);
	selector(std::begin(bridges), std::end(bridges), std::back_inserter(selected_coeffs));

	for (auto bridge : bridges) {
		std::cout << std::get<Ts::ParameterColumns::SIGMA_SQUARED>(bridge);
		std::cout << "\n";
	}

	for (auto coeff : selected_coeffs) {
		std::cout << "Coefficient: " << coeff << "\n";
	}
}
