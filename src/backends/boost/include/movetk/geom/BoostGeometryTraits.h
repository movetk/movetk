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

namespace movetk::backends::boost {
/**
 * @brief A traits class for the Boost geometry backend
 * @details
 * @tparam NumberType The number type to use
 * @tparam dimensions The number of dimensions to support
 * Currently we only support dimensions 2 and 3.
 */
template <class NumberType, size_t dimensions>
struct BoostGeometryTraits {
	static_assert(dimensions < 4);
	using NT = NumberType;
	constexpr static size_t dim = dimensions;
	using Point_d = ::boost::geometry::model::point<NT, dimensions, ::boost::geometry::cs::cartesian>;
	using Wrapper_Boost_Geometry = MovetkBoostKernel<BoostGeometryTraits>;
};
/**
 * @brief Convenience definition to get the Boost MoveTK kernel for the supplied arguments
 * @tparam NumberType The number type to use
 * @tparam dimensions The number of dimensions to support
 */
template <typename NumberType, size_t dimensions>
using KernelFor = MovetkBoostKernel<BoostGeometryTraits<NumberType, dimensions>>;
};  // namespace movetk::backends::boost


#endif  // MOVETK_BOOSTGEOMETRYTRAITS_H
