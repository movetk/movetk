#ifndef MOVETK_GEOM_INTERSECTIONS_H
#define MOVETK_GEOM_INTERSECTIONS_H
#include "movetk/utils/Requirements.h"
#include "movetk/utils/TypeChecks.h"
namespace movetk::geom {

struct sphere_segment_intersection_tag;

struct polylines_instersection_tag;

struct sphere_sphere_intersection_tag;

template <class _GeometryTraits, class _Norm, class Tag>
struct IntersectionTraits {};

template <class _GeometryTraits, class _Norm>
struct IntersectionTraits<_GeometryTraits, _Norm, sphere_segment_intersection_tag> {
	enum Attributes { ID, SIGN_DISCRIMINANT, SQUARED_RATIO, POINT };
	typedef std::tuple<std::size_t, int, typename _GeometryTraits::NT, typename _GeometryTraits::MovetkPoint> value_type;
	typedef _GeometryTraits GeometryTraits;
	typedef _Norm Norm;
};

template <class _GeometryTraits, class _Norm>
struct IntersectionTraits<_GeometryTraits, _Norm, polylines_instersection_tag> {
	typedef _GeometryTraits GeometryTraits;
	typedef _Norm Norm;
};

template <class _GeometryTraits, class _Norm>
struct IntersectionTraits<_GeometryTraits, _Norm, sphere_sphere_intersection_tag> {
	typedef _GeometryTraits GeometryTraits;
	typedef _Norm Norm;
};

template <class IntersectionTraits>
class ComputeIntersections {
public:
	template <class PointIterator,
	          typename = movetk::utils::requires_random_access_iterator<PointIterator>,
	          typename = movetk::utils::requires_movetk_point<typename IntersectionTraits::GeometryTraits,
	                                                          typename PointIterator::value_type>>
	std::size_t operator()(PointIterator first, PointIterator beyond) {
		typename IntersectionTraits::GeometryTraits::MovetkCurveIntersection compute_curve_intersections;
		return compute_curve_intersections(first, beyond);
	}

	template <class OutputIterator,
	          typename = movetk::utils::requires_output_iterator<OutputIterator>,
	          typename = movetk::utils::requires_tuple<typename OutputIterator::value_type>,
	          typename = movetk::utils::requires_L2_norm<typename IntersectionTraits::Norm>>
	void operator()(typename IntersectionTraits::GeometryTraits::MovetkSphere& sphere,
	                typename IntersectionTraits::GeometryTraits::MovetkSegment& segment,
	                OutputIterator result) {
		typename IntersectionTraits::Norm norm;
		typedef typename IntersectionTraits::GeometryTraits::NT NT;
		typedef typename IntersectionTraits::GeometryTraits::MovetkVector MovetkVector;
		MovetkVector v1 = segment[1] - segment[0];
		NT squared_length_v1 = norm(v1);
		MovetkVector v2 = segment[0] - sphere.center();
		NT squared_length_v2 = norm(v2);
		NT q1 = std::pow(v1 * v2, 2) * (1 / squared_length_v1);
		NT q2 = squared_length_v2 - sphere.squared_radius();
		if (q1 < q2)
			result = std::make_tuple(0, -1, -1.0, sphere.center());
		else if ((q1 - q2) > (MOVETK_EPS * 1000)) {
			NT n = 0.5;
			NT root = std::pow(squared_length_v1 * (q1 - q2), n);
			NT translation1 = -1 * (v1 * v2) + root;
			NT translation2 = -1 * (v1 * v2) - root;
			MovetkVector v3 = v1;
			v3 *= (translation1 / squared_length_v1);
			NT direction = v3 * v1;
			NT length = norm(v3);
			NT ratio = length / squared_length_v1;
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
			NT translation = -1 * (v1 * v2);
			MovetkVector v3 = v1;
			v3 *= (translation / squared_length_v1);
			NT direction = v3 * v1;
			NT length = norm(v3);
			NT ratio = length / squared_length_v1;
			if ((direction > 0) && (ratio <= 1)) {
				result = std::make_tuple(0, 0, ratio, segment[0] + v3);
			}
		}
	}

	template <typename = movetk::utils::requires_L2_norm<typename IntersectionTraits::Norm>>
	typename IntersectionTraits::GeometryTraits::MovetkSphere operator()(
	    typename IntersectionTraits::GeometryTraits::MovetkSphere& sphere_a,
	    typename IntersectionTraits::GeometryTraits::MovetkSphere& sphere_b) {
		// based on https://hal.archives-ouvertes.fr/hal-01955983/document
		typename IntersectionTraits::Norm norm;
		typedef typename IntersectionTraits::GeometryTraits GeometryTraits;
		typedef typename GeometryTraits::NT NT;
		typedef typename GeometryTraits::MovetkVector MovetkVector;
		typedef typename GeometryTraits::MovetkPoint MovetkPoint;
		movetk::geom::MakePoint<GeometryTraits> make_point;
		MakeSphere<GeometryTraits> make_sphere;
		MovetkPoint ORIGIN = make_point({0, 0});
		NT squared_r_a = sphere_a.squared_radius();
		NT r_a = std::pow(squared_r_a, 0.5);
		NT squared_r_b = sphere_b.squared_radius();
		NT r_b = std::pow(squared_r_b, 0.5);
		MovetkVector v = sphere_b.center() - sphere_a.center();
		MovetkVector v1 = (sphere_b.center() - ORIGIN) + (sphere_a.center() - ORIGIN);
		NT squared_length = norm(v);
		NT sum_squared_radius = squared_r_a + squared_r_b;
		NT squared_diff_radius = sum_squared_radius - 2 * r_a * r_b;
		NT squared_sum_radius = sum_squared_radius + 2 * r_a * r_b;
		bool disjoint = (squared_sum_radius - squared_length < 0) || (squared_diff_radius - squared_length > 0);
		if (!disjoint) {
			v *= (squared_r_a - squared_r_b) / (2 * squared_length);
			v1 *= 0.5;
			MovetkVector v2 = v1 + v;
			MovetkPoint center = ORIGIN + v2;
			NT op1 = (squared_r_a + squared_r_b) * 0.5;
			NT numerator = (squared_r_a - squared_r_b) * (squared_r_a - squared_r_b);
			NT op2 = 0.25 * numerator / squared_length;
			NT squared_radius = op1 - op2 - 0.25 * squared_length;
			return make_sphere(center, squared_radius, false);
		} else {
			return make_sphere(ORIGIN, 0);
		}
	}
};
}  // namespace movetk::geom
#endif