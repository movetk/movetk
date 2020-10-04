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

#include <movetk/utils/GeometryBackendTraits.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

typedef typename GeometryKernel::MovetkGeometryKernel MovetkGeometryKernel;
typedef typename movetk_core::movetk_basic_iterator<const typename MovetkGeometryKernel::NT> CoordinateIterator;

PYBIND11_MODULE(movetk_geometry, m)
{

    py::class_<typename MovetkGeometryKernel::MovetkPoint>(m, "point", py::buffer_protocol())
        .def(py::init([](py::array_t<typename MovetkGeometryKernel::NT,
                                     MovetkGeometryKernel::dim> const buf) {
            py::buffer_info info = buf.request();
            CoordinateIterator first(
                static_cast<const typename MovetkGeometryKernel::NT *>(info.ptr));
            CoordinateIterator beyond(
                static_cast<const typename MovetkGeometryKernel::NT *>(info.ptr) + MovetkGeometryKernel::dim);
            typename MovetkGeometryKernel::MovetkPoint pt(first, beyond);
            return pt;
        }))
        .def("__getitem__", [](const typename MovetkGeometryKernel::MovetkPoint &m, std::size_t idx) -> typename MovetkGeometryKernel::NT {
            if (idx > MovetkGeometryKernel::dim)
                throw std::runtime_error("Index out of bounds!");
            return m[idx];
        });

    py::class_<typename MovetkGeometryKernel::MovetkSegment>(m, "segment")
        .def(py::init<typename MovetkGeometryKernel::MovetkPoint,
                      typename MovetkGeometryKernel::MovetkPoint>())
        .def("__getitem__", [](const typename MovetkGeometryKernel::MovetkSegment &m, std::size_t idx) -> typename MovetkGeometryKernel::MovetkPoint {
            if (idx > 2)
                throw std::runtime_error("Index out of bounds!");
            return m[idx];
        });
}
