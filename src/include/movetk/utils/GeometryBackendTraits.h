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
#ifndef MOVETK_GEOMETRYBACKENDTRAITS_H
#define MOVETK_GEOMETRYBACKENDTRAITS_H
//
// Created by Mitra, Aniket on 29/01/2019.
//


#include <tuple>

#include "movetk/ds/FreeSpaceDiagram.h"
#include "movetk/geom/GeometryConcepts.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/io/TuplePrinter.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"


// Setup backend structures
#if MOVETK_WITH_CGAL_BACKEND
#include "movetk/geom/CGALTraits.h"
namespace movetk::backends {
/**
 * @brief Base 2D CGAL kernel using long double as number type
 */
struct CGALBackend {
	using NT = long double;
	static constexpr size_t dimensions = 2;
	// Define the kernel
	using MovetkGeometryKernel = movetk::backends::cgal::KernelFor<NT, dimensions>;
	static constexpr const char* name = "CGAL";
};
static_assert(movetk::geom::concepts::BaseKernel<CGALBackend::MovetkGeometryKernel>);
}  // namespace movetk::backends
#else
#endif
#if MOVETK_WITH_BOOST_BACKEND
#include "movetk/geom/BoostGeometryTraits.h"
namespace movetk::backends {
/**
 * @brief Base 2D Boost kernel using long double as number type
 */
struct BoostBackend : public movetk::backends::boost::KernelFor<long double, 2> {
	using NT = long double;
	static constexpr size_t dimensions = 2;
	// Define the Geometry Backend
	using MovetkGeometryKernel = BoostBackend;
	static constexpr const char* name = "Boost";
};
static_assert(movetk::geom::concepts::BaseKernel<BoostBackend::MovetkGeometryKernel>);
}  // namespace movetk::backends
#endif

namespace movetk::backends {
/**
 * @cond HIDDEN_SYMBOLS
 */
template <typename T>
struct remove_first_type;
template <typename T, typename... Ts>
struct remove_first_type<std::tuple<T, Ts...>> {
	using type = std::tuple<Ts...>;
};
struct noop {};
/**
 * @endcond
 */
using AvailableBackends = typename remove_first_type<std::tuple<noop
#if MOVETK_WITH_BOOST_BACKEND
                                                                ,
                                                                BoostBackend
#endif
#if MOVETK_WITH_CGAL_BACKEND
                                                                ,
                                                                CGALBackend
#endif
                                                                >>::type;
static_assert(std::tuple_size_v<AvailableBackends> > 0);
}  // namespace movetk::backends

namespace movetk {
/**
 * @brief Basic geometric kernel
 * @tparam Backend The backend kernel to use
 */
template <typename Backend>
struct BaseGeometryKernel : public Backend {
	using MovetkGeometryKernel = typename Backend::MovetkGeometryKernel;
	using Norm = movetk::metric::FiniteNorm<typename Backend::MovetkGeometryKernel, 2>;

	using SphSegIntersectionTraits =
	    movetk::geom::IntersectionTraits<MovetkGeometryKernel, Norm, movetk::geom::sphere_segment_intersection_tag>;

	using SphSphIntersectionTraits =
	    movetk::geom::IntersectionTraits<MovetkGeometryKernel, Norm, movetk::geom::sphere_sphere_intersection_tag>;

	using FreeSpaceCellTraits = movetk::ds::FreeSpaceCellTraits<SphSegIntersectionTraits>;
	using FreeSpaceCell = movetk::ds::FreeSpaceCell<FreeSpaceCellTraits>;
	using FreeSpaceDiagramTraits = movetk::ds::FreeSpaceDiagramTraits<FreeSpaceCell>;
	using FreeSpaceDiagram = movetk::ds::FreeSpaceDiagram<FreeSpaceDiagramTraits>;
};
}  // namespace movetk

// TODO:remove from global namespace
struct GeometryKernel
    : public movetk::BaseGeometryKernel<std::tuple_element_t<0, movetk::backends::AvailableBackends>> {};
#endif