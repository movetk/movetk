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

#include "SegmentationPredicates.h"
#include "Segmentation.h"

namespace movetk::algo {
    template<class NT, class MovetkGeometryKernel, size_t dimensions>
    struct SegmentationTraits {
        // type definitions required for algorithm

        typedef typename MovetkGeometryKernel::MovetkPoint Point;
        typedef movetk::algo::TEST<TestCriteria::meb, MovetkGeometryKernel> MebCriteria;
        typedef movetk::algo::MonotoneSegmentation<MovetkGeometryKernel, MebCriteria> LocationSegmentation;
        typedef movetk::algo::TEST<TestCriteria::difference, MovetkGeometryKernel> DiffCriteria;
        typedef movetk::algo::MonotoneSegmentation<MovetkGeometryKernel, DiffCriteria> SpeedSegmentation, TSSegmentation;
        typedef movetk::algo::TEST<TestCriteria::range, MovetkGeometryKernel> RangeCriteria;
        typedef movetk::algo::MonotoneSegmentation<MovetkGeometryKernel, RangeCriteria> HeadingSegmentation;
    };

}

#endif //MOVETK_SEGMENTATIONTRAITS_H
