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

/*! @file CGALTraits.h
 *  @brief  A collection of CGAL traits classes for parameterizing the
 *  CGAL Wrapper Traits class
 *  @authors Aniket Mitra (aniket.mitra@here.com)
 */


#ifndef MOVETK_CGALTRAITS_H
#define MOVETK_CGALTRAITS_H

#include "GeometryInterface.h"
#include "CGALWrapper.h"


namespace movetk_support {

    /*!struct CGALTraits
     * @brief  CGAL traits classes for parameterizing TSL::Wrapper_CGAL_Kernel
     * @tparam NumberType - The number type for parameterizing the CGAL kernel
     * and hence the geometry Interface of Movetk
     * @tparam CoordinateIterType - A random access iterator over a set of
     * Cartesian coordinates where each coordinate is of type \c NumberType
     * @tparam dimensions - The dimesionality of the Cartesian coordinate space
     */
    template<class NumberType, size_t dimensions,
            typename = requires_valid_CGAL_NT<NumberType> >
    struct CGALTraits {
        typedef NumberType NT;
        constexpr static size_t dim = dimensions;
        typedef CGAL::Cartesian_d<NT> GeometryType; // define the geometry type
        typedef CGAL::Min_sphere_of_points_d_traits_d<GeometryType, NT, dimensions> SphereTraits;
        typedef movetk_support::Wrapper_CGAL_Kernel<CGALTraits> Wrapper_CGAL_Geometry; // the traits class
    };

};

#endif //MOVETK_CGALTRAITS_H
