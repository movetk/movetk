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

/*! @file SegmentationPredicates.h
 *  @brief  A collection of Segmentation Predicates used by a trajectory segmentation algorithm
 *  @authors Aniket Mitra (aniket.mitra@here.com)
 */

#ifndef SEGMENTATIONPREDICATES_H
#define SEGMENTATIONPREDICATES_H

#include <algorithm>
#include <functional>
#include <iterator>

#include "movetk/geom/GeometryInterface.h"
#include "movetk/utils/TrajectoryUtils.h"

namespace movetk::segmentation {
enum TestCriteria { meb, ratio, difference, range };

/*!
 *
 * @tparam GeometryTraits
 */
template <int TestCriteria, class GeometryTraits>
class TEST {};

/**
 * @brief Predicate for determining whether a range of points have a minimum enclosing ball
 * that compares to true for a provided threshold, using the given comparison functor
 * @tparam GeometryTraits The kernel to use
 * @tparam ThresholdCompare Comparison functor
*/
template <typename Kernel, template <typename> typename ThresholdCompare = std::less>
class MinimumEnclosingBallPredicate {
public:
	using NT = typename Kernel::NT;

	/**
	 * @brief Construct the predicate
	 * @param threshold The threshold to use
	*/
	explicit MinimumEnclosingBallPredicate(NT threshold) : m_threshold(threshold) {}

	/**
	 * @brief Determine whether the predicate holds true for the given point range
	 * @param first Start of the point range
	 * @param beyond End of the point range
	 * @return Whether the predicate holds for the given point range.
	*/
	template <utils::RandomAccessPointIterator<Kernel> InputIterator>
	bool operator()(InputIterator first, InputIterator beyond) const {
		const auto radius = m_make_min_sphere(first, beyond);
		return m_compare(radius, m_threshold);
	}

private:
	ThresholdCompare<NT> m_compare;
	movetk::geom::MakeMinSphere<Kernel> m_make_min_sphere;
	NT m_threshold;
};

/*!
 *
 * @tparam GeometryTraits
 */
template <class GeometryTraits>
class TEST<TestCriteria::meb, GeometryTraits> {
	// based on https://doi.org/10.1145/1869790.1869821
private:
	using TEST_ = TEST<TestCriteria::meb, GeometryTraits>;
	using NT = typename GeometryTraits::NT;
	movetk::geom::MakeMinSphere<GeometryTraits> make_min_sphere;
	NT threshold;
	size_t NumPoints = 0;

public:
	/*!
	 *
	 * @tparam InputIterator
	 * @param first
	 * @param beyond
	 * @return
	 */
	template <utils::RandomAccessPointIterator<GeometryTraits> InputIterator>
	bool operator()(InputIterator first, InputIterator beyond) {
		NumPoints = std::distance(first, beyond);
		NT radius = make_min_sphere(first, beyond);
		return radius < threshold;
	}

	TEST() = default;

	/*!
	 *
	 * @param InThreshold
	 */
	TEST(NT InThreshold) : threshold(InThreshold){};
};

/*!
 *
 * @tparam GeometryTraits
 */
template <class GeometryTraits>
class TEST<TestCriteria::ratio, GeometryTraits> {
	// based on https://doi.org/10.1145/1869790.1869821
private:
	using TEST_ = TEST<TestCriteria::ratio, GeometryTraits>;
	using NT = typename GeometryTraits::NT_;
	NT threshold;

public:
	/*!
	 *
	 * @tparam InputIterator
	 * @param first
	 * @param beyond
	 * @return
	 */
	template <std::random_access_iterator InputIterator>
	bool operator()(InputIterator first, InputIterator beyond) {
		if (std::all_of(first, beyond, [](NT i) { return i == 0; }))
			return true;
		NT min = *movetk::utils::min_non_zero_element<GeometryTraits>(first, beyond);
		NT max = *std::max_element(first, beyond);
		NT ratio = max / min;
		return ratio < threshold;
	}

	TEST() = default;

	/*!
	 *
	 * @param InThreshold
	 */
	TEST(NT InThreshold) : threshold(InThreshold){};
};

template <typename GeometryTraits, template <typename> typename ThresholdCompare = std::less>
class MinMaxRatioPredicate {
public:
	using NT = typename GeometryTraits::NT;

	explicit MinMaxRatioPredicate(NT threshold) : m_threshold(threshold) {}

	template <std::random_access_iterator InputIterator>
	bool operator()(InputIterator first, InputIterator beyond) const {
		if (std::all_of(first, beyond, [](NT i) { return i == 0; }))
			return true;
		NT min = *movetk::utils::min_non_zero_element<GeometryTraits>(first, beyond);
		NT max = *std::max_element(first, beyond);
		NT ratio = max / min;
		return m_compare(ratio, m_threshold);
	}

private:
	NT m_threshold;
	ThresholdCompare<NT> m_compare;
};


/*!
 *
 * @tparam GeometryTraits
 */
template <class GeometryTraits>
class TEST<TestCriteria::difference, GeometryTraits> {
	// based on https://doi.org/10.1145/1869790.1869821
private:
	typedef TEST<TestCriteria::difference, GeometryTraits> TEST_;
	typedef typename GeometryTraits::NT NT;
	NT threshold;

public:
	/*!
	 *
	 * @tparam InputIterator
	 * @param first
	 * @param beyond
	 * @return
	 */
	template <std::random_access_iterator InputIterator>
	bool operator()(InputIterator first, InputIterator beyond) {
		NT min = *std::min_element(first, beyond);
		NT max = *std::max_element(first, beyond);
		NT diff = max - min;
		return diff < threshold;
	}

	TEST() = default;

	/*!
	 *
	 * @param InThreshold
	 */
	TEST(NT InThreshold) : threshold(InThreshold){};
};

template <typename GeometryTraits, template <typename> typename ThresholdCompare = std::less>
class MinMaxDifferencePredicate {
public:
	using NT = typename GeometryTraits::NT;

	MinMaxDifferencePredicate(NT threshold) : m_threshold(threshold) {}

	template <std::random_access_iterator InputIterator>
	bool operator()(InputIterator first, InputIterator beyond) const {
		NT min = *std::min_element(first, beyond);
		NT max = *std::max_element(first, beyond);
		NT diff = max - min;
		return m_compare(diff, m_threshold);
	}

private:
	NT m_threshold;
	ThresholdCompare<NT> m_compare;
};


/*!
 *
 * @tparam GeometryTraits
 */
template <class GeometryTraits>
class TEST<TestCriteria::range, GeometryTraits> {
	// based on https://doi.org/10.1145/1869790.1869821
private:
	using NT = typename GeometryTraits::NT;
	NT threshold;

public:
	/*!
	 *
	 * @tparam InputIterator
	 * @param first
	 * @param beyond
	 * @return
	 */
	template <std::random_access_iterator InputIterator>
	bool operator()(InputIterator first, InputIterator beyond) {
		NT lb = *movetk::utils::min_non_zero_element<GeometryTraits>(first, beyond);
		// NT lb = *std::min_element(first, beyond);
		NT ub = lb + threshold;
		InputIterator it = first;
		while (it != beyond) {
			if (*it == 0) {
				it++;
				continue;
			}
			if (*it < lb || *it > ub)
				return false;
			it++;
		}
		return true;
	}

	TEST() = default;

	/*!
	 *
	 * @param InThreshold
	 */
	TEST(NT InThreshold) : threshold(InThreshold){};
};


template <typename GeometryTraits>
class RangePredicate {
public:
	using NT = typename GeometryTraits::NT;

	RangePredicate(NT threshold) : m_threshold(threshold) {}

	template <std::random_access_iterator InputIterator>
	bool operator()(InputIterator first, InputIterator beyond) {
		NT lb = *movetk::utils::min_non_zero_element<GeometryTraits>(first, beyond);
		// NT lb = *std::min_element(first, beyond);
		NT ub = lb + m_threshold;
		InputIterator it = first;
		while (it != beyond) {
			if (*it == 0) {
				it++;
				continue;
			}
			if (*it < lb || *it > ub)
				return false;
			it++;
		}
		return true;
	}

private:
	NT m_threshold;
};

}  // namespace movetk::segmentation


#endif /* SEGMENTATIONPREDICATES_H */
