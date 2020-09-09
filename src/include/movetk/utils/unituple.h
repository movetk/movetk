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
// Created by onur on 10/25/18.
//

#ifndef MOVETK_UNITUPLE_H
#define MOVETK_UNITUPLE_H

#include <tuple>

namespace here {

template<size_t, class T>
using T_ = T;

template<class T, size_t... Is>
auto unituple(std::index_sequence<Is...>) { return std::tuple<T_<Is, T>...>{ }; }

/**
 * Generate a tuple of N Ts (uniform tuple).
 * @tparam T
 * @tparam N
 * @return
 */
template<class T, size_t N>
auto unituple() { return unituple<T>(std::make_index_sequence<N>{}); }

}

#endif //MOVETK_UNITUPLE_H
