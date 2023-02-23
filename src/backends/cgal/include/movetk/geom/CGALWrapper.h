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
#ifndef MOVEKT_GEOM_CGALWRAPPER_H
#define MOVEKT_GEOM_CGALWRAPPER_H

#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arrangement_2.h>
#include <CGAL/Cartesian_d.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_rational.h>
#include <CGAL/Gmpfr.h>
#include <CGAL/Homogeneous_d.h>
#include <CGAL/IO/io.h>
#include <CGAL/Kernel_d/Sphere_d.h>
#include <CGAL/Min_sphere_of_points_d_traits_d.h>
#include <CGAL/Min_sphere_of_spheres_d.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Surface_sweep_2_algorithms.h>
#include <CGAL/intersections_d.h>
#include <CGAL/squared_distance_2.h>

#include <cstdlib>
#include <list>

#include "movetk/geom/GeometryConcepts.h"
#include "movetk/utils/Meta.h"
#include "movetk/utils/Requirements.h"
#include "movetk/utils/StringUtils.h"


/*! @brief  CGAL backend
 *  @details A collection of adapter classes for all
 *  CGAL geometric objects and CGAL algorithms that are used by movetk.
 *  @authors Aniket Mitra (aniket.mitra@here.com) Bram Custers (bacusters@gmail.com)
 */
namespace movetk::backends::cgal {
using VALID_CGAL_TYPES = std::tuple<long double, CGAL::MP_Float, CGAL::Gmpfr, CGAL::Gmpq, CGAL::Mpzf>;

using STRING_CASTABLE_CGAL_TYPES = std::tuple<long double>;

template <typename T>
struct is_valid_NT : public movetk::utils::tuple_contains_type<VALID_CGAL_TYPES, T> {};

template <typename T>
struct is_string_castable_NT : public movetk::utils::tuple_contains_type<STRING_CASTABLE_CGAL_TYPES, T> {};


template <class T = void>
using requires_valid_CGAL_NT = typename std::enable_if<is_valid_NT<T>::value>::type;

template <typename T>
concept ValidCGALNumberType = is_valid_NT<T>::value;


template <bool, class Kernel, class Type>
struct OutputRep {};

template <bool type_is_string_castable, class Kernel>
struct OutputRep<type_is_string_castable, Kernel, typename Kernel::MovetkPoint> {
	std::ostream &operator()(std::ostream &out, const typename Kernel::MovetkPoint &pt) {
		if constexpr (type_is_string_castable) {
			return (out << movetk::utils::join(pt.begin(), pt.end()));
		} else {
			return (out << pt.get());
		}
	}
};

template <bool type_is_string_castable, class Kernel>
struct OutputRep<type_is_string_castable, Kernel, typename Kernel::MovetkVector> {
	std::ostream &operator()(std::ostream &out, const typename Kernel::MovetkVector &v) {
		if constexpr (type_is_string_castable) {
			return (out << movetk::utils::join(v.begin(), v.end()));
		} else {
			return (out << v.get());
		}
	}
};

template <bool type_is_string_castable, class Kernel>
struct OutputRep<type_is_string_castable, Kernel, typename Kernel::MovetkSphere> {
	std::ostream &operator()(std::ostream &out, const typename Kernel::MovetkSphere &s) {
		if constexpr (type_is_string_castable) {
			typename Kernel::Wrapper_Point center = s.center();
			out << movetk::utils::join(center.begin(), center.end()) + ";";
			out << s.squared_radius();
			return out;
		} else {
			typename Kernel::Wrapper_Point center = s.center();
			out << center.get();
			out << ",";
			out << s.squared_radius();
			return out;
		}
	}
};


template <bool type_is_string_castable, class Kernel>
struct OutputRep<type_is_string_castable, Kernel, typename Kernel::MovetkPolygon> {
	std::ostream &operator()(std::ostream &out, const typename Kernel::MovetkPolygon &poly) {
		if constexpr (type_is_string_castable) {
			auto it = poly.get().vertices_begin();
			auto beyond = poly.get().vertices_end();
			std::string mergedTokens = movetk::utils::join(it->cartesian_begin(), it->cartesian_end(), ';');
			it++;
			while (it != beyond) {
				mergedTokens += ',';
				mergedTokens += movetk::utils::join(it->cartesian_begin(), it->cartesian_end(), ';');
				it++;
			}
			out << mergedTokens;
			return out;
		} else {
			auto it = poly.get().vertices_begin();
			auto beyond = poly.get().vertices_end();
			out << *it;
			it++;
			while (it != beyond) {
				out << ",";
				out << *it;
				it++;
			}
			return out;
		}
	}
};

/**
 * @brief Collection of adapters of CGAL geometry to the MoveTK geometry interface
 */
namespace wrappers {
/** @brief A wrapper class for CGAL Point.
 *  @details This class acts as a wrapper to a CGAL point by encapsulating
 *  CGAL specific details of a point. Provides a generic interface for a Point.
 * @tparam Kernel - A traits class that defines CGAL geometry types
 */
template <class Kernel>
class Point {
private:
	using CGAL_Point = typename Kernel::CGAL_Point_;
	using Vector = typename Kernel::MovetkVector;
	CGAL_Point pt;

public:
	/*!
	 * 	Creates a CGAL point and a generic interface to a Point as defined
	 * 	by this class.
	 */
	Point() = default;
	Point(const Point &) = default;

	/*!
	 * Creates a CGAL point and a generic interface to a Point as defined
	 * by this class.
	 * @param p - A CGAL point
	 */
	Point(const CGAL_Point &p) : pt(p) {}

	/*!
	 * Creates a CGAL point and a generic interface to a Point as defined
	 * by this class.
	 * @details Infers the dimensions of the point from the input
	 * and creates a point with Cartesian coordinates \f$[first,beyond)\f$
	 * @pre InputIterator should be a random access iterator where
	 * each coordinate is of type GeometryTraits::NT
	 * @param first - Iterator to the first Cartesian coordinate
	 * @param beyond - Iterator to the end of the last Cartesian coordinate
	 */
	template <std::random_access_iterator CoordinateIterator>
	Point(CoordinateIterator first, CoordinateIterator beyond) {
		size_t dimensions = std::distance(first, beyond);
		pt = CGAL_Point(dimensions, first, beyond);
	}

	/*!
	 *
	 * @return Iterator to the first coordinate of the CGAL point
	 */
	auto begin() const { return pt.cartesian_begin(); }

	/*!
	 *
	 * @return Iterator to the end of the last coordinate of the CGAL point
	 */
	auto end() const { return pt.cartesian_end(); }

	/*!
	 * Compares two wrapped CGAL points for less than
	 * @param point - A point of type Wrapper_CGAL_Point<Kernel>
	 * @return  True / False
	 */
	bool operator<(const Point &point) const { return this->get() < point.get(); }


	/*!
	 * Compares two wrapped CGAL points for greater than
	 * @param point - A point of type Wrapper_CGAL_Point<Kernel>
	 * @return  True / False
	 */
	bool operator>(const Point &point) const { return this->get() > point.get(); }

	/*!
	 * Compares two wrapped CGAL points for less than equality
	 * @param point -  A point of type Wrapper_CGAL_Point<Kernel>
	 * @return True / False
	 */
	bool operator<=(const Point &point) const { return this->get() <= point.get(); }

	/*!
	 * Compares two wrapped CGAL points for greater than equality
	 * @param point - A point of type Wrapper_CGAL_Point<Kernel>
	 * @return  True / False
	 */
	bool operator>=(const Point &point) const { return this->get() >= point.get(); }

	Vector operator-(const Point &point) const { return Vector(this->get() - point.get()); }

	Point operator+(const Vector &v) const { return Point(this->get() + v.get()); }

	Point operator-(const Vector &v) const { return Point(this->get() - v.get()); }

	/*!
	 * @return  A CGAL point
	 */
	CGAL_Point get() const { return pt; }


	/*!
	 * Prints a point whose coordinates are separated by comma
	 * @param out - OutputStream
	 * @param point - A point of type Wrapper_CGAL_Point<Kernel>
	 */
	friend std::ostream &operator<<(std::ostream &out, const Point &point) {
		OutputRep<is_string_castable_NT<typename Kernel::NT>::value, Kernel, Point> output;
		return output(out, point);
	}
};


/**
 * @brief A wapper class for CGAL line
 * @details This class acts as a wrapper to a CGAL line by
 * encapsulating CGAL specific details of a line.
 * Provides a generic inteface for a Line.
 * @tparam Kernel - A traits class that defines CGAL geometry types
 */
template <class Kernel>
class Line {
private:
	using CGAL_Point = typename Kernel::CGAL_Point_;
	using CGAL_Line = typename Kernel::CGAL_Line_;
	using Point = typename Kernel::MovetkPoint;
	CGAL_Line line;

public:
	/*!
	 * Creates a CGAL line and a generic interface to a Line as defined
	 * by this class.
	 */
	Line() = default;

	/*!
	 * Creates a wrapped CGAL line
	 * @param l - A CGAL line
	 */
	Line(const CGAL_Line &l) : line(l) {}

	/*!
	 * Creates a CGAL line and a generic interface to a Line as defined
	 * by this class.
	 * @param p1 - A wrapped CGAL point through which the wrapped CGAL line passes
	 * @param p2 - A wrapped CGAL point through which the wrapped CGAL line passes
	 */
	Line(const Point &p1, const Point &p2) { line = CGAL_Line(p1.get(), p2.get()); }

	Point operator[](size_t idx) const { return line[idx]; }

	/*!
	 *
	 * @return A CGAL line
	 */
	CGAL_Line get() const { return line; }
};

/**
 *  @brief A wrapper class for CGAL Segment
 *  @details This class acts as a wrapper to a CGAL segment by encapsulating
 *  CGAL specific details of segment.
 *  Provides a generic inteface for a segment.
 * @tparam Kernel  - A traits class that defines CGAL geometry types
 */
template <class Kernel>
class Segment {
private:
	using CGAL_Point = typename Kernel::CGAL_Point_;
	using CGAL_Segment = typename Kernel::CGAL_Segment_;
	using Point = typename Kernel::MovetkPoint;
	using NT = typename Kernel::NT;
	CGAL_Segment seg;

public:
	/*!
	 * Creates a CGAL segment and a generic interface to a Segment as defined
	 * by this class.
	 */
	Segment() = default;

	/*!
	 * Creates a CGAL segment and a generic interface to a Segment as defined
	 * by this class.
	 * @param s  - A CGAL segment
	 */
	Segment(const CGAL_Segment &seg_) : seg(seg_) {}

	/*!
	 * Creates a CGAL segment and a generic interface to a Segment as defined
	 * by this class.
	 * @param p1 - A wrapped CGAL point that defines one end-point of
	 * a wrapped CGAL segment
	 * @param p2 - A wrapped CGAL point that defines the other end-point of
	 * a wrapped CGAL segment
	 */
	Segment(const Point &p1, const Point &p2) { seg = CGAL_Segment(p1.get(), p2.get()); }

	/*!
	 * Creates a CGAL segment and a generic interface to a Segment as defined
	 * by this class.
	 * @param p1 - A wrapped CGAL point that defines one end-point of
	 * a wrapped CGAL segment
	 * @param p2 - A wrapped CGAL point that defines the other end-point of
	 * a wrapped CGAL segment
	 */
	Segment(Point &&p1, Point &&p2) { seg = CGAL_Segment(p1.get(), p2.get()); }

	Point operator[](size_t idx) const { return seg[idx]; }

	/*!
	 *
	 * @return Length of the segment
	 */
	NT operator()() const { return seg.squared_length(); }

	/*!
	 *
	 * @return A CGAL segment
	 */
	CGAL_Segment get() const { return seg; }


	friend std::ostream &operator<<(std::ostream &out, Segment<Kernel> &seg) {
		OutputRep<is_string_castable_NT<NT>::value, Kernel, Point> output;
		return out << output(out, seg[0]) << ";" << output(out, seg[1]);
	}
};


/*!
 * @brief A wrapper class for CGAL vector
 * @details This class acts as a wrapper to a CGAL vector by encapsulating
 *  CGAL specific details of vector.
 *  Provides a generic inteface for a vector.
 * @tparam Kernel  - A traits class that defines CGAL geometry types
 */
template <class Kernel>
class Vector {
private:
	using CGAL_Point = typename Kernel::CGAL_Point_;
	using CGAL_Vector = typename Kernel::CGAL_Vector_;
	using Point = typename Kernel::MovetkPoint;
	using container = std::array<typename Kernel::NT, Kernel::dim>;
	CGAL_Vector vec;

public:
	/*!
	 * Creates a CGAL vector and a generic interface to a Vector as defined
	 * by this class.
	 * @param pt - A wrapped CGAL point
	 */
	explicit Vector(const Point &p) { vec = p.get() - CGAL::ORIGIN; }

	explicit Vector(const CGAL_Vector &vec_) : vec(vec_) {}
	Vector() : vec(CGAL::NULL_VECTOR) {}
	Vector(const Vector &) = default;

	typename Kernel::NT operator*(const Vector &vector) const { return this->get() * vector.get(); }

	Vector &operator*=(typename Kernel::NT scalar) {
		vec *= scalar;
		return *this;
	}

	Vector operator+(const Vector &vector) const { return Vector(this->get() + vector.get()); }

	Vector &operator+=(const Vector &vector) {
		vec += vector.vec;
		return *this;
	}
	Vector &operator-=(const Vector &vector) {
		vec -= vector.vec;
		return *this;
	}

	Vector operator-(const Vector &vector) const { return Vector(this->get() - vector.get()); }

	bool operator==(const Vector &vector) const { return std::equal(this->begin(), this->end(), vector.begin()); }

	Vector basis(std::size_t i) const {
		container e = {0};
		e[i] = 1;
		CGAL_Vector basis_vec(Kernel::dim, std::begin(e), std::end(e));
		return Vector(basis_vec);
	}

	/*!
	 *
	 * @return Iterator to the first coordinate of the CGAL vector
	 */
	auto begin() const { return vec.cartesian_begin(); }

	/*!
	 *
	 * @return Iterator to the end of the last coordinate of the CGAL vector
	 */
	auto end() const { return vec.cartesian_end(); }


	/*!
	 *
	 * @return A CGAL vector
	 */
	CGAL_Vector get() const { return vec; }


	/*!
	 * Prints a vector whose components are separated by comma
	 * @param out - OutputStream
	 * @param point - A point of type vector<Kernel>
	 */
	friend std::ostream &operator<<(std::ostream &out, const Vector &vec) {
		OutputRep<is_string_castable_NT<typename Kernel::NT>::value, Kernel, Vector> output;
		return output(out, vec);
	}
};


/*!
 *  @brief A wrapper class for CGAL polygon
 *  @details This class acts as a wrapper to a CGAL polygon by encapsulating
 *  CGAL specific details of polygon.
 *  Provides a generic inteface for a polygon.
 *  @note - Constructs a polygon in 2D
 * @tparam Kernel - A traits class that defines CGAL geometry types
 */
template <class Kernel>
class Polygon {
private:
	using K = typename Kernel::Wrapper_CGAL_Kernel_2::GeometryType_2;
	using Point_2 = typename K::Point_2;
	using CGAL_Polygon = CGAL::Polygon_2<K>;
	CGAL_Polygon polygon;

public:
	/*!
	 * 	Creates a wrapped CGAL polygon
	 */
	Polygon() = default;


	/*!
	 * Creates a CGAL polygon and a generic interface to a polygon as defined
	 * by this class.
	 * @tparam PointIterator - A random access iterator over a set of points
	 * where each point is of type Wrapper_CGAL_Point<Kernel>
	 * @param first - Iterator to the first point in a set of points
	 * where each point is of type Wrapper_CGAL_Point<Kernel>
	 * @param beyond - Iterator to the end of the last point in a set of points
	 * where each point is of type Wrapper_CGAL_Point<Kernel>
	 */
	template <utils::RandomAccessPointIterator<Kernel> PointIterator>
	Polygon(PointIterator first, PointIterator beyond) {
		PointIterator it = first;
		while (it != beyond) {
			auto CoordinateIter = it->begin();
			polygon.push_back(Point_2(*CoordinateIter, *(CoordinateIter + 1)));
			it++;
		}
		if (polygon[0] == polygon[polygon.size() - 1])
			polygon.erase(polygon.vertices_end() - 1);
	}

	/*!
	 *
	 * @return  A CGAL polygon in 2D
	 */
	CGAL_Polygon &get() { return polygon; }

	/*!
	 *
	 * @return  A CGAL polygon in 2D
	 */
	const CGAL_Polygon &get() const { return polygon; }

	/*!
	 *
	 * @return Iterator to the first vertex in a set of points
	 */
	auto v_begin() const { return polygon.vertices_begin(); }

	/*!
	 *
	 * @return Iterator to the end of the last vertex in a set of points
	 */
	auto v_end() const { return polygon.vertices_end(); }

	/*!
	 *
	 * @return Iterator to the first edge in a set of points
	 */
	auto e_begin() const { return polygon.edges_begin(); }

	/*!
	 * @return Iterator to the end of the last edge in a set of points
	 */
	auto e_end() const { return polygon.edges_end(); }

	/*!
	 * Prints a polygon whose vertices are separated by comma. Since
	 * each of these vertices are points, their coordinates are separated
	 * by semicolon
	 * @param out - OutputStream
	 * @param poly - A polygon of type Wrapper_CGAL_Polygon<Kernel>
	 */
	friend std::ostream &operator<<(std::ostream &out, const Polygon<Kernel> &poly) {
		OutputRep<is_string_castable_NT<typename Kernel::NT>::value, Kernel, Polygon<Kernel>> output;
		return (output(out, poly));
	}
};


template <class Kernel>
class Sphere {
private:
	using CGAL_Sphere = typename Kernel::CGAL_Sphere_;
	using CGAL_Point = typename Kernel::CGAL_Point_;
	using Point = typename Kernel::MovetkPoint;
	using Vector = typename Kernel::MovetkVector;
	using NT = typename Kernel::NT;
	std::set<CGAL_Point> points;
	CGAL_Sphere sphere;
	Point _center;
	NT _squared_radius;

public:
	Sphere(const Point &center, NT radius, bool square = true) : _center(center) {
		if (square)
			_squared_radius = std::pow(radius, 2);
		else
			_squared_radius = radius;
	}

	Point center() const { return _center; }

	NT squared_radius() const { return _squared_radius; }

	friend std::ostream &operator<<(std::ostream &out, const Sphere &sphere) {
		OutputRep<is_string_castable_NT<typename Kernel::NT>::value, Kernel, Sphere> output;
		return output(out, sphere);
	}
};


/*! @class Wrapper_Min_Sphere
 *  @brief A wrapper for Minimum Enclosing Ball construction of CGAL
 * @tparam Kernel - A traits class that defines CGAL geometry types
 */
template <class Kernel>
class MinSphere {
private:
	using CGAL_MinSphere = typename Kernel::CGAL_MinSphere_;
	using CGAL_Point = typename Kernel::CGAL_Point_;
	using NT = typename Kernel::NT;
	std::set<CGAL_Point> points;  // TODO: Remove the use of this datastructure
	NT radius;

	/*!
	 * @details Takes a set of points (not necessarily unique) and creates a
	 * unique set of points since it is required by the CGAL MEB
	 * algorithm to converge
	 * @tparam PointIterator - A random access iterator over a set of points
	 * where each point is of type Wrapper_CGAL_Point<Kernel>
	 * @param first - Iterator to the first point in a set of points
	 * where each point is of type Wrapper_CGAL_Point<Kernel>
	 * @param beyond - Iterator to the first point in a set of points
	 * where each point is of type Wrapper_CGAL_Point<Kernel>
	 */
	template <utils::RandomAccessPointIterator<Kernel> PointIterator>
	void to_CGAL_points(PointIterator first, PointIterator beyond) {
		PointIterator it = first;
		while (it != beyond) {
			points.insert((*it).get());
			it++;
		}
	}

public:
	/*!
	 * @brief constructs an MEB from a given set of points
	 * @tparam PointIterator A random access iterator over a set of points
	 * where each point is of type Wrapper_CGAL_Point<Kernel>
	 * @tparam CenterIterator An output iterator that points to location
	 * where a value has to be inserted
	 * @param first - Iterator to the first point in a set of points
	 * where each point is of type Wrapper_CGAL_Point<Kernel>
	 * @param beyond - Iterator to the end of the last point in a set of points
	 * where each point is of type Wrapper_CGAL_Point<Kernel>
	 * @param iter - Iterator to the location where a value has to be inserted
	 * @return Radius of the MEB
	 */
	template <utils::RandomAccessPointIterator<Kernel> PointIterator,
	          utils::OutputIterator<typename Kernel::NT> CenterIterator>
	NT operator()(PointIterator first, PointIterator beyond, CenterIterator iter) const {
		this->to_CGAL_points(first, beyond);
		CGAL_MinSphere ms(begin(points), end(points));
		radius = ms.radius().first + ms.radius().second * sqrt(ms.discriminant());
		for (typename CGAL_MinSphere::Cartesian_const_iterator cit = ms.center_cartesian_begin();
		     cit != ms.center_cartesian_end();
		     cit++) {
			*iter = cit->first + cit->second;
		}
		points.clear();
		return radius;
	}

	/*!
	 * @brief constructs an MEB from a given set of points
	 * @tparam PointIterator A random access iterator over a set of points
	 * where each point is of type Wrapper_CGAL_Point<Kernel>
	 * @param first - Iterator to the first point in a set of points
	 * where each point is of type Wrapper_CGAL_Point<Kernel>
	 * @param beyond - Iterator to the end of the last point in a set of points
	 * where each point is of type Wrapper_CGAL_Point<Kernel>
	 * @return Radius of the MEB
	 */
	template <utils::RandomAccessPointIterator<Kernel> PointIterator>
	NT operator()(PointIterator first, PointIterator beyond) {
		this->to_CGAL_points(first, beyond);
		CGAL_MinSphere ms(begin(points), end(points));
		radius = ms.radius().first + ms.radius().second * sqrt(ms.discriminant());
		points.clear();
		return radius;
	}
};


template <class Kernel>
class CurveIntersection {
private:
	using Point_2 = typename Kernel::Wrapper_CGAL_Kernel_2::Point_2;
	using Curve = typename Kernel::Wrapper_CGAL_Kernel_2::Curve_2;
	std::vector<Curve> Curves;
	std::list<Point_2> Points;

public:
	template <class InputIterator>
	std::size_t operator()(InputIterator first, InputIterator beyond) {
		InputIterator it = first + 1;
		Curves.clear();
		Points.clear();
		while (it != beyond) {
			auto p1 = *(it - 1);
			auto p2 = *it;
			auto v = p2 - p1;
			if ((v * v) < MOVETK_EPS) {
				it++;
				continue;
			}
			auto CoordinateIterP1 = p1.begin();
			auto CoordinateIterP2 = p2.begin();
			Curves.push_back(Curve(Point_2(*CoordinateIterP1, *(CoordinateIterP1 + 1)),
			                       Point_2(*CoordinateIterP2, *(CoordinateIterP2 + 1))));
			it++;
		}
		CGAL::compute_intersection_points(std::begin(Curves), std::end(Curves), std::back_inserter(Points));
		return Points.size();
	}
};


}  // namespace wrappers


namespace CGAL_Algorithms {

/*!
 * @class Intersection_visitor
 * @brief A visitor class for intersection between two geometric objects
 * @details An intersection of two geometric objects can result in a geometric object that
 *  varies based on how two objects intersect. This class provides an interface for
 *  manipulating an object from different geometry types in a uniform manner
 *  For further details , please see: https://www.boost.org/doc/libs/1_64_0/doc/html/variant.html
 * @tparam Kernel - A traits class that defines CGAL geometry types and a variant type
 *  e.g: boost::variant
 */
template <class Kernel>
class Intersection_visitor : public boost::static_visitor<typename Kernel::IntersectionVariant> {
private:
	typedef typename Kernel::CGAL_Point_ CGAL_Point;
	typedef typename Kernel::CGAL_Segment_ CGAL_Segment;
	typedef typename Kernel::CGAL_Line_ CGAL_Line;

public:
	using IntersectionVariant = typename Kernel::IntersectionVariant;
	/*!
	 *
	 * @param p - A CGAL point
	 * @return  A wrapped CGAL point
	 */
	IntersectionVariant operator()(const CGAL_Point &p) const {
		typename Kernel::MovetkPoint wrapper_point(p);
		return wrapper_point;
	}

	/*!
	 *
	 * @param s - A CGAL segment
	 * @return  A wrapped CGAL segment
	 */
	IntersectionVariant operator()(const CGAL_Segment &s) const {
		typename Kernel::MovetkSegment wrapper_seg(s);
		return wrapper_seg;
	}

	/*!
	 *
	 * @param l - A CGAL line
	 * @return A wrapped CGAL line
	 */
	IntersectionVariant operator()(const CGAL_Line &l) const {
		typename Kernel::MovetkLine line(l);
		return line;
	}
};

/*!
 * Check whether two lines intersect
 * @tparam Kernel - A traits class that defines CGAL geometry types
 * @param Line1 - A wrapped CGAL line
 * @param Line2 - A wrapped CGAL line
 * @return True / False
 */
template <class Kernel>
bool do_intersect(const wrappers::Line<Kernel> &Line1, const wrappers::Line<Kernel> &Line2) {
	return CGAL::do_intersect(Line1.get(), Line2.get());
}

/*!
 * Get the result of intersection of two lines
 * @tparam Kernel - A traits class that defines CGAL geometry types
 * @param Line1 - A wrapped CGAL line
 * @param Line2 - A wrapped CGAL line
 * @return A CGAL geometric object
 */
template <class Kernel>
auto intersect(const wrappers::Line<Kernel> &Line1, const wrappers::Line<Kernel> &Line2) {
	return intersection(Line1.get(), Line2.get());  // intersection() is part of <CGAL/intersections_d.h>
}

/*!
 * Check whether a point belongs to a polygon
 * @tparam Kernel - A traits class that defines CGAL geometry types
 * @param Point - A wrapped CGAL point
 * @param Polygon - A wrapped CGAL polygon
 * @return True / False
 */
template <class Kernel>
bool point_in_polygon(const wrappers::Point<Kernel> &Point, const wrappers::Polygon<Kernel> &Polygon) {
	typedef typename Kernel::Wrapper_CGAL_Kernel_2::GeometryType_2 K;
	typedef typename K::Point_2 Point_2;
	auto CoordinateIter = Point.begin();
	return ((Polygon.get()).bounded_side(Point_2(*CoordinateIter, *(CoordinateIter + 1))) == CGAL::ON_BOUNDED_SIDE);
}

};  // namespace CGAL_Algorithms

/**
 * @brief A traits class that defines CGAL geometry types and its wrappers
 * @tparam Kernel The CGAL Kernel
 */
template <class CGAL_Kernel>
struct MovetkCGALKernel {
	// CGAL wrapping types
	using SphereTraits = typename CGAL_Kernel::SphereTraits;
	using CGAL_MinSphere_ = typename CGAL::Min_sphere_of_spheres_d<SphereTraits>;
	using GeometryType = typename CGAL_Kernel::GeometryType;
	using CGAL_Point_ = typename GeometryType::Point_d;
	using CGAL_Vector_ = typename GeometryType::Vector_d;
	using CGAL_Line_ = typename GeometryType::Line_d;
	using CGAL_Segment_ = typename GeometryType::Segment_d;
	using CGAL_Sphere_ = typename GeometryType::Sphere_d;

	// Movetk kernel interface
	using NT = typename CGAL_Kernel::NT;
	constexpr static size_t dim = CGAL_Kernel::dim;
	using MovetkPoint = wrappers::Point<MovetkCGALKernel>;
	using MovetkLine = wrappers::Line<MovetkCGALKernel>;
	using MovetkSegment = wrappers::Segment<MovetkCGALKernel>;
	using MovetkVector = wrappers::Vector<MovetkCGALKernel>;
	using MovetkPolygon = wrappers::Polygon<MovetkCGALKernel>;
	using MovetkSphere = wrappers::Sphere<MovetkCGALKernel>;
	using MovetkMinSphere = wrappers::MinSphere<MovetkCGALKernel>;
	using MovetkIntersectionVisitor = CGAL_Algorithms::Intersection_visitor<MovetkCGALKernel>;
	using MovetkCurveIntersection = wrappers::CurveIntersection<MovetkCGALKernel>;
	using MovetkSquaredDistance = void;
	using MovetkDiscreteHausdorffDistance = void;
	using MovetkDiscreteFrechetDistance = void;
	using MovetkMinimumBoundingRectangle = void;

	// Other CGAL types
	using IntersectionVariant = boost::variant<MovetkPoint, MovetkLine, MovetkSegment>;

	struct Wrapper_CGAL_Kernel_2 {
		using GeometryType_2 = CGAL::Simple_cartesian<NT>;
		using Arrangement_Traits_2 = CGAL::Arr_segment_traits_2<GeometryType_2>;
		using Point_2 = typename GeometryType_2::Point_2;
		using Vector_2 = typename GeometryType_2::Vector_2;
		typedef typename GeometryType_2::Line_2 Line_2;
		typedef typename GeometryType_2::Segment_2 Segment_2;
		typedef typename Arrangement_Traits_2::Curve_2 Curve_2;
	};
};
}  // namespace movetk::backends::cgal

#endif /* GEOMETRY_H */
