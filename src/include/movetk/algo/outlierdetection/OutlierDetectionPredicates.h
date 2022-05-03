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

#include <algorithm>
#include <iterator>

#include "movetk/geom/GeometryInterface.h"
#include "movetk/io/CartesianProbeTraits.h"
#include "movetk/utils/TrajectoryUtils.h"

namespace movetk::outlierdetection {
/*!
 * @struct linear_speed_bounded_test_tag
 * @brief This tag is used to select the type of outlier detection predicate
 */
struct linear_speed_bounded_test_tag;

template <class TestTag, class CoordinateTag, class Traits>
class TEST {};


template <class T>
class TEST<linear_speed_bounded_test_tag, movetk::algo::geo_coordinates_tag, T> {
public:
	using NT = typename T::NT;
	using ProbeTraits = typename T::ProbeTraits;
	using Cols = typename ProbeTraits::ProbeColumns;
	TEST() = default;

	/*!
	 *@param InThreshold
	 */
	explicit TEST(NT threshold) : m_threshold(threshold){};


	template <class T1,
	          typename = movetk::utils::requires_tuple<T1>,
	          typename = movetk::utils::requires_tuple_element_as_arithmetic<ProbeTraits::ProbeColumns::LAT, T1>,
	          typename = movetk::utils::requires_tuple_element_as_arithmetic<ProbeTraits::ProbeColumns::LON, T1>,
	          typename = movetk::utils::requires_tuple_element_as_size_t<ProbeTraits::ProbeColumns::SAMPLE_DATE, T1>>
	bool operator()(const T1 &p1, const T1 &p2) const {
		auto [lat0, lon0, t1] = get_lat_lon_time(p1);
		auto [lat1, lon1, t2] = get_lat_lon_time(p2);
		const auto tdiff = static_cast<NT>(t2) - static_cast<NT>(t1);
		assert(tdiff > 0);
		auto len = movetk::geo::distance_exact(lat0, lon0, lat1, lon1);
		return len / tdiff <= m_threshold;
	}  // operator()

private:
	template <typename TUPLE>
	static auto get_lat_lon_time(TUPLE &&tuple) {
		return std::make_tuple(get<Cols::LAT>(tuple), get<Cols::LON>(tuple), get<Cols::SAMPLE_DATE>(tuple));
	}
	NT m_threshold;

};  // class DistanceOverTimeCheck


template <class T>
class TEST<linear_speed_bounded_test_tag, movetk::algo::cartesian_coordinates_tag, T> {
public:
	using NT = typename T::NT;
	using Vector = typename T::Vector;
	using Norm = typename T::Norm;
	using GeometryTraits = typename T::GeometryTraits;
	using ProbeTraits = typename T::ProbeTraits;
	TEST() = default;
	TEST(const TEST &) = default;

	/*!
	 *@param InThreshold
	 */
	explicit TEST(NT threshold) : m_threshold(threshold), m_squared_threshold(threshold * threshold) {}


	/*!
	 *
	 * @tparam ProbePoint
	 * @param p1
	 * @param p2
	 * @return
	 */

	template <
	    class T1,
	    typename = movetk::utils::requires_tuple<typename std::add_const<T1>::type>,
	    typename = movetk::utils::requires_tuple_element_as_movetk_point<GeometryTraits,
	                                                                     ProbeTraits::ProbeColumns::PROJECTED_COORDS,
	                                                                     typename std::add_const<T1>::type>,
	    typename = movetk::utils::requires_tuple_element_as_size_t<ProbeTraits::ProbeColumns::SAMPLE_DATE,
	                                                               typename std::add_const<T1>::type>>
	bool operator()(const T1 &p1, const T1 &p2) {
		Norm norm;
		auto point1 = std::get<ProbeTraits::ProbeColumns::PROJECTED_COORDS>(p1);
		auto point2 = std::get<ProbeTraits::ProbeColumns::PROJECTED_COORDS>(p2);
		Vector v = point1 - point2;
		auto t1 = std::get<ProbeTraits::ProbeColumns::SAMPLE_DATE>(p1);
		auto t2 = std::get<ProbeTraits::ProbeColumns::SAMPLE_DATE>(p2);
		const auto tdiff = static_cast<NT>(t2) - static_cast<NT>(t1);
		assert(tdiff > 0);
		const auto squared_distance = norm(v);
		const auto squared_speed = squared_distance / (tdiff * tdiff);
		return squared_speed <= m_squared_threshold;
	}  // operator()
private:
	NT m_threshold;
	NT m_squared_threshold;
};

}  // namespace movetk::outlierdetection

#endif  // MOVETK_OUTLIERDETECTIONPREDICATES_H
