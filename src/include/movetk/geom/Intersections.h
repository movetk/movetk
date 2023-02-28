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
#ifndef MOVETK_GEOM_INTERSECTIONS_H
#define MOVETK_GEOM_INTERSECTIONS_H
#include "ObjectCreation.h"
#include "movetk/utils/Requirements.h"
#include "movetk/utils/TypeChecks.h"
namespace movetk::geom {

struct sphere_segment_intersection_tag;

struct polylines_instersection_tag;

struct sphere_sphere_intersection_tag;

/**
 * @brief Traits class for defining required types for intersection computations
 * @tparam _GeometryTraits The kernel
 * @tparam _Norm The norm to use for distance computations
 * @tparam Tag Tag for the intersection type.
 */
template <class _GeometryTraits, class _Norm, class Tag>
struct IntersectionTraits {};

template <class _GeometryTraits, class _Norm>
struct IntersectionTraits<_GeometryTraits, _Norm, sphere_segment_intersection_tag> {
	enum Attributes { ID, SIGN_DISCRIMINANT, SQUARED_RATIO, POINT };
	using value_type = std::tuple<std::size_t, int, typename _GeometryTraits::NT, typename _GeometryTraits::MovetkPoint>;
	using GeometryTraits = _GeometryTraits;
	using Norm = _Norm;
};

template <class _GeometryTraits, class _Norm>
struct IntersectionTraits<_GeometryTraits, _Norm, polylines_instersection_tag> {
	using GeometryTraits = _GeometryTraits;
	using Norm = _Norm;
	using value_type = std::size_t;
};

template <class _GeometryTraits, class _Norm>
struct IntersectionTraits<_GeometryTraits, _Norm, sphere_sphere_intersection_tag> {
	using GeometryTraits = _GeometryTraits;
	using Norm = _Norm;
	using value_type = typename GeometryTraits::MovetkSphere;
};

/**
 * @brief Functor for computing intersections between geometric objects
 * @tparam IntersectionTraits The traits for the intersection to use
 */
template <class IntersectionTraits>
class ComputeIntersections {
public:
	using GeometryTraits = typename IntersectionTraits::GeometryTraits;

	/**
	 * @brief Computes the number of self intersections of a curve
	 * @param first Start of the range of points describing the curve
	 * @param beyond End of the range of points describing the curve
	 * @return The number of self intersections
	 */
	template <utils::RandomAccessIterator<typename GeometryTraits::MovetkPoint> PointIterator>
	std::size_t operator()(PointIterator first, PointIterator beyond) const {
		typename GeometryTraits::MovetkCurveIntersection compute_curve_intersections;
		return compute_curve_intersections(first, beyond);
	}

	/**
	 * @brief Computes the intersections between a sphere and a segment
	 * @tparam
	 * @param sphere the sphere
	 * @param segment The segment
	 * @param result Output iterator to write the resulting intersections to.
	 */
	template <utils::OutputIterator<typename IntersectionTraits::value_type> OutputIterator>
		requires utils::L2Norm<typename IntersectionTraits::Norm>
	void operator()(const typename GeometryTraits::MovetkSphere& sphere,
	                const typename GeometryTraits::MovetkSegment& segment,
	                OutputIterator result) const {
		typename IntersectionTraits::Norm norm;
		const auto v1 = segment[1] - segment[0];
		const auto squared_length_v1 = norm(v1);
		const auto v2 = segment[0] - sphere.center();
		const auto squared_length_v2 = norm(v2);
		const auto q1 = std::pow(v1 * v2, 2) * (1 / squared_length_v1);
		const auto q2 = squared_length_v2 - sphere.squared_radius();
		if (q1 < q2) {
			result = std::make_tuple(0, -1, -1.0, sphere.center());
		} else if ((q1 - q2) > (MOVETK_EPS * 1000)) {
			const auto n = 0.5;
			const auto root = std::pow(squared_length_v1 * (q1 - q2), n);
			const auto translation1 = -1 * (v1 * v2) + root;
			const auto translation2 = -1 * (v1 * v2) - root;
			auto v3 = v1;
			v3 *= (translation1 / squared_length_v1);
			auto direction = v3 * v1;
			auto length = norm(v3);
			auto ratio = length / squared_length_v1;
			if ((direction > 0) && (ratio <= 1)) {
				result = std::make_tuple(0, 1, ratio, segment[0] + v3);
			} else if (direction < 0) {
				result = std::make_tuple(0, -1, ratio, segment[0]);
			} else if (ratio > 1) {
				result = std::make_tuple(0, -1, ratio, segment[1]);
			}

			v3 *= (squared_length_v1 / translation1);
			v3 *= (translation2 / squared_length_v1);
			direction = v3 * v1;
			length = norm(v3);
			ratio = length / squared_length_v1;
			if ((direction > 0) && (ratio <= 1)) {
				result = std::make_tuple(0, 1, ratio, segment[0] + v3);
			} else if (direction < 0) {
				result = std::make_tuple(0, -1, ratio, segment[0]);
			} else if (ratio > 1) {
				result = std::make_tuple(0, -1, ratio, segment[1]);
			}
		} else {
			const auto translation = -1 * (v1 * v2);
			auto v3 = v1;
			v3 *= (translation / squared_length_v1);
			const auto direction = v3 * v1;
			const auto length = norm(v3);
			const auto ratio = length / squared_length_v1;
			if ((direction > 0) && (ratio <= 1)) {
				result = std::make_tuple(0, 0, ratio, segment[0] + v3);
			}
		}
	}

	/**
	 * @brief Computes the intersection between two spheres as another sphere.
	 * The center of the resulting sphere is the center of the intersection lense,
	 * or the halfway point between the spheres if they are distjoint.
	 * Based on https://hal.archives-ouvertes.fr/hal-01955983/document
	 * @tparam
	 * @param sphere_a The first sphere
	 * @param sphere_b The second sphere
	 * @return The intersection between the spheres, as another sphere.
	 */
	template <typename = void>
		requires utils::L2Norm<typename IntersectionTraits::Norm>
	typename GeometryTraits::MovetkSphere operator()(const typename GeometryTraits::MovetkSphere& sphere_a,
	                                                 const typename GeometryTraits::MovetkSphere& sphere_b) const {
		typename IntersectionTraits::Norm norm;
		movetk::geom::MakePoint<GeometryTraits> make_point;
		MakeSphere<GeometryTraits> make_sphere;
		auto ORIGIN = make_point({0, 0});
		const auto squared_r_a = sphere_a.squared_radius();
		const auto r_a = std::pow(squared_r_a, 0.5);
		const auto squared_r_b = sphere_b.squared_radius();
		const auto r_b = std::pow(squared_r_b, 0.5);
		auto v = sphere_b.center() - sphere_a.center();
		auto v1 = (sphere_b.center() - ORIGIN) + (sphere_a.center() - ORIGIN);
		const auto squared_length = norm(v);
		const auto sum_squared_radius = squared_r_a + squared_r_b;
		const auto squared_diff_radius = sum_squared_radius - 2 * r_a * r_b;
		const auto squared_sum_radius = sum_squared_radius + 2 * r_a * r_b;
		bool disjoint = (squared_sum_radius - squared_length < 0) || (squared_diff_radius - squared_length > 0);
		if (!disjoint) {
			v *= (squared_r_a - squared_r_b) / (2 * squared_length);
			v1 *= 0.5;
			const auto v2 = v1 + v;
			const auto center = ORIGIN + v2;
			const auto op1 = (squared_r_a + squared_r_b) * 0.5;
			const auto numerator = (squared_r_a - squared_r_b) * (squared_r_a - squared_r_b);
			const auto op2 = 0.25 * numerator / squared_length;
			const auto squared_radius = op1 - op2 - 0.25 * squared_length;
			return make_sphere(center, squared_radius, false);
		} else {
			return make_sphere(ORIGIN, 0);
		}
	}
};
}  // namespace movetk::geom
#endif