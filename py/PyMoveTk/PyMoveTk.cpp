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
 */

#include <movetk/utils/GeometryBackendTraits.h>
#include <pybind11/pybind11.h>

#include "Geometry.h"
#include "Similarity.h"
#include "Simplification.h"
#include "movetk/metric/DistanceInterface.h"


namespace PyMoveTk {
// TODO: replace with choices.
using Backend = movetk::BaseGeometryKernel<movetk::backends::BoostBackend>;

template <typename Backend>
void setup_movetk_modules(pybind11::module& parent_module) {
	auto geometry_module = parent_module.def_submodule("geometry", "MoveTk geometric objects");
	PyMoveTk::GeometryModule<Backend, typename Backend::Norm>::register_module(geometry_module);

	auto simplification_module = parent_module.def_submodule("simplification", "MoveTk geometric objects");
	PyMoveTk::SimplificationModule<Backend, typename Backend::Norm>::register_module(simplification_module);

	auto distances_module = parent_module.def_submodule("distances", "Distances and similarity measures for polylines");
	PyMoveTk::SimilarityModule<Backend, typename Backend::Norm>::register_module(distances_module);
}
}  // namespace PyMoveTk

PYBIND11_MODULE(PyMoveTk, m) {
	PyMoveTk::setup_movetk_modules<PyMoveTk::Backend>(m);
}
