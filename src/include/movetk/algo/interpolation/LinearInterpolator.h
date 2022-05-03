#ifndef MOVETK_INTERPOLATION_LINEARINTERPOLATOR_H
#define MOVETK_INTERPOLATION_LINEARINTERPOLATOR_H
#include <cassert>

#include "BaseInterpolator.h"
namespace movetk::interpolation {
struct linear_interpolator_tag;

template <class InterpolationTraits, int LatIdx, int LonIdx, int TsIdx, int SpeedIdx, int HeadingIdx>
class Interpolator<linear_interpolator_tag, InterpolationTraits, LatIdx, LonIdx, TsIdx, SpeedIdx, HeadingIdx> {
private:
	typename InterpolationTraits::Norm norm;
	typename InterpolationTraits::GeoProjection ref;
	using GeometryTraits = typename InterpolationTraits::GeometryTraits;
	geom::Scaling<GeometryTraits> scale;
	geom::Translation<GeometryTraits> translate;

public:
	Interpolator(typename InterpolationTraits::NT reflat, typename InterpolationTraits::NT reflon) {
		ref = typename InterpolationTraits::GeoProjection(reflat, reflon);
	}


	/**
	 * @brief Generates an interpolation between the two given points
	 * @tparam TSIterator Timestapmp input iterator
	 * @tparam OutputIterator The output iterator to save the new points to
	 * @param probe_u The start location
	 * @param probe_v The end location
	 * @param first Start of the timestamp range. Assumed to be an ordered range, with the first and last
	 * timestamps corresponding to the timestamps of the probes (these will not be visited)
	 * @param beyond End of the timestamp range
	 * @param result Output iterator to write to
	 */
	template <class TSIterator, class OutputIterator>
	void operator()(const typename InterpolationTraits::ProbePoint& probe_u,
	                const typename InterpolationTraits::ProbePoint& probe_v,
	                TSIterator first,
	                TSIterator beyond,
	                OutputIterator result) {
		const auto point_for_probe = [this](const auto& probe_point) {
			return movetk::utils::get_point<InterpolationTraits>(std::get<LatIdx>(probe_point),
			                                                     std::get<LonIdx>(probe_point),
			                                                     ref);
		};
		// Points
		auto p_u = point_for_probe(probe_u);
		auto p_v = point_for_probe(probe_v);

		// Timestamps
		auto ts_u = std::get<TsIdx>(probe_u);
		auto ts_v = std::get<TsIdx>(probe_v);

		assert(ts_v > ts_u);

		const auto delta_t = ts_v - ts_u;
		const auto interval = static_cast<typename InterpolationTraits::NT>(delta_t);

		using ProbePoint = typename InterpolationTraits::ProbePoint;

		std::vector<ProbePoint> interpolated_pts;

		auto tit = first;
		interpolated_pts.push_back(probe_u);
		while (++tit != (beyond - 1)) {
			assert(*tit >= ts_u);
			assert(*tit <= ts_v);
			ProbePoint interpolated_point;
			auto fraction = (*tit - ts_u) / interval;
			const auto v = scale(p_u, p_v, fraction);
			auto p = translate(p_u, v);
			auto x = movetk::utils::get_x<GeometryTraits>(p);
			auto y = movetk::utils::get_y<GeometryTraits>(p);
			auto geocoordinate = ref.inverse(y, x);
			std::get<LatIdx>(interpolated_point) = geocoordinate[0];
			std::get<LonIdx>(interpolated_point) = geocoordinate[1];
			std::get<TsIdx>(interpolated_point) = *tit;
			interpolated_pts.push_back(interpolated_point);
		}

		interpolated_pts.push_back(probe_v);

		auto ip_first = std::begin(interpolated_pts);
		auto ip_beyond = std::end(interpolated_pts);

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
		// Move the result to the output
		std::move(ip_first, ip_beyond, result);
	}
};


}  // namespace movetk::interpolation
#endif