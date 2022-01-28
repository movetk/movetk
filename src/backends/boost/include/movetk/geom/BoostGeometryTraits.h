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
// Created by Mitra, Aniket on 15/01/2019.
//

#ifndef MOVETK_BOOSTGEOMETRYTRAITS_H
#define MOVETK_BOOSTGEOMETRYTRAITS_H

#include <boost/geometry/algorithms/is_simple.hpp>
#include <boost/geometry/geometries/adapted/std_array.hpp>

#include "BoostGeometryWrapper.h"
#include "movetk/geom/GeometryInterface.h"

namespace movetk::geom {
template <class NumberType, size_t dimensions>
struct BoostGeometryTraits {
	static_assert(dimensions < 4);
	typedef NumberType NT;
	constexpr static size_t dim = dimensions;
	typedef boost::geometry::model::point<NT, dimensions, boost::geometry::cs::cartesian> Point_d;
	typedef Wrapper_Boost_Kernel<BoostGeometryTraits> Wrapper_Boost_Geometry;
};
};  // namespace movetk::geom


#endif  // MOVETK_BOOSTGEOMETRYTRAITS_H
