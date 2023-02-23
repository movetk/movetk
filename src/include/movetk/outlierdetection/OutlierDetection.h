/*
 * Copyright (C) 2018-2020
 * HERE Europe B.V.
 * Utrecht University (The Netherlands).
 * TU/e (The Netherlands).
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
#ifndef MOVETK_OUTLIERDETECTION_OUTLIERDETECTION_H
#define MOVETK_OUTLIERDETECTION_OUTLIERDETECTION_H
namespace movetk::outlierdetection {


/*!
 * @struct OutlierDetectionTraits
 * @brief This traits class serves as a collection of
 * types required by various outlier detection algorithms
 * @tparam _ProbeTraits - This class is a collection of types
 * required to define a Probe. For example @refitem _ProbeTraits
 * @tparam _GeometryTraits - This class is a collection of movetk
 * geometry types. For example @refitem movetk::geom::MovetkGeometryKernel
 * @tparam _Norm - The type that models Euclidean distance
 * For example @refitem movetk_support::FiniteNorm
 * @tparam _ContainerTraits - This class models the data structure used
 * by the outlier detection algorithnms
 */
template <class _ProbeTraits, class _GeometryTraits, class _Norm, class _ContainerTraits = void>
struct OutlierDetectionTraits {
	using Norm = _Norm;
	using ProbeTraits = _ProbeTraits;
	using GeometryTraits = _GeometryTraits;
	using ContainerTraits = _ContainerTraits;
	using NT = typename _GeometryTraits::NT;
	using Point = typename _GeometryTraits::MovetkPoint;
	using Vector = typename _GeometryTraits::MovetkVector;
};


// Stub for outlier detection algorithms
template <typename GeometryKernel, typename Predicate, typename AlgorithmTag>
class OutlierDetection {};
}  // namespace movetk::outlierdetection
#endif