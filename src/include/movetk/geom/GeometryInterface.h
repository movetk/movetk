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
#ifndef MOVETK_INTERFACE_H
#define MOVETK_INTERFACE_H

#include <math.h>

#include <array>
#include <iostream>

#include "GeometryConcepts.h"
#include "Intersections.h"
#include "ObjectCreation.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/Requirements.h"
#include "movetk/utils/TypeChecks.h"

#define PI 3.14159265
#define TWO_PI 6.2831853
#define LOG_TWO_PI 1.837877066409345

/*!
 *  @namespace movetk::geom
 *  @brief Defines geometric primitives for usage in MoveTK
 *  @details A collection of classes that provide a generic
 *  interface for construction of different geometric types
 *  by decoupling the underlying geometry library
 *  (e.g CGAL, Boost etc.) from the interface
 *  @authors Aniket Mitra (aniket.mitra@here.com)
 */
namespace movetk::geom {
/*!
 * @brief Converts from degree to radians
 * @tparam NT Type of the number to use
 * @param degrees The degrees
 * @return Angle in radians
 */
template <class NT>
NT deg2radians(const NT degrees) {
	return degrees * PI / 180.0;
}

/*!
 * @brief Converts from radians to degrees
 * @tparam NT The number type to use
 * @param radians Radians angle
 * @return Angle in degrees
 */
template <class NT>
NT rad2deg(const NT radians) {
	return radians * 180.0 / PI;
}

/**
 * @namespace movetk::backends
 * @brief Collection of supported MoveTK backends.
 */

/**
 * @brief Class representing a Wedge.
 * @details A wedge is defined by a center point and two infinite rays, where all points between
 * the rays are considered to be part of the wedge.
 */
template <utils::KernelSatisfying<utils::is_planar_geometry2> GeometryTraits, utils::L2Norm Norm>
class Wedge {
public:
	using MovetkVector = typename GeometryTraits::MovetkVector;
	using MovetkPoint = typename GeometryTraits::MovetkPoint;

private:
	using NT = typename GeometryTraits::NT;
	Norm norm;
	static inline geom::MakePoint<GeometryTraits> make_point = {};
	static inline MovetkPoint ORIGIN = make_point({0, 0});
	MovetkVector e1 = make_point({1, 0}) - ORIGIN;
	MovetkVector e2 = make_point({0, 1}) - ORIGIN;
	MovetkVector _slope = ORIGIN - ORIGIN;
	MovetkVector _intercept = ORIGIN - ORIGIN;
	enum class Direction : int8_t { NEGATIVE, POSITIVE, ZERO };
	Direction wedge_directions[2] = {Direction::ZERO, Direction::ZERO};
	bool horizontal = false, vertical = false;
	bool degenerate = false;
	bool upper_right = false, lower_left = false;
	bool lower_right = false, upper_left = false;

	/**
	 * @brief Sets the boolean direction flags, using the center ray direction of the wedge
	 * @param ray_x The x amount of the ray
	 * @param ray_y The y amount of the ray
	 */
	void set_direction_from_ray(NT ray_x, NT ray_y) {
		if (abs(ray_x) < MOVETK_EPS) {
			vertical = true;
		} else {
			wedge_directions[0] = ray_x > 0 ? Direction::POSITIVE : Direction::NEGATIVE;
		}
		if (abs(ray_y) < MOVETK_EPS) {
			horizontal = true;
		} else {
			wedge_directions[1] = ray_y > 0 ? Direction::POSITIVE : Direction::NEGATIVE;
		}

		if (ray_x >= 0 && ray_y >= 0) {
			upper_right = true;
		}
		if (ray_y <= 0 && ray_x >= 0) {
			lower_right = true;
		}
		if (ray_y >= 0 && ray_x <= 0) {
			upper_left = true;
		}
		if (ray_x <= 0 && ray_y <= 0) {
			lower_left = true;
		}
	}

	static auto make_vector(NT x, NT y) { return make_point({x, y}) - ORIGIN; }
	static auto make_vector(const MovetkPoint& p) { return p - ORIGIN; }

	void compute_slopes_from_radius_and_tangent_length_squared(NT ray_slope, NT radius, NT tangent_squared_length) {
		// Degenerate wedge has no slopes.
		if (degenerate) {
			_slope = ORIGIN - ORIGIN;
			return;
		}
		// Tangent of the angle between the center ray and a wedge line
		NT tanB = radius / sqrt(tangent_squared_length);
		NT m1, m2;
		// Handle degenerate central ray directions
		if (horizontal || wedge_directions[1] == Direction::ZERO) {
			// Ray pointing to the left
			if (wedge_directions[0] == Direction::NEGATIVE) {
				// Flip the slopes to be correct again.
				tanB *= -1;
			}
			_slope = make_vector(tanB, -tanB);
		} else if (vertical) {
			NT tanA = tan(PI / 2 - MOVETK_EPS * 0.001);
			if ((tanA * tanB) == 1)
				m1 = (tanA + tanB) / MOVETK_EPS;
			else
				m1 = (tanA + tanB) / (1 - tanA * tanB);
			m2 = -m1;
			_slope = make_vector(m1, m2);
		} else {
			// Compute the slopes of the lines of the wedge,
			// using tan(a+b)=(tan(a)+tan(b))/(1-tan(a)tan(b)) and
			// similar for tan(a-b).
			if ((ray_slope * tanB) == 1) {
				m1 = (ray_slope + tanB) / MOVETK_EPS;
				m2 = (ray_slope - tanB) / (1 + ray_slope * tanB);
			} else if ((ray_slope * tanB) == -1) {
				m1 = (ray_slope + tanB) / (1 - ray_slope * tanB);
				m2 = (ray_slope - tanB) / (-1 * MOVETK_EPS);
			} else {
				m1 = (ray_slope + tanB) / (1 - ray_slope * tanB);
				m2 = (ray_slope - tanB) / (1 + ray_slope * tanB);
			}

			if (upper_right || lower_right) {
				_slope = make_vector(m1, m2);
			} else if (upper_left || lower_left) {
				_slope = make_vector(m2, m1);
			}
		}
	}


	void construct(const MovetkPoint& p, const MovetkPoint& center, NT radius) {
		NT c1, c2, tanA, tanB;
		const auto squared_radius = radius * radius;
		const auto _slope_ray = center - p;

		NT segment_squared_length = norm(_slope_ray);
		NT root = norm ^ 1;
		if (segment_squared_length < squared_radius || root < MOVETK_EPS) {
			degenerate = true;
		}
		const auto v_x = _slope_ray * e1;
		const auto v_y = _slope_ray * e2;
		set_direction_from_ray(v_x, v_y);

		NT tangent_squared_length = segment_squared_length - squared_radius;
		if (abs(tangent_squared_length) < MOVETK_EPS) {
			degenerate = true;
		}
		const auto ray_slope = degenerate ? static_cast<NT>(0) : v_y / v_x;
		compute_slopes_from_radius_and_tangent_length_squared(ray_slope, radius, tangent_squared_length);

		c1 = make_vector(-1 * (_slope * e1), 1) * (p - ORIGIN);  // (y - y0) = m * (x - x0)
		c2 = make_vector(-1 * (_slope * e2), 1) * (p - ORIGIN);
		_intercept = make_point({c1, c2}) - ORIGIN;
	}

public:
	Wedge() = default;
	/**
	 * @brief Constructs the wedge from a point, center and radius.
	 * The lines of the wedge are determine as follows: we construct a ray from the point to
	 * the center. The wedge lines are then located such that the locations on the line that
	 * intersect the perpendicular line on the center point are of length radius.
	 * @param p The point to start the wedge ray from, and becomes the origin of the wedge.
	 * @param center The center of the (finite length) wedge
	 * @param radius The radius of the wedge lines
	 */
	Wedge(const MovetkPoint& p, const MovetkPoint& center, NT radius) { construct(p, center, radius); }

	/**
	 * @brief Construct a wedge by directly setting the slopes and intersects of the lines defining the
	 * wedge
	 * @param slopes The slopes
	 * @param intercepts The intercepts
	 */
	Wedge(const MovetkVector& slopes, const MovetkVector& intercepts) : _slope(slopes), _intercept(intercepts) {}

	/**
	 * @brief Get the slopes of the wedge lines
	 * @return The slopes
	 */
	const MovetkVector& slope() const { return _slope; }
	/**
	 * @brief Get the intercepts of the wedge lines
	 * @return The intercepts
	 */
	const MovetkVector& intercept() const { return _intercept; }
	/**
	 * @brief Check if the wedge is empty
	 * @return Is the wedge empty.
	 */
	bool is_empty() const { return _slope == (ORIGIN - ORIGIN) && _intercept == (ORIGIN - ORIGIN); }

	/**
	 * @brief Computes the intersection of the current wedge with another
	 * @param w The other wedge
	 * @return A Wedge representing the intersection of the two wedges (possibly empty).
	 */
	Wedge operator*(const Wedge& w) const {
		std::array<std::size_t, 4> positions = {0, 1, 2, 3};
		std::size_t sum = *(positions.end() - 1) + *(positions.end() - 2);
		typename GeometryTraits::MovetkVector that_slope = w.slope();
		typename GeometryTraits::MovetkVector that_intercept = w.intercept();
		std::array<NT, 4> slopes = {this->_slope * e1, this->_slope * e2, that_slope * e1, that_slope * e2};
		std::array<NT, 4> intercepts = {this->_intercept * e1,
		                                this->_intercept * e2,
		                                that_intercept * e1,
		                                that_intercept * e2};

		std::sort(std::begin(positions), std::end(positions), [&slopes](size_t i, size_t j) {
			return slopes[i] < slopes[j];
		});

		auto start = positions.cbegin();
		auto end = positions.cend();
		auto sit = slopes.cbegin();
		auto it = intercepts.cbegin();

		NT diff_this = (slopes[0] - slopes[1]) / (1 + (slopes[0] * slopes[1]));
		NT diff_that = (slopes[2] - slopes[3]) / (1 + (slopes[2] * slopes[3]));
		NT WedgeAngle_this = 2 * movetk::geom::rad2deg(diff_this);
		NT WedgeAngle_that = 2 * movetk::geom::rad2deg(diff_that);

		if ((*start + *(start + 1) == sum) || (*(end - 1) + *(end - 2) == sum)) {
			if ((abs(WedgeAngle_this) < 90) || (abs(WedgeAngle_that) < 90))
				return Wedge();
		}

		if (slopes[0] > slopes[1]) {
			that_slope = make_point({*(sit + *(start + 2)), *(sit + *(start + 1))}) - ORIGIN;
			that_intercept = make_point({*(it + *(start + 2)), *(it + *(start + 1))}) - ORIGIN;
		} else {
			that_slope = make_point({*(sit + *(start + 1)), *(sit + *(start + 2))}) - ORIGIN;
			that_intercept = make_point({*(it + *(start + 1)), *(it + *(start + 2))}) - ORIGIN;
		}
		return Wedge(that_slope, that_intercept);
	}

	/**
	 * @brief Determines whether a point p is located inside the wedge
	 * @param p The point
	 * @return Is the point inside the wedge.
	 */
	bool operator*(const MovetkPoint& p) const {
		NT mx1 = (make_point({this->_slope * this->e1, -1}) - ORIGIN) * (p - ORIGIN);
		NT mx2 = (make_point({this->_slope * this->e2, -1}) - ORIGIN) * (p - ORIGIN);
		typename GeometryTraits::MovetkVector v = make_point({mx1, mx2}) - ORIGIN;
		typename GeometryTraits::MovetkVector result = v + this->_intercept;

		if ((result * this->e1) >= 0 && (result * this->e2) <= 0)
			return true;

		if ((result * this->e1) <= 0 && (result * this->e2) >= 0)
			return true;

		return false;
	}
};

template <class GeometryTraits, class Norm>
std::ostream& operator<<(std::ostream& out, Wedge<GeometryTraits, Norm>& wedge) {
	MakePoint<GeometryTraits> make_point;
	const auto ORIGIN = make_point({0, 0});
	const auto e1 = make_point({1, 0}) - ORIGIN;
	const auto e2 = make_point({0, 1}) - ORIGIN;
	const auto slope = wedge.slope();
	const auto intercept = wedge.intercept();
	const auto result_m1 = slope * e1;
	const auto result_m2 = slope * e2;
	const auto result_c1 = intercept * e1;
	const auto result_c2 = intercept * e2;
	out << "Line1 slope: " << result_m1 << ", Line1 intercept: " << result_c1 << "\n";
	out << "Line2 slope: " << result_m2 << ", Line2 intercept: " << result_c2 << "\n";
	return out;
}

/**
 * @brief Class representing the minimum bounding rectangle algorithm.
 * Currently computes the minimum bounding rectangle for intersection of spheres.
 * @tparam Norm Norm to use for length computations of vectors
 */
template <utils::KernelSatisfying<utils::is_planar_geometry2> GeometryTraits,
          utils::L2Norm Norm>
class MBR {
private:
	using NT = typename GeometryTraits::NT;
	using Point = typename GeometryTraits::MovetkPoint;
	using Vector = typename GeometryTraits::MovetkVector;
	using Sphere = typename GeometryTraits::MovetkSphere;
	using IntersectionTraits = geom::IntersectionTraits<GeometryTraits, Norm, geom::sphere_sphere_intersection_tag>;
	typename IntersectionTraits::Norm norm;

	geom::MakePoint<GeometryTraits> make_point;
	geom::MakeSphere<GeometryTraits> make_sphere;
	geom::ComputeIntersections<IntersectionTraits> compute_intersections;

	Point ORIGIN = make_point({0, 0});

	/**
	 * @brief Returns the x  coordinate of the vector
	 * @param v The vector
	 * @return The x coordinate
	 */
	NT get_x(const Vector& v) const { return v * v.basis(0); }

	NT get_x(const Point& p) const {
		Vector v = p - ORIGIN;
		return v * v.basis(0);
	}

	NT get_y(const Vector& v) const { return v * v.basis(1); }

	NT get_y(const Point& p) const {
		Vector v = p - ORIGIN;
		return v * v.basis(1);
	}

	NT get_length(const Point& p_u, const Point& p_v) const {
		Vector direction = p_v - p_u;
		norm(direction);
		NT length = norm ^ 1;
		return length;
	}

	NT get_length(const Vector& direction) const {
		norm(direction);
		NT length = norm ^ 1;
		return length;
	}

	Vector get_direction_vector(const Point& p_u, const Point& p_v) const {
		return ((p_v - p_u) *= (1 / get_length(p_u, p_v)));
	}

	Vector get_direction_vector(Vector& v) const {
		Vector direction = v;
		return (v *= (1 / get_length(direction)));
	}

	Point translate(Vector unit_vector, const Point& start, NT translation_length) const {
		unit_vector *= translation_length;
		return start + unit_vector;
	}

public:
	/**
	 * @brief Computes the minimum bounding rectangle of the intersection of two spheres
	 * Returns a pair of the same points if no intersection is present.
	 * todo(bram): missing one degree of freedom here...
	 * @param p_u Center of first sphere
	 * @param p_v Center of second sphere
	 * @param radius_u Radius of first sphere
	 * @param radius_v Radius of second sphere
	 * @return Diagonally opposite points of the box.
	 */
	std::pair<Point, Point> operator()(const Point& p_u, const Point& p_v, NT radius_u, NT radius_v) const {
		Sphere sphere_u = make_sphere(p_u, radius_u);
		Sphere sphere_v = make_sphere(p_v, radius_v);
		Sphere intersection_sphere = compute_intersections(sphere_u, sphere_v);
		Point center = intersection_sphere.center();

		if (intersection_sphere.squared_radius() == 0)
			return std::make_pair(center, center);

		NT half_length = std::pow(intersection_sphere.squared_radius(), 0.5);

		Vector direction_uv = get_direction_vector(p_u, p_v);
		Point point_sphere_u = translate(direction_uv, p_u, radius_u);
		Point point_sphere_v = translate(direction_uv, p_v, -radius_v);

		// counterclockwise rotation by pi/2
		Vector direction_p = make_point({-1 * get_y(direction_uv), get_x(direction_uv)}) - ORIGIN;

		Point p1 = translate(direction_p, point_sphere_u, half_length);
		Point p2 = translate(direction_p, point_sphere_v, -half_length);

		return std::make_pair(p1, p2);
	}
};


/**
 * @brief Scaling functor for returning a scaled vector
 * @tparam GeometryTraits The kernel
 */
template <concepts::BaseKernel Kernel>
struct Scaling {
	/**
	 * @brief Returns the vector from \p p1 to \p p2, scaled using \p scale
	 * @param p1 Start of the vector
	 * @param p2 End of the vector
	 * @param scale Scale to apply
	 * @return Scaled vector
	 */
	typename Kernel::MovetkVector operator()(typename Kernel::MovetkPoint p1,
	                                         typename Kernel::MovetkPoint p2,
	                                         typename Kernel::NT scale) {
		auto v = p2 - p1;
		v *= scale;
		return v;
	}
	/**
	 * @brief Returns a copy of vector \p v, scaled by \p scale
	 * @param v The vector
	 * @param scale The scale
	 * @return Scaled vector
	 */
	typename Kernel::MovetkVector operator()(typename Kernel::MovetkVector v, typename Kernel::NT scale) {
		v *= scale;
		return v;
	}
};

/**
 * @brief Translation functor for translating a MovetkPoint
 * @tparam Kernel The kernel for the point
 */
template <concepts::BaseKernel Kernel>
struct Translation {
	/**
	 * @brief Return a translated copy of point \p p, translated by the vector \p v
	 * @param p The point
	 * @param v The vector
	 * @return The translated point
	 */
	typename Kernel::MovetkPoint operator()(const typename Kernel::MovetkPoint& p,
	                                        const typename Kernel::MovetkVector& v) {
		auto p1 = p + v;
		return p1;
	}
};
}  // namespace movetk::geom

#endif  // MOVETK_INTERFACE_H
