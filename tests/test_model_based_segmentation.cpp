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

// Created by Wouter Jongeling (wouter.jongeling@gmail.com)
// Modified by Mitra, Aniket on 2019-06-27.
// Modified by Custers, Bram on 2022-03-12.
//


#include <array>
#include <iostream>
#include <vector>

#include "helpers/CustomCatchTemplate.h"
#include "movetk/Segmentation.h"
#include "movetk/ds/TabularTrajectory.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/Norm.h"
#include "movetk/segmentation/BrownianBridge.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"

// TODO: rework to use all backends.
//
//#if CGAL_BACKEND_ENABLED
////==============================
//// Define the Number Type
//// For the CGAL backend,
//// One can choose from the
//// following number types
// typedef long double NT;
//// typedef CGAL::Mpzf NT;
//// typedef CGAL::Gmpfr NT;
//// typedef CGAL::Gmpq NT;
////==============================
//
////==============================
//// Define the Dimensions
// const size_t dimensions = 2;
////==============================
//
////==============================
//// Define the Geometry Backend
// typedef movetk::geom::CGALTraits<NT, dimensions> CGAL_GeometryBackend;
//// Using the Geometry Backend define the Movetk Geometry Kernel
// typedef movetk::geom::MovetkGeometryKernel<typename CGAL_GeometryBackend::Wrapper_CGAL_Geometry>
// MovetkGeometryKernel;
////==============================
//#else
////==============================
//// Define the Number Type
//// For the Boost Backend
// typedef long double NT;
////==============================
//
////==============================
//// Define the Dimensions
//// It is possible to choose
//// a higher dimension
//// Note: Boost Geometry Adapter supports geometry in two
//// dimensions only
// const static size_t dimensions = 2;
////==============================
//
////==============================
//// Define the Geometry Backend
// using Boost_Geometry_Backend = movetk::geom::BoostGeometryTraits<long double, dimensions>;
//// Using the Geometry Backend define the Movetk Geometry Kernel
// using MovetkGeometryKernel =
//    movetk::geom::MovetkGeometryKernel<typename Boost_Geometry_Backend::Wrapper_Boost_Geometry>;
////==============================
//#endif
//
// using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;
//
//
MOVETK_TEMPLATE_LIST_TEST_CASE("brownian bridge model segmentation 1", "[test brownian bridge model segmentation 1]") {
	std::ios_base::sync_with_stdio(false);
	std::cout.setf(std::ios::fixed);

	struct ProbeTraits {
		enum ProbeColumns { LAT, LON, SAMPLE_DATE };
	};


	using ProbePoint = std::tuple<long double, long double, std::size_t>;

	ProbePoint p0(51.450165054, 5.48272431, 1);
	ProbePoint p1(51.44912450872306, 5.481725186681871, 2);
	ProbePoint p2(51.448560348, 5.481458307, 3);
	ProbePoint p3(51.447891704, 5.480986238, 4);
	ProbePoint p4(51.446982332, 5.480449796, 5);
	ProbePoint p5(51.446447399, 5.480342508, 6);
	ProbePoint p6(51.446407278, 5.480621458, 7);
	ProbePoint p7(51.446313664, 5.480814577, 8);
	ProbePoint p8(51.446300291, 5.48023522, 9);
	ProbePoint p9(51.446273544, 5.480492712, 10);
	ProbePoint p10(51.446113062, 5.480342508, 11);
	ProbePoint p11(51.445631614, 5.480342508, 12);
	ProbePoint p12(51.445243776, 5.480406881, 13);
	ProbePoint p13(51.44515016, 5.480642915, 14);
	ProbePoint p14(51.445230403, 5.480685831, 15);
	ProbePoint p15(51.445203655, 5.480278135, 16);
	ProbePoint p16(51.445110039, 5.480299593, 17);
	ProbePoint p17(51.444936179, 5.480492712, 18);
	ProbePoint p18(51.444849249, 5.480659009, 19);
	ProbePoint p19(51.444782379, 5.480524898, 20);
	ProbePoint p20(51.444688762, 5.480557085, 21);
	ProbePoint p21(51.444524931, 5.480347872, 22);
	ProbePoint p22(51.444391191, 5.479366184, 23);
	ProbePoint p23(51.444083588, 5.479291082, 24);
	ProbePoint p24(51.443846197, 5.479366184, 25);
	ProbePoint p25(51.44389635, 5.479484201, 26);
	ProbePoint p26(51.443882976, 5.479591489, 27);
	ProbePoint p27(51.44382948, 5.479527116, 28);
	ProbePoint p28(51.443799388, 5.479280353, 29);
	ProbePoint p29(51.443826136, 5.479366184, 30);
	ProbePoint p30(51.443779326, 5.479409099, 31);
	ProbePoint p31(51.443759265, 5.479457379, 32);
	ProbePoint p32(51.443645584, 5.479811431, 33);
	ProbePoint p33(51.443755922, 5.480476618, 34);
	ProbePoint p34(51.443618836, 5.480787755, 35);
	ProbePoint p35(51.444020061, 5.48124373, 36);
	ProbePoint p36(51.444100306, 5.481452942, 37);
	ProbePoint p37(51.443929786, 5.481501222, 38);
	ProbePoint p38(51.443765952, 5.481699706, 39);
	ProbePoint p39(51.444130398, 5.481790901, 40);
	ProbePoint p40(51.444508214, 5.481640697, 41);
	ProbePoint p41(51.444722197, 5.481334925, 42);
	ProbePoint p42(51.444745601, 5.481098891, 43);
	ProbePoint p43(51.444832531, 5.48119545, 44);
	ProbePoint p44(51.444982987, 5.481216908, 45);
	ProbePoint p45(51.444926149, 5.481066704, 46);
	ProbePoint p46(51.444819158, 5.480980874, 47);
	ProbePoint p47(51.444742258, 5.480980874, 48);
	ProbePoint p48(51.444875997, 5.481088162, 49);
	ProbePoint p49(51.445069917, 5.480986238, 50);
	ProbePoint p50(51.445380857, 5.480959416, 51);
	ProbePoint p51(51.445778724, 5.480937958, 52);
	ProbePoint p52(51.446126436, 5.480868221, 53);
	ProbePoint p53(51.447229736, 5.481157899, 54);
	ProbePoint p54(51.44821934, 5.481651426, 55);
	ProbePoint p55(51.449075197, 5.482423902, 56);

	std::vector<ProbePoint> data = {p0,  p1,  p2,  p3,  p4,  p5,  p6,  p7,  p8,  p9,  p10, p11, p12, p13,
	                                p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27,
	                                p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41,
	                                p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55};
	movetk::ds::TabularTrajectory<long double, long double, std::size_t> t{data};
	using MovetkGeometryKernel = typename TestType::MovetkGeometryKernel;

	using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;
	Norm norm;
	using ParameterTraits =
	    movetk::segmentation::brownian_bridge::ParameterTraits<MovetkGeometryKernel, decltype(t.begin())>;
	using BridgeIterator = typename std::vector<typename ParameterTraits::Parameters>::const_iterator;
	using Projection = movetk::geo::LocalCoordinateReference<typename MovetkGeometryKernel::NT>;
	std::vector<typename ParameterTraits::Parameters> bridges;

	using BBMM = movetk::segmentation::brownian_bridge::
	    Model<MovetkGeometryKernel, ProbeTraits, ParameterTraits, Norm, Projection>;

	BBMM bb(t.begin(), t.end(), std::back_inserter(bridges));

	std::vector<typename MovetkGeometryKernel::NT> selected_coeffs;

	movetk::segmentation::brownian_bridge::MLE<MovetkGeometryKernel, ParameterTraits, Norm, BridgeIterator, 1000> mle(
	    std::cbegin(bridges),
	    std::cend(bridges));
	std::cout << "MLE of the Trajectory: " << mle() << "\n";

	std::cout << "Bridges: "
	          << "\n";
	std::cout << "POINT,MU,SIGMA_SQUARED\n";
	auto bit = begin(bridges);
	while (bit != end(bridges)) {
		movetk::segmentation::brownian_bridge::MLE<MovetkGeometryKernel, ParameterTraits, Norm, BridgeIterator, 1000> mle(
		    bit,
		    bit + 1);
		std::get<ParameterTraits::ParameterColumns::SIGMA_SQUARED>(*bit) = mle();
		bit++;
	}


	for (auto bridge : bridges) {
		std::cout << std::get<ParameterTraits::ParameterColumns::POINT>(bridge) << ",";
		std::cout << std::get<ParameterTraits::ParameterColumns::MU>(bridge) << ",";
		std::cout << std::get<ParameterTraits::ParameterColumns::SIGMA_SQUARED>(bridge);
		std::cout << "\n";
	}


	std::cout << " Running coefficient selector to select 5 coefficients: \n";
	movetk::segmentation::brownian_bridge::ParameterSelector<MovetkGeometryKernel, ParameterTraits> selector(5);
	selector(std::cbegin(bridges), std::cend(bridges), std::back_inserter(selected_coeffs));

	for (auto coeff : selected_coeffs) {
		std::cout << " Selected Coefficient: " << coeff << "\n";
	}

	using LogLikelihood =
	    movetk::segmentation::brownian_bridge::LogLikelihood<MovetkGeometryKernel, ParameterTraits, Norm>;

	using ModelBasedSegmentation = movetk::segmentation::ModelBasedSegmentation<MovetkGeometryKernel, LogLikelihood>;

	std::cout << "Segmentation with high penalty factor p = 1000 \n";
	ModelBasedSegmentation segmentation(1000);

	std::vector<BridgeIterator> segments;

	segmentation(std::cbegin(bridges),
	             std::cend(bridges),
	             std::cbegin(selected_coeffs),
	             std::cend(selected_coeffs),
	             std::back_inserter(segments));


	REQUIRE(segments.size() == 1);
	std::reverse(std::begin(segments), std::end(segments));

	std::cout << "Splits based on bridges: \n";
	for (auto seg : segments) {
		std::cout << std::get<ParameterTraits::ParameterColumns::POINT>(*seg) << "\n";
	}

	auto make_segment = movetk::utils::make_segment_id_generator(std::begin(segments), std::end(segments));
	std::cout << "Segments: \n";
	bit = std::begin(bridges);
	std::size_t id = 0;
	while (bit != std::end(bridges)) {
		auto first = std::get<ParameterTraits::ParameterColumns::BEGIN>(*bit);
		auto beyond = std::get<ParameterTraits::ParameterColumns::END>(*bit);
		auto it = first;
		while (it != beyond) {
			std::cout << std::get<0>(*it) << "," << std::get<1>(*it) << "," << std::get<2>(*it) << "," << id << "\n";
			it++;
		}
		id = make_segment.getSegmentID(bit);
		bit++;
	}
	make_segment.reset();


	std::cout << "Segmentation with low penalty factor p = 0 \n";
	ModelBasedSegmentation segmentation1(0);
	segments.clear();
	segmentation1(std::cbegin(bridges),
	              std::cend(bridges),
	              std::cbegin(selected_coeffs),
	              std::cend(selected_coeffs),
	              std::back_inserter(segments));

	REQUIRE(segments.size() == 24);
	std::reverse(std::begin(segments), std::end(segments));

	std::cout << "Splits based on bridges: \n";
	for (auto seg : segments) {
		std::cout << std::get<ParameterTraits::ParameterColumns::POINT>(*seg) << "\n";
	}

	auto make_segment1 = movetk::utils::make_segment_id_generator(std::begin(segments), std::end(segments));
	std::cout << "Segments: \n";
	bit = std::begin(bridges);
	id = 0;
	while (bit != std::end(bridges)) {
		auto first = std::get<ParameterTraits::ParameterColumns::BEGIN>(*bit);
		auto beyond = std::get<ParameterTraits::ParameterColumns::END>(*bit);
		auto it = first;
		while (it != beyond) {
			std::cout << std::get<0>(*it) << "," << std::get<1>(*it) << "," << std::get<2>(*it) << "," << id << "\n";
			it++;
		}
		id = make_segment1.getSegmentID(bit);
		bit++;
	}
	make_segment1.reset();
}
