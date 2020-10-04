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
// Created by Mitra, Aniket on 29/01/2019.
//

#define CGAL_BACKEND_ENABLED 0

#if CGAL_BACKEND_ENABLED
#include "movetk/geom/CGALTraits.h"
#else
#include "movetk/geom/BoostGeometryTraits.h"
#endif

#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"
#include "movetk/ds/FreeSpaceDiagram.h"
#include "movetk/TuplePrinter.h"

struct GeometryKernel
{

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
    // It is possible to choose
    // a higher dimension
    const static size_t dimensions = 2;
    //const size_t dimensions = 7;
    //==============================

    //==============================
    // Define the Geometry Backend
    typedef movetk_support::CGALTraits<NT, dimensions> CGAL_GeometryBackend;
    //Using the Geometry Backend define the Movetk Geometry Kernel
    typedef movetk_core::MovetkGeometryKernel<
        typename CGAL_GeometryBackend::Wrapper_CGAL_Geometry>
        MovetkGeometryKernel;
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

    typedef movetk_core::IntersectionTraits<MovetkGeometryKernel, Norm,
                                            movetk_core::sphere_segment_intersection_tag>
        SphSegIntersectionTraits;

    typedef movetk_core::IntersectionTraits<MovetkGeometryKernel, Norm,
                                            movetk_core::sphere_sphere_intersection_tag>
        SphSphIntersectionTraits;

    typedef movetk_support::FreeSpaceCellTraits<SphSegIntersectionTraits> FreeSpaceCellTraits;
    typedef movetk_support::FreeSpaceCell<FreeSpaceCellTraits> FreeSpaceCell;
    typedef movetk_support::FreeSpaceDiagramTraits<FreeSpaceCell> FreeSpaceDiagramTraits;
    typedef movetk_support::FreeSpaceDiagram<FreeSpaceDiagramTraits> FreeSpaceDiagram;
};
