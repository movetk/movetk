#ifndef MOVETK_GEOM_GEOMETRYCONCEPTS_H
#define MOVETK_GEOM_GEOMETRYCONCEPTS_H
#include <type_traits>
#include <concepts>
#include <vector>
namespace movetk::geom::concepts {
    template<typename POINT, typename KERNEL>
    concept Point = requires(POINT & point, const POINT & const_point, size_t index) {
        // Copy and default constructible
        POINT(std::cref(point));
        POINT();
        typename KERNEL::NT;

        // Iterations
        const_point.begin();
        const_point.end();

        // Indexing
        {const_point[index]}->std::same_as<const typename KERNEL::NT&>;
        {point[index]}->std::same_as<typename KERNEL::NT&>;

        // Arithmetic
        {const_point - const_point}->std::convertible_to<typename KERNEL::Vector>;
        {const_point + std::declval<typename KERNEL::Vector>()}->std::convertible_to<POINT>;
    };

    template<typename VECTOR, typename KERNEL>
    concept Vector = requires(VECTOR & vector, const VECTOR & const_vector) {
        // Copy and default constructible
        typename KERNEL::Point;
        typename KERNEL::NT;
        VECTOR(std::declval<typename KERNEL::Point>());
        VECTOR();

        // Iterations
        const_vector.begin();
        const_vector.end();

        // Arithmetic
        {const_vector* const_vector}->std::convertible_to<typename KERNEL::NT>; // Inner product
        {const_vector - const_vector}->std::convertible_to<VECTOR>;
        {vector -= const_vector}->std::same_as<VECTOR&>;
        {const_vector + const_vector}->std::convertible_to<VECTOR>;
        {vector += const_vector}->std::same_as<VECTOR&>;

        requires std::equality_comparable<VECTOR>;
    };

    template<typename SPHERE, typename KERNEL>
    concept Sphere = requires(SPHERE & sphere, const SPHERE & const_sphere) {
        typename KERNEL::Point;
        typename KERNEL::NT;
        SPHERE();
        SPHERE(std::declval<typename KERNEL::POINT>(), std::declval<typename KERNEL::NT>());
        {const_sphere.center()}->std::convertible_to<typename KERNEL::Point>;
        {const_sphere.squared_radius()}->std::same_as<typename KERNEL::NT>;
    };

    template<typename LINE, typename KERNEL>
    concept Line = requires(LINE & line, const LINE & const_line) {
        typename KERNEL::Point;
        requires requires(const typename KERNEL::Point & pnt) {
            LINE(pnt, pnt);
        };
    };
    template<typename SEGMENT, typename KERNEL>
    concept Segment = requires(SEGMENT & segment, const SEGMENT & const_segment, size_t index) {
        typename KERNEL::Point;
        typename KERNEL::NT;
        SEGMENT();
        SEGMENT(const_segment);
        requires requires(const typename KERNEL::Point & pnt) {
            SEGMENT(pnt, pnt);
        };
        {const_segment[index]}->std::convertible_to<typename KERNEL::Point>;
        // TODO: try renaming this to just squared_length() instead
        {const_segment()}->std::convertible_to<typename KERNEL::NT>; // Squared length
    };

    template<typename POLYGON, typename KERNEL>
    concept Polygon = requires(POLYGON & polygon, const POLYGON & const_polygon, size_t index) {
        typename KERNEL::Point;
        typename KERNEL::NT;
        // Should be constructible from iterators that are random access and of point type.
        requires requires(const std::vector<typename KERNEL::Point>&points) {
            POLYGON(points.begin(), points.end());
        };
        POLYGON();
        const_polygon.v_begin();
        const_polygon.v_end();
    };

    template<typename T>
    concept BaseKernel = requires() {
        typename T::NT;
        {T::dim}->std::convertible_to<size_t>;
        typename T::MovetkPoint;
        typename T::MovetkVector;
    };

    template<typename T>
    concept Kernel = BaseKernel<T> && requires() {
        typename T::MovetkLine;
        typename T::MovetkSegment;
        typename T::MovetkMinSphere;
        typename T::MovetkPolygon;
        typename T::MovetkSphere;
    };
}
#endif
