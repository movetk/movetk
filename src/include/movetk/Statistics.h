/*
 * Copyright (C) 2018-2020
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
// Created by Custers, Bram on 2020-02-08.
//

#ifndef MOVETK_STATISTICS_H
#define MOVETK_STATISTICS_H

#include <movetk/utils/TrajectoryUtils.h>

#include <boost/iterator/zip_iterator.hpp>
#include <functional>
#include <iterator>

#include "movetk/geom/GeometryInterface.h"
#include "movetk/io/TrajectoryTraits.h"

namespace movetk::statistics {
/**
 * @brief Functor for computingthe length of a trajectory
 * Currently only supports 2 dimensions.
 * @tparam GeometryKernel The geometry kernel to use
 * @tparam PointDistanceFunc Distance function to compute the distance between points
*/
template <class GeometryKernel, typename PointDistanceFunc = movetk::geom::ComputeLength<GeometryKernel>>
class TrajectoryLength {
private:
	using NT = typename GeometryKernel::NT;

public:
	/**
	 * @brief Computes the length of a trajectory, supplied as two ranges of coordinates for the
	 * first and second coordinate
	 * @tparam CoordinateIterator Type of the coordinate iterator
	 * @param firstCoordBegin Start of the range of values of the first coordinate
	 * @param firstCoordEnd End of the range of values of the first coordinate
	 * @param secondCoordBegin Start of the range of values of the second coordinate
	 * @param secondCoordEnd End of the range of values of the second coordinate
	 * @return The length
	*/
	template <typename CoordinateIterator>
	typename GeometryKernel::NT operator()(CoordinateIterator firstCoordBegin,
	                                       CoordinateIterator firstCoordEnd,
	                                       CoordinateIterator secondCoordBegin,
	                                       CoordinateIterator secondCoordEnd) const {
		assert(std::distance(firstCoordBegin, firstCoordEnd) == std::distance(secondCoordBegin, secondCoordEnd));

		if (firstCoordBegin == firstCoordEnd || secondCoordBegin == secondCoordEnd)
			return 0;

		auto firstCoordPair = std::make_pair(firstCoordBegin, firstCoordEnd);
		auto secondCoordPair = std::make_pair(secondCoordBegin, secondCoordEnd);


		// Point iterator begin and end
		auto beginEndPair = movetk::utils::point_iterators_from_coordinates<GeometryKernel, CoordinateIterator>(
		    std::array<decltype(firstCoordPair), 2>{firstCoordPair, secondCoordPair});

		std::vector<NT> distances;
		movetk::utils::get_distances<GeometryKernel>(beginEndPair.first,
		                                             beginEndPair.second,
		                                             std::back_inserter(distances));
		// Defaults to summing everything
		return std::accumulate(distances.begin(), distances.end(), (NT)0.0);
	}
};

/**
 * @brief Functor for computing the duration of a trajectory.
 * @details It computes the difference between the minimum and maximum time.
*/
class TrajectoryDuration {
public:
	/**
	 * @brief Computes the duration of the trajectory. Does not assume the trajectory to be
	 * sorted by time. The duration is returned with the type of the timestamp field in the
	 * trajectory. Note that the timestamp type should be comparable and subtractable.
	 * @tparam TimeIterator The time point iterator.
	 * @complexity O(n), with n the number of points
	 * @return Duration of the trajectory, returned as the type of the difference of the
	 */
	template <std::input_iterator TimeIterator>
	decltype(std::declval<typename TimeIterator::value_type>() - std::declval<typename TimeIterator::value_type>())
	operator()(TimeIterator begin, TimeIterator end) const {
		using Time_t = typename TimeIterator::value_type;
		using Duration_t = decltype(std::declval<Time_t>() - std::declval<Time_t>());

		// Catch empty case. Assume difference type
		if (begin == end)
			return 0;

		// Find min and max iterators over all time points
		auto minMaxIts = std::minmax_element(begin, end);

		return *(minMaxIts.second) - *(minMaxIts.first);
	}
};
/**
 * @brief Computs statistics for the constant speed between probes
 * @tparam GeometryKernel The kernel to use
 * @tparam PointDistanceFunc Point distance to use
*/
template <typename GeometryKernel, typename PointDistanceFunc = movetk::geom::ComputeLength<GeometryKernel>>
class TrajectorySpeedStatistic {
public:
	enum class Statistic { Median, Mean, Max, Min, Variance };
	// Duration is the difference of the time type.
	template <typename Time_t>
	using Duration_t = decltype(std::declval<Time_t>() - std::declval<Time_t>());
	// Point type
	using Point_t = typename GeometryKernel::MovetkPoint;
	// Distance type
	using Dist_t = decltype(std::declval<PointDistanceFunc>()(std::declval<Point_t>(), std::declval<Point_t>()));
	// Speed type = Distance type / Duration type
	template <typename Time_t>
	using Speed_t = decltype(std::declval<Dist_t>() / std::declval<Duration_t<Time_t>>());

	/**
	 * @brief Computes the requested statistics for the speed between probes for a given
	 * trajectory.
	 * @tparam PointIterator The iterator type of the point range
	 * @tparam TimeIterator The iterator type of the time range
	 * @param pointsBegin Start of the range of points
	 * @param pointsEnd End of the range of points
	 * @param timeBegin Start of the range of timestamps
	 * @param timeEnd End of the range of timestamps
	 * @param requiredStatistics The requested statistics
	 * @return The computed statistics, corresponding to the requested statistics in requiredStatistics 
	*/
	template <utils::InputIterator<typename GeometryKernel::MovetkPoint> PointIterator, std::input_iterator TimeIterator>
	std::vector<Speed_t<typename TimeIterator::value_type>> operator()(
	    PointIterator pointsBegin,
	    PointIterator pointsEnd,
	    TimeIterator timeBegin,
	    TimeIterator timeEnd,
	    const std::vector<Statistic>& requiredStatistics) const {
		assert(std::distance(pointsBegin, pointsEnd) == std::distance(timeBegin, timeEnd));

		using Speed = Speed_t<typename TimeIterator::value_type>;
		using Duration = Duration_t<typename TimeIterator::value_type>;
		std::vector<Speed> speeds;
		std::vector<Speed> output(requiredStatistics.size(), 0);
		// We cannot derive speeds for empty or single point trajectories, so return zero values.
		if (std::distance(pointsBegin, pointsEnd) <= 1 || std::distance(timeBegin, timeEnd) <= 1)
			return output;

		// Compute speeds
		{
			std::vector<Dist_t> distances;
			auto backInsert = std::back_inserter(distances);
			movetk::utils::get_distances<GeometryKernel, decltype(pointsBegin), decltype(backInsert), PointDistanceFunc>(
			    pointsBegin,
			    pointsEnd,
			    backInsert,
			    true);
			std::vector<Duration> timeDiffs;
			movetk::utils::get_time_diffs(timeBegin, timeEnd, std::back_inserter(timeDiffs), true);

			movetk::utils::get_speeds<GeometryKernel>(timeDiffs.begin(),
			                                          timeDiffs.end(),
			                                          distances.begin(),
			                                          std::back_inserter(speeds));
		}

		// Calculate the statistics
		for (int i = 0; i < requiredStatistics.size(); ++i) {
			switch (requiredStatistics[i]) {
				case Statistic::Median: {
					std::nth_element(speeds.begin(), speeds.begin() + speeds.size() / 2, speeds.end());
					// Odd size
					if (speeds.size() & 1) {
						output[i] = speeds[speeds.size() / 2];
					} else {
						// Even size
						output[i] = 0.5 * (speeds[speeds.size() / 2] + speeds[speeds.size() / 2 - 1]);
					}
					break;
				}
				case Statistic::Mean: {
					// Compute the average
					output[i] =
					    std::accumulate(speeds.begin(), speeds.end(), static_cast<Speed>(0)) / static_cast<Speed>(speeds.size());
					break;
				}
				case Statistic::Min: {
					output[i] = *std::min_element(speeds.begin(), speeds.end());
					break;
				}
				case Statistic::Variance: {
					auto totalNum = static_cast<Speed>(speeds.size());
					auto mean = std::accumulate(speeds.begin(), speeds.end(), static_cast<Speed>(0)) / totalNum;
					// Lambda for accumulating squares of differences to mean.
					auto sqDiff = [&mean](auto accum, auto newVal) { return accum + (newVal - mean) * (newVal - mean); };
					output[i] = std::accumulate(speeds.begin(), speeds.end(), static_cast<Speed>(0), sqDiff) / totalNum;
					break;
				}
				case Statistic::Max: {
					output[i] = *std::max_element(speeds.begin(), speeds.end());
					break;
				}
				default: break;
			}
		}
		return output;
	}

	
	/**
	 * @brief Computes a single statistic for the speed between probes for a given
	 * trajectory.
	 * @tparam PointIterator The iterator type of the point range
	 * @tparam TimeIterator The iterator type of the time range
	 * @param pointsBegin Start of the range of points
	 * @param pointsEnd End of the range of points
	 * @param timeBegin Start of the range of timestamps
	 * @param timeEnd End of the range of timestamps
	 * @param requiredStatistic The requested statistics
	 * @return The computed statistic, corresponding to the requested statistic \p requiredStatistic
	 */
	template <utils::InputIterator<typename GeometryKernel::MovetkPoint> PointIterator, std::input_iterator TimeIterator>
	Speed_t<typename TimeIterator::value_type> operator()(PointIterator pointsBegin,
	                                                      PointIterator pointsEnd,
	                                                      TimeIterator timeBegin,
	                                                      TimeIterator timeEnd,
	                                                      Statistic requiredStatistic) const {
		std::vector<Speed_t<typename TimeIterator::value_type>> stats =
		    this->operator()(pointsBegin, pointsEnd, timeBegin, timeEnd, std::vector<Statistic>{requiredStatistic});
		return stats[0];
	}
};

/**
 * @brief Computes the dominant difference for a given input range
*/
class ComputeDominantDifference {
public:
	template <typename T>
	using Difference_t = decltype(std::declval<T>() - std::declval<T>());

	/**
	 * @brief Computes the dominant sampling interval in the trajectory.
	 * Assumes the trajectory to be sorted on time
	 * In case of integral durations, returns the exact interval that was most common.
	 * In case of floating point durations, returns the center of the bin containing the most
	 * sampling intervals. The number of bins is determined by the parameter given in the constructor.
	 * The bins are computed by setting the center of the first and last bin to the minimum and maximum
	 * interval as found in the trajectory
	 * \complexity O(n \log n), with n the number of probe points
	 * \param trajectory The trajectory
	 * \return Dominant sampling time interval
	 */
	template <std::random_access_iterator InputIterator>
	Difference_t<typename InputIterator::value_type>
	operator()(InputIterator begin, InputIterator end, Difference_t<typename InputIterator::value_type> threshold) const {
		// No differences to compute
		if (begin == end) {
			return 0;
		}

		using Difference = Difference_t<typename InputIterator::value_type>;
		std::vector<Difference> differences;
		differences.reserve(std::distance(begin, end) - 1);


		auto current = begin;
		// Determine all time intervals
		for (; std::next(current) != end; ++current) {
			differences.push_back(*std::next(current) - *current);
		}
		if (differences.size() == 1) {
			return differences[0];
		}

		// Sort the intervals
		std::sort(differences.begin(), differences.end());

		std::size_t maxCount = 1;
		Difference maxVal = *differences.begin();
		Difference prevVal = *differences.begin();
		using It = decltype(differences.begin());
		auto currIt = differences.begin();
		// prevVal by reference so it can be modified during the search loop.
		auto isDifferent = [&prevVal, threshold](const Difference& el) {
			auto pair = std::minmax(prevVal, el);
			return pair.second - pair.first > threshold;
		};

		// Compute the element that has most elements within the threshold distance
		It minIt = differences.begin(), maxIt = currIt;

		for (; currIt != differences.end(); ++currIt) {
			prevVal = *currIt;
			// Find new min and max elements
			minIt = std::find_if(minIt, currIt, [isDifferent](const Difference& el) { return !isDifferent(el); });
			maxIt = std::find_if(maxIt, differences.end(), isDifferent);

			// Count occurences of elements deemed the same
			const auto count = static_cast<std::size_t>(std::distance(minIt, maxIt));
			// Save element with maximum count.
			if (count > maxCount) {
				maxVal = prevVal;
				maxCount = count;
			}
		}
		return maxVal;
	}
};
}  // namespace movetk::statistics

#endif  // MOVETK_SIMILARITY_H