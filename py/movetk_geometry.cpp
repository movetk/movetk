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

PYBIND11_MAKE_OPAQUE(std::vector<MovetkGeometryKernel::MovetkPoint, std::allocator<MovetkGeometryKernel::MovetkPoint>>);

using Points = std::vector<MovetkGeometryKernel::MovetkPoint, std::allocator<MovetkGeometryKernel::MovetkPoint>>;

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
            std::size_t size = std::distance(first, beyond);
            if (size != MovetkGeometryKernel::dim)
                throw std::runtime_error("Size of input does not satisfy the predefined diemension of MovetkPoint!");
            typename MovetkGeometryKernel::MovetkPoint pt(first, beyond);
            return pt;
        }))
        .def("__len__", [](const typename MovetkGeometryKernel::MovetkPoint &m) {
            return std::distance(m.begin(), m.end());
        })
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

    py::class_<Points>(m, "points")
        .def(py::init<>())
        .def("push_back", (void (Points::*)(const MovetkGeometryKernel::MovetkPoint &)) & Points::push_back)
        .def("__len__", [](const Points &v) { return v.size(); })
        .def(
            "__iter__", [](Points &v) {
                return py::make_iterator(v.begin(), v.end());
            },
            py::keep_alive<0, 1>());

    py::class_<typename MovetkGeometryKernel::MovetkPolygon>(m, "polygon", py::buffer_protocol())
        .def(py::init<>())
        .def(py::init([](Points &p) {
            typename MovetkGeometryKernel::MovetkPolygon poly(p.cbegin(), p.cend());
            return poly;
        }))
        .def(
            "__getitem__", [](typename MovetkGeometryKernel::MovetkPolygon &polygon, std::size_t idx) -> typename MovetkGeometryKernel::MovetkPoint {
                std::size_t num_vertices = std::distance(polygon.v_begin(), polygon.v_end());
                if (idx >= num_vertices)
                    throw std::runtime_error("Index out of bounds!");
                auto it = polygon.v_begin() + idx;
                typename MovetkGeometryKernel::MovetkPoint pt(*it);
                return pt;
            })
        .def("num_vertices", [](MovetkGeometryKernel::MovetkPolygon &polygon) {
            return std::distance(polygon.v_begin(), polygon.v_end());
        });
}
