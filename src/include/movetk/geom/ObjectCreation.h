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
#ifndef MOVETK_GEOM_OBJECTCREATION_H
#define MOVETK_GEOM_OBJECTCREATION_H
#include <numeric>

#include "movetk/utils/Requirements.h"
#include "movetk/utils/TypeChecks.h"
namespace movetk::geom {

/*!@struct MakePoint
 * @brief Constructs a point from a set of input Cartesian coordinates
 * @tparam GeometryTraits - A traits class that defines movetk geometry types
 */
template <class GeometryTraits>
struct MakePoint {
	/*!
	 * @details Infers the dimensions of the point from the input
	 * and creates a point with Cartesian coordinates \f$[first,beyond)\f$
	 * @tparam CoordinateIterator - A random access iterator over a set of
	 * Cartesian coordinates where each coordinate is of type GeometryTraits::NT
	 * @param first - Iterator to the first Cartesian coordinate
	 * @param beyond - Iterator to the end of the last Cartesian coordinate
	 * @return A movetk point
	 */
	template <utils::RandomAccessIterator<typename GeometryTraits::NT> CoordinateIterator>
	typename GeometryTraits::MovetkPoint operator()(CoordinateIterator first, CoordinateIterator beyond) const {
		typename GeometryTraits::MovetkPoint p(first, beyond);
		return p;
	}
	/**
	 * @brief Construct a point from a list of coordinates. The number of coordinates should be at least the number
	 * of dimensions of the kernel.
	 * @param l The list of coordinates
	 * @return The point
	 */
	typename GeometryTraits::MovetkPoint operator()(std::initializer_list<typename GeometryTraits::NT> l) const {
		typename GeometryTraits::MovetkPoint p(l.begin(), l.end());
		return p;
	}
};

/*!@struct MakeLine
 * @brief Constructs a line form two input points
 * @tparam GeometryTraits - A traits class that defines movetk geometry types
 */
template <class GeometryTraits>
struct MakeLine {
	/*!
	 *
	 * @param p1 - A movetk point
	 * @param p2 - A movetk point
	 * @return A movetk line
	 */
	typename GeometryTraits::MovetkLine operator()(typename GeometryTraits::MovetkPoint p1,
	                                               typename GeometryTraits::MovetkPoint p2) const {
		typename GeometryTraits::MovetkLine l(p1, p2);
		return l;
	}
};

/*!@struct ComputeLength
 * @brief  Computes the euclidean distance between two points
 * @tparam GeometryTraits - A traits class that defines movetk geometry types
 */
template <class GeometryTraits>
struct ComputeLength {
	using NT = typename GeometryTraits::NT;
	/*!
	 * @details Computes the length of a segment between p1 and p2.
	 * @param p1 - A movetk point
	 * @param p2 - A movetk point
	 * @return Length of a segment
	 */
	NT operator()(typename GeometryTraits::MovetkPoint p1, typename GeometryTraits::MovetkPoint p2) const {
		typename GeometryTraits::MovetkSegment l(p1, p2);
		return sqrt(l());
	}

	NT operator()(typename GeometryTraits::MovetkSegment l) const { return sqrt(l()); }

	/**
	 * @brief Computes the length of a polyline, specified as an iterator range
	 * @tparam PointIterator Input iterator that dereferences to MovetkPoint elements.
	 * @param first Begin of the polyline points
	 * @param beyond End of the polyline points
	 * @return The length of the polyline
	 */
	template <utils::RandomAccessIterator<typename GeometryTraits::MovetkPoint> PointIterator>
	NT operator()(PointIterator first, PointIterator beyond) const {
		return std::inner_product(first,
		                          std::prev(beyond),
		                          std::next(first),
		                          0.0,
		                          std::plus<>(),
		                          [this](const auto& pnt0, const auto& pnt1) { return operator()(pnt0, pnt1); });
	}
};

/**
 * @brief Functor for creating segments
 * @tparam GeometryTraits The kernel
 */
template <class Kernel>
struct MakeSegment {
	using Segment = typename Kernel::MovetkSegment;
	using Point = typename Kernel::MovetkPoint;

	/**
	 * @brief Creates a segment from a pair of points
	 * @param p1 The first point
	 * @param p2 The second point
	 * @return The segment
	 */
	Segment operator()(Point p1, Point p2) const {
		Segment s(p1, p2);
		return s;
	}

	/**
	 * @brief Creates a segment from a pair of initializer lists, which represent the coordinates of the points
	 * @param l1 The first initializer list
	 * @param l2 The second initializer list
	 * @return The segment
	 */
	Segment operator()(std::initializer_list<typename Kernel::NT> l1,
	                   std::initializer_list<typename Kernel::NT> l2) const {
		MakePoint<Kernel> make_point;
		const auto p1 = make_point(l1);
		const auto p2 = make_point(l2);
		Segment s(p1, p2);
		return s;
	}
};

/**
 * @brief Functor for creating a sphere (disc in 2D).
 * @tparam Kernel The kernel
 */
template <class Kernel>
struct MakeSphere {
	/**
	 * @brief Creates a sphere from a center and a (squared) radius
	 * @param center The center
	 * @param radius The (squared) radius
	 * @param square Whether is squared (true) or not squared (false)
	 * @return The sphere
	 */
	typename Kernel::MovetkSphere operator()(typename Kernel::MovetkPoint center,
	                                         typename Kernel::NT radius,
	                                         bool square = true) const {
		typename Kernel::MovetkSphere s(center, radius, square);
		return s;
	}

	/**
	 * @brief Creates a sphere from an initializer list representing the coordinates of the
	 * center and a (squared) radius
	 * @param l The list of coordinates for the center
	 * @param radius The (squared) radius
	 * @param square Whether is squared (true) or not squared (false)
	 * @return The sphere
	 */
	typename Kernel::MovetkSphere operator()(std::initializer_list<typename Kernel::NT> l,
	                                         typename Kernel::NT radius,
	                                         bool square = true) const {
		MakePoint<Kernel> make_point;
		const auto center = make_point(l.begin(), l.end());
		typename Kernel::MovetkSphere s(center, radius, square);
		return s;
	}
};

/*!@struct MakePolygon
 * @brief  Constructs a Polygon  from a set of Movetk points
 * @tparam GeometryTraits - A traits class that defines movetk geometry types
 */
template <class GeometryTraits>
struct MakePolygon {
	/*!
	 *
	 * @tparam PointIterator - A random access iterator over a set of points
	 * @param first -  Iterator to the first point in a set of Movetk points
	 * @param beyond - Iterator to the last point in a set of Movetk points
	 * @return A movetk polygon
	 */
	template <utils::RandomAccessIterator<typename GeometryTraits::MovetkPoint> PointIterator>
	typename GeometryTraits::MovetkPolygon operator()(PointIterator first, PointIterator beyond) const {
		typename GeometryTraits::MovetkPolygon polygon(first, beyond);
		return polygon;
	}
};

/*! @struct MakeMinSphere
 *  @brief Constructs a minimum enclolsing ball from a set of Movetk points
 *  @tparam GeometryTraits - A traits class that defines movetk geometry types
 */
template <class GeometryTraits>
struct MakeMinSphere {
	typename GeometryTraits::MovetkMinSphere make_min_sphere;

	/*!
	 *
	 * @tparam PointIterator - A random access iterator over a set of points
	 * @param first - Iterator to the first point in a set of Movetk points
	 * @param beyond - Iterator to the last point in a set of Movetk points
	 * @return Radius of the Minimum Enclosing Ball
	 */
	template <utils::RandomAccessIterator<typename GeometryTraits::MovetkPoint> PointIterator>
	typename GeometryTraits::NT operator()(PointIterator first, PointIterator beyond) const {
		return make_min_sphere(first, beyond);
	}

	/*!
	 *
	 * @tparam PointIterator -  A random access iterator over a set of points
	 * @tparam CenterIterator -  A forward iterator that points to the location
	 * where a value has to be inserted
	 * @param first - Iterator to the first point in a set of Movetk points
	 * @param beyond - Iterator to the last point in a set of Movetk points
	 * @param iter - Iterator to the location where a value has to be inserted
	 * @return Radius of the Minimum Enclosing Ball
	 */
	template <utils::RandomAccessIterator<typename GeometryTraits::MovetkPoint> PointIterator,
	          utils::OutputIterator<typename GeometryTraits::NT> CenterIterator>
	typename GeometryTraits::NT operator()(PointIterator first, PointIterator beyond, CenterIterator iter) const {
		return make_min_sphere(first, beyond, iter);
	}
};
}  // namespace movetk::geom
#endif