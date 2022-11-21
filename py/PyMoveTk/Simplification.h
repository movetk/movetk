#ifndef PYMOVETK_SIMPLIFICATION_H
#define PYMOVETK_SIMPLIFICATION_H
#include <pybind11/pybind11.h>

#include "Geometry.h"
#include "movetk/Simplification.h"

namespace PyMoveTk {
template <typename GeometryKernel, typename Norm>
struct SimplificationModule {
	using MovetkKernel = typename GeometryKernel::MovetkGeometryKernel;
	using UsedGeometryModule = GeometryModule<typename GeometryKernel::MovetkGeometryKernel, Norm>;
	using NT = typename GeometryKernel::NT;
	using Polyline = typename UsedGeometryModule::Polyline;

private:
	// TODO: move down
	static void reconstruct_from_iterators(const std::vector<typename Polyline::const_iterator>& result,
	                                       Polyline& output) {
		output.reserve(result.size());
		for (const auto& el : result) {
			output.push_back(*el);
		}
	}
	template <typename ALGORITHM>
	static Polyline apply_simplification(ALGORITHM&& algorithm, const Polyline& polyline) {
		std::vector<std::decay_t<decltype(polyline.cbegin())>> result;
		algorithm(std::begin(polyline), std::end(polyline), std::back_inserter(result));
		// Reconstruct output and return
		Polyline output;
		reconstruct_from_iterators(result, output);
		return output;
	}

public:
	static void register_douglas_peucker(pybind11::module& mod) {
		mod.def("douglas_peucker", [](const Polyline& polyline, NT epsilon) -> Polyline {
			using FindFarthest = movetk::simplification::FindFarthest<MovetkKernel, Norm>;
			// Apply algorithm
			movetk::simplification::DouglasPeucker<MovetkKernel, FindFarthest> douglas_peucker_alg(epsilon);
			return apply_simplification(douglas_peucker_alg, polyline);
		});
	}
	static void register_imai_iri(pybind11::module& mod) {
		mod.def("imai_iri", [](const Polyline& polyline, NT epsilon) -> Polyline {
			// Apply algorithm
			using ChanChin = movetk::simplification::ChanChin<MovetkKernel,
			                                                  typename UsedGeometryModule::Wedge>;  // Use Chan Chin for
			                                                                                        // constructing
			                                                                                        // viable shortcuts.
			using ImaiIri = movetk::simplification::ImaiIri<MovetkKernel, ChanChin>;
			ImaiIri imai_iri(epsilon);
			// Reconstruct output and return
			return apply_simplification(imai_iri, polyline);
		});
	}
	static void register_agarwal(pybind11::module& mod) {
		mod.def("agarwal", [](const Polyline& polyline, NT epsilon) -> Polyline {
			// Apply algorithm
			using SqDistance = movetk::metric::squared_distance_d<MovetkKernel, Norm>;
			movetk::simplification::Agarwal<MovetkKernel, SqDistance> agarwal(epsilon);
			return apply_simplification(agarwal, polyline);
		});
	}

	static void register_module(pybind11::module& mod) {
		register_douglas_peucker(mod);
		register_imai_iri(mod);
		register_agarwal(mod);
	}
};
}  // namespace PyMoveTk
#endif