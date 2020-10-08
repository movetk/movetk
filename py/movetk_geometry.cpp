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

/**
 * @file movetk_geometry.cpp
 * @author Aniket Mitra (aniket.mitra@here.com)
 * @brief  Python bindings for Movetk Geometry
 * @version 0.1
 * @date 2020-09-26
 * 
 * 
 */

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
    /**
     * @brief Binding for MovetkPoint
     * 
     */
    py::class_<typename MovetkGeometryKernel::MovetkPoint>(m, "point", py::buffer_protocol())
        /**
         * @param buf numpy array as a buffer
         * @return A  MovetkPoint
         */
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
        /** 
         * @param  m A MovetkPoint
         * @return dimension of the point
         */
        .def("__len__", [](const typename MovetkGeometryKernel::MovetkPoint &m) -> std::size_t {
            return std::distance(m.begin(), m.end());
        })
        /**
         * @param m A MovetkPoint
         * @param idx index of the point
         * @return element at the corresponding index of the point
         */
        .def("__getitem__", [](const typename MovetkGeometryKernel::MovetkPoint &m, std::size_t idx) -> typename MovetkGeometryKernel::NT {
            if (idx > MovetkGeometryKernel::dim)
                throw std::runtime_error("Index out of bounds!");
            return m[idx];
        });

    /**
     * @brief 
     * 
     */
    py::class_<typename MovetkGeometryKernel::MovetkSegment>(m, "segment")
        /**
         * @brief 
         * 
         */
        .def(py::init<typename MovetkGeometryKernel::MovetkPoint,
                      typename MovetkGeometryKernel::MovetkPoint>())
        /**
         * @brief 
         * 
         */
        .def("length",
             [](const typename MovetkGeometryKernel::MovetkSegment &m) -> typename MovetkGeometryKernel::NT {
                 movetk_core::ComputeLength<MovetkGeometryKernel> compute_length;
                 return compute_length(m);
             })
        /**
         * @brief 
         * @param m
         * @param idx
         */
        .def("__getitem__",
             [](const typename MovetkGeometryKernel::MovetkSegment &m, std::size_t idx) -> typename MovetkGeometryKernel::MovetkPoint {
                 if (idx > 2)
                     throw std::runtime_error("Index out of bounds!");
                 return m[idx];
             });
    /**
     * @brief 
     * 
     */
    py::class_<Points>(m, "points")
        /**
         * @brief 
         * 
         */
        .def(py::init<>())
        /**
         * @brief 
         * 
         */
        .def("push_back", (void (Points::*)(const MovetkGeometryKernel::MovetkPoint &)) & Points::push_back)
        /**
         * @brief 
         * @param v
         */
        .def("__len__", [](const Points &v) { return v.size(); })
        /**
         * @brief 
         * @param v
         */
        .def(
            "__iter__", [](Points &v) {
                return py::make_iterator(v.begin(), v.end());
            },
            py::keep_alive<0, 1>());

    /**
     * @brief 
     * 
     */
    py::class_<typename MovetkGeometryKernel::MovetkPolygon>(m, "polygon",
                                                             py::buffer_protocol())
        /**
         * @brief 
         * 
         */
        .def(py::init<>())
        /**
         * @brief 
         * @param p
         */
        .def(py::init([](Points &p) {
            typename MovetkGeometryKernel::MovetkPolygon poly(p.cbegin(), p.cend());
            return poly;
        }))
        /**
         * @brief 
         * @param polygon
         * @param idx
         */
        .def("__getitem__",
             [](typename MovetkGeometryKernel::MovetkPolygon &polygon, std::size_t idx) -> typename MovetkGeometryKernel::MovetkPoint {
                 std::size_t num_vertices = std::distance(polygon.v_begin(),
                                                          polygon.v_end());
                 if (idx >= num_vertices)
                     throw std::runtime_error("Index out of bounds!");
                 auto it = polygon.v_begin() + idx;
                 typename MovetkGeometryKernel::MovetkPoint pt(*it);
                 return pt;
             })
        /**
         * @brief 
         * @param polygon
         */
        .def("num_vertices",
             [](MovetkGeometryKernel::MovetkPolygon &polygon) -> std::size_t {
                 return std::distance(polygon.v_begin(), polygon.v_end());
             });
    /**
     * @brief 
     * 
     */
    py::class_<typename MovetkGeometryKernel::MovetkSphere>(m, "sphere",
                                                            py::buffer_protocol())
        /**
         * @brief 
         * 
         */
        .def(py::init<>())
        /**
         * @brief 
         * @param center
         * @param radius
         */
        .def(py::init([](MovetkGeometryKernel::MovetkPoint &center,
                         MovetkGeometryKernel::NT radius) {
            typename MovetkGeometryKernel::MovetkSphere sphere(center, radius);
            return sphere;
        }))
        .def("radius", [](typename MovetkGeometryKernel::MovetkSphere &s) -> typename MovetkGeometryKernel::NT {
            MovetkGeometryKernel::NT squared_radius = s.squared_radius();
            return std::sqrt(squared_radius);
        })
        .def("center", [](typename MovetkGeometryKernel::MovetkSphere &s) -> typename MovetkGeometryKernel::MovetkPoint {
            return s.center();
        })
        .def("min_sphere", [](typename MovetkGeometryKernel::MovetkSphere &s, Points &p) -> MovetkGeometryKernel::MovetkSphere {
            typedef std::vector<MovetkGeometryKernel::NT> vector;
            typedef movetk_core::movetk_back_insert_iterator<vector> back_insert_iterator;
            vector vec;
            movetk_core::MakeMinSphere<MovetkGeometryKernel> min_sphere;
            MovetkGeometryKernel::NT radius = min_sphere(std::cbegin(p), std::cend(p), back_insert_iterator(vec));
            MovetkGeometryKernel::MovetkPoint center(std::begin(vec), std::end(vec));
            MovetkGeometryKernel::MovetkSphere sphere(center, radius);
            s = sphere;
            return s;
        });
}
