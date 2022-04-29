#ifndef MOVETK_GEOM_OBJECTCREATION_H
#define MOVETK_GEOM_OBJECTCREATION_H
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
	template <class CoordinateIterator, typename = utils::requires_random_access_iterator<CoordinateIterator>>
	typename GeometryTraits::MovetkPoint operator()(CoordinateIterator first, CoordinateIterator beyond) const {
		// ASSERT_RANDOM_ACCESS_ITERATOR(CoordinateIterator);
		//ASSERT_NUMBER_TYPE(GeometryTraits, first);
		typename GeometryTraits::MovetkPoint p(first, beyond);
		return p;
	}

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
	                                               typename GeometryTraits::MovetkPoint p2) {
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
	/*!
	 * @details Constructs a segment which has an associated measure of length
	 * i.e euclidean distance between two points
	 * @param p1 - A movetk point
	 * @param p2 - A movetk point
	 * @return Length of a segment
	 */
	typename GeometryTraits::NT operator()(typename GeometryTraits::MovetkPoint p1,
	                                       typename GeometryTraits::MovetkPoint p2) {
		typename GeometryTraits::MovetkSegment l(p1, p2);
		return sqrt(l());
	}

	typename GeometryTraits::NT operator()(typename GeometryTraits::MovetkSegment l) { return sqrt(l()); }
};

template <class GeometryTraits>
struct MakeSegment {
	typename GeometryTraits::MovetkSegment operator()(typename GeometryTraits::MovetkPoint p1,
	                                                  typename GeometryTraits::MovetkPoint p2) {
		typename GeometryTraits::MovetkSegment s(p1, p2);
		return s;
	}

	typename GeometryTraits::MovetkSegment operator()(std::initializer_list<typename GeometryTraits::NT> l1,
	                                                  std::initializer_list<typename GeometryTraits::NT> l2) {
		MakePoint<GeometryTraits> make_point;
		typename GeometryTraits::MovetkPoint p1 = make_point(l1);
		typename GeometryTraits::MovetkPoint p2 = make_point(l2);
		typename GeometryTraits::MovetkSegment s(p1, p2);
		return s;
	}
};

template <class GeometryTraits>
struct MakeSphere {
	typename GeometryTraits::MovetkSphere operator()(typename GeometryTraits::MovetkPoint center,
	                                                 typename GeometryTraits::NT radius,
	                                                 bool square = true) {
		typename GeometryTraits::MovetkSphere s(center, radius, square);
		return s;
	}

	typename GeometryTraits::MovetkSphere operator()(std::initializer_list<typename GeometryTraits::NT> l,
	                                                 typename GeometryTraits::NT radius,
	                                                 bool square = true) {
		typename GeometryTraits::MovetkPoint center(l.begin(), l.end());
		typename GeometryTraits::MovetkSphere s(center, radius, square);
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
	template <class PointIterator,
	          typename = movetk::utils::requires_random_access_iterator<PointIterator>,
	          typename = movetk::utils::requires_movetk_point<GeometryTraits, typename PointIterator::value_type>>
	typename GeometryTraits::MovetkPolygon operator()(PointIterator first, PointIterator beyond) {
		// ASSERT_RANDOM_ACCESS_ITERATOR(PointIterator);
		// ASSERT_MOVETK_POINT_TYPE(GeometryTraits, first);
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
	template <class PointIterator,
	          typename = movetk::utils::requires_random_access_iterator<PointIterator>,
	          typename = movetk::utils::requires_movetk_point<GeometryTraits, typename PointIterator::value_type>>
	typename GeometryTraits::NT operator()(PointIterator first, PointIterator beyond) {
		// ASSERT_RANDOM_ACCESS_ITERATOR(PointIterator);
		// ASSERT_MOVETK_POINT_TYPE(GeometryTraits, first);
		typename GeometryTraits::NT Radius = make_min_sphere(first, beyond);
		return Radius;
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
	template <class PointIterator,
	          class CenterIterator,
	          typename = movetk::utils::requires_random_access_iterator<PointIterator>,
	          typename = movetk::utils::requires_movetk_point<GeometryTraits, typename PointIterator::value_type>,
	          typename = movetk::utils::requires_output_iterator<CenterIterator>,
	          typename = movetk::utils::requires_NT<GeometryTraits, typename CenterIterator::value_type>>
	typename GeometryTraits::NT operator()(PointIterator first, PointIterator beyond, CenterIterator iter) {
		// ASSERT_RANDOM_ACCESS_ITERATOR(PointIterator);
		// ASSERT_MOVETK_POINT_TYPE(GeometryTraits, first);
		// ASSERT_OUTPUT_ITERATOR(CenterIterator);
		typename GeometryTraits::NT Radius = make_min_sphere(first, beyond, iter);
		return Radius;
	}
};
}  // namespace movetk::geom
#endif