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

/*! @file SegmentationTraits.h
 *  @brief  A collection of traits classes required for segmentation
 *  @authors Aniket Mitra (aniket.mitra@here.com)
 */

#ifndef MOVETK_SEGMENTATIONTRAITS_H
#define MOVETK_SEGMENTATIONTRAITS_H

#include "MonotoneSegmentation.h"
#include "SegmentationPredicates.h"

namespace movetk::segmentation {
/**
 * @brief Traits for segmentation algorithms
 * @tparam NT The number type to use
 * @tparam MovetkGeometryKernel The kernel to use
 * @tparam dimensions The dimensions of the kernel to use
*/
template <class NT, class MovetkGeometryKernel, size_t dimensions>
struct SegmentationTraits {
	// type definitions required for algorithm

	using Point = typename MovetkGeometryKernel::MovetkPoint;
	using MebCriteria = TEST<TestCriteria::meb, MovetkGeometryKernel>;
	using LocationSegmentation = MonotoneSegmentation<MovetkGeometryKernel, MebCriteria>;
	using DiffCriteria = TEST<TestCriteria::difference, MovetkGeometryKernel>;
	using TSSegmentation = MonotoneSegmentation<MovetkGeometryKernel, DiffCriteria>;
	using SpeedSegmentation = MonotoneSegmentation<MovetkGeometryKernel, DiffCriteria>;
	using RangeCriteria = TEST<TestCriteria::range, MovetkGeometryKernel>;
	using HeadingSegmentation = MonotoneSegmentation<MovetkGeometryKernel, RangeCriteria>;
};

}  // namespace movetk::segmentation

#endif  // MOVETK_SEGMENTATIONTRAITS_H
