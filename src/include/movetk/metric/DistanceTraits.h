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
// Created by Mitra, Aniket on 2020-10-13.
//

#ifndef DISTANCETRAITS_H
#define DISTANCETRAITS_H
#include "Distances.h"

namespace movetk::metric
{

    /**
     * @brief A traits class for the squared distance algorithm
     * @tparam GeometryTraits 
     * @tparam Norm 
     * @tparam T 
     */
    template <class GeometryTraits, class Norm, class T>
    struct squared_distance_algorithm
    {
        typedef T square_distance;
    };

    /**
     * @brief Specialization of the traits class to use the MoveTK 
     * distance algorithm if not available with the backend
     * @tparam GeometryTraits 
     * @tparam Norm 
     */
    template <class GeometryTraits, class Norm>
    struct squared_distance_algorithm<GeometryTraits, Norm, void>
    {
        typedef squared_distance_d<GeometryTraits, Norm> square_distance;
    };

    /**
     * @brief  A traits class for the discrete Hausdorff distance algorithm
     * @tparam GeometryTraits 
     * @tparam Norm 
     * @tparam T 
     */
    template <class GeometryTraits, class Norm, class T>
    struct discrete_hausdorff_distance_algorithm
    {
        typedef T discrete_hausdorff_distance;
    };

    /**
     * @brief Specialization of the traits class to use the MoveTK 
     * discrete Hausdorff distance algorithm if not available with the backend
     * @tparam GeometryTraits 
     * @tparam Norm 
     */
    template <class GeometryTraits, class Norm>
    struct discrete_hausdorff_distance_algorithm<GeometryTraits, Norm, void>
    {
        typedef Discrete_Hausdorff<GeometryTraits, Norm> discrete_hausdorff_distance;
    };

    /**
     * @brief  A traits class for the discrete Frechet distance algorithm
     * @tparam GeometryTraits 
     * @tparam Norm 
     * @tparam T 
     */
    template <class GeometryTraits, class Norm, class T>
    struct discrete_frechet_distance_algorithm
    {
        typedef T discrete_frechet_distance;
    };

    /**
     * @brief Specialization of the traits class to use the MoveTK 
     *  discrete Frechet distance algorithm if not available with the backend
     * @tparam GeometryTraits 
     * @tparam Norm 
     */
    template <class GeometryTraits, class Norm>
    struct discrete_frechet_distance_algorithm<GeometryTraits, Norm, void>
    {
        typedef Discrete_Frechet<GeometryTraits, Norm> discrete_frechet_distance;
    };

} // namespace movetk::utils
#endif // DISTANCETRAITS_H
