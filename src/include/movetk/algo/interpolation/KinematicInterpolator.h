#ifndef MOVETK_INTERPOLATION_KINEMATICINTERPOLATOR_H
#define MOVETK_INTERPOLATION_KINEMATICINTERPOLATOR_H
#include "BaseInterpolator.h"
#include "movetk/utils/TrajectoryUtils.h"
namespace movetk::interpolation {
struct kinematic_interpolator_tag;

template <class InterpolationTraits, int LatIdx, int LonIdx, int TsIdx, int SpeedIdx, int HeadingIdx>
class Interpolator<kinematic_interpolator_tag, InterpolationTraits, LatIdx, LonIdx, TsIdx, SpeedIdx, HeadingIdx> {
	// based on https://doi.org/10.1080/13658816.2015.1081909
private:
	using GeometryTraits = typename InterpolationTraits::GeometryTraits;
	typename InterpolationTraits::Norm norm;
	typename InterpolationTraits::GeoProjection ref;
	geom::Scaling<GeometryTraits> scale;
	geom::Translation<GeometryTraits> translate;
	geom::MakePoint<GeometryTraits> make_point;
	typename InterpolationTraits::MovetkPoint ORIGIN = make_point({0, 0});

public:
	Interpolator(typename InterpolationTraits::NT reflat, typename InterpolationTraits::NT reflon)
	    : ref(reflat, reflon) {}

	template <class TSIterator, class OutputIterator>
	void operator()(typename InterpolationTraits::ProbePoint& probe_u,
	                typename InterpolationTraits::ProbePoint& probe_v,
	                TSIterator first,
	                TSIterator beyond,
	                OutputIterator result) {
		const auto point_for_probe = [this](const auto& probe_point) {
			return movetk::utils::get_point<InterpolationTraits>(std::get<LatIdx>(probe_point),
			                                                     std::get<LonIdx>(probe_point),
			                                                     ref);
		};
		using ProbePoint = typename InterpolationTraits::ProbePoint;

		auto p_u = point_for_probe(probe_u);
		auto p_v = point_for_probe(probe_v);

		auto velocity_u =
		    movetk::utils::get_velocity<GeometryTraits>(std::get<SpeedIdx>(probe_u), std::get<HeadingIdx>(probe_u));
		auto velocity_v =
		    movetk::utils::get_velocity<GeometryTraits>(std::get<SpeedIdx>(probe_v), std::get<HeadingIdx>(probe_v));

		auto delta_velocity = velocity_v - velocity_u;
		auto delta_position = p_v - p_u;

		auto t_u = std::get<TsIdx>(probe_u);
		auto t_v = std::get<TsIdx>(probe_v);

		auto delta_t = t_v - t_u;
		auto delta_t_squared = delta_t * delta_t;
		auto delta_t_cube = delta_t_squared * delta_t;
		auto interval = static_cast<typename InterpolationTraits::NT>(delta_t);

		auto norm_delta_velocity = norm(delta_velocity);
		auto norm_delta_position = norm(delta_position);
		auto displacement = norm ^ 1;

		if (norm_delta_position > MOVETK_EPS && norm_delta_velocity < MOVETK_EPS) {
			typename InterpolationTraits::NT speed_v = displacement / delta_t;
			velocity_v =
			    movetk::utils::get_velocity<typename InterpolationTraits::GeometryTraits>(speed_v,
			                                                                              std::get<HeadingIdx>(probe_v));
			std::get<SpeedIdx>(probe_v) = speed_v;
		}

		auto scaled_velocity = scale(velocity_u, delta_t);

		auto numerator = movetk::utils::get_x<GeometryTraits>(delta_position) -
		                 movetk::utils::get_x<GeometryTraits>(scaled_velocity) -
		                 movetk::utils::get_x<GeometryTraits>(delta_velocity) / 2.0;
		const auto denominator = delta_t_cube / 6.0 - delta_t_squared / 4.0;
		auto m_x = numerator / denominator;

		numerator = movetk::utils::get_y<GeometryTraits>(delta_position) -
		            movetk::utils::get_y<GeometryTraits>(scaled_velocity) -
		            movetk::utils::get_y<GeometryTraits>(delta_velocity) / 2.0;
		auto m_y = numerator / denominator;

		auto b_x = movetk::utils::get_x<GeometryTraits>(delta_velocity) / delta_t_squared - m_x / 2.0;
		auto b_y = movetk::utils::get_y<GeometryTraits>(delta_velocity) / delta_t_squared - m_y / 2.0;

		auto m = make_point({m_x, m_y});
		auto b = make_point({b_x, b_y});

		auto v1 = scale(b, ORIGIN, delta_t_squared);
		auto v2 = scale(m, ORIGIN, delta_t_squared / 2.0);

		auto rhs = v1 + v2;
		// auto eps = delta_velocity - rhs;
		// assert(eps * eps < MOVETK_EPS);

		v1 = scale(b, ORIGIN, delta_t_squared / 2.0);
		v2 = scale(m, ORIGIN, delta_t_squared / 6.0);

		rhs = v1 + v2;
		// eps = delta_position - scaled_velocity - rhs;
		// assert(eps * eps < MOVETK_EPS);

		std::size_t num_elements = std::distance(first, beyond);

		std::vector<ProbePoint> interpolated_pts(num_elements);
		std::vector<typename InterpolationTraits::MovetkVector> velocities;
		auto ip_first = std::begin(interpolated_pts);
		auto ip_it = ip_first;
		*ip_it = probe_u;

		auto tit = first;
		while (++tit != (beyond - 1)) {
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

			auto op1 = movetk::utils::get_x<GeometryTraits>(b) * (delta_t_squared / 2.0);
			auto op2 = movetk::utils::get_x<GeometryTraits>(m) * (delta_t_cube / 6.0);

			auto interpolated_x =
			    op1 + op2 + movetk::utils::get_x<GeometryTraits>(scaled_velocity) + movetk::utils::get_x<GeometryTraits>(p_u);

			op1 = movetk::utils::get_y<GeometryTraits>(b) * (delta_t_squared / 2.0);
			op2 = movetk::utils::get_y<GeometryTraits>(m) * (delta_t_cube / 6.0);

			auto interpolated_y =
			    op1 + op2 + movetk::utils::get_y<GeometryTraits>(scaled_velocity) + movetk::utils::get_y<GeometryTraits>(p_u);

			auto geocoordinate = ref.inverse(interpolated_y, interpolated_x);

			std::get<LatIdx>(probe) = geocoordinate[0];
			std::get<LonIdx>(probe) = geocoordinate[1];

			std::get<TsIdx>(probe) = *tit;

			*(++ip_it) = probe;

			v1 = scale(b, ORIGIN, delta_t);
			v2 = scale(m, ORIGIN, delta_t_squared / 2.0);

			typename InterpolationTraits::MovetkVector fwdV = velocity_u + v1 + v2;

			fwdV *= (1 - fraction);
			// fwdV *= 0.5;

			t_u = std::get<TsIdx>(probe);
			t_v = std::get<TsIdx>(probe_v);

			delta_t = t_v - t_u;
			delta_t_squared = delta_t * delta_t;
			delta_t_cube = delta_t_squared * delta_t;

			v1 = scale(b, ORIGIN, delta_t);
			v2 = scale(m, ORIGIN, delta_t_squared / 2.0);

			auto bwdV = velocity_v - v1 - v2;

			bwdV *= fraction;
			// bwdV *= 0.5;

			auto V = fwdV + bwdV;

			velocities.push_back(V);
		}

		*(++ip_it) = probe_v;

		auto ip_beyond = std::end(interpolated_pts);

		if constexpr (HeadingIdx > -1) {
			movetk::utils::get_headings<InterpolationTraits, decltype(ip_first), LatIdx, LonIdx, HeadingIdx>(ip_first,
			                                                                                                 ip_beyond);
		}

		if constexpr (SpeedIdx > -1) {
			ip_it = ip_first;
			auto vit = std::begin(velocities);
			while (++ip_it != (ip_beyond - 1)) {
				auto heading_v = std::get<HeadingIdx>(*ip_it);
				auto squared_length = norm(*vit);
				auto speed_v = norm ^ 1;
				std::get<SpeedIdx>(*ip_it) = speed_v;
				vit++;
			}
		}
		std::move(ip_first, ip_beyond, result);
	}
};

}  // namespace movetk::interpolation
#endif