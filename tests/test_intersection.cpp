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
#include "catch2/catch.hpp"
#if CGAL_BACKEND_ENABLED
#include "movetk/geom/CGALTraits.h"
#else

#include "movetk/geom/BoostGeometryTraits.h"
#endif

#include "movetk/geom/GeometryInterface.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "movetk/metric/Norm.h"

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

TEST_CASE("Check sphere segment intersection 1", "[sphere_segment_intersection_1]") {
    typedef typename MovetkGeometryKernel::NT NT;
    typedef typename MovetkGeometryKernel::MovetkPoint MovetkPoint;
    typedef movetk_core::IntersectionTraits<MovetkGeometryKernel, Norm,
            movetk_core::sphere_segment_intersection_tag> IntersectionTraits;
    movetk_core::MakeSphere<MovetkGeometryKernel> make_sphere;
    MovetkGeometryKernel::MovetkSphere sphere = make_sphere({5, 3}, 1.371);
    movetk_core::MakeSegment<MovetkGeometryKernel> make_segment;
    MovetkGeometryKernel::MovetkSegment segment = make_segment({2, 2}, {4, 6});
    std::vector<IntersectionTraits::value_type> intersections;
    typedef movetk_core::movetk_back_insert_iterator<std::vector<IntersectionTraits::value_type> > OutputIterator;
    movetk_core::ComputeIntersections<IntersectionTraits> compute_intersections;
    compute_intersections(sphere, segment,OutputIterator(intersections));
    REQUIRE (intersections.size() == 1);
    REQUIRE ( std::get<IntersectionTraits::Attributes::SIGN_DISCRIMINANT>(intersections[0]) == -1);
    REQUIRE ( std::get<IntersectionTraits::Attributes::SQUARED_RATIO>(intersections[0]) == -1.0);
}


TEST_CASE("Check sphere segment intersection 2","[sphere_segment_intersection_2]"){
    typedef typename MovetkGeometryKernel::NT NT;
    typedef typename MovetkGeometryKernel::MovetkPoint MovetkPoint;
    typedef movetk_core::IntersectionTraits<MovetkGeometryKernel, Norm,
            movetk_core::sphere_segment_intersection_tag> IntersectionTraits;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    movetk_core::MakeSphere<MovetkGeometryKernel> make_sphere;
    MovetkGeometryKernel::MovetkSphere sphere = make_sphere({5, 3}, 2.2364778);
    movetk_core::MakeSegment<MovetkGeometryKernel> make_segment;
    MovetkGeometryKernel::MovetkSegment segment = make_segment({2, 2}, {4, 6});
    std::vector<IntersectionTraits::value_type> intersections;
    std::vector<MovetkGeometryKernel::MovetkPoint> expected_intersection{make_point({3, 4})};
    typedef movetk_core::movetk_back_insert_iterator<std::vector<IntersectionTraits::value_type> > OutputIterator;
    movetk_core::ComputeIntersections<IntersectionTraits> compute_intersections;
    compute_intersections(sphere, segment, OutputIterator(intersections) );
    REQUIRE (intersections.size() == 1);
    std::cout<<std::get<IntersectionTraits::Attributes::POINT>(intersections[0]);
    std::cout<<"\n";
    MovetkGeometryKernel::MovetkVector v =
            std::get<IntersectionTraits::Attributes::POINT>(intersections[0]) - expected_intersection[0];
    REQUIRE (v * v < MOVETK_EPS);
}


TEST_CASE("Check sphere segment intersection 3","[sphere_segment_intersection_3]"){
    typedef typename MovetkGeometryKernel::NT NT;
    typedef typename MovetkGeometryKernel::MovetkPoint MovetkPoint;
    typedef movetk_core::IntersectionTraits<MovetkGeometryKernel, Norm,
            movetk_core::sphere_segment_intersection_tag> IntersectionTraits;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    movetk_core::MakeSphere<MovetkGeometryKernel> make_sphere;
    MovetkGeometryKernel::MovetkSphere sphere = make_sphere({5, 3}, 2.509885);
    movetk_core::MakeSegment<MovetkGeometryKernel> make_segment;
    MovetkGeometryKernel::MovetkSegment segment = make_segment({2, 2}, {4, 6});
    std::vector<IntersectionTraits::value_type> intersections;
    std::vector<MovetkGeometryKernel::MovetkPoint> expected_intersection{make_point({3.509808, 5.019617}),
                                                                         make_point({2.490192,2.980383})};
    typedef movetk_core::movetk_back_insert_iterator<std::vector<IntersectionTraits::value_type> > OutputIterator;
    movetk_core::ComputeIntersections<IntersectionTraits> compute_intersections;
    compute_intersections(sphere, segment, OutputIterator(intersections) );
    REQUIRE (intersections.size() == 2);
    std::cout<<std::get<IntersectionTraits::Attributes::POINT>(intersections[0]);
    std::cout<<"\n";
    MovetkGeometryKernel::MovetkVector v =
            std::get<IntersectionTraits::Attributes::POINT>(intersections[0]) - expected_intersection[0];
    REQUIRE (v * v < MOVETK_EPS);
    std::cout<<std::get<IntersectionTraits::Attributes::POINT>(intersections[1]);
    std::cout<<"\n";
    v = std::get<IntersectionTraits::Attributes::POINT>(intersections[1]) - expected_intersection[1];
    REQUIRE (v * v < MOVETK_EPS);
}

TEST_CASE("Check sphere sphere intersection 1","[sphere_sphere_intersection_1]"){
    typedef typename MovetkGeometryKernel::NT NT;
    typedef typename MovetkGeometryKernel::MovetkPoint MovetkPoint;
    typedef movetk_core::IntersectionTraits<MovetkGeometryKernel, Norm,
            movetk_core::sphere_sphere_intersection_tag> IntersectionTraits;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    movetk_core::MakeSphere<MovetkGeometryKernel> make_sphere;
    MovetkGeometryKernel::MovetkSphere sphere1 = make_sphere({6, 6}, 5);
    MovetkGeometryKernel::MovetkSphere sphere2 = make_sphere({14, 10}, 5);
    movetk_core::ComputeIntersections<IntersectionTraits> compute_intersections;
    MovetkGeometryKernel::MovetkSphere sphere3 = compute_intersections(sphere1, sphere2);
    MovetkGeometryKernel::MovetkPoint expected_center = make_point({10, 8});
    MovetkGeometryKernel::NT expected_squared_radius = 5;
    MovetkGeometryKernel::MovetkVector eps = sphere3.center() - expected_center;
    std::cout<<"Input Sphere 1: {"<<sphere1<<" }\n";
    std::cout<<"Input Sphere 2: {"<<sphere2<<" }\n";
    std::cout<<"Output Sphere: {"<<sphere3<<" }\n";
    REQUIRE( (eps * eps) < MOVETK_EPS );
    REQUIRE( abs(expected_squared_radius - sphere3.squared_radius() )  < MOVETK_EPS);
}

TEST_CASE("Check sphere sphere intersection 2","[sphere_sphere_intersection_2]"){
    typedef typename MovetkGeometryKernel::NT NT;
    typedef typename MovetkGeometryKernel::MovetkPoint MovetkPoint;
    typedef movetk_core::IntersectionTraits<MovetkGeometryKernel, Norm,
            movetk_core::sphere_sphere_intersection_tag> IntersectionTraits;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    movetk_core::MakeSphere<MovetkGeometryKernel> make_sphere;
    MovetkGeometryKernel::MovetkSphere sphere1 = make_sphere({6, 6}, 2.5);
    MovetkGeometryKernel::MovetkSphere sphere2 = make_sphere({14, 10}, 5);
    movetk_core::ComputeIntersections<IntersectionTraits> compute_intersections;
    MovetkGeometryKernel::MovetkSphere sphere3 = compute_intersections(sphere1, sphere2);
    MovetkGeometryKernel::MovetkPoint expected_center = make_point({0, 0});
    MovetkGeometryKernel::NT expected_squared_radius = 0;
    MovetkGeometryKernel::MovetkVector eps = sphere3.center() - expected_center;
    std::cout<<"Input Sphere 1: {"<<sphere1<<" }\n";
    std::cout<<"Input Sphere 2: {"<<sphere2<<" }\n";
    std::cout<<"Output Sphere: {"<<sphere3<<" }\n";
    REQUIRE( (eps * eps) < MOVETK_EPS );
    REQUIRE( abs(expected_squared_radius - sphere3.squared_radius() )  < MOVETK_EPS);
}

