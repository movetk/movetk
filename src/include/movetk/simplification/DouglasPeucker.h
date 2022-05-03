#ifndef MOVETK_ALGO_SIMPLIFICATION_DOUGLASPEUCKER_H
#define MOVETK_ALGO_SIMPLIFICATION_DOUGLASPEUCKER_H
#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/DistanceInterface.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/Requirements.h"
namespace movetk::algo::simplification {

template <class GeometryTraits, class Norm>
struct FindFarthest {
	template <class InputIterator,
	          typename = utils::requires_random_access_iterator<InputIterator>,
	          typename = utils::requires_movetk_point<GeometryTraits, typename InputIterator::value_type>>
	InputIterator operator()(typename GeometryTraits::MovetkSegment& segment,
	                         InputIterator first,
	                         InputIterator beyond,
	                         typename GeometryTraits::NT& dist) {
		movetk::metric::ComputeSquaredDistance<GeometryTraits, Norm> squared_distance;
		Norm norm;
		InputIterator it = first + 1;
		InputIterator FarthestVertex = it;
		typename GeometryTraits::MovetkVector v = *it - segment[1];
		if (norm(v) < MOVETK_EPS) {
			dist = 0;
			return FarthestVertex;
		}
		dist = squared_distance(*it, segment);
		it++;
		while (it != (beyond - 1)) {
			typename GeometryTraits::NT CurrDist = squared_distance(*it, segment);
			if (CurrDist > dist) {
				FarthestVertex = it;
				dist = CurrDist;
			}
			it++;
		}
		return FarthestVertex;
	}
};

template <class GeometryTraits, class FindFarthest>
class DouglasPeucker {
	// based on http://doi.acm.org/10.1145/177424.178097
private:
	typedef typename GeometryTraits::NT NT;
	movetk::geom::MakeSegment<GeometryTraits> make_segment;
	FindFarthest farthest;
	NT eps;

public:
	DouglasPeucker(NT epsilon) { eps = epsilon * epsilon; }

	template <class InputIterator,
	          class OutputIterator,
	          typename = movetk::utils::requires_random_access_iterator<InputIterator>,
	          typename = movetk::utils::requires_output_iterator<OutputIterator>,
	          typename = movetk::utils::requires_movetk_point<GeometryTraits, typename InputIterator::value_type>,
	          typename = movetk::utils::requires_equality<typename InputIterator::value_type,
	                                                      typename OutputIterator::value_type::value_type>>
	void operator()(InputIterator first, InputIterator beyond, OutputIterator result, size_t level = 0) {
		typename GeometryTraits::MovetkSegment segment = make_segment(*first, *(beyond - 1));
		NT distance = 0;
		level++;
		InputIterator FarthestVertex = farthest(segment, first, beyond, distance);
		if (distance > eps) {
			(*this)(first, (FarthestVertex + 1), result, level);
			(*this)(FarthestVertex, beyond, result, level);
		} else {
			*result = first;
		}
		if (level == 1)
			*result = beyond - 1;
	}
};

}  // namespace movetk::algo::simplification
#endif