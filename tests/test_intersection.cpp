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
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "movetk/metric/Norm.h"

#include "test_includes.h"

template<typename Backend>
struct IntersectionTests : public test_helpers::BaseTestFixture<Backend> {
    using Norm = movetk_support::FiniteNorm<MovetkGeometryKernel, 2>;

    movetk_core::MakeSphere<MovetkGeometryKernel> make_sphere;
    movetk_core::MakeSegment<MovetkGeometryKernel> make_segment;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;

    using SphereSegmentIntersectionTraits = movetk_core::IntersectionTraits<MovetkGeometryKernel, Norm,
        movetk_core::sphere_segment_intersection_tag>;

    using SphereSphereIntersectionTraits = movetk_core::IntersectionTraits<MovetkGeometryKernel, Norm,
        movetk_core::sphere_sphere_intersection_tag>;
};


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(IntersectionTests, "Check sphere segment intersection 1", "[sphere_segment_intersection_1]") {
    auto sphere = make_sphere({ 5, 3 }, 1.371);
    auto segment = make_segment({ 2, 2 }, { 4, 6 });
    using intersection_t = typename SphereSegmentIntersectionTraits::value_type;
    std::vector<intersection_t> intersections;
    using OutputIterator = movetk_core::movetk_back_insert_iterator<std::vector<intersection_t> >;
    movetk_core::ComputeIntersections<SphereSegmentIntersectionTraits> compute_intersections;
    compute_intersections(sphere, segment, OutputIterator(intersections));
    REQUIRE(intersections.size() == 1);
    REQUIRE(std::get<SphereSegmentIntersectionTraits::Attributes::SIGN_DISCRIMINANT>(intersections[0]) == -1);
    REQUIRE(std::get<SphereSegmentIntersectionTraits::Attributes::SQUARED_RATIO>(intersections[0]) == -1.0);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(IntersectionTests, "Check sphere segment intersection 2", "[sphere_segment_intersection_2]") {
    auto sphere = make_sphere({ 5, 3 }, 2.2364778);
    auto segment = make_segment({ 2, 2 }, { 4, 6 });
    using intersection_t = typename SphereSegmentIntersectionTraits::value_type;
    std::vector<intersection_t> intersections;
    std::vector<MovetkPoint> expected_intersection{ make_point({3, 4}) };
    movetk_core::ComputeIntersections<SphereSegmentIntersectionTraits> compute_intersections;
    compute_intersections(sphere, segment, movetk_core::make_back_inserter(intersections));
    REQUIRE(intersections.size() == 1);
    auto v = std::get<SphereSegmentIntersectionTraits::Attributes::POINT>(intersections[0]) - expected_intersection[0];
    REQUIRE(v * v < MOVETK_EPS);
}


MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(IntersectionTests, "Check sphere segment intersection 3", "[sphere_segment_intersection_3]") {
    auto sphere = make_sphere({ 5, 3 }, 2.509885);
    auto segment = make_segment({ 2, 2 }, { 4, 6 });
    using intersection_t = typename SphereSegmentIntersectionTraits::value_type;
    std::vector<intersection_t> intersections;
    std::vector<MovetkPoint> expected_intersection{ make_point({3.509808, 5.019617}),
                                                                         make_point({2.490192,2.980383}) };
    movetk_core::ComputeIntersections<SphereSegmentIntersectionTraits> compute_intersections;
    compute_intersections(sphere, segment, movetk_core::make_back_inserter(intersections));
    REQUIRE(intersections.size() == 2);
    auto v = std::get<SphereSegmentIntersectionTraits::Attributes::POINT>(intersections[0]) - expected_intersection[0];
    REQUIRE(v * v < MOVETK_EPS);
    v = std::get<SphereSegmentIntersectionTraits::Attributes::POINT>(intersections[1]) - expected_intersection[1];
    REQUIRE(v * v < MOVETK_EPS);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(IntersectionTests, "Check sphere sphere intersection 1", "[sphere_sphere_intersection_1]") {
    auto sphere1 = make_sphere({ 6, 6 }, 5);
    auto sphere2 = make_sphere({ 14, 10 }, 5);
    movetk_core::ComputeIntersections<SphereSphereIntersectionTraits> compute_intersections;
    auto sphere3 = compute_intersections(sphere1, sphere2);
    auto expected_center = make_point({ 10, 8 });
    NT expected_squared_radius = 5;
    auto eps = sphere3.center() - expected_center;
    REQUIRE((eps * eps) < MOVETK_EPS);
    REQUIRE(abs(expected_squared_radius - sphere3.squared_radius()) < MOVETK_EPS);
}

MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(IntersectionTests, "Check sphere sphere intersection 2", "[sphere_sphere_intersection_2]") {
    auto sphere1 = make_sphere({ 6, 6 }, 2.5);
    auto sphere2 = make_sphere({ 14, 10 }, 5);
    movetk_core::ComputeIntersections<SphereSphereIntersectionTraits> compute_intersections;
    auto sphere3 = compute_intersections(sphere1, sphere2);
    auto expected_center = make_point({ 0, 0 });
    NT expected_squared_radius = 0;
    auto eps = sphere3.center() - expected_center;
    std::cout << "Input Sphere 1: {" << sphere1 << " }\n";
    std::cout << "Input Sphere 2: {" << sphere2 << " }\n";
    std::cout << "Output Sphere: {" << sphere3 << " }\n";
    REQUIRE((eps * eps) < MOVETK_EPS);
    REQUIRE(abs(expected_squared_radius - sphere3.squared_radius()) < MOVETK_EPS);
}

