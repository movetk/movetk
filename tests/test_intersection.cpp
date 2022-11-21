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
// Created by Mitra, Aniket on 2019-09-10.
//


#include <array>
#include <catch2/catch.hpp>

#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "helpers/CustomCatchTemplate.h"

template <typename Backend>
struct IntersectionTests : public test_helpers::GeometryConstructors<Backend> {
	using MovetkGeometryKernel = typename Backend::MovetkGeometryKernel;
	using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;

	using SphereSegmentIntersectionTraits =
	    movetk::geom::IntersectionTraits<MovetkGeometryKernel, Norm, movetk::geom::sphere_segment_intersection_tag>;

	using SphereSphereIntersectionTraits =
	    movetk::geom::IntersectionTraits<MovetkGeometryKernel, Norm, movetk::geom::sphere_sphere_intersection_tag>;
};


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(IntersectionTests,
                                      "Check sphere-segment intersections",
                                      "[sphere_segment_intersection]") {
	using Fixture = IntersectionTests<TestType>;
	using BaseTest = test_helpers::BaseTestFixture<TestType>;
	using IntersectionTraits = typename Fixture::SphereSegmentIntersectionTraits;
	using MovetkPoint = typename TestType::MovetkGeometryKernel::MovetkPoint;
	auto make_sphere = Fixture::make_sphere;
	auto make_point = Fixture::make_point;
	auto make_segment = Fixture::make_segment;
	auto spheres = BaseTest::create_vector_of(make_sphere({5, 3}, 1.371),
	                                          make_sphere({5, 3}, 2.2364778),
	                                          make_sphere({5, 3}, 2.509885));
	auto segments = BaseTest::create_vector_of(make_segment({2, 2}, {4, 6}),
	                                           make_segment({2, 2}, {4, 6}),
	                                           make_segment({2, 2}, {4, 6}));

	// Expectations
	std::vector<std::optional<int>> discriminant_signs{-1, {}, {}};
	std::vector<size_t> expected_intersections{1, 1, 2};
	std::vector<std::optional<typename TestType::NT>> squared_ratios{-1.0, {}, {}};
	std::vector<std::vector<MovetkPoint>> intersection_points{
	    {},
	    {make_point({3, 4})},
	    {make_point({3.509808, 5.019617}), make_point({2.490192, 2.980383})}};

	for (std::size_t i = 0; i < spheres.size(); ++i) {
		SECTION(std::string{"test"} + std::to_string(i)) {
			using intersection_t = typename IntersectionTraits::value_type;
			std::vector<intersection_t> intersections;
			movetk::geom::ComputeIntersections<IntersectionTraits> compute_intersections;
			compute_intersections(spheres[i], segments[i], std::back_inserter(intersections));
			REQUIRE(intersections.size() == expected_intersections[i]);
			if (discriminant_signs[i].has_value()) {
				REQUIRE(std::get<IntersectionTraits::Attributes::SIGN_DISCRIMINANT>(intersections[0]) ==
				        *discriminant_signs[i]);
			}
			if (squared_ratios[i].has_value()) {
				REQUIRE(std::get<IntersectionTraits::Attributes::SQUARED_RATIO>(intersections[0]) == *squared_ratios[i]);
			}
			if (expected_intersections[i] > 0) {
				for (std::size_t j = 0; j < intersection_points[i].size(); ++j) {
					const auto v = std::get<IntersectionTraits::Attributes::POINT>(intersections[j]) - intersection_points[i][j];
					REQUIRE(v * v < MOVETK_EPS);
				}
			}
		}
	}
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(IntersectionTests,
                                      "Check sphere sphere intersection 1",
                                      "[sphere_sphere_intersection_1]") {
	using Fixture = IntersectionTests<TestType>;
	using IntersectionTraits = typename IntersectionTests<TestType>::SphereSphereIntersectionTraits;
	using MovetkSphere = typename TestType::MovetkGeometryKernel::MovetkSphere;
	using MovetkPoint = typename TestType::MovetkGeometryKernel::MovetkPoint;
	using NT = typename TestType::MovetkGeometryKernel::NT;
	auto make_sphere = Fixture::make_sphere;
	auto make_point = Fixture::make_point;
	std::vector<MovetkSphere> first_spheres{make_sphere({6, 6}, 5), make_sphere({6, 6}, 2.5)};
	std::vector<MovetkSphere> second_spheres{make_sphere({14, 10}, 5), make_sphere({14, 10}, 5)};
	std::vector<MovetkPoint> expected_centers{make_point({10, 8}), make_point({0, 0})};
	std::vector<NT> expected_radii{5, 0};

	movetk::geom::ComputeIntersections<IntersectionTraits> compute_intersections;
	for (std::size_t i = 0; i < first_spheres.size(); ++i) {
		SECTION(std::string{"test"} + std::to_string(i)) {
			auto sphere3 = compute_intersections(first_spheres[i], second_spheres[i]);
			auto eps = sphere3.center() - expected_centers[i];
			REQUIRE((eps * eps) < MOVETK_EPS);
			REQUIRE(abs(expected_radii[i] - sphere3.squared_radius()) < MOVETK_EPS);
		}
	}
}
