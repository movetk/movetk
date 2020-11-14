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

#include <iterator>
#include <functional>
#include "movetk/TrajectoryTraits.h"
#include "movetk/geom/GeometryInterface.h"

namespace movetk_algorithms {
    /**
     * @brief Helper for accessing fields of a trajectory iterator.
     * Supports both tabular and columnar mode
     * @tparam Trajectory The trajectory type
     */
    template<class Trajectory>
    struct IteratorAccessor
    {
        // Reference to the parent trajectory
        Trajectory* m_trajectory = nullptr;
        using TrajectoryIt = typename Trajectory::TrajectoryIterator;
        // The iterator
        TrajectoryIt m_it;

        using Self_t = IteratorAccessor<Trajectory>;

        // Is the trajectory stored in columnar mode
        static constexpr bool isColumnar = Trajectory::storage_scheme() == StorageScheme::columnar;

        IteratorAccessor(Trajectory& trajectory, TrajectoryIt it):
        m_trajectory(&trajectory), m_it(it){}

        IteratorAccessor(Trajectory& trajectory) :
            m_trajectory(&trajectory), m_it({}) {}

        /**
         * @brief Creates an accessor for the start iterator of the trajectory
         * @param trajectory The target trajectory
         * @return Accessor with the start iterator assigned
         */
        static Self_t start(Trajectory& trajectory)
        {
            if constexpr(Self_t::isColumnar){
                return Self_t(trajectory, trajectory.row_begin());
            }
            else
            {
                return Self_t(trajectory, trajectory.begin());
            }
        }

        /**
         * @brief Creates an accessor for the end iterator of the trajectory
         * @param trajectory The target trajectory
         * @return Accessor with the end iterator assigned
         */
        static Self_t end(Trajectory& trajectory)
        {
            if constexpr (Self_t::isColumnar) {
                return Self_t(trajectory, trajectory.row_end());
            }
            else
            {
                return Self_t(trajectory, trajectory.end());
            }
        }

        /**
         * @brief Retrieves the value of the field with the given id from
         * the current iterator location
         * @tparam Idx The ID of the field
         * @return The value of the field at the current iterator location
         */
        template<int Idx>
        typename Trajectory::template FieldType<Idx> getField()
        {
            if constexpr(Trajectory::storage_scheme() == StorageScheme::columnar){
                // Iterator is a tuple of iterators
                return *std::get<Idx>(m_it);
            }
            else
            {
                return std::get<Idx>(*m_it);
            }
        }
        Self_t& operator*()
        {
            return *this;
        }

        /**
         * @brief Creates a new iterator that is the current operator, offset by
         * the given offset
         * @param offset The offset
         * @return The new iterator
         */
        Self_t operator+(int offset)
        {
            Self_t newIt(*m_trajectory, m_it);
            for(int i = 0; i < offset; ++i)
            {
                ++newIt;
            }
            return newIt;
        }

        /**
         * @brief Moves the iterator to the next 
         * @return Reference to this object.
         */
        Self_t& operator++()
        {
            if constexpr(Trajectory::storage_scheme() == StorageScheme::columnar){
                m_trajectory->row_next(m_it);
            }
            else
            {
                ++m_it;
            }
            return *this;
        }

        /**
         * @brief Returns whether the iterators of this and another accessor are not the same
         * @param other The other accessor
         * @return Are the inner iterators not the same
         */
        bool operator!=(const Self_t& other) const
        {
            return m_it != other.m_it;
        }
        bool operator==(const Self_t& other) const
        {
            return m_it == other.m_it;
        }
    };

    template <class Trajectory, class GeometryKernel, class PointToPointDistance, int XcoordIdx, int YcoordIdx>
    class TrajectoryLength {
    private:
        typedef typename GeometryKernel::NT NT;
        PointToPointDistance m_dist;
        using ItAccessor = IteratorAccessor<Trajectory>;
        
        /**
         * @brief Aggregator function for edge lengths of the trajectory
         */
        struct AggregateLength
        {
            NT total = 0;
            PointToPointDistance m_dist;
            // Underlying trajectory
            Trajectory& m_trajectory;

            AggregateLength(Trajectory& trajectory, PointToPointDistance dist):
            m_trajectory(trajectory),
            m_dist(dist){}

            void operator()(IteratorAccessor<Trajectory> point0, IteratorAccessor<Trajectory> point1)
            {
                movetk_core::MakePoint<GeometryKernel> make_point;
                auto p0 = make_point({ 
                    point0.template getField<XcoordIdx>(),
                    point0.template getField<YcoordIdx>()
                });
                auto p1 = make_point({
                    point1.template getField<XcoordIdx>(),
                    point1.template getField<YcoordIdx>()
                    });
                total += m_dist(p0, p1);
            }
        };
    public:
        explicit TrajectoryLength(PointToPointDistance n): m_dist(n){}
        
        typename GeometryKernel::NT operator()(Trajectory trajectory)
        {
            AggregateLength aggregator(trajectory, m_dist);
            ItAccessor current = ItAccessor::start(trajectory);
            ItAccessor end = ItAccessor::end(trajectory);

            // Empty trajectory: return zero length.
            if (current == end) return 0;

            for(; (current + 1) != end; ++current)
            {
                aggregator(current,current+1);
            }

            return aggregator.total;
        }
    };

    template <typename Trajectory, int TimeStampIdx>
    class TrajectoryDuration
    {
        using ItAccessor = IteratorAccessor<Trajectory>;
    public:
        using Time_t = typename Trajectory::template FieldType<TimeStampIdx>;
        // Duration is the difference of the time type.
        using Duration_t = decltype(std::declval<Time_t>() - std::declval<Time_t>());

        /**
         * @brief Computes the duration of the trajectory. Does not assume the trajectory to be
         * sorted by time. The duration is returned with the type of the timestamp field in the
         * trajectory. Note that the timestamp type should be comparable and subtractable.
         * @tparam Trajectory The trajectory type
         * @param trajectory The trajectory
         * @complexity O(n), with n the number of points
         * @return Duration of the trajectory, returned as the type of the difference of the
         */
        Duration_t operator()(Trajectory trajectory) const
        {
            // Setup the iterators
            ItAccessor current = ItAccessor::start(trajectory);
            ItAccessor end = ItAccessor::end(trajectory);

            // Catch empty case
            if (current == end) return 0;

            auto getTime = [](auto& it)
            {
                return it.template getField<TimeStampIdx>();
            };

            // Minimum and maximum time
            Time_t maxTime;
            Time_t minTime = maxTime = getTime(current);

            // Find min and max time over all points
            auto minMaxIts = std::minmax_element(current, end, [getTime](auto& it0, auto& it1)
            {
                return getTime(it0) < getTime(it1);
            });

            return getTime(minMaxIts.second) - getTime(minMaxIts.first);
        }
    };

    template<typename Trajectory, typename GeometryKernel, class PointToPointDistance, int XcoordIdx, int YcoordIdx, int TimeStampIdx>
    class TrajectorySpeedStatistic
    {
    public:
        enum class Statistic
        {
            Median,
            Mean,
            Max,
            Min,
            Variance
        };
        // Time type
        using Time_t = typename Trajectory::template FieldType<TimeStampIdx>;
        // Duration is the difference of the time type.
        using Duration_t = decltype(std::declval<Time_t>() - std::declval<Time_t>());
        // Point type
        using Point_t = typename GeometryKernel::MovetkPoint;
        // Distance type
        using Dist_t = decltype(std::declval<PointToPointDistance>()(std::declval<Point_t>(), std::declval<Point_t>()));
        // Speed type = Distance type / Duration type
        using Speed_t = decltype(std::declval<Duration_t>() / std::declval<Dist_t>());
    private:
        using ItAccessor = IteratorAccessor<Trajectory>;
        // The distance function
        PointToPointDistance m_dist;

        /**
         * @brief Constructs a point for the given accessor with the coordinates
         * at the specified field IDs
         * @param it The accessor
         * @return The point 
         */
        Point_t getPoint(ItAccessor it) 
        {
            movetk_core::MakePoint<GeometryKernel> make_point;
            return make_point({ it.template getField<XcoordIdx>(), it.template getField<YcoordIdx>() });
        }

        /**
         * @brief Determines the speed between the probes at the current iterator it and the next element
         * Assumes linear interpolation of the trajectory.
         * @param it The current iterator 
         * @return The speed between the current and next point
         */
        Speed_t getSpeed(ItAccessor it0, ItAccessor it1)
        {
            auto timeDiff = it1.template getField<TimeStampIdx>() - it0.template getField<TimeStampIdx>();
            // Calculate the distance
            auto dist = m_dist(getPoint(it0), getPoint(it1));
            // Return the speed
            return dist / timeDiff;
        }
    public:

        explicit TrajectorySpeedStatistic(PointToPointDistance dist):
        m_dist(dist){}

        std::vector<Speed_t> operator()(Trajectory trajectory, const std::vector<Statistic>& requiredStatistics)
        {
            std::vector<Speed_t> speeds;
            movetk_core::MakePoint<GeometryKernel> make_point;
            std::vector<Speed_t> output(requiredStatistics.size(), 0);

            ItAccessor current = ItAccessor::start(trajectory);
            ItAccessor end = ItAccessor::end(trajectory);

            if (current == end) return output;

            // Find min and max time over all points
            for (; (current + 1) != end; ++current)
            {
                speeds.push_back(getSpeed(current,current+1));
            }

            // Check if the speeds are empty to avoid divisions by zero
            if (speeds.empty()) return output;

            // Calculate the statistics
            for(int i = 0; i < requiredStatistics.size(); ++i)
            {
                switch (requiredStatistics[i])
                {
                case Statistic::Median:
                {
                    std::nth_element(speeds.begin(), speeds.begin() + speeds.size() / 2, speeds.end());
                    // Odd size
                    if (speeds.size() & 1)
                    {
                        output[i] = speeds[speeds.size() / 2];
                    }
                    else
                    {
                        // Even size
                        output[i] = 0.5 * (speeds[speeds.size() / 2] + speeds[speeds.size() / 2 - 1]);
                    }
                    break;
                }
                case Statistic::Mean:
                {
                    // Compute the average
                    output[i] = std::accumulate(speeds.begin(), speeds.end(), static_cast<Speed_t>(0)) / static_cast<Speed_t>(speeds.size());
                    break;
                }
                case Statistic::Min:
                {
                    output[i] = *std::min_element(speeds.begin(), speeds.end());
                    break;
                }
                case Statistic::Variance:
                {
                    auto totalNum = static_cast<Speed_t>(speeds.size());
                    auto mean = std::accumulate(speeds.begin(), speeds.end(), static_cast<Speed_t>(0)) / totalNum;
                    // Lambda for accumulating squares of differences to mean.
                    auto sqDiff = [&mean](Speed_t accum, Speed_t newVal)
                    {
                        return accum + (newVal - mean)*(newVal - mean);
                    };
                    output[i] = std::accumulate(speeds.begin(), speeds.end(), static_cast<Speed_t>(0), sqDiff) / totalNum;
                    break;
                }
                case Statistic::Max:
                {
                    output[i] = *std::max_element(speeds.begin(), speeds.end());
                    break;
                }
                }
            }
            return output;
        }

        Speed_t operator()(Trajectory trajectory, Statistic requiredStatistic)
        {
            auto stats = this->operator()(trajectory, std::vector<Statistic>{ requiredStatistic });
            return stats[0];
        }
    };

    template<typename Trajectory, int TimeStampIdx>
    class TrajectoryTimeIntervalMode
    {
        using ItAccessor = IteratorAccessor<Trajectory>;
        // Number of bins to use when computing mode on double precision durations
        int m_binNum;
    public:
        using Time_t = typename Trajectory::template FieldType<TimeStampIdx>;
        // Duration is the difference of the time type.
        using Duration_t = decltype(std::declval<Time_t>() - std::declval<Time_t>());

        /**
         * @brief Constructs the algorithm
         * @param binNum The number of bins to use for double precision durations.
         */
        TrajectoryTimeIntervalMode(int binNum = 5): m_binNum(binNum){}

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
        Duration_t operator()(Trajectory trajectory)
        {
            std::vector<Duration_t> intervals;
            ItAccessor current = ItAccessor::start(trajectory);
            ItAccessor end = ItAccessor::end(trajectory);
            // Determine all time intervals
            for(; (current + 1) != end; ++current)
            {
                auto next = current + 1;
                auto timeDiff = next.template getField<TimeStampIdx>() - current.template getField<TimeStampIdx>();
                intervals.push_back(timeDiff);
            }

            // Return 0 when no intervals exist
            if (intervals.empty()) return 0;

            // Sort the intervals
            std::sort(intervals.begin(), intervals.end());

            // With integral types, we count occurences
            if constexpr(std::is_integral_v<Duration_t>){
                // Count occurences of the intervals
                int occurences = 0;
                int maxOccurences = 0;
                Duration_t maxOccuredValue, prevValue;
                bool isFirst = true;
                for (auto val : intervals)
                {
                    if (isFirst || val != prevValue)
                    {
                        prevValue = val;
                        occurences = 1;
                    }
                    else
                    {
                        occurences++;
                    }
                    isFirst = false;
                    if (maxOccurences < occurences)
                    {
                        maxOccurences = occurences;
                        maxOccuredValue = val;
                    }
                }
                return maxOccuredValue;
            }
            // With double types, use actual bins instead.
            else
            {
                // Assumes that subtraction/addition and multiplication on
                // the durations is defined
                assert(m_binNum > 1);

                // Get min and max interval sizes
                auto minInt = intervals[0];
                auto maxInt = intervals[intervals.size() - 1];
                auto diff = maxInt - minInt;

                // Check explicitly that not all values are the same
                if (diff == 0) return intervals[0]; // This works for doubles since exact zero exists

                // Determine bin width. The bins are assigned
                // such that the min and max value are the centers
                // of the outer bins.
                auto binWidth = diff / (m_binNum - 1);
                auto start = minInt - 0.5 * binWidth;
                //Setup the bins
                std::vector<int> binCounts(m_binNum, 0);
                // Count
                for(auto el : intervals)
                {
                    const int bin = static_cast<int>((el - start) / binWidth);
                    ++binCounts[bin];
                }
                // Return center of bin with maximum number of elements
                const auto maxEl = std::max_element(binCounts.begin(), binCounts.end());
                const int maxBin = static_cast<int>(maxEl - binCounts.begin());
                return start + (maxBin + 0.5) * binWidth;
            }
        }
    };
}

#endif //MOVETK_SIMILARITY_H