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

/*! @file TrajectoryUtils.h
  *  @brief  A collection of utility functions for trajectories
  *  @authors Aniket Mitra (aniket.mitra@here.com),  Mees van de Kerkhof (m.a.vandekerkhof@uu.nl)
  */

#ifndef MOVETK_TRAJECTORYUTILS_H
#define MOVETK_TRAJECTORYUTILS_H

#include <numeric>
#include <algorithm>
#include <iterator>
#include <array>
#include "movetk/utils/Requirements.h"
#include "movetk/geom/GeometryInterface.h"
#include <boost/iterator.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>

/*! @file TrajectoryUtils.h
   *  @brief  A collection of utility functions for trajectories
   *  @authors Aniket Mitra (aniket.mitra@here.com)
   */
namespace movetk_core
{

    template <class Traits>
    typename Traits::MovetkPoint get_point(typename Traits::NT lat,
                                           typename Traits::NT lon,
                                           typename Traits::GeoProjection &georef)
    {
        movetk_core::MakePoint<typename Traits::GeometryTraits> make_point;
        auto projected_point = georef.project(lat, lon);
        std::reverse(std::begin(projected_point), std::end(projected_point));
        return make_point(std::cbegin(projected_point), std::cend(projected_point));
    }

    /**
     * @brief Returns the x coordinate of the given vector
     * @tparam GeometryTraits The kernel
     * @param v The vector
     * @return The x-coordinate (first coordinate) of the vector
     */
    template <class GeometryTraits>
    typename GeometryTraits::NT get_x(typename GeometryTraits::MovetkVector &v)
    {
        return v * v.basis(0);
    }

    /**
     * @brief Returns the x coordinate of the given point
     * @tparam GeometryTraits The geometry kernel
     * @param p The point
     * @return The x-coordinate (first coordinate) of the point
     */
    template <class GeometryTraits>
    typename GeometryTraits::NT get_x(typename GeometryTraits::MovetkPoint &p)
    {
        movetk_core::MakePoint<GeometryTraits> make_point;
        typename GeometryTraits::MovetkPoint ORIGIN = make_point({0, 0});
        typename GeometryTraits::MovetkVector v = p - ORIGIN;
        return v * v.basis(0);
    }

    /**
     * @brief Returns the y coordinate of the given vector
     * @tparam GeometryTraits The kernel
     * @param v The vector
     * @return The y-coordinate (second coordinate) of the vector
     */
    template <class GeometryTraits>
    typename GeometryTraits::NT get_y(typename GeometryTraits::MovetkVector &v)
    {
        return v * v.basis(1);
    }

    /**
     * @brief Returns the y coordinate of the given point
     * @tparam GeometryTraits The kernel
     * @param p The point
     * @return The y-coordinate (second coordinate) of the point
     */
    template <class GeometryTraits>
    typename GeometryTraits::NT get_y(typename GeometryTraits::MovetkPoint &p)
    {
        movetk_core::MakePoint<GeometryTraits> make_point;
        typename GeometryTraits::MovetkPoint ORIGIN = make_point({0, 0});
        typename GeometryTraits::MovetkVector v = p - ORIGIN;
        return v * v.basis(1);
    }

    /*!
     * @brief checks if input values are a sequence
     * TODO: this check is necessry, but not sufficient for the input to be a sequence.
     * @tparam InputIterator A random access iterator over a set of values of type size_t
     * @param first
     * @param beyond
     * @return True / False
     */
    template <class InputIterator,
              typename = movetk_core::requires_random_access_iterator<InputIterator>,
              typename = movetk_core::requires_size_t<
                  typename InputIterator::value_type>>
    bool is_sequence(InputIterator first, InputIterator beyond)
    {
        //ASSERT_RANDOM_ACCESS_ITERATOR(InputIterator);
        size_t init = 0;
        size_t sum = std::accumulate(first, beyond, init);
        size_t last = *(beyond - 1);
        size_t ExpectedSum = (last * (last + 1)) / 2;
        if (sum != ExpectedSum)
            return false;
        else
            return true;
    }

    /*!
    * @brief Finds the minimum non-zero element over a set of values
    * @tparam GeometryTraits A traits class that defines the expected number type
    * of the input
    * @tparam InputIterator A random access iterator over a set of values of type
    * GeometryTraits::NT
    * @param first
    * @param beyond
    * @return Iterator to the smallest non zero element
    */
    template <class GeometryTraits, class InputIterator,
              typename = movetk_core::requires_random_access_iterator<InputIterator>,
              typename = movetk_core::requires_NT<GeometryTraits,
                                                  typename InputIterator::value_type>>
    InputIterator min_non_zero_element(InputIterator first, InputIterator beyond)
    {
        //ASSERT_RANDOM_ACCESS_ITERATOR(InputIterator);
        if (first == beyond)
            return beyond;
        InputIterator it = first;
        InputIterator smallest = first;
        ++it;
        while (it != beyond)
        {
            if (*smallest == 0)
            {
                smallest = it;
            }
            else if ((*it < *smallest) && (*it != 0))
            {
                smallest = it;
            }
            it++;
        }
        return smallest;
    }

    /*!
     * @brief Calculates the time difference between two consecutive points for an input
     * consisting of an ordered sequence of points
     * @tparam InputIterator  A random access iterator over a set of input points
     * @tparam OutputIterator An insert iterator
     * @param first
     * @param beyond
     * @param iter
     */
    template <class InputIterator, class OutputIterator,
              typename = movetk_core::requires_random_access_iterator<InputIterator>,
              /* typename = movetk_core::requires_date_t<
         typename InputIterator::value_type>,*/
              typename = movetk_core::requires_output_iterator<OutputIterator>
              /*typename = movetk_core::requires_size_t<
        typename OutputIterator::value_type> */
              >
    void get_time_diffs(InputIterator first, InputIterator beyond, OutputIterator iter, bool match_input_range_length = true)
    {
        //TODO Add support for initial value
        //TODO check std::adjacent_difference
        //ASSERT_RANDOM_ACCESS_ITERATOR(InputIterator);
        InputIterator pit = first;
        InputIterator cit = first + 1;
        if (match_input_range_length)
            *iter = 0;
        while (cit != beyond)
        {
            *iter = *cit - *pit;
            pit = cit;
            cit++;
        }
    }

    /*!
     * @brief Computes the spatial distance between two consecutive points for an input
     * consisting of an ordered sequence of points
     * @tparam InputIterator The iterator type for the point sequence
     * @tparam OutputIterator Output iterator for assigning distance
     * @param first Iterator pointing at the first point of the sequence
     * @param beyond End sentinel for the point sequence
     * @param iter Output iterator for assigning the distances
     * @param match_input_range_length If true, adds 0 as first element to the output, such that
     * the number of output elements is equal to the number of input points in the sequence.
     */
    template <class GeometryKernel, class InputIterator, class OutputIterator,
              typename PointDistanceFunc = movetk_core::ComputeLength<GeometryKernel>,
              typename = movetk_core::requires_atleast_input_iterator<InputIterator>,
              typename = movetk_core::requires_output_iterator<OutputIterator>,
              typename = movetk_core::requires_movetk_point<GeometryKernel,
                                                            typename InputIterator::value_type>,
              typename = movetk_core::requires_NT<GeometryKernel,
                                                  typename OutputIterator::value_type>>
    void get_distances(InputIterator first, InputIterator beyond, OutputIterator iter, bool match_input_range_length = true)
    {
        //ASSERT_RANDOM_ACCESS_ITERATOR(InputIterator);
        PointDistanceFunc distance;
        InputIterator pit = first;
        InputIterator cit = std::next(first);
        if (match_input_range_length)
            *iter = 0;
        while (cit != beyond)
        {
            *iter = distance(*pit, *cit);
            pit = cit;
            cit++;
        }
    }

    /*!
     * @brief Computes the spatial distance between two consecutive points for an input
     * consisting of an ordered sequence of measurements, containing spatial coordinates.
     * The result is assigned to the distance element in the input sequence, as identified by DistIdx.
     * Assigns 0 to the first measurement in the sequence, and the distance between point i and i+1 to
     * measurement i+1;
     * TODO: Traits class is rather redundant, use auto!
     * @tparam Traits Class specifying number type of the spatial coordinates
     * @tparam InputIterator The iterator type for the point sequence
     * @tparam LatIdx Index of the latitude coordinate in an input measurement
     * @tparam LonIdx Index of the longitude coordinate in an input measurement
     * @tparam DistIdx Index of the output distance element in an input measuremet.
     * @param first Iterator pointing at the first point of the sequence
     * @param beyond End sentinel for the point sequence
     */
    template <class Traits, class InputIterator, std::size_t LatIdx,
              std::size_t LonIdx, std::size_t DistIdx>
    void get_distances(InputIterator first, InputIterator beyond)
    {
        InputIterator pit = first;
        InputIterator cit = std::next(first);
        std::get<DistIdx>(*pit) = 0;
        while (cit != beyond)
        {
            typename Traits::NT lat0 = std::get<LatIdx>(*pit);
            typename Traits::NT lon0 = std::get<LonIdx>(*pit);
            typename Traits::NT lat1 = std::get<LatIdx>(*cit);
            typename Traits::NT lon1 = std::get<LonIdx>(*cit);
            std::get<DistIdx>(*cit) = distance_exact(lat0, lon0, lat1, lon1);
            pit = cit;
            ++cit;
        }
    }

    /*!
     * Computes speeds for a range of time differences and distances.
     * Note that it is assumed that the number of elements in the distance range is atleast
     * the number of elements in the time difference range.
     * TODO: should we check/assert?
     * @tparam TDiffIterator Iterator type for the range of time differences
     * @tparam DistanceIterator Iterator type for the range of distances
     * @tparam OutputIterator The output iterator
     * @param TdiffFirst Iterator to first time difference in range
     * @param TdiffBeyond End sentinel for time difference range
     * @param DistFirst Iterator to first distance element.
     * @param iter Output iterator for assigning the computed speeds.
     */
    template <class GeometryKernel, class TDiffIterator, class DistanceIterator, class OutputIterator,
              typename = movetk_core::requires_random_access_iterator<TDiffIterator>,
              typename = movetk_core::requires_random_access_iterator<DistanceIterator>,
              typename = movetk_core::requires_output_iterator<OutputIterator>,
              /*typename = movetk_core::requires_size_t<
        typename TDiffIterator::value_type>,*/
              typename = movetk_core::requires_NT<GeometryKernel,
                                                  typename DistanceIterator::value_type>,
              typename = movetk_core::requires_NT<GeometryKernel,
                                                  typename OutputIterator::value_type>>
    void get_speeds(TDiffIterator TdiffFirst, TDiffIterator TdiffBeyond,
                    DistanceIterator DistFirst, OutputIterator iter)
    {
        //ASSERT_RANDOM_ACCESS_ITERATOR(TDiffIterator);
        //ASSERT_RANDOM_ACCESS_ITERATOR(DistanceIterator);

        TDiffIterator CurrTit = TdiffFirst;
        DistanceIterator CurrDit = DistFirst;

        while (CurrTit != TdiffBeyond)
        {
            if (*CurrTit == 0)
            {
                *iter = 0;
            }
            else
            {
                *iter = *CurrDit / *CurrTit;
            }
            CurrDit++;
            CurrTit++;
        }
    }

    /*!
     * Computes speeds for an ordered sequence of measurement in-place. 
     * The speed at point i is given as the average speed between points i-1 and i.
     * The speed at point 0 is set to zero.
     * @tparam Traits Geometry kernel 
     * @tparam InputIterator Iterator type for the range of distances
     * @tparam LatIdx Index of the latitude in the measurement
     * @tparam LonIdx Index of the longitude in the measurement
     * @tparam TsIdx Index of the timestamp in the measurement
     * @tparam SpeedIdx Output index of the speed in the measurement
     * @param first Iterator to first measurement in range
     * @param beyond End sentinel for measurement range
     */
    template <class Traits, class InputIterator, std::size_t LatIdx,
              std::size_t LonIdx, std::size_t TsIdx, std::size_t SpeedIdx>
    void get_speeds(InputIterator first, InputIterator beyond)
    {

        typename Traits::Norm norm;
        typename Traits::GeoProjection ref(std::get<LatIdx>(*first),
                                           std::get<LonIdx>(*first));
        typename Traits::MovetkPoint p = get_point<Traits>(std::get<LatIdx>(*first),
                                                           std::get<LonIdx>(*first), ref);
        typename Traits::MovetkPoint prev = p;
        auto it = first;
        while (++it != beyond - 1)
        {
            p = get_point<Traits>(std::get<LatIdx>(*it),
                                  std::get<LonIdx>(*it), ref);
            typename Traits::MovetkVector v = p - prev;
            typename Traits::NT squared_length = norm(v);
            typename Traits::NT length = norm ^ 1;
            typename Traits::NT delta_t = std::get<TsIdx>(*it) - std::get<TsIdx>(*(it - 1));
            typename Traits::NT speed_v = length / delta_t;
            std::get<SpeedIdx>(*it) = speed_v;
            prev = p;
        }
    }

    /*!
     * @tparam GeometryKernel The geometry kernel
     * @tparam InputIterator Iterator type for the input sequence
     * @tparam PointsIterator 
     * @tparam OutputIterator
     * @param first
     * @param beyond
     * @param[out] iter Ouptut iterator to which headings will be saved
     */
    template <class GeometryKernel, class InputIterator, class PointsIterator, class OutputIterator,
              typename = movetk_core::requires_random_access_iterator<InputIterator>,
              typename = movetk_core::requires_random_access_iterator<PointsIterator>,
              typename = movetk_core::requires_output_iterator<OutputIterator>,
              typename = movetk_core::requires_pair<typename InputIterator::value_type>,
              typename = movetk_core::requires_movetk_point<GeometryKernel,
                                                            typename PointsIterator::value_type>,
              typename = movetk_core::requires_NT<GeometryKernel,
                                                  typename OutputIterator::value_type>>
    void get_headings(InputIterator first, InputIterator beyond,
                      PointsIterator pfirst, OutputIterator iter)
    {
        //ASSERT_RANDOM_ACCESS_ITERATOR(InputIterator);
        //ASSERT_RANDOM_ACCESS_ITERATOR(PointsIterator);
        // formula from: https://www.movable-type.co.uk/scripts/latlong.html
        //TODO use default implementation of Geographiclib
        typedef typename GeometryKernel::NT NT;
        movetk_core::ComputeLength<GeometryKernel> distance;
        InputIterator pit = first;
        InputIterator cit = first + 1;
        PointsIterator p_pit = pfirst;
        PointsIterator c_pit = pfirst + 1;
        *iter = 0;
        iter++;
        NT phi_1, lambda_1, phi_2, lambda_2;
        NT x, y, c_heading, p_heading = 0;
        while (cit != beyond)
        {
            phi_1 = deg2radians(pit->first);
            lambda_1 = deg2radians(pit->second);
            phi_2 = deg2radians(cit->first);
            lambda_2 = deg2radians(cit->second);
            y = std::sin(lambda_2 - lambda_1) * std::cos(phi_2);
            x = std::cos(phi_1) * std::sin(phi_2) - std::sin(phi_1) * std::cos(phi_2) * std::cos(lambda_2 - lambda_1);
            c_heading = std::fmod(rad2deg(std::atan2(y, x)) + 360.0, 360.0);
            if ((c_heading < MOVETK_EPS) && (distance(*p_pit, *c_pit) < MOVETK_EPS))
            {
                *iter = p_heading;
            }
            else
            {
                *iter = c_heading;
            }
            pit = cit;
            p_pit = c_pit;
            p_heading = c_heading;
            cit++;
            c_pit++;
        }
    }

    template <class Traits, class InputIterator, std::size_t LatIdx,
              std::size_t LonIdx, std::size_t HeadingIdx>
    void get_headings(InputIterator first, InputIterator beyond)
    {
        auto it = first;
        while (++it != (beyond - 1))
        {
            typename Traits::NT azimuth = bearing_exact(std::get<LatIdx>(*first), std::get<LonIdx>(*first),
                                                        std::get<LatIdx>(*it), std::get<LonIdx>(*it));
            typename Traits::NT heading = fmod(azimuth + 360.0, 360.0);
            std::get<HeadingIdx>(*it) = heading;
        }
    }

    /*!
     *
     * @tparam GeometryKernel
     * @tparam MaginitudeIterator
     * @tparam DirectionIterator
     * @tparam OutputIterator
     * @param Mfirst
     * @param Mbeyond
     * @param DFirst
     * @param iter
     */
    template <class GeometryKernel, class MagnitudeIterator,
              class DirectionIterator, class OutputIterator,
              typename = movetk_core::requires_random_access_iterator<MagnitudeIterator>,
              typename = movetk_core::requires_random_access_iterator<DirectionIterator>,
              typename = movetk_core::requires_output_iterator<OutputIterator>,
              typename = movetk_core::requires_NT<GeometryKernel,
                                                  typename MagnitudeIterator::value_type>,
              typename = movetk_core::requires_NT<GeometryKernel,
                                                  typename DirectionIterator::value_type>,
              typename = movetk_core::requires_movetk_point<GeometryKernel,
                                                            typename OutputIterator::value_type>>
    void get_velocities(MagnitudeIterator Mfirst, MagnitudeIterator Mbeyond,
                        DirectionIterator DFirst, OutputIterator iter)
    {
        //ASSERT_RANDOM_ACCESS_ITERATOR(MagnitudeIterator);
        //ASSERT_RANDOM_ACCESS_ITERATOR(DirectionIterator);
        MagnitudeIterator mit = Mfirst;
        DirectionIterator dit = DFirst;
        std::array<typename GeometryKernel::NT, 2> PointContainer;
        movetk_core::MakePoint<GeometryKernel> make_point;
        while (mit != Mbeyond)
        {
            PointContainer[0] = (*mit) * cos(deg2radians(*dit));
            PointContainer[1] = (*mit) * sin(deg2radians(*dit));
            typename GeometryKernel::MovetkPoint P = make_point(std::cbegin(PointContainer),
                                                                std::cend(PointContainer));
            *iter = P;
            mit++;
            dit++;
        }
    }

    template <class GeometryTraits>
    typename GeometryTraits::MovetkVector get_velocity(typename GeometryTraits::NT speed,
                                                       typename GeometryTraits::NT heading)
    {
        movetk_core::MakePoint<GeometryTraits> make_point;
        typename GeometryTraits::MovetkPoint ORIGIN = make_point({0, 0});
        std::array<typename GeometryTraits::NT, 2> point_container;
        point_container[0] = speed * cos(movetk_core::deg2radians(heading));
        point_container[1] = speed * sin(movetk_core::deg2radians(heading));
        return make_point(std::cbegin(point_container), std::cend(point_container)) - ORIGIN;
    }

    namespace detail
    {
        // TODO: move to more appropriate location
        template <template <typename...> typename TupleType, typename T, std::size_t I>
        struct RepeatedTypeTuple
        {
            using type = decltype(std::tuple_cat(
                std::declval<TupleType<T>>(),
                std::declval<typename RepeatedTypeTuple<TupleType, T, I - 1>::type>()));
        };
        template <template <typename...> typename TupleType, typename T>
        struct RepeatedTypeTuple<TupleType, T, 1>
        {
            using type = TupleType<T>;
        };

        template <typename SrcType, template <typename...> typename TupleType>
        struct TransferTypes
        {
        };
        template <template <typename...> typename TupleType, typename... Ts>
        struct TransferTypes<std::tuple<Ts...>, TupleType>
        {
            using type = TupleType<Ts...>;
        };

        template <typename T>
        struct TypeHolder
        {
            using type = T;
        };

        template <class GeometryKernel, typename CoordinateIterator, std::size_t... Is>
        auto point_iterators_from_coordinates(TypeHolder<GeometryKernel>, const std::array<std::pair<CoordinateIterator, CoordinateIterator>, GeometryKernel::dim> &beginEndPairs, std::index_sequence<Is...>)
        {
            using NT = typename GeometryKernel::NT;

            using tuple_t_std = typename detail::RepeatedTypeTuple<std::tuple, NT, GeometryKernel::dim>::type;
            using tuple_t = typename detail::TransferTypes<tuple_t_std, boost::tuple>::type;

            std::function<typename GeometryKernel::MovetkPoint(const tuple_t &)> toPoint = [](const tuple_t &coordinateTuple) {
                movetk_core::MakePoint<GeometryKernel> makePoint;
                return makePoint({coordinateTuple.template get<Is>()...});
            };
            auto begin = boost::make_transform_iterator(boost::make_zip_iterator(boost::make_tuple(std::get<Is>(beginEndPairs).first...)), toPoint);
            auto end = boost::make_transform_iterator(boost::make_zip_iterator(boost::make_tuple(std::get<Is>(beginEndPairs).second...)), toPoint);
            return std::make_pair(begin, end);
        }
    } // namespace detail

    /**
     * \brief Creates a start and end iterator that dereference to a point, constructed from the input coordinate iterators per dimension, as specified
     * by the kernel
     * \tparam GeometryKernel The geometry kernel. Specifies the dimensions of the point via GeometryKernel::dim
     * \tparam CoordinateIterator The iterator type for the coordinates per dimension
     * \param beginEndPairs Array of size GeometryKernel::dim containing the start and end iterator per coordinate as a pair
     * \return A pair of start and end iterator that dereference to the correct point.
     */
    template <class GeometryKernel, typename CoordinateIterator>
    auto point_iterators_from_coordinates(const std::array<std::pair<CoordinateIterator, CoordinateIterator>, GeometryKernel::dim> &beginEndPairs)
    {
        return detail::point_iterators_from_coordinates(detail::TypeHolder<GeometryKernel>{}, beginEndPairs, std::make_index_sequence<GeometryKernel::dim>{});
    }

    /*!
    *
    * @tparam PolyLineIdxIterator
    */
    template <class PolyLineIdxIterator,
              typename = movetk_core::requires_random_access_iterator<PolyLineIdxIterator>>
    class SegmentIdGenerator
    {
    private:
        size_t SegmentId = 0;
        PolyLineIdxIterator it;
        PolyLineIdxIterator __first, __beyond;

    public:
        /*!
             *
             * @param first
             * @param beyond
             */
        SegmentIdGenerator(PolyLineIdxIterator first,
                           PolyLineIdxIterator beyond)
            : it(first), __first(first), __beyond(beyond){};

        /*!
             *
             * @tparam PolyLineIteratorType
             * @param iter
             * @return
             */
        template <class PolyLineIteratorType,
                  typename = movetk_core::requires_random_access_iterator<PolyLineIdxIterator>,
                  typename = movetk_core::requires_equality<typename PolyLineIteratorType::value_type,
                                                            typename PolyLineIdxIterator::value_type::value_type>>
        size_t getSegmentID(PolyLineIteratorType iter)
        {
            if (it == __beyond)
            {
                return SegmentId;
            }
            if (iter == *it)
            {
                SegmentId++;
                if (it != __beyond)
                    it++;
                return SegmentId;
            }
            else
            {
                return SegmentId;
            }
        }

        void reset()
        {
            it = __first;
            SegmentId = 0;
        }

        SegmentIdGenerator() = default;
    };

    /**
     * \brief For a range of intervals, specified as pairs of numbers (TODO: order?), merges
     * overlapping intervals. 
     * TODO: move this to more appropriate header
     * by Mees van de Kerkhof
     * \tparam GeometryKernel The geometry kernel
     * \tparam InputIterator Type for the input intervals range
     * \param first Iterator to first element in the range of intervals
     * \param beyond End sentinel for the range of intervals
     * \param sorted Is the given input range sorted? TODO: by what exactly?
     * \return 
     */
    template <class GeometryKernel, class InputIterator,
              typename = movetk_core::requires_random_access_iterator<InputIterator>,
              typename = movetk_core::requires_pair<typename InputIterator::value_type>,
              typename = movetk_core::requires_NT<GeometryKernel,
                                                  typename InputIterator::value_type::first_type>,
              typename = movetk_core::requires_NT<GeometryKernel,
                                                  typename InputIterator::value_type::second_type>>
    InputIterator merge_intervals(InputIterator first, InputIterator beyond, bool sorted = false)
    {
        if (!sorted)
            std::sort(first, beyond, [](auto a, auto b) {
                if (a.first != b.first)
                    return a.first > b.first;
                else
                    return a.second > b.second;
            });

        InputIterator curr = first;
        InputIterator next = curr + 1;
        InputIterator last = beyond - 1;
        std::size_t moves = 0;
        while (curr != last)
        {
            if (next->second >= curr->first)
            {
                moves++;
                if (curr->second > next->second)
                    *curr = std::make_pair(next->first, curr->second);
                else
                    *curr = std::make_pair(next->first, next->second);
                *next = std::move(*(next + moves));
                last--;
            }
            else
            {
                curr++;
                next++;
                if (moves > 0)
                    *next = std::move(*(next + moves));
            }
        }
        return (last + 1);
    }

    template <class GeometryTraits, class Norm, class InputIterator, class OutputIterator>
    void compute_curve_squared_length(InputIterator first, InputIterator beyond, OutputIterator result)
    {
        Norm norm;
        auto it = first;
        while (it != (beyond - 1))
        {
            auto jit = it + 1;
            typename GeometryTraits::NT sum = 0;
            while (jit != beyond)
            {
                typename GeometryTraits::MovetkVector v = *jit - *(jit - 1);
                typename GeometryTraits::NT squared_length = norm(v);
                sum += squared_length;
                jit++;
            }
            result = sum;
            it++;
        }
    }

    template <class Trajectory_t, class OfStream>
    void downsample(Trajectory_t trajectory, OfStream &ofstream, std::size_t interval)
    {
        std::size_t idx = 0;
        for (auto &row : trajectory)
        {
            if (idx == 0)
            {
                print_tuple(ofstream, row);
                ofstream << "\n";
            }
            else if (idx % 10 == 0)
            {
                print_tuple(ofstream, row);
                ofstream << "\n";
            }
            idx++;
        }
    }

    template <class Trajectory_t, class OfStream>
    void floor(Trajectory_t trajectory, OfStream &ofstream, std::size_t interval)
    {
        std::size_t idx = 0;
        std::size_t num_elems = std::distance(trajectory.begin(), trajectory.end());
        std::size_t last_idx = (num_elems - 1) - ((num_elems - 1) % interval);
        for (auto &row : trajectory)
        {
            if (idx <= last_idx)
            {
                print_tuple(ofstream, row);
                ofstream << "\n";
            }
            idx++;
        }
    }
}; // namespace movetk_core

#endif //MOVETK_TRAJECTORYUTILS_H
