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

// TODO: replace with choices.
namespace PyMoveTk {
using Backend = movetk::BaseGeometryKernel<movetk::backends::BoostBackend>;
using GeometryKernel = Backend::GeometryBackend;
using MovetkGeometryKernel = Backend::MovetkGeometryKernel;
}  // namespace PyMoveTk

PYBIND11_MODULE(PyMoveTk, m) {
	// Or maybe not. Depends on what we consider top level
	auto geometry_module = m.def_submodule("geometry", "MoveTk geometric objects");
	PyMoveTk::GeometryModule<PyMoveTk::Backend, PyMoveTk::Backend::Norm>::register_module(geometry_module);

	auto simplification_module = m.def_submodule("simplification", "MoveTk geometric objects");
	PyMoveTk::SimplificationModule<PyMoveTk::Backend, PyMoveTk::Backend::Norm>::register_module(simplification_module);

	auto distances_module = m.def_submodule("distances", "Distances and similarity measures for polylines");
	PyMoveTk::SimilarityModule<PyMoveTk::Backend, PyMoveTk::Backend::Norm>::register_module(distances_module);
}
