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

#include "movetk/utils/Asserts.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/Requirements.h"
#include "movetk/utils/StringUtils.h"
#include "third_party/boost_geometry/discrete_frechet_distance.hpp"
#include "third_party/boost_geometry/discrete_hausdorff_distance.hpp"
#include "third_party/miniball/Seb.h"

// TODO Rename this to a Wrapper

namespace movetk::geom {

namespace bg = boost::geometry;

template <class Kernel>
class Wrapper_Boost_Point {
private:
	typedef typename Kernel::Boost_Point_ Boost_Point;
	typedef typename Kernel::NT NT;
	typedef typename utils::movetk_basic_iterator<const NT> CoordinateIterator;
	typedef typename Kernel::Wrapper_Vector Wrapper_Vector;
	typedef std::array<typename Kernel::NT, Kernel::dim> Point_Container;
	Boost_Point pt;
	Wrapper_Boost_Point(Point_Container &&p) {
		pt.template set<0>(std::move(p[0]));
		pt.template set<1>(std::move(p[1]));
		if constexpr (Kernel::dim == 3) {
			pt.template set<2>(std::move(p[2]));
		}
	}

public:
	Wrapper_Boost_Point() = default;

	Wrapper_Boost_Point(const Boost_Point &p) : pt(p) {}

	template <class CoordinateIterator, typename = movetk::utils::requires_random_access_iterator<CoordinateIterator>>
	Wrapper_Boost_Point(CoordinateIterator first, CoordinateIterator beyond) {
		// ASSERT_NUMBER_TYPE(Kernel, first);
		pt.template set<0>(*first);
		pt.template set<1>(*(first + 1));
		if constexpr (Kernel::dim == 3) {
			pt.template set<2>(*(first + 2));
		}
	}

	auto begin() const {
		auto __addr = std::addressof(pt.template get<0>());
		CoordinateIterator first(__addr);
		return first;
	}

	auto end() const {
		auto __addr = std::addressof(pt.template get<0>()) + Kernel::dim;
		CoordinateIterator beyond(__addr);
		return beyond;
	}

	const NT &operator[](size_t i) const {
		auto __addr = std::addressof(pt.template get<0>());
		CoordinateIterator first(__addr + i);
		return *first;
	}

	NT &operator[](size_t i) {
		auto __addr = std::addressof(pt.template get<0>());
		CoordinateIterator first(__addr + i);
		return *first;
	}

	Wrapper_Vector operator-(const Wrapper_Boost_Point &point) const {
		Wrapper_Vector v1(*this);
		Wrapper_Vector v2(point);
		return v1 - v2;
	}

	Wrapper_Boost_Point operator+(const Wrapper_Vector &v) const {
		Point_Container result;
		std::transform(this->begin(), this->end(), v.begin(), result.begin(), std::plus<typename Kernel::NT>());
		Wrapper_Boost_Point _point(std::move(result));
		return _point;
	}

	Boost_Point get() const { return pt; }
};

template <class Kernel>
std::ostream &operator<<(std::ostream &out, const Wrapper_Boost_Point<Kernel> &point) {
	return (out << movetk::utils::join(point.begin(), point.end()));
}

template <class Kernel>
class Wrapper_Boost_Segment {
private:
	typedef typename Kernel::Boost_Segment_ Boost_Segment;
	typedef typename Kernel::NT NT;
	Boost_Segment seg;

public:
	Wrapper_Boost_Segment() = default;

	Wrapper_Boost_Segment(const Boost_Segment &s) : seg(s) {}

	Wrapper_Boost_Segment(Wrapper_Boost_Point<Kernel> &p1, Wrapper_Boost_Point<Kernel> &p2) {
		seg = Boost_Segment(p1.get(), p2.get());
	}

	Wrapper_Boost_Segment(Wrapper_Boost_Point<Kernel> &&p1, Wrapper_Boost_Point<Kernel> &&p2) {
		seg = Boost_Segment(p1.get(), p2.get());
	}

	typename Kernel::Wrapper_Point operator[](size_t idx) const {
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
};

template <class Kernel>
std::ostream &operator<<(std::ostream &out, Wrapper_Boost_Segment<Kernel> &seg) {
	Wrapper_Boost_Point<Kernel> p1 = seg[0];
	Wrapper_Boost_Point<Kernel> p2 = seg[1];
	out << p1;
	out << ";";
	out << p2;
	return out;
}

template <class Kernel>
class Wrapper_Boost_Line {
private:
	typedef typename Kernel::Boost_PolyLine_ Boost_Line;
	typedef typename Kernel::NT NT;
	Boost_Line line;

public:
	Wrapper_Boost_Line() = default;

	Wrapper_Boost_Line(const Boost_Line &l) : line(l) {}

	Wrapper_Boost_Line(Wrapper_Boost_Point<Kernel> &p1, Wrapper_Boost_Point<Kernel> &p2) {
		line = Boost_Line({p1.get(), p2.get()});
	}

	Boost_Line get() const { return line; }
};

template <class Kernel>
class Wrapper_Boost_Vector {
	typedef std::array<typename Kernel::NT, Kernel::dim> Boost_Vector;
	typedef std::array<typename Kernel::NT, Kernel::dim> basis_container;
	Boost_Vector vec;

	Wrapper_Boost_Vector(Boost_Vector &&vector) : vec(std::move(vector)) {  }
	Wrapper_Boost_Vector(const Boost_Vector &vector) : vec(vector) { }

public:
	explicit Wrapper_Boost_Vector() = default;

	Wrapper_Boost_Vector(const Wrapper_Boost_Point<Kernel> &p) { std::copy_n(p.begin(), Kernel::dim, vec.begin()); }

	Wrapper_Boost_Vector<Kernel> operator*(typename Kernel::NT scalar) const {
		Wrapper_Boost_Vector<Kernel> copy(*this);
		copy *= scalar;
		return copy;
	}
	Wrapper_Boost_Vector<Kernel> &operator*=(typename Kernel::NT scalar) {
		std::for_each(std::begin(vec), std::end(vec), [&scalar](typename Kernel::NT &i) { i *= scalar; });
		return *this;
	}

	Wrapper_Boost_Vector<Kernel> operator/(typename Kernel::NT scalar) const {
		Wrapper_Boost_Vector<Kernel> copy(*this);
		copy /= scalar;
		return copy;
	}
	Wrapper_Boost_Vector<Kernel> &operator/=(typename Kernel::NT scalar) {
		std::for_each(std::begin(vec), std::end(vec), [&scalar](typename Kernel::NT &i) { i /= scalar; });
		return *this;
	}

	/**
	 * @brief Inner product operator
	 * @param vector Other boost vector
	 * @return The inner product between this and the other vector
	 */
	typename Kernel::NT operator*(const Wrapper_Boost_Vector<Kernel> &vector) const {
		return std::inner_product(this->begin(), this->end(), vector.begin(), 0.0);
	}

	Wrapper_Boost_Vector<Kernel> operator-(const Wrapper_Boost_Vector<Kernel> &vector) const {
		Wrapper_Boost_Vector<Kernel> copy(*this);
		copy -= vector;
		return copy;
	}
	Wrapper_Boost_Vector<Kernel> &operator-=(const Wrapper_Boost_Vector<Kernel> &vector) {
		for (std::size_t i = 0; i < vec.size(); ++i) {
			vec[i] -= vector.vec[i];
		}
		return *this;
	}

	Wrapper_Boost_Vector<Kernel> operator+(const Wrapper_Boost_Vector<Kernel> &vector) const {
		Wrapper_Boost_Vector copy(*this);
		copy += vector;
		return copy;
	}
	Wrapper_Boost_Vector<Kernel> &operator+=(const Wrapper_Boost_Vector<Kernel> &vector) {
		for (std::size_t i = 0; i < vec.size(); ++i) {
			vec[i] += vector.vec[i];
		}
		return *this;
	}

	bool operator==(const Wrapper_Boost_Vector<Kernel> &vector) const {
		return std::equal(this->begin(), this->end(), vector.begin());
	}
	bool operator!=(const Wrapper_Boost_Vector<Kernel> &vector) const { return !((*this) == vector);
	}

	Wrapper_Boost_Vector<Kernel> basis(std::size_t i) const {
		Boost_Vector _e = {0};
		_e[i] = 1;
		Wrapper_Boost_Vector basis_vec(std::move(_e));
		return basis_vec;
	}

	auto begin() const { return std::cbegin(vec); }

	auto end() const { return std::cend(vec); }

	Boost_Vector get() const { return vec; }
};

template <class Kernel>
std::ostream &operator<<(std::ostream &out, const Wrapper_Boost_Vector<Kernel> &vec) {
	return (out << movetk_support::join(vec.begin(), vec.end()));
}

template <class Kernel>
class Wrapper_SEB {
private:
	typedef typename Kernel::NT NT;
	typedef typename Kernel::Wrapper_Point Point;
	typedef Seb::Smallest_enclosing_ball<NT, Point> miniball;
	template <class T>
	static std::pair<Point, NT> dispatcher(const size_t &dimensions, T &&values) {
		miniball mb(dimensions, std::forward<T>(values));
		Point pt(mb.center_begin(), mb.center_end());
		return std::make_pair(pt, mb.radius());
	}

public:
	template <class PointIterator,
	          class CenterIterator,
	          typename = movetk::utils::requires_random_access_iterator<PointIterator>,
	          typename = movetk::utils::requires_wrapper_point<Kernel, typename PointIterator::value_type>,
	          typename = movetk::utils::requires_output_iterator<CenterIterator>,
	          typename = movetk::utils::requires_NT<Kernel, typename CenterIterator::value_type>>
	NT operator()(PointIterator first, PointIterator beyond, CenterIterator iter) const {
		auto result = dispatcher(std::distance(first->begin(), first->end()), std::vector<Point>(first, beyond));
		auto cit = result.first.begin();
		while (cit != result.first.end()) {
			iter = *cit;
			cit++;
		}
		return result.second;
	}

	template <class PointIterator,
	          typename = movetk::utils::requires_random_access_iterator<PointIterator>,
	          typename = movetk::utils::requires_wrapper_point<Kernel, typename PointIterator::value_type>>
	NT operator()(PointIterator first, PointIterator beyond) const {
		auto result = dispatcher(std::distance(first->begin(), first->end()), std::vector<Point>(first, beyond));
		return result.second;
	}
};

template <class Kernel>
class Wrapper_Boost_Polygon {
private:
	typedef typename Kernel::Boost_Polygon_ Boost_Polygon;
	typedef typename Kernel::NT NT;
	Boost_Polygon polygon;

public:
	Wrapper_Boost_Polygon() = default;

	template <class PointIterator,
	          typename = movetk::utils::requires_random_access_iterator<PointIterator>,
	          typename = movetk::utils::requires_wrapper_point<Kernel, typename PointIterator::value_type>>
	Wrapper_Boost_Polygon(PointIterator first, PointIterator beyond) {
		PointIterator it = first;
		while (it != beyond) {
			bg::append(polygon.outer(), it->get());
			it++;
		}
	}

	auto v_begin() const { return polygon.outer().cbegin(); }

	auto v_end() const { return polygon.outer().cend(); }

	Boost_Polygon get() const { return polygon; }
};

/*!
 * Prints a polygon whose vertices are separated by comma. Since
 * each of these vertices are points, their coordinates are separated
 * by semicolon
 * @param out - OutputStream
 * @param poly - A polygon of type Wrapper_Boost_Polygon<Kernel>
 */
template <class Kernel>
std::ostream &operator<<(std::ostream &out, Wrapper_Boost_Polygon<Kernel> &poly) {
	typedef typename Kernel::Wrapper_Point Point;
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

template <class Kernel>
struct Wrapper_Boost_Squared_Distance {
	template <class T1, class T2>
	typename Kernel::NT operator()(T1 &geometry1, T2 &geometry2) {
		typename Kernel::NT distance = bg::distance(geometry1.get(), geometry2.get());
		return distance * distance;
	}
};

template <class Kernel>
struct Wrapper_Boost_Discrete_Hausdorff_Distance {
	template <class InputIterator,
	          typename = movetk::utils::requires_random_access_iterator<InputIterator>,
	          typename = movetk::utils::requires_wrapper_point<Kernel, typename InputIterator::value_type>>
	typename Kernel::NT operator()(InputIterator polyline_a_first,
	                               InputIterator polyline_a_beyond,
	                               InputIterator polyline_b_first,
	                               InputIterator polyline_b_beyond) {
		typename Kernel::Boost_PolyLine_ poly1, poly2;

		InputIterator it = polyline_a_first;
		while (it != polyline_a_beyond) {
			poly1.push_back(it->get());
			it++;
		}
		it = polyline_b_first;
		while (it != polyline_b_beyond) {
			poly2.push_back(it->get());
			it++;
		}

		return bg::discrete_hausdorff_distance(poly1, poly2);
	}
};

template <class Kernel>
struct Wrapper_Boost_Discrete_Frechet_Distance {
	template <class InputIterator,
	          typename = movetk::utils::requires_random_access_iterator<InputIterator>,
	          typename = movetk::utils::requires_wrapper_point<Kernel, typename InputIterator::value_type>>
	typename Kernel::NT operator()(InputIterator polyline_a_first,
	                               InputIterator polyline_a_beyond,
	                               InputIterator polyline_b_first,
	                               InputIterator polyline_b_beyond) {
		typename Kernel::Boost_PolyLine_ poly1, poly2;

		InputIterator it = polyline_a_first;
		while (it != polyline_a_beyond) {
			poly1.push_back(it->get());
			it++;
		}
		it = polyline_b_first;
		while (it != polyline_b_beyond) {
			poly2.push_back(it->get());
			it++;
		}

		return bg::discrete_frechet_distance(poly1, poly2);
	}
};

template <class Kernel>
class Wrapper_Boost_Sphere {
private:
	typedef typename Kernel::Wrapper_Point Wrapper_Point;
	typedef typename Kernel::NT NT;
	Wrapper_Point _center;
	NT _squared_radius;

public:
	Wrapper_Boost_Sphere() = default;

	Wrapper_Boost_Sphere(Wrapper_Point &center, NT radius, bool square = true) {
		_center = center;
		if (square)
			_squared_radius = std::pow(radius, 2);
		else
			_squared_radius = radius;
	}
	Wrapper_Point center() const { return _center; }

	NT squared_radius() const { return _squared_radius; }
};

template <class Kernel>
std::ostream &operator<<(std::ostream &out, const Wrapper_Boost_Sphere<Kernel> &sphere) {
	Wrapper_Boost_Point<Kernel> center = sphere.center();
	out << movetk_support::join(center.begin(), center.end()) + ";";
	out << sphere.squared_radius();
	return out;
}

template <class Boost_Kernel>
struct Wrapper_Boost_Kernel {
	typedef typename Boost_Kernel::NT NT;
	constexpr static size_t dim = Boost_Kernel::dim;
	typedef typename Boost_Kernel::Point_d Boost_Point_;
	typedef typename bg::model::segment<Boost_Point_> Boost_Segment_;
	typedef typename bg::model::linestring<Boost_Point_> Boost_PolyLine_;
	typedef typename bg::model::polygon<Boost_Point_> Boost_Polygon_;
	typedef Wrapper_Boost_Point<Wrapper_Boost_Kernel> Wrapper_Point;
	typedef Wrapper_Boost_Segment<Wrapper_Boost_Kernel> Wrapper_Segment;
	typedef Wrapper_Boost_Line<Wrapper_Boost_Kernel> Wrapper_Line;
	typedef Wrapper_Boost_Polygon<Wrapper_Boost_Kernel> Wrapper_Polygon;
	typedef Wrapper_SEB<Wrapper_Boost_Kernel> Wrapper_MinSphere;
	typedef Wrapper_Boost_Vector<Wrapper_Boost_Kernel> Wrapper_Vector;
	typedef Wrapper_Boost_Sphere<Wrapper_Boost_Kernel> Wrapper_Sphere;
	typedef Wrapper_Boost_Squared_Distance<Wrapper_Boost_Kernel> Wrapper_Squared_Distance;
	typedef Wrapper_Boost_Discrete_Hausdorff_Distance<Wrapper_Boost_Kernel> Wrapper_Discrete_Hausdorff_Distance;
	typedef Wrapper_Boost_Discrete_Frechet_Distance<Wrapper_Boost_Kernel> Wrapper_Discrete_Frechet_Distance;
	typedef void Intersection_visitor;
	typedef void Wrapper_Curve_Intersection;
	typedef void Wrapper_Minimum_Bounding_Rectangle;
};

};  // namespace movetk_support

#endif  // MOVETK_BOOSTGEOMETRYADAPTER_H
