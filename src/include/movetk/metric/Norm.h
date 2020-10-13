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
// Created by Mitra, Aniket on 2019-02-18.
//

#ifndef MOVETK_NORM_H
#define MOVETK_NORM_H

#include <cmath>
#include <numeric>
#include <algorithm>
#include <iostream>
#include "movetk/utils/Asserts.h"

namespace movetk_support
{

    template <class Kernel, std::size_t p>
    class FiniteNorm
    {
    private:
        typename Kernel::NT result;

    public:
        constexpr static size_t P = p;
        FiniteNorm()
        {
            ASSERT_MIN_NORM(p);
        }

        typename Kernel::NT operator()(typename Kernel::MovetkVector &v)
        {
            auto sum_exponent_p = [](typename Kernel::NT sum, typename Kernel::NT coord) -> typename Kernel::NT {
                return std::move(sum) + std::pow(abs(coord), p);
            };
            result = std::accumulate(std::begin(v), std::end(v), 0.0, sum_exponent_p);
            return result;
        }

        typename Kernel::NT operator^(std::size_t exponent)
        {
            typename Kernel::NT n = exponent / static_cast<typename Kernel::NT>(p);
            return std::pow(result, n);
        }
    };

    template <class Kernel>
    class InfinityNorm
    {
    private:
        typename Kernel::NT result;
        static bool abs_compare(typename Kernel::NT coord1, typename Kernel::NT coord2)
        {
            return (std::abs(coord1) < std::abs(coord2));
        };

    public:
        typename Kernel::NT operator()(typename Kernel::MovetkVector &v)
        {
            result = abs(*std::max_element(std::begin(v), std::end(v), abs_compare));
            return result;
        }

        typename Kernel::NT operator^(std::size_t exponent)
        {
            return std::pow(result, exponent);
        }
    };
} // namespace movetk_support
#endif //MOVETK_NORM_H
