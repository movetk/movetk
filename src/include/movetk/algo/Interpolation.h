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

//
// Created by onur on 1/24/18.
// Other Interpolators added by Aniket Mitra (aniket.mitra@here.com)
//

#ifndef MOVETK_INTERPOLATION_H
#define MOVETK_INTERPOLATION_H

#include <random>
#include <algorithm>
#include <numeric>
#include "movetk/geo/geo.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "movetk/algo/AlgorithmTraits.h"

namespace movetk_algorithms
{

    template <class _GeometryTraits, class _GeoProjection,
              class _ProbeTraits, class _Norm>
    struct InterpolationTraits
    {
        typedef _GeometryTraits GeometryTraits;
        typedef _GeoProjection GeoProjection;
        typedef _ProbeTraits ProbeTraits;
        typedef typename ProbeTraits::ProbePoint ProbePoint;
        typedef typename GeometryTraits::MovetkPoint MovetkPoint;
        typedef typename GeometryTraits::MovetkVector MovetkVector;
        typedef typename GeometryTraits::NT NT;
        typedef _Norm Norm;
        typedef typename movetk_core::mbr_selector<GeometryTraits, Norm,
                                                   typename GeometryTraits::MovetkMinimumBoundingRectangle>::MinimumBoundingRectangle MinimumBoundingRectangle;
    };

    template <class TrajectoryPoint, int LatIdx, int LonIdx, int SpeedIdx = -1, int HeadingIdx = -1>
    class DummyOffsetInterpolator_2
    {
    public:
        /**
         * Return an interpolated ProbePoint between p1 and p2 at the given offset from p1.
         * This dummy interpolator only interpolates lat and lon and uses the speed and heading from p2.
         */
        TrajectoryPoint operator()(const TrajectoryPoint &p1, const TrajectoryPoint &p2, double offset)
        {
            double lat1 = std::get<LatIdx>(p1), lon1 = std::get<LonIdx>(p1),
                   lat2 = std::get<LatIdx>(p2), lon2 = std::get<LonIdx>(p2);
            double bearing = bearing_exact(lat1, lon1, lat2, lon2);

            double lat, lon;
            destination_by_bearing_exact(lat1, lon1, bearing, offset, lat, lon);

            TrajectoryPoint p = p2;
            std::get<LatIdx>(p) = lat;
            std::get<LonIdx>(p) = lon;
            return p;
        }
    };

    template <class AlgorithmTag,
              class InterpolationTraits,
              int LatIdx, int LonIdx, int TsIdx,
              int SpeedIdx = -1, int HeadingIdx = -1>
    class Interpolator
    {
    };

    template <class InterpolationTraits,
              int LatIdx, int LonIdx, int TsIdx,
              int SpeedIdx, int HeadingIdx>
    class Interpolator<movetk_algorithms::linear_interpolator_tag,
                       InterpolationTraits, LatIdx, LonIdx, TsIdx,
                       SpeedIdx, HeadingIdx>
    {
    private:
        typename InterpolationTraits::Norm norm;
        typename InterpolationTraits::GeoProjection ref;
        movetk_core::Scaling<typename InterpolationTraits::GeometryTraits> scale;
        movetk_core::Translation<typename InterpolationTraits::GeometryTraits> translate;

    public:
        Interpolator(typename InterpolationTraits::NT reflat,
                     typename InterpolationTraits::NT reflon)
        {
            ref = typename InterpolationTraits::GeoProjection(reflat, reflon);
        }

        template <class TSIterator, class OutputIterator>
        void operator()(const typename InterpolationTraits::ProbePoint &probe_u,
                        const typename InterpolationTraits::ProbePoint &probe_v,
                        TSIterator first, TSIterator beyond, OutputIterator result)
        {

            auto lat_u = std::get<LatIdx>(probe_u);
            auto lon_u = std::get<LonIdx>(probe_u);
            typename InterpolationTraits::MovetkPoint p_u = movetk_core::get_point<InterpolationTraits>(lat_u, lon_u,
                                                                                                        ref);

            auto lat_v = std::get<LatIdx>(probe_v);
            auto lon_v = std::get<LonIdx>(probe_v);
            typename InterpolationTraits::MovetkPoint p_v = movetk_core::get_point<InterpolationTraits>(lat_v, lon_v,
                                                                                                        ref);

            auto ts_u = std::get<TsIdx>(probe_u);
            auto ts_v = std::get<TsIdx>(probe_v);

            assert(ts_v > ts_u);

            std::size_t delta_t = ts_v - ts_u;
            typename InterpolationTraits::NT interval = static_cast<typename InterpolationTraits::NT>(delta_t);

            std::vector<typename InterpolationTraits::ProbePoint> interpolated_pts;

            auto tit = first;

            interpolated_pts.push_back(probe_u);

            while (++tit != (beyond - 1))
            {
                assert(*tit >= ts_u);
                assert(*tit <= ts_v);
                typename InterpolationTraits::ProbePoint probe;
                delta_t = *tit - ts_u;
                typename InterpolationTraits::NT fraction = delta_t / interval;
                typename InterpolationTraits::MovetkVector v = scale(p_u, p_v, fraction);
                typename InterpolationTraits::MovetkPoint p = translate(p_u, v);
                typename InterpolationTraits::NT x = movetk_core::get_x<typename InterpolationTraits::GeometryTraits>(
                    p);
                typename InterpolationTraits::NT y = movetk_core::get_y<typename InterpolationTraits::GeometryTraits>(
                    p);
                auto geocoordinate = ref.inverse(y, x);
                std::get<LatIdx>(probe) = geocoordinate[0];
                std::get<LonIdx>(probe) = geocoordinate[1];
                std::get<TsIdx>(probe) = *tit;
                interpolated_pts.push_back(probe);
            }

            interpolated_pts.push_back(probe_v);

            auto ip_first = std::begin(interpolated_pts);
            auto ip_beyond = std::end(interpolated_pts);

            if constexpr (SpeedIdx > -1)
            {
                std::get<SpeedIdx>(*ip_first) = std::get<SpeedIdx>(probe_u);
                std::get<SpeedIdx>(*(ip_beyond - 1)) = std::get<SpeedIdx>(probe_v);
                movetk_core::get_speeds<InterpolationTraits, decltype(ip_first), LatIdx,
                                        LonIdx, TsIdx, SpeedIdx>(ip_first, ip_beyond);
            }

            if constexpr (HeadingIdx > -1)
            {
                std::get<HeadingIdx>(*ip_first) = std::get<HeadingIdx>(probe_u);
                std::get<HeadingIdx>(*(ip_beyond - 1)) = std::get<HeadingIdx>(probe_v);
                movetk_core::get_headings<InterpolationTraits, decltype(ip_first), LatIdx,
                                          LonIdx, HeadingIdx>(ip_first, ip_beyond);
            }

            std::move(ip_first, ip_beyond, result);
        }
    };

    template <class InterpolationTraits,
              int LatIdx, int LonIdx, int TsIdx,
              int SpeedIdx, int HeadingIdx>
    class Interpolator<movetk_algorithms::kinematic_interpolator_tag,
                       InterpolationTraits, LatIdx, LonIdx, TsIdx,
                       SpeedIdx, HeadingIdx>
    {
        // based on https://doi.org/10.1080/13658816.2015.1081909
    private:
        typename InterpolationTraits::Norm norm;
        typename InterpolationTraits::GeoProjection ref;
        movetk_core::Scaling<typename InterpolationTraits::GeometryTraits> scale;
        movetk_core::Translation<typename InterpolationTraits::GeometryTraits> translate;
        movetk_core::MakePoint<typename InterpolationTraits::GeometryTraits> make_point;
        typename InterpolationTraits::MovetkPoint ORIGIN = make_point({0, 0});

    public:
        Interpolator(typename InterpolationTraits::NT reflat,
                     typename InterpolationTraits::NT reflon)
        {
            ref = typename InterpolationTraits::GeoProjection(reflat, reflon);
        }

        template <class TSIterator, class OutputIterator>
        void operator()(typename InterpolationTraits::ProbePoint &probe_u,
                        typename InterpolationTraits::ProbePoint &probe_v,
                        TSIterator first, TSIterator beyond, OutputIterator result)
        {

            auto lat_u = std::get<LatIdx>(probe_u);
            auto lon_u = std::get<LonIdx>(probe_u);
            typename InterpolationTraits::MovetkPoint p_u = movetk_core::get_point<InterpolationTraits>(lat_u, lon_u,
                                                                                                        ref);

            auto lat_v = std::get<LatIdx>(probe_v);
            auto lon_v = std::get<LonIdx>(probe_v);
            typename InterpolationTraits::MovetkPoint p_v = movetk_core::get_point<InterpolationTraits>(lat_v, lon_v,
                                                                                                        ref);

            typename InterpolationTraits::MovetkVector velocity_u = movetk_core::get_velocity<typename InterpolationTraits::GeometryTraits>(
                std::get<SpeedIdx>(probe_u),
                std::get<HeadingIdx>(probe_u));
            typename InterpolationTraits::MovetkVector velocity_v = movetk_core::get_velocity<typename InterpolationTraits::GeometryTraits>(
                std::get<SpeedIdx>(probe_v),
                std::get<HeadingIdx>(probe_v));

            typename InterpolationTraits::MovetkVector delta_velocity = velocity_v - velocity_u;

            typename InterpolationTraits::MovetkVector delta_position = p_v - p_u;

            auto t_u = std::get<TsIdx>(probe_u);
            auto t_v = std::get<TsIdx>(probe_v);

            auto delta_t = t_v - t_u;
            auto delta_t_squared = delta_t * delta_t;
            auto delta_t_cube = delta_t_squared * delta_t;
            typename InterpolationTraits::NT interval = static_cast<typename InterpolationTraits::NT>(delta_t);

            typename InterpolationTraits::NT norm_delta_velocity = norm(delta_velocity);
            typename InterpolationTraits::NT norm_delta_position = norm(delta_position);
            typename InterpolationTraits::NT displacement = norm ^ 1;

            if (norm_delta_position > MOVETK_EPS && norm_delta_velocity < MOVETK_EPS)
            {
                typename InterpolationTraits::NT speed_v = displacement / delta_t;
                velocity_v = movetk_core::get_velocity<typename InterpolationTraits::GeometryTraits>(speed_v,
                                                                                                     std::get<HeadingIdx>(
                                                                                                         probe_v));
                std::get<SpeedIdx>(probe_v) = speed_v;
            }

            typename InterpolationTraits::MovetkVector scaled_velocity = scale(velocity_u, delta_t);

            typename InterpolationTraits::NT numerator =
                movetk_core::get_x<typename InterpolationTraits::GeometryTraits>(delta_position) -
                movetk_core::get_x<typename InterpolationTraits::GeometryTraits>(scaled_velocity) -
                movetk_core::get_x<typename InterpolationTraits::GeometryTraits>(delta_velocity) / 2.0;
            typename InterpolationTraits::NT denominator = delta_t_cube / 6.0 - delta_t_squared / 4.0;
            typename InterpolationTraits::NT m_x = numerator / denominator;

            numerator = movetk_core::get_y<typename InterpolationTraits::GeometryTraits>(delta_position) -
                        movetk_core::get_y<typename InterpolationTraits::GeometryTraits>(scaled_velocity) -
                        movetk_core::get_y<typename InterpolationTraits::GeometryTraits>(delta_velocity) / 2.0;
            typename InterpolationTraits::NT m_y = numerator / denominator;

            typename InterpolationTraits::NT b_x =
                movetk_core::get_x<typename InterpolationTraits::GeometryTraits>(delta_velocity) / delta_t_squared -
                m_x / 2.0;
            typename InterpolationTraits::NT b_y =
                movetk_core::get_y<typename InterpolationTraits::GeometryTraits>(delta_velocity) / delta_t_squared -
                m_y / 2.0;

            typename InterpolationTraits::MovetkPoint m = make_point({m_x, m_y});
            typename InterpolationTraits::MovetkPoint b = make_point({b_x, b_y});

            typename InterpolationTraits::MovetkVector v1 = scale(b, ORIGIN, delta_t_squared);
            typename InterpolationTraits::MovetkVector v2 = scale(m, ORIGIN, delta_t_squared / 2.0);

            typename InterpolationTraits::MovetkVector rhs = v1 + v2;
            typename InterpolationTraits::MovetkVector eps = delta_velocity - rhs;
            assert(eps * eps < MOVETK_EPS);

            v1 = scale(b, ORIGIN, delta_t_squared / 2.0);
            v2 = scale(m, ORIGIN, delta_t_squared / 6.0);

            rhs = v1 + v2;
            eps = delta_position - scaled_velocity - rhs;
            assert(eps * eps < MOVETK_EPS);

            std::size_t num_elements = std::distance(first, beyond);

            std::vector<typename InterpolationTraits::ProbePoint> interpolated_pts(num_elements);
            std::vector<typename InterpolationTraits::MovetkVector> velocities;
            auto ip_first = std::begin(interpolated_pts);
            auto ip_it = ip_first;
            *ip_it = probe_u;

            auto tit = first;
            auto idx = 2.0;
            while (++tit != (beyond - 1))
            {

                t_u = std::get<TsIdx>(probe_u);
                t_v = std::get<TsIdx>(probe_v);

                assert(*tit >= t_u);
                assert(*tit <= t_v);

                typename InterpolationTraits::ProbePoint probe;

                delta_t = *tit - t_u;
                delta_t_squared = delta_t * delta_t;
                delta_t_cube = delta_t_squared * delta_t;

                typename InterpolationTraits::NT fraction = delta_t / interval;

                scaled_velocity = velocity_u;
                scaled_velocity *= delta_t;

                typename InterpolationTraits::NT op1 =
                    movetk_core::get_x<typename InterpolationTraits::GeometryTraits>(b) * (delta_t_squared / 2.0);
                typename InterpolationTraits::NT op2 =
                    movetk_core::get_x<typename InterpolationTraits::GeometryTraits>(m) * (delta_t_cube / 6.0);

                typename InterpolationTraits::NT interpolated_x =
                    op1 + op2 + movetk_core::get_x<typename InterpolationTraits::GeometryTraits>(scaled_velocity) +
                    movetk_core::get_x<typename InterpolationTraits::GeometryTraits>(p_u);

                op1 = movetk_core::get_y<typename InterpolationTraits::GeometryTraits>(b) * (delta_t_squared / 2.0);
                op2 = movetk_core::get_y<typename InterpolationTraits::GeometryTraits>(m) * (delta_t_cube / 6.0);

                typename InterpolationTraits::NT interpolated_y =
                    op1 + op2 + movetk_core::get_y<typename InterpolationTraits::GeometryTraits>(scaled_velocity) +
                    movetk_core::get_y<typename InterpolationTraits::GeometryTraits>(p_u);

                auto geocoordinate = ref.inverse(interpolated_y, interpolated_x);

                std::get<LatIdx>(probe) = geocoordinate[0];
                std::get<LonIdx>(probe) = geocoordinate[1];

                std::get<TsIdx>(probe) = *tit;

                *(++ip_it) = probe;

                v1 = scale(b, ORIGIN, delta_t);
                v2 = scale(m, ORIGIN, delta_t_squared / 2.0);

                typename InterpolationTraits::MovetkVector fwdV = velocity_u + v1 + v2;

                fwdV *= (1 - fraction);
                //fwdV *= 0.5;

                t_u = std::get<TsIdx>(probe);
                t_v = std::get<TsIdx>(probe_v);

                delta_t = t_v - t_u;
                delta_t_squared = delta_t * delta_t;
                delta_t_cube = delta_t_squared * delta_t;

                v1 = scale(b, ORIGIN, delta_t);
                v2 = scale(m, ORIGIN, delta_t_squared / 2.0);

                typename InterpolationTraits::MovetkVector bwdV = velocity_v - v1 - v2;

                bwdV *= fraction;
                //bwdV *= 0.5;

                typename InterpolationTraits::MovetkVector V = fwdV + bwdV;

                //std::cout<<eps<<","<<fwdV<<","<<bwdV<<"\n";

                velocities.push_back(V);
            }

            *(++ip_it) = probe_v;

            auto ip_beyond = std::end(interpolated_pts);

            if constexpr (HeadingIdx > -1)
            {
                movetk_core::get_headings<InterpolationTraits, decltype(ip_first), LatIdx,
                                          LonIdx, HeadingIdx>(ip_first, ip_beyond);
            }

            if constexpr (SpeedIdx > -1)
            {
                ip_it = ip_first;
                auto vit = std::begin(velocities);
                while (++ip_it != (ip_beyond - 1))
                {
                    typename InterpolationTraits::NT heading_v = std::get<HeadingIdx>(*ip_it);
                    //std::cout<<std::get<0>(probe_u)<<",";
                    //std::cout<<*vit<<",";
                    //std::cout<<cos( movetk_core::deg2radians(heading_v) )<<",";
                    //typename InterpolationTraits::NT speed_v = abs( movetk_core::get_x<typename InterpolationTraits::GeometryTraits>(*vit) / cos( movetk_core::deg2radians(heading_v) ));
                    //std::cout<<speed_v<<",";
                    typename InterpolationTraits::NT squared_length = norm(*vit);
                    typename InterpolationTraits::NT speed_v = norm ^ 1;
                    //std::cout<<length<<",";
                    //std::cout<<abs(length - speed_v)<<"\n";
                    //std::get<SpeedIdx>(*ip_it) = speed_v;
                    std::get<SpeedIdx>(*ip_it) = speed_v;
                    vit++;
                }
            }
            std::move(ip_first, ip_beyond, result);
        }
    };

    template <class InterpolationTraits,
              int LatIdx, int LonIdx, int TsIdx,
              int SpeedIdx, int HeadingIdx>
    class Interpolator<movetk_algorithms::random_trajectory_generator_tag,
                       InterpolationTraits, LatIdx, LonIdx, TsIdx,
                       SpeedIdx, HeadingIdx>
    {
        // based on https://doi.org/10.1080/13658816.2014.999682
    private:
        typename InterpolationTraits::NT eps;
        typename InterpolationTraits::Norm norm;
        typename InterpolationTraits::GeoProjection ref;
        typename InterpolationTraits::MinimumBoundingRectangle mbr;
        movetk_core::Scaling<typename InterpolationTraits::GeometryTraits> scale;
        movetk_core::Translation<typename InterpolationTraits::GeometryTraits> translate;
        movetk_core::MakePoint<typename InterpolationTraits::GeometryTraits> make_point;
        typename InterpolationTraits::MovetkPoint ORIGIN = make_point({0, 0});
        std::default_random_engine rng;

        template <class Iterator>
        Iterator find_source(Iterator first, Iterator current)
        {
            Iterator it = current;
            while (it != first)
            {
                typename InterpolationTraits::MovetkVector v = *it - ORIGIN;
                if ((v * v) > MOVETK_EPS)
                    return it;
                it--;
            }
            return first;
        }

        template <class Iterator>
        Iterator find_destination(Iterator beyond, Iterator current)
        {
            Iterator it = current;
            while (it != beyond)
            {
                typename InterpolationTraits::MovetkVector v = *it - ORIGIN;
                if ((v * v) > MOVETK_EPS)
                    return it;
                it++;
            }
            return (beyond - 1);
        }

    public:
        Interpolator(typename InterpolationTraits::NT reflat,
                     typename InterpolationTraits::NT reflon,
                     typename InterpolationTraits::NT epsilon)
        {
            ref = typename InterpolationTraits::GeoProjection(reflat, reflon);
            std::random_device random;
            rng = std::default_random_engine(random());
            eps = epsilon;
        }

        Interpolator(typename InterpolationTraits::NT reflat,
                     typename InterpolationTraits::NT reflon)
        {
            ref = typename InterpolationTraits::GeoProjection(reflat, reflon);
            std::random_device random;
            rng = std::default_random_engine(random());
            eps = 0.9999;
        }

        template <class TSIterator, class OutputIterator>
        void operator()(const typename InterpolationTraits::ProbePoint &probe_u,
                        const typename InterpolationTraits::ProbePoint &probe_v,
                        TSIterator first, TSIterator beyond,
                        typename InterpolationTraits::NT max_speed, OutputIterator result)
        {
            auto lat_u = std::get<LatIdx>(probe_u);
            auto lon_u = std::get<LonIdx>(probe_u);
            typename InterpolationTraits::MovetkPoint p_u = movetk_core::get_point<InterpolationTraits>(lat_u, lon_u,
                                                                                                        ref);

            auto lat_v = std::get<LatIdx>(probe_v);
            auto lon_v = std::get<LonIdx>(probe_v);
            typename InterpolationTraits::MovetkPoint p_v = movetk_core::get_point<InterpolationTraits>(lat_v, lon_v,
                                                                                                        ref);

            auto t_u = std::get<TsIdx>(probe_u);
            auto t_v = std::get<TsIdx>(probe_v);

            std::size_t num_elements = std::distance(first, beyond);

            std::vector<typename InterpolationTraits::ProbePoint> interpolated_pts(num_elements);

            if (num_elements == 2)
            {
                result = probe_u;
                result = probe_v;
                return;
            }

            std::vector<int> sequence(num_elements - 2);
            std::iota(std::begin(sequence), std::end(sequence), 1);

            std::shuffle(std::begin(sequence), std::end(sequence), rng);

            std::vector<typename InterpolationTraits::MovetkPoint> output(num_elements);
            std::fill(std::begin(output), std::end(output), ORIGIN);

            output[0] = p_u;
            output[num_elements - 1] = p_v;
            interpolated_pts[0] = probe_u;
            interpolated_pts[num_elements - 1] = probe_v;

            bool intersects = false;
            for (auto &&i : sequence)
            {
                auto tit = first + i;
                auto pit = std::begin(output) + i;
                auto source = find_source(std::begin(output), pit);
                auto destination = find_destination(std::end(output), pit);
                std::size_t position_source = std::distance(std::begin(output), source);
                std::size_t position_destination = std::distance(std::begin(output), destination);
                typename InterpolationTraits::NT delta_forward = *tit - *(first + position_source);
                typename InterpolationTraits::NT delta_backward = *(first + position_destination) - *tit;
                typename InterpolationTraits::NT radius_u = delta_forward * max_speed;
                typename InterpolationTraits::NT radius_v = delta_backward * max_speed;
                typename InterpolationTraits::NT squared_radius_u = radius_u * radius_u;
                typename InterpolationTraits::NT squared_radius_v = radius_v * radius_v;
                /*std::cerr<<"source: "<<*source<<"\n";
                std::cerr<<"destination: "<<*destination<<"\n";
                std::cerr<<"radius_u: "<<radius_u<<"\n";
                std::cerr<<"radius_v: "<<radius_v<<"\n";*/
                typename InterpolationTraits::MovetkVector v = *source - *destination;
                typename InterpolationTraits::NT squared_length = norm(v);
                if (squared_length < MOVETK_EPS)
                    continue;

                auto bounds = mbr(*source, *destination, radius_u, radius_v);
                typename InterpolationTraits::NT x_min = std::min(
                    movetk_core::get_x<typename InterpolationTraits::GeometryTraits>(bounds.first),
                    movetk_core::get_x<typename InterpolationTraits::GeometryTraits>(bounds.second));
                typename InterpolationTraits::NT x_max = std::max(
                    movetk_core::get_x<typename InterpolationTraits::GeometryTraits>(bounds.first),
                    movetk_core::get_x<typename InterpolationTraits::GeometryTraits>(bounds.second));
                typename InterpolationTraits::NT y_min = std::min(
                    movetk_core::get_y<typename InterpolationTraits::GeometryTraits>(bounds.first),
                    movetk_core::get_y<typename InterpolationTraits::GeometryTraits>(bounds.second));
                typename InterpolationTraits::NT y_max = std::max(
                    movetk_core::get_y<typename InterpolationTraits::GeometryTraits>(bounds.first),
                    movetk_core::get_y<typename InterpolationTraits::GeometryTraits>(bounds.second));
                /*    std::cerr<<"x_min: "<<x_min<<"\n";
                    std::cerr<<"x_max: "<<x_max<<"\n";
                    std::cerr<<"y_min: "<<y_min<<"\n";
                    std::cerr<<"y_max: "<<y_max<<"\n";*/

                if ((x_min == 0) && (x_max == 0))
                {
                    if ((y_min == 0) && (y_max == 0))
                    {
                        continue;
                    }
                }

                intersects = true;
                std::uniform_real_distribution<typename InterpolationTraits::NT> distributionx(x_min, x_max);
                typename InterpolationTraits::NT x = distributionx(rng);
                std::uniform_real_distribution<typename InterpolationTraits::NT> distributiony(y_min, y_max);
                typename InterpolationTraits::NT y = distributiony(rng);
                while (true)
                {
                    typename InterpolationTraits::MovetkPoint p = make_point({x, y});
                    typename InterpolationTraits::MovetkVector pu = p - *source;
                    typename InterpolationTraits::MovetkVector pv = p - *destination;
                    typename InterpolationTraits::NT squared_length_pu = norm(pu);
                    typename InterpolationTraits::NT squared_length_pv = norm(pv);

                    if ((squared_length_pu <= squared_radius_u + eps) &&
                        (squared_length_pv <= squared_radius_v + eps))
                    {
                        /*std::cerr<<"x: "<<x<<"\n";
                        std::cerr<<"y: "<<y<<"\n";*/
                        if ((abs(x) > 0) && (abs(y) > 0))
                            break;
                    }
                    x = distributionx(rng);
                    y = distributiony(rng);
                }

                output[i] = make_point({x, y});
                typename InterpolationTraits::ProbePoint probe;
                auto geocoordinate = ref.inverse(y, x);
                std::get<LatIdx>(probe) = geocoordinate[0];
                std::get<LonIdx>(probe) = geocoordinate[1];
                std::get<TsIdx>(probe) = *tit;
                interpolated_pts[i] = probe;
            }

            auto ip_first = std::begin(interpolated_pts);
            auto ip_beyond = std::remove_if(std::begin(interpolated_pts), std::end(interpolated_pts),
                                            [](auto &item) {
                                                if ((std::get<LatIdx>(item) == 0.00) &&
                                                    (std::get<LonIdx>(item) == 0.00))
                                                    return true;
                                                else
                                                    return false;
                                            });

            num_elements = std::distance(ip_first, ip_beyond);

            if (num_elements == 0)
                return;

            if constexpr (SpeedIdx > -1)
            {
                std::get<SpeedIdx>(*ip_first) = std::get<SpeedIdx>(probe_u);
                std::get<SpeedIdx>(*(ip_beyond - 1)) = std::get<SpeedIdx>(probe_v);
                movetk_core::get_speeds<InterpolationTraits, decltype(ip_first), LatIdx,
                                        LonIdx, TsIdx, SpeedIdx>(ip_first, ip_beyond);
            }

            if constexpr (HeadingIdx > -1)
            {
                std::get<HeadingIdx>(*ip_first) = std::get<HeadingIdx>(probe_u);
                std::get<HeadingIdx>(*(ip_beyond - 1)) = std::get<HeadingIdx>(probe_v);
                movetk_core::get_headings<InterpolationTraits, decltype(ip_first), LatIdx,
                                          LonIdx, HeadingIdx>(ip_first, ip_beyond);
            }

            std::move(ip_first, ip_beyond, result);
        }
    };

} // namespace movetk_algorithms

#endif //MOVETK_INTERPOLATION_H
