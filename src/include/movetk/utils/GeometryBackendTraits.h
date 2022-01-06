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


#include <tuple>

#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"
#include "movetk/ds/FreeSpaceDiagram.h"
#include "movetk/io/TuplePrinter.h"


// Setup backend structures
#if MOVETK_WITH_CGAL_BACKEND
#include "movetk/geom/CGALTraits.h"
namespace movetk::backends {
    struct CGALBackend {
        using NT = long double;
        static constexpr size_t dimensions = 2;
        // Define the Geometry Backend
        using GeometryBackend = movetk_support::CGALTraits<NT, dimensions>;
        //Using the Geometry Backend define the Movetk Geometry Kernel
        using MovetkGeometryKernel = movetk_core::MovetkGeometryKernel<
            typename GeometryBackend::Wrapper_CGAL_Geometry
        >;
    };
}
#else
#endif
#if MOVETK_WITH_BOOST_BACKEND
#include "movetk/geom/BoostGeometryTraits.h"
namespace movetk::backends {
    struct BoostBackend {
        using NT = long double;
        static constexpr size_t dimensions = 2;
        // Define the Geometry Backend
        using GeometryBackend = movetk_support::BoostGeometryTraits<NT, dimensions>;
        //Using the Geometry Backend define the Movetk Geometry Kernel
        using MovetkGeometryKernel = movetk_core::MovetkGeometryKernel<
            typename GeometryBackend::Wrapper_Boost_Geometry
        >;
    };
}
#endif

namespace movetk::backends {
    template<typename T> struct remove_first_type;
    template<typename T, typename...Ts>
    struct remove_first_type < std::tuple<T, Ts...>> {
        using type = std::tuple<Ts...>;
    };
    struct noop {};

    using AvailableBackends = typename remove_first_type<std::tuple<
        noop
#if MOVETK_WITH_BOOST_BACKEND
        , BoostBackend
#endif
#if MOVETK_WITH_CGAL_BACKEND
        , CGALBackend
#endif
        >>::type;
    static_assert(std::tuple_size_v<AvailableBackends> > 0);
}

struct GeometryKernel : public std::tuple_element_t<0,movetk::backends::AvailableBackends>
{
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
