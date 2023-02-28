/*
 * Copyright (C) 2018-2022
 * HERE Europe B.V.
 * Utrecht University (The Netherlands).
 * TU/e (The Netherlands).
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
// Created by Bram Custers (b.a.custers@tue.nl)
//
#ifndef MOVETK_ALGO_SIMPLIFICATION_AGARWAL_H
#define MOVETK_ALGO_SIMPLIFICATION_AGARWAL_H
#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/DistanceInterface.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/Requirements.h"

/**
 * @brief Collection of simplification algorithms for trajectories
 */
namespace movetk::simplification {
/**
 * @brief Computes the polyline simplification using the algorithm by Agarwal et al.\mtkCite{agarwal2005near}
 * @include{doc} pages/agarwal.md
 * Based on https://doi.org/10.1007/s00453-005-1165-y
 * @headerfile Agarwal.h movetk/simplification/Agarwal.h
 * @tparam GeometryTraits The geometry kernel
 * @tparam SqDistanceFunc Algorithm for computing the squared distance between points and points/segments.
 */
template <
    class GeometryTraits,
    class SqDistanceFunc =
        movetk::metric::squared_distance_algorithm<GeometryTraits, movetk::metric::FiniteNorm<GeometryTraits, 2>, void>>
class Agarwal {
	using NT = typename GeometryTraits::NT;
	NT m_epsilon;
	NT m_tolerance = static_cast<NT>(0.001);
	movetk::metric::StrongFrechet<GeometryTraits, SqDistanceFunc> m_strong_frechet_distance;

public:
	Agarwal(typename GeometryTraits::NT epsilon = static_cast<NT>(1.0)) : m_epsilon(epsilon) {
		m_strong_frechet_distance.setTolerance(m_tolerance);
	}

	void setEpsilon(typename GeometryTraits::NT epsilon) { m_epsilon = epsilon; }
	typename GeometryTraits::NT epsilon() const { return m_epsilon; }
	void setTolerance(const typename GeometryTraits::NT& tolerance) {
		m_strong_frechet_distance.setTolerance(tolerance);
		m_tolerance = tolerance;
	}
	typename GeometryTraits::NT tolerance() const { return m_tolerance; }

	template <utils::RandomAccessIterator<typename GeometryTraits::MovetkPoint> InputIterator,
	          utils::OutputIterator<InputIterator> OutputIterator>
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
				if (offset == 2 || m_strong_frechet_distance.decide(segment.begin(), segment.end(), curr, beyond, m_epsilon)) {
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
			if (!m_strong_frechet_distance.decide(segment.begin(), segment.end(), curr, nextPointIt, m_epsilon)) {
				auto upper = binary_search_for_violating_point(searchLower, searchUpper, segment, curr);
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

private:
	/**
	 * @brief Perform binary search to find a point \f$p_j\f$ such that the subpath to \f$p_{j+1}\f$ is above epsilon
	 * @param search_lower Lower index of the range to search in
	 * @param search_upper Upper index of the range to search in
	 * @param segment_to_check The segment to check
	 * @param curr The iterator to the current point
	 * @returns The index j for \f$p_j\f$
	 */
	template <utils::RandomAccessIterator<typename GeometryTraits::MovetkPoint> InputIterator>
	size_t binary_search_for_violating_point(size_t search_lower,
	                                         size_t search_upper,
	                                         std::array<typename GeometryTraits::MovetkPoint, 2>& segment_to_check,
	                                         InputIterator curr) const {
		// Perform binary search to find p_j such that the subpath to p_{j+1} is above epsilon
		auto lower = search_lower;  // Guaranteed to be within epsilon
		auto upper = search_upper;  // Guaranteed to be outside epsilon
		while (upper > lower + 1) {
			auto mid = (lower + upper) / 2;  // Floored when uneven.
			segment_to_check[1] = *(curr + mid);
			// Higher than epsilon
			if (!m_strong_frechet_distance
			         .decide(segment_to_check.begin(), segment_to_check.end(), curr, curr + mid + 1, m_epsilon)) {
				upper = mid;
			} else {
				lower = mid;
			}
		}
		return upper;
	}
};
}  // namespace movetk::simplification
#endif