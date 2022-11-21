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
// Created by Mitra, Aniket on 2019-06-04.
//

#include <array>
#include <catch2/catch.hpp>

#include "helpers/CustomCatchTemplate.h"
#include "movetk/Similarity.h"
#include "movetk/Simplification.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"

MOVETK_TEMPLATE_LIST_TEST_CASE("Check Longest Common Subsequence 1", "[longest_common_sense_1]") {
	using MovetkGeometryKernel = typename TestType::MovetkGeometryKernel;
	using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	typedef std::vector<typename MovetkGeometryKernel::MovetkPoint> PolyLine;
	PolyLine polyline1({make_point({-5, -4}),
	                    make_point({-4.79, -1.62}),
	                    make_point({-2, -1}),
	                    make_point({-0.87, -4.56}),
	                    make_point({2.03, -0.74}),
	                    make_point({3.71, 1.28})});
	PolyLine polyline2({make_point({-4, -3}),
	                    make_point({-4.37, -1.88}),
	                    make_point({-1.61, -1.02}),
	                    make_point({2.57, -0.56}),
	                    make_point({-4.53, -3.94}),
	                    make_point({-1, -5})

	});

	std::vector<std::pair<typename MovetkGeometryKernel::MovetkPoint, typename MovetkGeometryKernel::MovetkPoint>>
	    ExpectedLCSS{std::make_pair(make_point({-4.79, -1.62}), make_point({-4.37, -1.88})),
	                 std::make_pair(make_point({-2, -1}), make_point({-1.61, -1.02})),
	                 std::make_pair(make_point({2.03, -0.74}), make_point({2.57, -0.56}))};
	std::vector<std::pair<typename PolyLine::const_iterator, typename PolyLine::const_iterator>> output;
	movetk::similarity::LongestCommonSubSequence<MovetkGeometryKernel, Norm> lcs(2, 7);
	std::size_t lcs_length = lcs(std::cbegin(polyline1),
	                             std::cend(polyline1),
	                             std::cbegin(polyline2),
	                             std::cend(polyline2),
	                             std::back_inserter(output));
	REQUIRE(lcs_length == ExpectedLCSS.size());

	auto eit = std::cbegin(ExpectedLCSS);
	for (auto reference : output) {
		const auto v1 = *reference.first - eit->first;
		REQUIRE((v1 * v1) < MOVETK_EPS);
		const auto v2 = *reference.second - eit->second;
		REQUIRE((v2 * v2) < MOVETK_EPS);
		eit++;
	}
}


MOVETK_TEMPLATE_LIST_TEST_CASE("Check Longest Common Subsequence 2", "[longest_common_sense_2]") {
	using MovetkGeometryKernel = typename TestType::MovetkGeometryKernel;
	using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	typedef std::vector<typename MovetkGeometryKernel::MovetkPoint> PolyLine;
	PolyLine polyline1(
	    {make_point({7, 10.5}), make_point({11, 10.5}), make_point({11, 7}), make_point({7, 7}), make_point({7, 10})});
	PolyLine polyline2({make_point({7.084, 10.182}),
	                    make_point({10.806, 10.189}),
	                    make_point({10.799, 7.312}),
	                    make_point({7.197, 7.312}),
	                    make_point({7.203, 9.813}),
	                    make_point({10.502, 9.813}),
	                    make_point({10.5, 7.5}),
	                    make_point({7.498, 7.5})

	});
	std::vector<std::pair<typename MovetkGeometryKernel::MovetkPoint, typename MovetkGeometryKernel::MovetkPoint>>
	    ExpectedLCSS{std::make_pair(make_point({7, 10.5}), make_point({7.084, 10.182})),
	                 std::make_pair(make_point({11, 10.5}), make_point({10.806, 10.189})),
	                 std::make_pair(make_point({11, 7}), make_point({10.799, 7.312})),
	                 std::make_pair(make_point({7, 7}), make_point({7.197, 7.312})),
	                 std::make_pair(make_point({7, 10}), make_point({7.203, 9.813}))};
	std::vector<std::pair<typename PolyLine::const_iterator, typename PolyLine::const_iterator>> output;
	movetk::similarity::LongestCommonSubSequence<MovetkGeometryKernel, Norm> lcs(2, 3);
	std::size_t lcs_length = lcs(std::cbegin(polyline1),
	                             std::cend(polyline1),
	                             std::cbegin(polyline2),
	                             std::cend(polyline2),
	                             std::back_inserter(output));
	REQUIRE(lcs_length == ExpectedLCSS.size());

	auto eit = std::cbegin(ExpectedLCSS);
	for (auto reference : output) {
		const auto v1 = *reference.first - eit->first;
		REQUIRE((v1 * v1) < MOVETK_EPS);
		const auto v2 = *reference.second - eit->second;
		REQUIRE((v2 * v2) < MOVETK_EPS);
		eit++;
	}
}


MOVETK_TEMPLATE_LIST_TEST_CASE("Check Longest Common Subsequence 3", "[longest_common_sense_3]") {
	using MovetkGeometryKernel = typename TestType::MovetkGeometryKernel;
	using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	typedef std::vector<typename MovetkGeometryKernel::MovetkPoint> PolyLine;
	PolyLine polyline1({make_point({-5, -4}),
	                    make_point({-4.79, -1.62}),
	                    make_point({-2, -1}),
	                    make_point({-0.87, -4.56}),
	                    make_point({2.03, -0.74}),
	                    make_point({3.71, 1.28})});
	PolyLine polyline2({make_point({-4, -3}),
	                    make_point({-4.37, -1.88}),
	                    make_point({-1.61, -1.02}),
	                    make_point({2.57, -0.56}),
	                    make_point({-4.53, -3.94}),
	                    make_point({-1, -5})

	});
	std::vector<std::pair<typename PolyLine::const_iterator, typename PolyLine::const_iterator>> output;
	movetk::similarity::LongestCommonSubSequence<MovetkGeometryKernel, Norm> lcs(2, 0);
	std::size_t lcs_length = lcs(std::cbegin(polyline1),
	                             std::cend(polyline1),
	                             std::cbegin(polyline2),
	                             std::cend(polyline2),
	                             std::back_inserter(output));
	/*static_assert(movetk::utils::InputIteratorWithType<decltype(std::cbegin(polyline1)),
	                                                   typename MovetkGeometryKernel::MovetkPoint>);*/
	REQUIRE(lcs_length == 0);
	REQUIRE(output.size() == 0);
}