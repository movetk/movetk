#ifndef PYMOVETK_SIMPLIFICATION_H
#define PYMOVETK_SIMPLIFICATION_H
#include <pybind11/pybind11.h>
#include "Geometry.h"
#include "movetk/algo/Simplification.h"

namespace PyMoveTk
{
    template<typename GeometryKernel,typename Norm>
    struct SimplificationModule
    {
        using MovetkKernel = typename GeometryKernel::MovetkGeometryKernel;
        using GeometryModule = GeometryModule<typename GeometryKernel::MovetkGeometryKernel, Norm>;
        using NT = typename GeometryKernel::NT;
        using Polyline = typename GeometryModule::Polyline;
    private:
        //TODO: move down
        static void reconstruct_from_iterators(const std::vector<typename Polyline::const_iterator>& result, Polyline& output)
        {
            output.reserve(result.size());
            for (const auto& el : result)
            {
                output.push_back(*el);
            }
        }
    public:
        static void register_douglas_peucker(pybind11::module& mod)
        {
            mod.def("douglas_peucker",[](const typename GeometryModule::Polyline& polyline, NT epsilon) -> typename GeometryModule::Polyline
            {
                std::vector<std::decay_t<decltype(polyline.cbegin())>> result;
                using FindFarthest = movetk_algorithms::FindFarthest<MovetkKernel, Norm> ;
                // Apply algorithm
                movetk_algorithms::DouglasPeucker<MovetkKernel, FindFarthest> douglas_peucker_alg(epsilon);
                douglas_peucker_alg(std::begin(polyline), std::end(polyline), movetk_core::movetk_back_insert_iterator(result));
                // Reconstruct output and return
                typename GeometryModule::Polyline output;
                reconstruct_from_iterators(result, output);
                return output;
            });
        }
        static void register_imai_iri(pybind11::module& mod)
        {
            mod.def("imai_iri", [](const typename GeometryModule::Polyline& polyline, NT epsilon) -> typename GeometryModule::Polyline
            {
                std::vector<std::decay_t<decltype(polyline.cbegin())>> result;
                // Apply algorithm
                using ChanChin = movetk_algorithms::ChanChin<MovetkKernel, typename GeometryModule::Wedge>;//Use Chan Chin for constructing viable shortcuts.
                using ImaiIri = movetk_algorithms::ImaiIri<MovetkKernel, ChanChin>;
                movetk_algorithms::ImaiIri<MovetkKernel, ChanChin> imai_iri(epsilon);
                imai_iri(std::begin(polyline), std::end(polyline), movetk_core::movetk_back_insert_iterator(result));
                // Reconstruct output and return
                typename GeometryModule::Polyline output;
                reconstruct_from_iterators(result, output);
                return output;
            });
        }
        static void register_agarwal(pybind11::module& mod)
        {
            mod.def("agarwal", [](const typename GeometryModule::Polyline& polyline, NT epsilon) -> typename GeometryModule::Polyline
            {
                std::vector<std::decay_t<decltype(polyline.cbegin())>> result;
                // Apply algorithm
                using SqDistance = movetk_support::squared_distance_d<MovetkKernel, Norm>;
                movetk_algorithms::Agarwal<MovetkKernel,SqDistance> agarwal(epsilon);
                agarwal(std::begin(polyline), std::end(polyline), movetk_core::movetk_back_insert_iterator(result));
                // Reconstruct output and return
                typename GeometryModule::Polyline output;
                reconstruct_from_iterators(result, output);
                return output;
            });
        }

        static void register_module(pybind11::module& mod)
        {
            register_douglas_peucker(mod);
            register_imai_iri(mod);
            register_agarwal(mod);
        }
    };
}
#endif