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

#ifndef MOVETK_BOOSTGEOMETRYADAPTER_H
#define MOVETK_BOOSTGEOMETRYADAPTER_H

#include <movetk/utils/Requirements.h>

#include <boost/geometry.hpp>
#include <iostream>
#include <utility>

#include "movetk/utils/Iterators.h"
#include "movetk/utils/Requirements.h"
#include "movetk/utils/StringUtils.h"
#include "third_party/boost_geometry/discrete_frechet_distance.hpp"
#include "third_party/boost_geometry/discrete_hausdorff_distance.hpp"
#include "third_party/miniball/Seb.h"

/**
 * @brief Contains the definitions for the Boost geometry backend
 */
namespace movetk::backends::boost {

namespace bg = ::boost::geometry;

/**
 * @brief Contains a collection of adapters from Boost geometry primitives to the MoveTK interface
 */
namespace wrappers {
/**
 * @brief An adapted point for a Boost point
 * @tparam Kernel The kernel to use
 */
template <class Kernel>
class Point {
private:
	using Boost_Point = typename Kernel::Boost_Point_;
	using NT = typename Kernel::NT;
	using CoordinateIterator = typename utils::movetk_basic_iterator<const NT>;
	using Vector = typename Kernel::MovetkVector;
	using Point_Container = std::array<typename Kernel::NT, Kernel::dim>;
	Boost_Point pt{0, 0};
	/**
	 * @brief Constructs the point from the Boost container for the point
	 * @param p The container
	 */
	Point(Point_Container&& p) {
		pt.template set<0>(std::move(p[0]));
		pt.template set<1>(std::move(p[1]));
		if constexpr (Kernel::dim == 3) {
			pt.template set<2>(std::move(p[2]));
		}
	}

public:
	Point() = default;

	/**
	 * @brief Construct the Point from a native Boost point
	 * @param p The point
	 */
	Point(const Boost_Point& p) : pt(p) {}

	/**
	 * @brief Constructs a point from a pair of coordinate iterators
	 * @param first The start of the coordinate range
	 * @param beyond The end of the coordinate range
	 */
	template <utils::InputIterator<NT> CoordinateIterator>
	Point(CoordinateIterator first, CoordinateIterator beyond) {
		pt.template set<0>(*first);
		pt.template set<1>(*(first + 1));
		if constexpr (Kernel::dim == 3) {
			pt.template set<2>(*(first + 2));
		}
	}
	/**
	 * @brief Begin iterator of the coordinate range
	 * @return The begin iterator
	 */
	auto begin() const { return std::addressof(pt.template get<0>()); }

	/**
	 * @brief End iterator of the coordinate range
	 * @return The end iterator
	 */
	auto end() const { return std::addressof(pt.template get<0>()) + Kernel::dim; }

	/**
	 * @brief Indexing operator to get the \p i-th coordinate of the point
	 * @param i The index
	 * @return The coordinate
	 */
	const NT& operator[](size_t i) const { return *(std::addressof(pt.template get<0>()) + i); }

	/**
	 * @brief Indexing operator to get the modifiable \p i-th coordinate of the point
	 * @param i The index
	 * @return Reference to the coordinate
	 */
	NT& operator[](size_t i) { return *(std::addressof(pt.template get<0>()) + i); }

	/**
	 * @brief Subtracts another point from this point, resulting in a vector
	 * @param point The other point
	 * @return The vector pointing from the other point to this point
	 */
	Vector operator-(const Point& point) const {
		Vector v1(*this);
		Vector v2(point);
		return v1 - v2;
	}

	/**
	 * @brief Adds the vector to this point
	 * @param v The vector
	 * @return A copy of this point, translated by the given vector
	 */
	Point operator+(const Vector& v) const {
		Point_Container result;
		std::transform(this->begin(), this->end(), v.begin(), result.begin(), std::plus<typename Kernel::NT>());
		Point _point(std::move(result));
		return _point;
	}
	/**
	 * @brief Adds the vector to this point
	 * @param v The vector
	 * @return A copy of this point, translated by the given vector
	 */
	Point operator-(const Vector& v) const {
		Point_Container result;
		std::transform(this->begin(), this->end(), v.begin(), result.begin(), std::minus<typename Kernel::NT>());
		Point _point(std::move(result));
		return _point;
	}

	/**
	 * @brief Returns a copy of the native point
	 * @return The native point
	 */
	Boost_Point get() const { return pt; }

	/**
	 * @brief Writes the coordinates to an output stream
	 * @param out The output stream
	 * @param point The point
	 * @return The output stream
	 */
	friend std::ostream& operator<<(std::ostream& out, const Point& point) {
		return (out << movetk::utils::join(point.begin(), point.end()));
	}
};

/**
 * @brief An adapted segment for a Boost segment
 * @tparam Kernel
 */
template <class Kernel>
class Segment {
private:
	using Boost_Segment = typename Kernel::Boost_Segment_;
	using NT = typename Kernel::NT;
	Boost_Segment seg;

public:
	using Point = typename Kernel::MovetkPoint;
	Segment() = default;

	Segment(const Boost_Segment& s) : seg(s) {}

	Segment(const Point& p1, const Point& p2) { seg = Boost_Segment(p1.get(), p2.get()); }

	Segment(Point&& p1, Point&& p2) { seg = Boost_Segment(p1.get(), p2.get()); }

	Point operator[](size_t idx) const {
		if (idx == 0)
			return seg.first;
		if (idx == 1)
			return seg.second;
		else
			return seg.first;
	}

	NT operator()() {
		typename Kernel::NT distance = bg::distance(seg.first, seg.second);
		return distance * distance;
	}

	Boost_Segment get() const { return seg; }

	friend std::ostream& operator<<(std::ostream& out, Segment& seg) { return out << seg[0] << ";" << seg[1]; }
};

/**
 * @brief An adapted line for a Boost line
 * @tparam Kernel The kernel to use
*/
template <class Kernel>
class Line {
private:
	using Boost_Line = typename Kernel::Boost_PolyLine_;
	using NT = typename Kernel::NT;
	Boost_Line line;

public:
	Line() = default;
	/**
	 * @brief Construct a line from a native Boost line
	 * @param l The native Boost line
	*/
	Line(const Boost_Line& l) : line(l) {}

	/**
	 * @brief Construct a line from two adapted points
	 * @param p1 First point
	 * @param p2 Second point
	*/
	Line(const Point<Kernel>& p1, const Point<Kernel>& p2) { line = Boost_Line({p1.get(), p2.get()}); }

	/**
	 * @brief Returns a copy of the native Boost line
	 * @return The copy of the Boost line
	*/
	Boost_Line get() const { return line; }
};

/**
 * @brief Adapted vector for a Boost vector
 * @tparam Kernel The kernel to use
*/
template <class Kernel>
class Vector {
	using Boost_Vector = std::array<typename Kernel::NT, Kernel::dim>;
	using basis_container = std::array<typename Kernel::NT, Kernel::dim>;
	Boost_Vector vec;
	using NT = typename Kernel::NT;

	Vector(Boost_Vector&& vector) : vec(std::move(vector)) {}
public:
	Vector() : vec({0, 0}) {}
	/**
	 * @brief Construct a vector from the native Boost vector
	 * @param vector The Boost vector
	*/
	Vector(const Boost_Vector& vector) : vec(vector) {}

	Vector(const Vector& p) = default;
	/**
	 * @brief Construct a vector from a kernel Point.
	 * @details The vector points from the origin to the provided point
	 * @param p The point
	*/
	Vector(const typename Kernel::MovetkPoint& p) { std::copy(std::begin(p), std::end(p), std::begin(vec)); }

	/** @name Arithmetic Operators
	 *  Operators on the Vector
	 */
	/**@{*/
	/**
	 * @brief Multiplication with a scalar. Scales the vector
	 * @param scalar The scalar
	 * @return Scaled vector
	*/
	Vector operator*(NT scalar) const {
		Vector copy(*this);
		copy *= scalar;
		return copy;
	}
	/**
	 * @brief Scales this vector
	 * @return Reference to self
	 */
	Vector& operator*=(NT scalar) {
		std::for_each(std::begin(vec), std::end(vec), [&scalar](NT& i) { i *= scalar; });
		return *this;
	}
	/**
	 * @brief Returns a scaled vector, scaled by 1/scalar
	 * @param scalar The scale
	 * @return Scaled vector.
	*/
	Vector operator/(NT scalar) const {
		Vector copy(*this);
		copy /= scalar;
		return copy;
	}
	/**
	 * @brief Scales this vector by 1/scalar.
	 * @param scalar The scalar
	 * @return Reference to self
	*/
	Vector& operator/=(NT scalar) {
		std::for_each(std::begin(vec), std::end(vec), [&scalar](NT& i) { i /= scalar; });
		return *this;
	}
	/**
	 * @brief Inner product operator
	 * @param vector Other boost vector
	 * @return The inner product between this and the other vector
	 */
	NT operator*(const Vector& vector) const {
		return std::inner_product(this->begin(), this->end(), vector.begin(), 0.0);
	}
	/**
	 * @brief Returns the subtraction of \p vector from this vector.
	 * @param vector The other vector
	 * @return The new vector
	*/
	Vector operator-(const Vector& vector) const {
		Vector copy(*this);
		copy -= vector;
		return copy;
	}
	/**
	 * @brief Subtracts \p vector from this vector
	 * @param vector The other vector
	 * @return Reference to self
	 */
	Vector& operator-=(const Vector& vector) {
		for (std::size_t i = 0; i < vec.size(); ++i) {
			vec[i] -= vector.vec[i];
		}
		return *this;
	}
	/**
	 * @brief Returns the addition of \p vector and this vector.
	 * @param vector The other vector
	 * @return The new vector
	 */
	Vector operator+(const Vector& vector) const {
		Vector copy(*this);
		copy += vector;
		return copy;
	}
	/**
	 * @brief Adds \p vector to this vector
	 * @param vector The other vector
	 * @return Reference to self
	 */
	Vector& operator+=(const Vector& vector) {
		for (std::size_t i = 0; i < vec.size(); ++i) {
			vec[i] += vector.vec[i];
		}
		return *this;
	}
	/**@}*/ 

	bool operator==(const Vector& vector) const { return std::equal(this->begin(), this->end(), vector.begin()); }
	bool operator!=(const Vector& vector) const { return !((*this) == vector); }

	/**
	 * @brief Returns the (unit) basis for the given axis
	 * @param i The axis
	 * @return The basis vector
	*/
	Vector basis(std::size_t i) const {
		Boost_Vector _e = {0};
		_e[i] = 1;
		Vector basis_vec(std::move(_e));
		return basis_vec;
	}

	auto begin() const { return std::cbegin(vec); }

	auto end() const { return std::cend(vec); }

	Boost_Vector get() const { return vec; }
	friend std::ostream& operator<<(std::ostream& out, const Vector& vec) {
		return (out << movetk::utils::join(vec.begin(), vec.end()));
	}
};


template <class Kernel>
class MinSphere {
private:
	using NT = typename Kernel::NT;
	using Point = typename Kernel::MovetkPoint;
	using miniball = Seb::Smallest_enclosing_ball<NT, Point>;
	template <class T>
	static std::pair<Point, NT> dispatcher(const size_t& dimensions, T&& values) {
		miniball mb(dimensions, std::forward<T>(values));
		Point pt(mb.center_begin(), mb.center_end());
		return std::make_pair(pt, mb.radius());
	}

public:
	/**
	 * @brief Computes the smallest enclosing sphere of a range of points.
	 * @param first Start of the range of points
	 * @param beyond End of the range of points 
	 * @param iter The output iterator to write the coordinates of the center of the minsphere to.
	 * @return The radius of the smallest enclosing sphere
	*/
	template <utils::RandomAccessIterator<Point> PointIterator, utils::OutputIterator<typename Kernel::NT> CenterIterator>
	NT operator()(PointIterator first, PointIterator beyond, CenterIterator iter) const {
		auto result = dispatcher(std::distance(first->begin(), first->end()), std::vector<Point>(first, beyond));
		std::copy(result.first.begin(), result.first.end(), iter);
		return result.second;
	}

	/**
	 * @brief Computes the radius of the smallest enclosing sphere of a range of points.
	 * @param first Start of the range of points
	 * @param beyond End of the range of points
	 * @param iter The output iterator to write the coordinates of the center of the minsphere to.
	 * @return The radius of the smallest enclosing sphere
	 */
	template <utils::RandomAccessIterator<Point> PointIterator>
	NT operator()(PointIterator first, PointIterator beyond) const {
		auto result = dispatcher(std::distance(first->begin(), first->end()), std::vector<Point>(first, beyond));
		return result.second;
	}
};

template <class Kernel>
class Polygon {
private:
	using Boost_Polygon = typename Kernel::Boost_Polygon_;
	using NT = typename Kernel::NT;
	Boost_Polygon polygon;

public:
	Polygon() = default;

	template <utils::RandomAccessIterator<typename Kernel::MovetkPoint> PointIterator>
	Polygon(PointIterator first, PointIterator beyond) {
		PointIterator it = first;
		while (it != beyond) {
			bg::append(polygon.outer(), it->get());
			it++;
		}
	}

	auto v_begin() const { return polygon.outer().cbegin(); }

	auto v_end() const { return polygon.outer().cend(); }

	Boost_Polygon get() const { return polygon; }

	/*!
	 * Prints a polygon whose vertices are separated by comma. Since
	 * each of these vertices are points, their coordinates are separated
	 * by semicolon
	 * @param out - OutputStream
	 * @param poly - A polygon of type Wrapper_Boost_Polygon<Kernel>
	 */
	friend std::ostream& operator<<(std::ostream& out, Polygon& poly) {
		using Point = typename Kernel::MovetkPoint;
		auto it = poly.v_begin();
		Point pt(*it);
		out << pt;
		it++;
		while (it != poly.v_end()) {
			out << ";";
			Point pt(*it);
			out << pt;
			it++;
		}
		return out;
	}
};

template <class Kernel>
struct SquaredDistance {
	template <class T1, class T2>
	typename Kernel::NT operator()(const T1& geometry1, const T2& geometry2) const {
		auto distance = bg::distance(geometry1.get(), geometry2.get());
		return distance * distance;
	}
};

template <class Kernel>
struct DiscreteHausdorffDistance {
	template <utils::RandomAccessIterator<typename Kernel::MovetkPoint> InputIterator>
	typename Kernel::NT operator()(InputIterator polyline_a_first,
	                               InputIterator polyline_a_beyond,
	                               InputIterator polyline_b_first,
	                               InputIterator polyline_b_beyond) const {
		typename Kernel::Boost_PolyLine_ poly1, poly2;

		for (auto it = polyline_a_beyond; it != polyline_a_beyond; ++it) {
			poly1.push_back(it->get());
		}
		for (auto it = polyline_b_first; it != polyline_b_beyond; ++it) {
			poly2.push_back(it->get());
		}
		return bg::discrete_hausdorff_distance(poly1, poly2);
	}
};

template <class Kernel>
struct DiscreteFrechetDistance {
	template <utils::RandomAccessIterator<typename Kernel::MovetkPoint> InputIterator>
	typename Kernel::NT operator()(InputIterator polyline_a_first,
	                               InputIterator polyline_a_beyond,
	                               InputIterator polyline_b_first,
	                               InputIterator polyline_b_beyond) const {
		typename Kernel::Boost_PolyLine_ poly1, poly2;

		for (auto it = polyline_a_beyond; it != polyline_a_beyond; ++it) {
			poly1.push_back(it->get());
		}
		for (auto it = polyline_b_first; it != polyline_b_beyond; ++it) {
			poly2.push_back(it->get());
		}
		return bg::discrete_frechet_distance(poly1, poly2);
	}
};

template <class Kernel>
class Sphere {
private:
	using Point = typename Kernel::MovetkPoint;
	using NT = typename Kernel::NT;
	Point _center;
	NT _squared_radius;

public:
	Sphere() = default;

	Sphere(const Point& center, NT radius, bool square = true) {
		_center = center;
		if (square)
			_squared_radius = std::pow(radius, 2);
		else
			_squared_radius = radius;
	}
	Point center() const { return _center; }

	NT squared_radius() const { return _squared_radius; }
	friend std::ostream& operator<<(std::ostream& out, const Sphere& sphere) {
		const auto center = sphere.center();
		return out << movetk::utils::join(center.begin(), center.end()) << ";" << sphere.squared_radius();
	}
};

}  // namespace wrappers
template <class Boost_Kernel>
struct MovetkBoostKernel {
	// Movetk kernel interface
	using NT = typename Boost_Kernel::NT;
	constexpr static size_t dim = Boost_Kernel::dim;
	using MovetkPoint = wrappers::Point<MovetkBoostKernel>;
	using MovetkSegment = wrappers::Segment<MovetkBoostKernel>;
	using MovetkLine = wrappers::Line<MovetkBoostKernel>;
	using MovetkPolygon = wrappers::Polygon<MovetkBoostKernel>;
	using MovetkMinSphere = wrappers::MinSphere<MovetkBoostKernel>;
	using MovetkVector = wrappers::Vector<MovetkBoostKernel>;
	using MovetkSphere = wrappers::Sphere<MovetkBoostKernel>;
	using MovetkSquaredDistance = wrappers::SquaredDistance<MovetkBoostKernel>;
	using MovetkDiscreteHausdorffDistance = wrappers::DiscreteHausdorffDistance<MovetkBoostKernel>;
	using MovetkDiscreteFrechetDistance = wrappers::DiscreteFrechetDistance<MovetkBoostKernel>;
	using MovetkIntersectionVisitor = void;
	using MovetkCurveIntersection = void;
	using MovetkMinimumBoundingRectangle = void;

	// Boost types
	using Boost_Point_ = typename Boost_Kernel::Point_d;
	using Boost_Segment_ = typename bg::model::segment<Boost_Point_>;
	using Boost_PolyLine_ = typename bg::model::linestring<Boost_Point_>;
	using Boost_Polygon_ = typename bg::model::polygon<Boost_Point_>;
};

};  // namespace movetk::backends::boost

#endif  // MOVETK_BOOSTGEOMETRYADAPTER_H
