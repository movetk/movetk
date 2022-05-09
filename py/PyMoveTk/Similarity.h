#ifndef PYMOVETK_SIMILARITY_H
#define PYMOVETK_SIMILARITY_H
#include <movetk/Similarity.h>
#include <pybind11/pybind11.h>

#include "Geometry.h"
namespace PyMoveTk {
template <typename GeometryKernel, typename Norm>
struct SimilarityModule {
	using UsedGeometryModule = GeometryModule<typename GeometryKernel::MovetkGeometryKernel, Norm>;
	using MoveTkKernel = typename GeometryKernel::MovetkGeometryKernel;
	using Polyline = typename UsedGeometryModule::Polyline;
	using NT = typename GeometryKernel::NT;

	static void register_lcss(pybind11::module& mod) {
		namespace py = pybind11;
		using namespace pybind11::literals;
		mod.def(
		    "compute_lcss",
		    [](const Polyline& p0, const Polyline& p1, NT epsilon, std::size_t delta) -> std::size_t {
			    movetk::similarity::LongestCommonSubSequence<GeometryKernel, Norm> lcss(epsilon, delta);

			    return lcss(p0.begin(), p0.end(), p1.begin(), p1.end());
		    },
		    "Compute longest common subsequence. Epsilon specifies the maximum distance, while delta gives the maximum "
		    "allowed index distance",
		    "polyline0"_a,
		    "polyline1"_a,
		    "epsilon"_a,
		    "delta"_a);
	}

	static void register_distances(pybind11::module& mod) {
		namespace py = pybind11;
		using namespace pybind11::literals;
		mod.def(
		       "compute_discrete_hausdorff",
		       [](const Polyline& p0, const Polyline& p1) -> std::size_t {
			       movetk::metric::Discrete_Hausdorff<MoveTkKernel, Norm> discrete_hausdorff;
			       return discrete_hausdorff(p0.begin(), p0.end(), p1.begin(), p1.end());
		       },
		       "Compute the discrete Hausdorff distance",
		       "polyline0"_a,
		       "polyline1"_a)
		    .def(
		        "compute_discrete_frechet",
		        [](const Polyline& p0, const Polyline& p1) -> std::size_t {
			        movetk::metric::Discrete_Frechet<MoveTkKernel, Norm> discrete_frechet;
			        return discrete_frechet(p0.begin(), p0.end(), p1.begin(), p1.end());
		        },
		        "Compute the discrete Hausdorff distance",
		        "polyline0"_a,
		        "polyline1"_a)
		    .def(
		        "decide_strong_frechet",
		        [](const Polyline& p0, const Polyline& p1, NT epsilon) -> bool {
			        movetk::metric::StrongFrechet<MoveTkKernel, movetk::metric::squared_distance_d<MoveTkKernel, Norm>>
			            strong_frechet;
			        return strong_frechet.decide(p0.begin(), p0.end(), p1.begin(), p1.end(), epsilon);
		        },
		        "Decide whether the strong Frechet distance is at most epsilon between the polylines.",
		        "polyline0"_a,
		        "polyline1"_a,
		        "epsilon"_a)
		    .def(
		        "compute_strong_frechet",
		        [](const Polyline& p0, const Polyline& p1, NT tolerance) -> NT {
			        using StrongFrechet =
			            movetk::metric::StrongFrechet<MoveTkKernel, movetk::metric::squared_distance_d<MoveTkKernel, Norm>>;
			        StrongFrechet strong_frechet;
			        strong_frechet.setMode(StrongFrechet::Mode::DoubleAndSearch);
			        strong_frechet.setTolerance(tolerance);
			        return strong_frechet(p0.begin(), p0.end(), p1.begin(), p1.end());
		        },
		        "Compute the strong Frechet distance by double and search on the value of epsilon, stopping as soon as the "
		        "desired tolerance is reached.",
		        "polyline0"_a,
		        "polyline1"_a,
		        "tolerance"_a);
	}

	static void register_module(pybind11::module& mod) {
		register_distances(mod);
		register_lcss(mod);
	}
};
}  // namespace PyMoveTk

#endif
