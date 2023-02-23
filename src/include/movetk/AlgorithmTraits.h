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

/*!
 * @file AlgorithmTraits.h
 * @brief A collection of traits classes for various algorithms in movetk
 * @authors Aniket Mitra (aniket.mitra@here.com)
 */

#ifndef MOVETK_ALGORITHMTRAITS_H
#define MOVETK_ALGORITHMTRAITS_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/property_map/property_map.hpp>

/*!
 *  @brief a collection of algorithms in movetk
 */
namespace movetk::algo {
/*!
 * @struct geo_coordinates_tag
 * @brief This tag is used to specify whether the input will be a collection of
 * geo coordinates
 */
struct geo_coordinates_tag;
/*!
 * @struct cartesian_coordinates_tag
 * @brief This tag is used to specify whether the input will be a collection of
 * cartesian coordinates
 */
struct cartesian_coordinates_tag;

/*!
 * @struct BBParameterTraits
 * @brief This traits class serves as a collection of types
 * for parameterization of BBMM
 * @tparam _GeometryTraits -  This class is a collection of movetk
 *  geometry types. For example @refitem movetk::geom::MovetkGeometryKernel
 * @tparam TrajectoryIterator - An iterator type.
 * For example @refitem TabularTrajectory::TrajectoryIterator
 */
template <class _GeometryTraits, class TrajectoryIterator, class _Norm>
struct BBMMParameterTraits {
	typedef _Norm Norm;
	typedef _GeometryTraits GeometryTraits;
	/*!*
	 * @typedef ::NT
	 * @brief typedef  of the number type defined in GeometryTraits.
	 * For example @refitem  movetk::geom::MovetkGeometryKernel::NT
	 * */
	typedef typename GeometryTraits::NT NT;
	/*!*
	 * @typedef ::Point
	 * @brief typedef of the point type defined in GeometryTraits.
	 * For example @refitem movetk::geom::MovetkGeometryKernel::MovetkPoint
	 *  */
	typedef typename GeometryTraits::MovetkPoint Point;
	/*!* @typedef ::Vector
	 * @brief typedef of the vector type defined in GeometryTraits.
	 * For example @refitem  movetk::geom::MovetkGeometryKernel::MovetkVector
	 * */
	typedef typename GeometryTraits::MovetkVector Vector;
	/*!* @typedef ::Parameters
	 */
	typedef std::tuple<Point, Point, NT, TrajectoryIterator, TrajectoryIterator> Parameters;
	/*!
	 * @enum ParameterColumns
	 *
	 */
	enum ParameterColumns {
		POINT,         /*!< global enum value 1 */
		MU,            /*!< global enum value 1 */
		SIGMA_SQUARED, /*!< global enum value 1 */
		BEGIN,         /*!< global enum value 1 */
		END            /*!< global enum value 1 */
	};
};


}  // namespace movetk::algo

#endif  // MOVETK_ALGORITHMTRAITS_H
