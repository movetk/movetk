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
// Created by Mitra, Aniket on 2019-04-03.
//

#include <array>
#include <catch2/catch.hpp>

#include "helpers/CustomCatchTemplate.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/Norm.h"
#include "movetk/simplification/ChanChin.h"
#include "movetk/simplification/ImaiIri.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"

template <typename Backend>
struct ImaiIriTests : public test_helpers::BaseTestFixture<Backend> {
	using MovetkGeometryKernel = typename test_helpers::BaseTestFixture<Backend>::MovetkGeometryKernel;
	using MovetkPoint = typename test_helpers::BaseTestFixture<Backend>::MovetkPoint;

	using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	using PolyLine = std::vector<MovetkPoint>;
	using Wedge = movetk::geom::Wedge<MovetkGeometryKernel, Norm>;
	using ChanChin = movetk::simplification::ChanChin<MovetkGeometryKernel, Wedge>;
	using ImaiIri = movetk::simplification::ImaiIri<MovetkGeometryKernel, ChanChin>;
};


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(ImaiIriTests, "Check Imai-Iri Simplification 1", "[imai_iri_1]") {
	using Fixture = ImaiIriTests<TestType>;
	auto make_point = Fixture::make_point;
	typename Fixture::PolyLine polyline({make_point({1, -6}),
	                                     make_point({4, -4}),
	                                     make_point({5, -2}),
	                                     make_point({6, -5}),
	                                     make_point({7, -2}),
	                                     make_point({8, -5}),
	                                     make_point({9, -2}),
	                                     make_point({10, -5}),
	                                     make_point({11, -2}),
	                                     make_point({13, -4})});
	typename Fixture::ImaiIri simplification(2);
	std::vector<decltype(polyline.begin())> result;
	simplification(std::begin(polyline), std::end(polyline), std::back_inserter(result));

	typename Fixture::PolyLine ExpectedPolyline({make_point({1, -6}), make_point({4, -4}), make_point({13, -4})});
	REQUIRE(result.size() == ExpectedPolyline.size());
	auto eit = std::begin(ExpectedPolyline);
	for (auto reference : result) {
		const auto v = *reference - *eit;
		REQUIRE((v * v) < MOVETK_EPS);
		eit++;
	}
}