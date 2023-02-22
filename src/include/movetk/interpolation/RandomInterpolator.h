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
#ifndef MOVETK_INTERPOLATION_RANDOMINTERPOLATOR_H
#define MOVETK_INTERPOLATION_RANDOMINTERPOLATOR_H
#include "BaseInterpolator.h"
namespace movetk::interpolation {
struct random_trajectory_generator_tag;

template <class InterpolationTraits, int LatIdx, int LonIdx, int TsIdx, int SpeedIdx, int HeadingIdx>
class Interpolator<random_trajectory_generator_tag, InterpolationTraits, LatIdx, LonIdx, TsIdx, SpeedIdx, HeadingIdx> {
	// based on https://doi.org/10.1080/13658816.2014.999682
private:
	typename InterpolationTraits::NT eps;
	typename InterpolationTraits::Norm norm;
	typename InterpolationTraits::GeoProjection ref;
	typename InterpolationTraits::MinimumBoundingRectangle mbr;
	geom::Scaling<typename InterpolationTraits::GeometryTraits> scale;
	geom::Translation<typename InterpolationTraits::GeometryTraits> translate;
	geom::MakePoint<typename InterpolationTraits::GeometryTraits> make_point;
	typename InterpolationTraits::MovetkPoint ORIGIN = make_point({0, 0});
	std::default_random_engine rng;

	template <class Iterator>
	Iterator find_source(Iterator first, Iterator current) {
		Iterator it = current;
		while (it != first) {
			typename InterpolationTraits::MovetkVector v = *it - ORIGIN;
			if ((v * v) > MOVETK_EPS)
				return it;
			it--;
		}
		return first;
	}

	template <class Iterator>
	Iterator find_destination(Iterator beyond, Iterator current) {
		Iterator it = current;
		while (it != beyond) {
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
	             typename InterpolationTraits::NT epsilon) {
		ref = typename InterpolationTraits::GeoProjection(reflat, reflon);
		std::random_device random;
		rng = std::default_random_engine(random());
		eps = epsilon;
	}

	Interpolator(typename InterpolationTraits::NT reflat, typename InterpolationTraits::NT reflon) {
		ref = typename InterpolationTraits::GeoProjection(reflat, reflon);
		std::random_device random;
		rng = std::default_random_engine(random());
		eps = 0.9999;
	}

	template <class TSIterator, class OutputIterator>
	void operator()(const typename InterpolationTraits::ProbePoint& probe_u,
	                const typename InterpolationTraits::ProbePoint& probe_v,
	                TSIterator first,
	                TSIterator beyond,
	                typename InterpolationTraits::NT max_speed,
	                OutputIterator result) {
		auto lat_u = std::get<LatIdx>(probe_u);
		auto lon_u = std::get<LonIdx>(probe_u);
		auto p_u = movetk::utils::get_point<InterpolationTraits>(lat_u, lon_u, ref);

		auto lat_v = std::get<LatIdx>(probe_v);
		auto lon_v = std::get<LonIdx>(probe_v);
		auto p_v = movetk::utils::get_point<InterpolationTraits>(lat_v, lon_v, ref);

		auto t_u = std::get<TsIdx>(probe_u);
		auto t_v = std::get<TsIdx>(probe_v);

		std::size_t num_elements = std::distance(first, beyond);

		std::vector<typename InterpolationTraits::ProbePoint> interpolated_pts(num_elements);

		if (num_elements == 2) {
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
		for (auto&& i : sequence) {
			auto tit = first + i;
			auto pit = std::begin(output) + i;
			auto source = find_source(std::begin(output), pit);
			auto destination = find_destination(std::end(output), pit);
			std::size_t position_source = std::distance(std::begin(output), source);
			std::size_t position_destination = std::distance(std::begin(output), destination);
			auto delta_forward = *tit - *(first + position_source);
			auto delta_backward = *(first + position_destination) - *tit;
			auto radius_u = delta_forward * max_speed;
			auto radius_v = delta_backward * max_speed;
			auto squared_radius_u = radius_u * radius_u;
			auto squared_radius_v = radius_v * radius_v;
			typename InterpolationTraits::MovetkVector v = *source - *destination;
			typename InterpolationTraits::NT squared_length = norm(v);
			if (squared_length < MOVETK_EPS)
				continue;

			auto bounds = mbr(*source, *destination, radius_u, radius_v);
			const auto [x_min, x_max] =
			    std::minmax(movetk::utils::get_x<typename InterpolationTraits::GeometryTraits>(bounds.first),
			                movetk::utils::get_x<typename InterpolationTraits::GeometryTraits>(bounds.second));
			const auto [y_min, y_max] =
			    std::minmax(movetk::utils::get_y<typename InterpolationTraits::GeometryTraits>(bounds.first),
			                movetk::utils::get_y<typename InterpolationTraits::GeometryTraits>(bounds.second));
			if ((x_min == 0) && (x_max == 0) && (y_min == 0) && (y_max == 0)) {
				continue;
			}

			intersects = true;
			std::uniform_real_distribution<typename InterpolationTraits::NT> distributionx(x_min, x_max);
			typename InterpolationTraits::NT x = distributionx(rng);
			std::uniform_real_distribution<typename InterpolationTraits::NT> distributiony(y_min, y_max);
			typename InterpolationTraits::NT y = distributiony(rng);
			while (true) {
				const auto p = make_point({x, y});
				const auto pu = p - *source;
				const auto pv = p - *destination;
				const auto squared_length_pu = norm(pu);
				const auto squared_length_pv = norm(pv);

				if ((squared_length_pu <= squared_radius_u + eps) && (squared_length_pv <= squared_radius_v + eps)) {
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
		auto ip_beyond = std::remove_if(std::begin(interpolated_pts), std::end(interpolated_pts), [](auto& item) {
			if ((std::get<LatIdx>(item) == 0.00) && (std::get<LonIdx>(item) == 0.00))
				return true;
			else
				return false;
		});

		num_elements = std::distance(ip_first, ip_beyond);

		if (num_elements == 0)
			return;

		if constexpr (SpeedIdx > -1) {
			std::get<SpeedIdx>(*ip_first) = std::get<SpeedIdx>(probe_u);
			std::get<SpeedIdx>(*(ip_beyond - 1)) = std::get<SpeedIdx>(probe_v);
			movetk::utils::get_speeds<InterpolationTraits, decltype(ip_first), LatIdx, LonIdx, TsIdx, SpeedIdx>(ip_first,
			                                                                                                    ip_beyond);
		}

		if constexpr (HeadingIdx > -1) {
			std::get<HeadingIdx>(*ip_first) = std::get<HeadingIdx>(probe_u);
			std::get<HeadingIdx>(*(ip_beyond - 1)) = std::get<HeadingIdx>(probe_v);
			movetk::utils::get_headings<InterpolationTraits, decltype(ip_first), LatIdx, LonIdx, HeadingIdx>(ip_first,
			                                                                                                 ip_beyond);
		}

		std::move(ip_first, ip_beyond, result);
	}
};

}  // namespace movetk::interpolation
#endif