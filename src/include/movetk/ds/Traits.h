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
// Created by Mitra, Aniket on 2019-08-01.
//

#ifndef MOVETK_DSTRAITS_H
#define MOVETK_DSTRAITS_H

#include <map>
#include "movetk/utils/Iterators.h"

namespace movetk_support {


    struct list_tag;
    struct stack_tag;
    struct trie_tag;
    struct forward_list_tag;

    template<class T,
            template<typename, typename> class Container = std::vector>
    struct StackTraits {
        typedef Container<T, std::allocator<T>> container_type;
        typedef typename container_type::iterator iterator;
        typedef typename container_type::const_iterator const_iterator;
        typedef typename container_type::value_type value_type;
        typedef std::back_inserter<container_type> back_inserter;
    };

    template<class K, class V,
            template<typename, typename> class DataContainer = std::vector,
            template<typename, typename, typename, typename> class LinkContainer = std::map>
    struct TrieTraits {
        typedef K Key;
        typedef DataContainer<V, std::allocator<V>> container_type;
        typedef typename container_type::iterator values_iterator;
        typedef typename container_type::const_iterator values_const_iterator;
        typedef typename container_type::value_type value_type;

        template<class T>
        using links_type = LinkContainer<K, T, std::less<K>, std::allocator<std::pair<const K, T>>>;
        template<class T>
        using links_iterator = typename links_type<T>::iterator;
        template<class T>
        using links_const_iterator = typename links_type<T>::const_iterator;
    };





}
#endif //MOVETK_DSTRAITS_H
