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
// Created by Mitra, Aniket on 26/09/2020.
//

//#include "movetk/geom/CGALTraits.h"
#include "movetk/geom/BoostGeometryTraits.h"
#include "movetk/geom/GeometryInterface.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;



struct BoostGeometry2{

    const static size_t dimensions = 2;
    typedef movetk_support::BoostGeometryTraits<long double, dimensions> Boost_Geometry_Backend;
    typedef movetk_core::MovetkGeometryKernel<typename Boost_Geometry_Backend::Wrapper_Boost_Geometry> MovetkGeometryKernel;
    typedef movetk_core::MakePoint<typename BoostGeometry2::MovetkGeometryKernel> MakePoint;
    typedef MovetkGeometryKernel::MovetkPoint MovetkPoint;
    typedef MovetkGeometryKernel::NT NT;
};

PYBIND11_MODULE(movetk_geometry, m) {
    py::class_<typename BoostGeometry2::MakePoint>(m, "make_point")
        .def(py::init<>())
        .def("__call__",[](typename BoostGeometry2::MakePoint& f,
                std::array<typename BoostGeometry2::NT, BoostGeometry2::dimensions> arr) {
                    return f(std::cbegin(arr), std::cend(arr));
        });
    py::class_<typename BoostGeometry2::MovetkPoint>(m, "movetk_point")
            .def(py::init<>())
            .def("__repr__",[](const typename BoostGeometry2::MovetkPoint& self){
                auto it =  self.begin();
                return "[ " + std::to_string(*it) + "," + std::to_string(*(it + 1)) + " ]\n";
            });
}
