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
// Created by Mees van de Kerkhof (m.a.vandekerkhof@uu.nl),
// Bram Custers (b.a.custers@tue.nl),
// Kevin Verbeek (k.a.b.verbeek@tue.nl)
// Modified by Aniket Mitra (aniket.mitra@here.com)
//

#ifndef MOVETK_OUTLIERDETECTIONPREDICATES_H
#define MOVETK_OUTLIERDETECTIONPREDICATES_H

#include "movetk/geom/GeometryInterface.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "movetk/CartesianProbeTraits.h"
#include <iterator>
#include <algorithm>

namespace movetk_algorithms {

    namespace outlier_detection{
        //using speed_test_tag = movetk_algorithms::outlier_detection_traits::linear_speed_bounded_test_tag;

        template<class TestTag,
                class CoordinateTag,
                class T>
        class TEST {

        };


        template <class T>
        class TEST<movetk_algorithms::linear_speed_bounded_test_tag,
                movetk_algorithms::geo_coordinates_tag,
                T> {
        private:
            typedef typename T::NT NT;
            typedef typename T::ProbeTraits ProbeTraits;
            NT threshold;

        public:
            TEST() = default;

            /*!
            *@param InThreshold
            */
            TEST(NT InThreshold): threshold(InThreshold) {};


            template<class T1,
                    typename = movetk_core::requires_tuple<T1>,
                    typename = movetk_core::requires_tuple_element_as_arithmetic<
                            ProbeTraits::ProbeColumns::LAT, T1>,
                    typename = movetk_core::requires_tuple_element_as_arithmetic<
                            ProbeTraits::ProbeColumns::LON, T1>,
                    typename = movetk_core::requires_tuple_element_as_size_t<
                            ProbeTraits::ProbeColumns::SAMPLE_DATE, T1>>
            bool operator()(T1 &p1, T1 &p2) {
                auto lat0 = std::get<ProbeTraits::ProbeColumns::LAT>(p1);
                auto lat1 = std::get<ProbeTraits::ProbeColumns::LAT>(p2);
                auto lon0 = std::get<ProbeTraits::ProbeColumns::LON>(p1);
                auto lon1 = std::get<ProbeTraits::ProbeColumns::LON>(p2);
                auto t1 = std::get<ProbeTraits::ProbeColumns::SAMPLE_DATE>(p1);
                auto t2 = std::get<ProbeTraits::ProbeColumns::SAMPLE_DATE>(p2);
                NT tdiff = abs(static_cast<NT>(t1) - static_cast<NT>(t2));
                assert(tdiff > 0);
                auto len = distance_exact(lat0, lon0, lat1, lon1);
                return len / tdiff <= threshold;
            } //operator()



        };//class DistanceOverTimeCheck


        template <class T>
        class TEST<movetk_algorithms::linear_speed_bounded_test_tag,
                movetk_algorithms::cartesian_coordinates_tag,
                T> {

        private:
            typedef typename T::NT NT;
            typedef typename T::Vector Vector;
            typedef typename T::Norm Norm;
            typedef typename T::GeometryTraits GeometryTraits;
            typedef typename T::ProbeTraits ProbeTraits;
            NT threshold;
            NT squared_threshold;
        public:
            TEST() = default;

            /*!
            *@param InThreshold
            */
            TEST(NT InThreshold): threshold(InThreshold) {
                squared_threshold = threshold * threshold;
            };


            /*!
             *
             * @tparam ProbePoint
             * @param p1
             * @param p2
             * @return
             */

            template<class T1,
                    typename = movetk_core::requires_tuple<typename std::add_const<T1>::type>,
                    typename = movetk_core::requires_tuple_element_as_movetk_point<
                            GeometryTraits,
                            ProbeTraits::ProbeColumns::PROJECTED_COORDS,
                            typename std::add_const<T1>::type>,
                    typename = movetk_core::requires_tuple_element_as_size_t<
                            ProbeTraits::ProbeColumns::SAMPLE_DATE,
                            typename std::add_const<T1>::type> >
            bool operator()(const T1 &p1, const T1 &p2){
                Norm norm;
                auto point1 = std::get<ProbeTraits::ProbeColumns::PROJECTED_COORDS>(p1);
                auto point2 = std::get<ProbeTraits::ProbeColumns::PROJECTED_COORDS>(p2);
                Vector v = point1 - point2;
                auto t1 = std::get<ProbeTraits::ProbeColumns::SAMPLE_DATE>(p1);
                auto t2 = std::get<ProbeTraits::ProbeColumns::SAMPLE_DATE>(p2);
                NT tdiff = abs(static_cast<NT>(t1) - static_cast<NT>(t2));
                assert(tdiff > 0);
                NT squared_distance = norm(v);
                NT squared_speed = squared_distance / (tdiff * tdiff);
                return squared_speed <= squared_threshold;
            } //operator()

        };
    }

}

#endif //MOVETK_OUTLIERDETECTIONPREDICATES_H
