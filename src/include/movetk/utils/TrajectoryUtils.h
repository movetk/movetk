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

#include <algorithm>
#include <array>
#include <boost/iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <iterator>
#include <numeric>

#include "movetk/geo/geo.h"
#include "movetk/geom/GeometryInterface.h"
#include "movetk/utils/Requirements.h"


/*! @file TrajectoryUtils.h
 *  @brief  A collection of utility functions for trajectories
 *  @authors Aniket Mitra (aniket.mitra@here.com)
 */
namespace movetk::utils {

/**
 * @brief Constructs a projected MovetkPoint from the latitude and longitude
 * @tparam Traits The geometry kernel to use
 * @param lat The latitude
 * @param lon The longitude
 * @param georef Projection object
 * @return The constructed point after projection
*/
template <class Traits>
typename Traits::MovetkPoint get_point(typename Traits::NT lat,
                                       typename Traits::NT lon,
                                       typename Traits::GeoProjection& georef) {
	geom::MakePoint<typename Traits::GeometryTraits> make_point;
	auto projected_point = georef.project(lat, lon);
	std::reverse(std::begin(projected_point), std::end(projected_point));
	return make_point(std::cbegin(projected_point), std::cend(projected_point));
}

/**
 * @brief Retrieves the first coordinate of the vector
 * @tparam GeometryTraits Kernel to use
 * @param v The vector
 * @return The first coordinate
*/
template <class GeometryTraits>
typename GeometryTraits::NT get_x(const typename GeometryTraits::MovetkVector& v) {
	return v * v.basis(0);
}

/**
 * @brief Retrieves the first coordinate of the point
 * @tparam GeometryTraits Kernel to use
 * @param p The point
 * @return The first coordinate
 */
template <class GeometryTraits>
typename GeometryTraits::NT get_x(const typename GeometryTraits::MovetkPoint& p) {
	geom::MakePoint<GeometryTraits> make_point;
	typename GeometryTraits::MovetkPoint ORIGIN = make_point({0, 0});
	typename GeometryTraits::MovetkVector v = p - ORIGIN;
	return v * v.basis(0);
}

/**
 * @brief Retrieves the second coordinate of the vector
 * @tparam GeometryTraits Kernel to use
 * @param v The vector
 * @return The second coordinate
 */
template <class GeometryTraits>
typename GeometryTraits::NT get_y(const typename GeometryTraits::MovetkVector& v) {
	return v * v.basis(1);
}

/**
 * @brief Retrieves the second coordinate of the point
 * @tparam GeometryTraits Kernel to use
 * @param p The point
 * @return The second coordinate
 */
template <class GeometryTraits>
typename GeometryTraits::NT get_y(const typename GeometryTraits::MovetkPoint& p) {
	geom::MakePoint<GeometryTraits> make_point;
	typename GeometryTraits::MovetkPoint ORIGIN = make_point({0, 0});
	typename GeometryTraits::MovetkVector v = p - ORIGIN;
	return v * v.basis(1);
}


/*!
 * @brief checks if input values are a sequence
 * TODO: this check is necessary, but not sufficient for the input to be a sequence.
 * @tparam InputIterator A random access iterator over a set of values of type size_t
 * @param first
 * @param beyond
 * @return True / False
 */
template <utils::RandomAccessIterator<size_t> InputIterator>
bool is_sequence(InputIterator first, InputIterator beyond) {
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
template <class GeometryTraits, utils::RandomAccessIterator<typename GeometryTraits::NT> InputIterator>
InputIterator min_non_zero_element(InputIterator first, InputIterator beyond) {
	if (first == beyond)
		return beyond;
	auto it = first;
	auto smallest = first;
	++it;
	while (it != beyond) {
		if (*smallest == 0) {
			smallest = it;
		} else if ((*it < *smallest) && (*it != 0)) {
			smallest = it;
		}
		it++;
	}
	return smallest;
}


/*!
 * @brief Calculates the tdiff between two consecutive points for an input
 * consisting of a set of ordered points
 * @tparam GeometryTraits A traits class that defines the expected number type
 * of the input
 * @tparam InputIterator  A random access iterator over a set of input points
 * @tparam OutputIterator An insert iterator
 * @param first
 * @param beyond
 * @param iter
 */
template <std::random_access_iterator InputIterator,
          utils::OutputIterator<decltype(std::declval<std::iter_value_t<InputIterator>>() -
                                         std::declval<std::iter_value_t<InputIterator>>())> OutputIterator>
void get_time_diffs(InputIterator first, InputIterator beyond, OutputIterator iter, bool without_leading_zero = false) {
	InputIterator pit = first;
	InputIterator cit = first + 1;
	if (!without_leading_zero) {
		*iter = 0;
	}
	while (cit != beyond) {
		*iter = *cit - *pit;
		pit = cit;
		cit++;
	}
}

/*!
 * Computes the distances between consecutive points in a point range.
 * @tparam GeometryKernel The geometry kernel to use.
 * @tparam InputIterator Iterator type for the input range of points
 * @tparam OutputIterator Output iterator type that can accept the distance
 * @tparam PointDistanceFunc Type of the distance function to use. Default to ComputeLength<GeometryKerneL>
 * @param first
 * @param beyond
 * @param iter
 * @param without_leading_zero Do not add the zero for the first element, resulting in a
 * range of size one less than the given input range
 */
template <class GeometryKernel,
          utils::InputIterator<typename GeometryKernel::MovetkPoint> InputIterator,
          utils::OutputIterator<typename GeometryKernel::NT> OutputIterator,
          typename PointDistanceFunc = geom::ComputeLength<GeometryKernel>>
void get_distances(InputIterator first, InputIterator beyond, OutputIterator iter, bool without_leading_zero = false) {
	PointDistanceFunc distance;
	InputIterator pit = first;
	InputIterator cit = std::next(first);
	if (!without_leading_zero) {
		*iter = 0;
	}
	while (cit != beyond) {
		*iter = distance(*pit, *cit);
		pit = cit;
		cit++;
	}
}

/**
 * @brief computes the distance between consecutive probes and stores it in the probe field, defined by
 * DistIdx
 * @tparam Traits The kernel to use
 * @tparam InputIterator The type of the probe iterator
 * @tparam LatIdx Index of the latitude coordinate in the probe
 * @tparam LonIdx Index of the longitude coordinate in the probe
 * @tparam DistIdx Index to store the distance in in the probe
 * @param first Start of the probe range
 * @param beyond End of the probe range
*/
template <class Traits, class InputIterator, std::size_t LatIdx, std::size_t LonIdx, std::size_t DistIdx>
void get_distances(InputIterator first, InputIterator beyond) {
	InputIterator pit = first;
	InputIterator cit = std::next(first);
	std::get<DistIdx>(*pit) = 0;
	while (cit != beyond) {
		typename Traits::NT lat0 = std::get<LatIdx>(*pit);
		typename Traits::NT lon0 = std::get<LonIdx>(*pit);
		typename Traits::NT lat1 = std::get<LatIdx>(*cit);
		typename Traits::NT lon1 = std::get<LonIdx>(*cit);
		std::get<DistIdx>(*cit) = movetk::geo::distance_exact(lat0, lon0, lat1, lon1);
		pit = cit;
		cit++;
	}
}

/*!
 *
 * @tparam TDiffIterator
 * @tparam DistanceIterator
 * @tparam OutputIterator
 * @param TdiffFirst
 * @param TdiffBeyond
 * @param DistFirst
 * @param iter
 */
template <
    class GeometryKernel,
    std::input_iterator TDiffIterator,
    std::input_iterator DistanceIterator,
    utils::OutputIterator<decltype(*std::declval<DistanceIterator>() / *std::declval<TDiffIterator>())> OutputIterator>
void get_speeds(TDiffIterator TdiffFirst, TDiffIterator TdiffBeyond, DistanceIterator DistFirst, OutputIterator iter) {
	auto dit = DistFirst;
	for (auto tit = TdiffFirst; tit != TdiffBeyond; ++tit, ++dit) {
		if (*tit == 0) {
			*iter = 0;
		} else {
			*iter = *dit / *tit;
		}
	}
}

template <class Traits,
          class InputIterator,
          std::size_t LatIdx,
          std::size_t LonIdx,
          std::size_t TsIdx,
          std::size_t SpeedIdx>
void get_speeds(InputIterator first, InputIterator beyond) {
	typename Traits::Norm norm;
	typename Traits::GeoProjection ref(std::get<LatIdx>(*first), std::get<LonIdx>(*first));
	typename Traits::MovetkPoint p = get_point<Traits>(std::get<LatIdx>(*first), std::get<LonIdx>(*first), ref);
	typename Traits::MovetkPoint prev = p;
	auto it = first;
	while (++it != beyond - 1) {
		p = get_point<Traits>(std::get<LatIdx>(*it), std::get<LonIdx>(*it), ref);
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
 *
 * @tparam InputIterator
 * @tparam OutputIterator
 * @param first
 * @param beyond
 * @param iter
 */
template <
    class GeometryKernel,
    utils::RandomAccessIterator<std::pair<typename GeometryKernel::NT, typename GeometryKernel::NT>> InputIterator,
    utils::RandomAccessIterator<typename GeometryKernel::MovetkPoint> PointsIterator,
    utils::OutputIterator<typename GeometryKernel::NT> OutputIterator>
void get_headings(InputIterator first, InputIterator beyond, PointsIterator pfirst, OutputIterator iter) {
	// formula from: https://www.movable-type.co.uk/scripts/latlong.html
	// TODO use default implementation of Geographiclib
	using NT = typename GeometryKernel::NT;
	movetk::geom::ComputeLength<GeometryKernel> distance;
	InputIterator pit = first;
	InputIterator cit = first + 1;
	PointsIterator p_pit = pfirst;
	PointsIterator c_pit = pfirst + 1;
	*iter = 0;
	iter++;
	NT phi_1, lambda_1, phi_2, lambda_2;
	NT x, y, c_heading, p_heading = 0;
	while (cit != beyond) {
		phi_1 = geom::deg2radians(pit->first);
		lambda_1 = geom::deg2radians(pit->second);
		phi_2 = geom::deg2radians(cit->first);
		lambda_2 = geom::deg2radians(cit->second);
		y = std::sin(lambda_2 - lambda_1) * std::cos(phi_2);
		x = std::cos(phi_1) * std::sin(phi_2) - std::sin(phi_1) * std::cos(phi_2) * std::cos(lambda_2 - lambda_1);
		c_heading = std::fmod(geom::rad2deg(atan2(y, x)) + 360.0, 360.0);
		if ((c_heading < MOVETK_EPS) && (distance(*p_pit, *c_pit) < MOVETK_EPS)) {
			*iter = p_heading;
		} else {
			*iter = c_heading;
		}
		pit = cit;
		p_pit = c_pit;
		p_heading = c_heading;
		cit++;
		c_pit++;
	}
}


template <class Traits, class InputIterator, std::size_t LatIdx, std::size_t LonIdx, std::size_t HeadingIdx>
void get_headings(InputIterator first, InputIterator beyond) {
	auto it = first;
	while (++it != (beyond - 1)) {
		typename Traits::NT azimuth = movetk::geo::bearing_exact(std::get<LatIdx>(*first),
		                                                         std::get<LonIdx>(*first),
		                                                         std::get<LatIdx>(*it),
		                                                         std::get<LonIdx>(*it));
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
template <class GeometryKernel,
          utils::RandomAccessIterator<typename GeometryKernel::NT> MagnitudeIterator,
          utils::RandomAccessIterator<typename GeometryKernel::NT> DirectionIterator,
          utils::OutputIterator<typename GeometryKernel::MovetkPoint> OutputIterator>
void get_velocities(MagnitudeIterator Mfirst,
                    MagnitudeIterator Mbeyond,
                    DirectionIterator DFirst,
                    OutputIterator iter) {
	MagnitudeIterator mit = Mfirst;
	DirectionIterator dit = DFirst;
	std::array<typename GeometryKernel::NT, 2> PointContainer;
	movetk::geom::MakePoint<GeometryKernel> make_point;
	while (mit != Mbeyond) {
		PointContainer[0] = (*mit) * std::cos(movetk::geom::deg2radians(*dit));
		PointContainer[1] = (*mit) * std::sin(movetk::geom::deg2radians(*dit));
		typename GeometryKernel::MovetkPoint P = make_point(std::cbegin(PointContainer), std::cend(PointContainer));
		*iter = P;
		mit++;
		dit++;
	}
}

template <class GeometryTraits>
typename GeometryTraits::MovetkVector get_velocity(typename GeometryTraits::NT speed,
                                                   typename GeometryTraits::NT heading) {
	movetk::geom::MakePoint<GeometryTraits> make_point;
	typename GeometryTraits::MovetkPoint ORIGIN = make_point({0, 0});
	std::array<typename GeometryTraits::NT, 2> point_container;
	point_container[0] = speed * std::cos(movetk::geom::deg2radians(heading));
	point_container[1] = speed * std::sin(movetk::geom::deg2radians(heading));
	return make_point(std::cbegin(point_container), std::cend(point_container)) - ORIGIN;
}

namespace detail {
// TODO: move to more appropriate location
template <template <typename...> typename TupleType, typename T, std::size_t I>
struct RepeatedTypeTuple {
	using type = decltype(std::tuple_cat(std::declval<TupleType<T>>(),
	                                     std::declval<typename RepeatedTypeTuple<TupleType, T, I - 1>::type>()));
};
template <template <typename...> typename TupleType, typename T>
struct RepeatedTypeTuple<TupleType, T, 1> {
	using type = TupleType<T>;
};

template <typename SrcType, template <typename...> typename TupleType>
struct TransferTypes {};
template <template <typename...> typename TupleType, typename... Ts>
struct TransferTypes<std::tuple<Ts...>, TupleType> {
	using type = TupleType<Ts...>;
};

template <typename T>
struct TypeHolder {
	using type = T;
};

template <class GeometryKernel, typename CoordinateIterator, std::size_t... Is>
auto point_iterators_from_coordinates(
    TypeHolder<GeometryKernel>,
    const std::array<std::pair<CoordinateIterator, CoordinateIterator>, GeometryKernel::dim>& beginEndPairs,
    std::index_sequence<Is...>) {
	using NT = typename GeometryKernel::NT;

	using tuple_t_std = typename detail::RepeatedTypeTuple<std::tuple, NT, GeometryKernel::dim>::type;
	using tuple_t = typename detail::TransferTypes<tuple_t_std, boost::tuple>::type;

	std::function<typename GeometryKernel::MovetkPoint(const tuple_t&)> toPoint = [](const tuple_t& coordinateTuple) {
		movetk::geom::MakePoint<GeometryKernel> makePoint;
		return makePoint({coordinateTuple.template get<Is>()...});
	};
	auto begin =
	    boost::make_transform_iterator(boost::make_zip_iterator(boost::make_tuple(std::get<Is>(beginEndPairs).first...)),
	                                   toPoint);
	auto end =
	    boost::make_transform_iterator(boost::make_zip_iterator(boost::make_tuple(std::get<Is>(beginEndPairs).second...)),
	                                   toPoint);
	return std::make_pair(begin, end);
}
}  // namespace detail

/**
 * \brief Creates a start and end iterator that dereference to a point, constructed from the input coordinate iterators
 * per dimension, as specified by the kernel \tparam GeometryKernel The geometry kernel. Specifies the dimensions of the
 * point via GeometryKernel::dim \tparam CoordinateIterator The iterator type for the coordinates per dimension \param
 * beginEndPairs Array of size GeometryKernel::dim containing the start and end iterator per coordinate as a pair
 * \return A pair of start and end iterator that dereference to the correct point.
 */
template <class GeometryKernel, typename CoordinateIterator>
auto point_iterators_from_coordinates(
    const std::array<std::pair<CoordinateIterator, CoordinateIterator>, GeometryKernel::dim>& beginEndPairs) {
	return detail::point_iterators_from_coordinates(detail::TypeHolder<GeometryKernel>{},
	                                                beginEndPairs,
	                                                std::make_index_sequence<GeometryKernel::dim>{});
}


/*!
 *
 * @tparam PolyLineIdxIterator
 */
template <std::random_access_iterator PolyLineIdxIterator>
class SegmentIdGenerator {
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
	SegmentIdGenerator(PolyLineIdxIterator first, PolyLineIdxIterator beyond)
	    : it(first)
	    , __first(first)
	    , __beyond(beyond){};


	/*!
	 *
	 * @tparam PolyLineIteratorType
	 * @param iter
	 * @return
	 */
	template <class PolyLineIteratorType>
	/*typename = movetk::utils::requires_equality<typename PolyLineIteratorType::value_type,
	typename PolyLineIdxIterator::value_type::value_type>>*/
	size_t getSegmentID(PolyLineIteratorType iter) {
		if (it == __beyond) {
			return SegmentId;
		}
		if (iter == *it) {
			SegmentId++;
			if (it != __beyond)
				it++;
			return SegmentId;
		} else {
			return SegmentId;
		}
	}

	void reset() {
		it = __first;
		SegmentId = 0;
	}

	SegmentIdGenerator() = default;
};

template <typename PolyLineIdxIterator>
auto make_segment_id_generator(PolyLineIdxIterator begin, PolyLineIdxIterator end) {
	return SegmentIdGenerator<PolyLineIdxIterator>(begin, end);
}

/// \brief Merges intervals
///
/// \typeparam GeometryKernel The geometry kernel to use
/// \typeparam InputIterator
/// \param first Start of the range of intervals
/// \param beyond End of the range of intervals
/// \param sorted Is the range sorted (descending...?)
/// \returns
// by Mees van de Kerkhof
template <class GeometryKernel,
          utils::RandomAccessIterator<utils::single_type_pair<typename GeometryKernel::NT>> InputIterator>
InputIterator merge_intervals(InputIterator first, InputIterator beyond, bool sorted = false) {
	if (!sorted) {
		// Sort in descending order, lexicographically.
		std::sort(first, beyond, [](auto a, auto b) {
			if (a.first != b.first)
				return a.first > b.first;
			else
				return a.second > b.second;
		});
	}
	auto pairs_overlap = [](const auto& pair0, const auto& pair1) {
		return !(pair0.first > pair1.second || pair1.first > pair0.second);
	};
	if (first == beyond || first + 1 == beyond) {
		return first;
	}

	InputIterator curr = first;
	InputIterator next_placement_it = first;
	InputIterator next = curr + 1;
	for (; next != beyond; ++next) {
		if (pairs_overlap(*next, *curr)) {
			*curr = std::make_pair(std::min(next->first, curr->first), std::max(curr->second, next->second));
		} else {
			if (curr + 1 != next) {
				*(curr + 1) = *next;
			}
			++curr;
		}
	}
	return (curr + 1);
}

/**
 * @brief TODO: check this
 * @tparam GeometryTraits The traits to use
 * @tparam Norm The norm to use
 * @tparam InputIterator Input iterator type
 * @tparam OutputIterator Output iterator type
 * @param first Start of the curve
 * @param beyond End of the curve
 * @param result Output iterator to write the result to
*/
template <class GeometryTraits, class Norm, class InputIterator, class OutputIterator>
void compute_curve_squared_length(InputIterator first, InputIterator beyond, OutputIterator result) {
	Norm norm;
	auto it = first;
	while (it != (beyond - 1)) {
		auto jit = it + 1;
		typename GeometryTraits::NT sum = 0;
		while (jit != beyond) {
			typename GeometryTraits::MovetkVector v = *jit - *(jit - 1);
			typename GeometryTraits::NT squared_length = norm(v);
			sum += squared_length;
			jit++;
		}
		result = sum;
		it++;
	}
}
}  // namespace movetk::utils


#endif  // MOVETK_TRAJECTORYUTILS_H
