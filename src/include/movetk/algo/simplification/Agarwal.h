#ifndef MOVETK_ALGO_SIMPLIFICATION_AGARWAL_H
#define MOVETK_ALGO_SIMPLIFICATION_AGARWAL_H
#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/DistanceInterface.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/Requirements.h"
namespace movetk::algo::simplification {
/**
 * \brief
 * Based on https://doi.org/10.1007/s00453-005-1165-y
 * \tparam GeometryTraits The geometry kernel
 * \tparam SqDistanceFunc Algorithm for computing the squared distance between points and points/segments.
 */
template <
    class GeometryTraits,
    class SqDistanceFunc =
        movetk::metric::squared_distance_algorithm<GeometryTraits, movetk::metric::FiniteNorm<GeometryTraits, 2>, void>>
class Agarwal {
	using NT = typename GeometryTraits::NT;
	NT m_epsilon;
	NT m_tolerance = static_cast<NT>(0.001);

public:
	Agarwal(typename GeometryTraits::NT epsilon = static_cast<NT>(1.0)) : m_epsilon(epsilon) {}

	void setEpsilon(typename GeometryTraits::NT epsilon) { m_epsilon = epsilon; }
	typename GeometryTraits::NT epsilon() const { return m_epsilon; }
	void setTolerance(const typename GeometryTraits::NT& tolerance) { m_tolerance = tolerance; }
	typename GeometryTraits::NT tolerance() const { return m_tolerance; }

	template <class InputIterator,
	          class OutputIterator,
	          typename = movetk::utils::requires_random_access_iterator<InputIterator>,
	          typename = movetk::utils::requires_movetk_point_iterator<GeometryTraits, InputIterator>,
	          typename = movetk::utils::requires_output_iterator<OutputIterator>,
	          typename = std::enable_if_t<std::is_same_v<typename OutputIterator::value_type, InputIterator>,
	                                      InputIterator>  // Require output iterator assignable with input iterator
	          >
	void operator()(InputIterator first, InputIterator beyond, OutputIterator result) const {
		InputIterator curr = first;

		// Empty: no simplification
		if (curr == beyond) {
			return;
		}
		// Degenerate point: return the point
		if (std::next(curr) == beyond) {
			*result = curr;
			return;
		}

		const auto polyLength = static_cast<std::size_t>(std::distance(first, beyond));

		// Assign initial point as output
		*result = first;

		// Exponential search offset. Note that offset=1 is always trivially within epsilon with itself...
		std::size_t offset = 2;
		// Index within the polyline of the first point that we are searching with
		std::size_t startPointIndex = 0;
		// Setup Frechet distance
		movetk::metric::StrongFrechet<GeometryTraits, SqDistanceFunc> sfd;
		sfd.setTolerance(m_tolerance);

		// The segment to check with
		std::array<typename GeometryTraits::MovetkPoint, 2> segment;
		segment[0] = *curr;

		while (true) {
			// The bounds to search in when the Frechet predicate fails
			std::size_t searchLower = offset / 2;
			std::size_t searchUpper = offset;

			// Handle termination
			if (startPointIndex + offset >= polyLength) {
				// Assign the last segment
				segment[1] = *(std::prev(beyond));

				// If the segment to the end point is within epsilon or we only have one segment left, we are done
				if (offset == 2 || sfd.decide(segment.begin(), segment.end(), curr, beyond, m_epsilon)) {
					*result = std::prev(beyond);
					break;
				}
				// Otherwise, set the upperbound for binary search
				searchUpper = polyLength - 1 - startPointIndex;
			}

			// Assign the current segment to check
			segment[1] = *(curr + searchUpper);

			// Iterator to the point to check
			auto nextPointIt = curr + searchUpper + 1;
			// Distance is larger than epsilon
			if (!sfd.decide(segment.begin(), segment.end(), curr, nextPointIt, m_epsilon)) {
				// Perform binary search to find p_j such that the subpath to p_{j+1} is above epsilon
				auto lower = searchLower;  // Guaranteed to be within epsilon
				auto upper = searchUpper;  // Guaranteed to be outside epsilon
				while (upper > lower + 1) {
					auto mid = (lower + upper) / 2;  // Floored when uneven.
					segment[1] = *(curr + mid);
					// Higher than epsilon
					if (!sfd.decide(segment.begin(), segment.end(), curr, curr + mid + 1, m_epsilon)) {
						upper = mid;
					} else {
						lower = mid;
					}
				}
				// Assign the output
				*result = curr + upper - 1;
				startPointIndex += upper - 1;
				// Update current point iterator
				curr += upper - 1;
				// Update offset
				offset = 2;
				// Update query segment with new start point
				segment[0] = *curr;
			}
			// Less than epsilon Frechet distance, so continue the exponential search
			else {
				offset *= 2;  // Or offset <<= 1;
			}
		}
	}
};
}  // namespace movetk::algo::simplification
#endif