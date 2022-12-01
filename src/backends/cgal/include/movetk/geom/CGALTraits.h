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

/*! @file CGALTraits.h
 *  @brief  A collection of CGAL traits classes for parameterizing the
 *  CGAL Wrapper Traits class
 *  @authors Aniket Mitra (aniket.mitra@here.com)
 */


#ifndef MOVETK_CGALTRAITS_H
#define MOVETK_CGALTRAITS_H

#include "CGALWrapper.h"
#include "movetk/geom/GeometryInterface.h"


namespace movetk::backends::cgal {

/*!struct CGALTraits
 * @brief  CGAL traits classes for parameterizing TSL::Wrapper_CGAL_Kernel
 * @tparam NumberType - The number type for parameterizing the CGAL kernel
 * and hence the geometry Interface of Movetk
 * @tparam CoordinateIterType - A random access iterator over a set of
 * Cartesian coordinates where each coordinate is of type \c NumberType
 * @tparam dimensions - The dimesionality of the Cartesian coordinate space
 */
template <ValidCGALNumberType NumberType, size_t dimensions>
struct CGALTraits {
	typedef NumberType NT;
	constexpr static size_t dim = dimensions;
	using GeometryType = CGAL::Cartesian_d<NT>;  // define the geometry type
	using SphereTraits = CGAL::Min_sphere_of_points_d_traits_d<GeometryType, NT, dimensions>;
	using Wrapper_CGAL_Geometry = MovetkCGALKernel<CGALTraits>;  // the traits class
};
template<typename NumberType, size_t dimensions>
using KernelFor = typename CGALTraits<NumberType,dimensions>::Wrapper_CGAL_Geometry;
};  // namespace movetk::geom

#endif  // MOVETK_CGALTRAITS_H
