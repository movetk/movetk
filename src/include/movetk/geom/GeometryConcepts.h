#ifndef MOVETK_GEOM_GEOMETRYCONCEPTS_H
#define MOVETK_GEOM_GEOMETRYCONCEPTS_H
#include <concepts>
#include <type_traits>
#include <vector>
/**
 * @brief Contains concepts for geometric objects used in MoveTK
 */
namespace movetk::geom::concepts {
/**
 * @concept Point
 * @brief Concept describing a Point for a Kernel.
 */
template <typename POINT, typename KERNEL>
concept Point = requires(POINT& point, const POINT& const_point, size_t index) {
	typename KERNEL::NT;
	typename KERNEL::MovetkVector;

	// Copy and default constructible
	POINT(const_point);
	POINT();

	// Constructible from an initalizer list of coordinates
	//POINT(std::declval<std::initializer_list<typename KERNEL::NT>>());
	// Constructible from an input iterator pair. Note that we use std::vector<> iterators as a proxy
	requires requires(typename std::vector<typename KERNEL::NT>::iterator iterator) { POINT(iterator, iterator); };

	// Iteration of coordinates
	const_point.begin();
	const_point.end();

	// Arithmetic
	{ const_point - const_point } -> std::convertible_to<typename KERNEL::MovetkVector>;
};

template <typename VECTOR, typename KERNEL>
concept Vector = requires(VECTOR& vector, const VECTOR& const_vector) {
	// Copy and default constructible
	requires std::is_default_constructible_v<VECTOR>;
	VECTOR(std::declval<typename KERNEL::MovetkPoint>());

	typename KERNEL::MovetkPoint;
	typename KERNEL::NT;

	// Iterations
	const_vector.begin();
	const_vector.end();

	// Arithmetic
	{ const_vector* const_vector } -> std::convertible_to<typename KERNEL::NT>;  // Inner product
	{ const_vector - const_vector } -> std::convertible_to<VECTOR>;
	{ vector -= const_vector } -> std::same_as<VECTOR&>;
	{ const_vector + const_vector } -> std::convertible_to<VECTOR>;
	{ vector += const_vector } -> std::same_as<VECTOR&>;
	requires requires(const typename KERNEL::MovetkPoint& point) {
		{ point + vector } -> std::convertible_to<typename KERNEL::MovetkPoint>;
		{ point - vector } -> std::convertible_to<typename KERNEL::MovetkPoint>;
	};

	requires std::equality_comparable<VECTOR>;
};

template <typename SPHERE, typename KERNEL>
concept Sphere = requires(SPHERE& sphere, const SPHERE& const_sphere) {
	typename KERNEL::MovetkPoint;
	typename KERNEL::NT;
	SPHERE();
	SPHERE(std::declval<typename KERNEL::MovetkPoint>(), std::declval<typename KERNEL::NT>());
	{ const_sphere.center() } -> std::convertible_to<typename KERNEL::Point>;
	{ const_sphere.squared_radius() } -> std::same_as<typename KERNEL::NT>;
};

template <typename LINE, typename KERNEL>
concept Line = requires(LINE& line, const LINE& const_line) {
	typename KERNEL::MovetkPoint;
	// Constructible from a pair of points
	requires requires(const typename KERNEL::Point& pnt) { LINE(pnt, pnt); };
};
template <typename SEGMENT, typename KERNEL>
concept Segment = requires(SEGMENT& segment, const SEGMENT& const_segment, size_t index) {
	typename KERNEL::MovetkPoint;
	typename KERNEL::NT;
	SEGMENT();
	SEGMENT(const_segment);
	requires requires(const typename KERNEL::MovetkPoint& pnt) { SEGMENT(pnt, pnt); };
	{ const_segment[index] } -> std::convertible_to<typename KERNEL::MovetkPoint>;
	// TODO: try renaming this to just squared_length() instead
	{ const_segment() } -> std::convertible_to<typename KERNEL::NT>;  // Squared length
};

template <typename POLYGON, typename KERNEL>
concept Polygon = requires(POLYGON& polygon, const POLYGON& const_polygon, size_t index) {
	typename KERNEL::MovetkPoint;
	typename KERNEL::NT;
	// Should be constructible from iterators that are random access and of point type.
	requires requires(const std::vector<typename KERNEL::MovetkPoint>& points) { POLYGON(points.begin(), points.end()); };
	POLYGON();
	const_polygon.v_begin();
	const_polygon.v_end();
};

template <typename Kernel>
concept BaseKernel = requires() {
	typename Kernel::NT;
	{ Kernel::dim } -> std::convertible_to<size_t>;
	// Require a point type
	typename Kernel::MovetkPoint;
	requires Point<typename Kernel::MovetkPoint, Kernel>;

	// Require a vector type
	typename Kernel::MovetkVector;
	requires Vector<typename Kernel::MovetkVector, Kernel>;
};

template <typename Kernel>
concept FullKernel = BaseKernel<Kernel> && requires() {
	typename Kernel::MovetkLine;
	typename Kernel::MovetkSegment;
	typename Kernel::MovetkMinSphere;
	typename Kernel::MovetkPolygon;
	typename Kernel::MovetkSphere;
};
}  // namespace movetk::geom::concepts
#endif
