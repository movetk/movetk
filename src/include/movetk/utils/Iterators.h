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

/*! @file Iterators.h
 *  @brief  A collection of iterators for movetk
 *  @details A collection of custom iterators for movetk that are
 *  models of std::iterator_traits
 *  @authors Aniket Mitra (aniket.mitra@here.com)
 */

#ifndef MOVETK_ITERATORS_H
#define MOVETK_ITERATORS_H

#include <iterator>
#include <iostream>
#include "movetk/utils/Requirements.h"

/*!
 * @brief the core of movetk
 */
namespace movetk_core {
    /*!
     * @brief an adaptor to std::iterator for for inserting at the end of a container
     * @details unlike the std::back_inserter, ths iterator has a value_type.
     * Increment operator has no effect. The assignment operator extend the size of the container
     * by inserting an element at the end and returns an iterator to the end
     * @tparam Container
     */
    template<class Container>
    class movetk_back_insert_iterator : public std::iterator<std::output_iterator_tag,
            void,
            void,
            void,
            void> {
    protected:
        Container *container;
    public:

        typedef Container container_type;
        typedef typename Container::value_type value_type;
        typedef void difference_type;
        typedef void reference;
        typedef void pointer;

        /*!
         *
         * @param x
         */
        explicit movetk_back_insert_iterator(Container &x) : container(std::addressof(x)) {}

        /*!
         *
         * @param value
         * @return
         */
        movetk_back_insert_iterator &operator=(const typename Container::value_type &value) {
            container->push_back(value);
            return *this;
        }

        movetk_back_insert_iterator &operator-=(const typename Container::value_type &value) {
            container->pop_back();
            container->push_back(value);
            return *this;
        }

        /*!
         *
         * @return
         */
        movetk_back_insert_iterator &operator*() { return *this; }

        /*!
         *
         * @return
         */
        movetk_back_insert_iterator &operator++() { return *this; }

        /*!
         *
         * @return
         */
        movetk_back_insert_iterator operator++(int) { return *this; }

    };

    /*!
 * @brief an adaptor to std::iterator for inserting into a container at a given position
 * @details unlike the std::inserter, ths iterator has a value_type.
 * Increment operator has no effect. The assignment operator extends the size of the container
 * by inserting an element at a given position
 * @tparam Container
 */
    template<class Container>
    class movetk_insert_iterator : public std::iterator<std::output_iterator_tag,
            void,
            void,
            void,
            void> {
    protected:
        Container *container;
        typename Container::iterator iter;
    public:

        typedef Container container_type;
        typedef typename Container::value_type value_type;
        typedef void difference_type;
        typedef void reference;
        typedef void pointer;

        explicit movetk_insert_iterator(Container &x, typename Container::iterator _iter) : container(
                std::addressof(x)),
                                                                                            iter(_iter) {}

        movetk_insert_iterator &operator=(const typename Container::value_type &value) {
            iter = container->insert(iter, value);
            ++iter;
            return *this;
        }

        movetk_insert_iterator &operator*() { return *this; }

        movetk_insert_iterator &operator++() { return *this; }

        movetk_insert_iterator operator++(int) { return *this; }
    };


    template<class T>
    class movetk_basic_iterator : public std::iterator<std::random_access_iterator_tag,
            T,
            std::size_t,
            T *,
            T &> {
    protected:
        T *it;
    public:

        typedef typename movetk_basic_iterator::reference reference;
        typedef typename movetk_basic_iterator::pointer pointer;
        typedef typename movetk_basic_iterator::difference_type difference_type;


        explicit movetk_basic_iterator(pointer value) : it(value) {}

        pointer base() const { return it; }

        movetk_basic_iterator &operator=(const T *iter) {
            it = iter;
            return *this;
        }

        reference operator*() const { return *it; }

        pointer operator->() const { return it; }

        movetk_basic_iterator &operator++() {
            it++;
            return *this;
        }

        movetk_basic_iterator operator+(difference_type d) {
            movetk_basic_iterator iter(this->base() + d);
            return iter;
        }

        movetk_basic_iterator operator++(int) {
            movetk_basic_iterator iter(this->base());
            ++(*this);
            return iter;
        }

        bool operator==(movetk_basic_iterator const &other) const {
            return (this->base() == other.base());
        }

        bool operator!=(movetk_basic_iterator const &other) const {
            return (this->base() != other.base());
        }

        auto operator-(movetk_basic_iterator const &other) const {
            return (this->base() - other.base());
        }
    };


    template<class Container,
            typename = movetk_core::requires_random_access_iterator<
                    typename Container::value_type::const_iterator>>
    class movetk_grid_iterator : public std::iterator<std::random_access_iterator_tag,
            Container,
            std::size_t,
            typename Container::const_iterator,
            typename Container::reference> {
    protected:
        typename Container::const_iterator rit;
        typename Container::value_type::const_iterator cit;
    public:

        typedef typename movetk_grid_iterator::reference reference;
        typedef typename movetk_grid_iterator::pointer pointer;
        typedef typename movetk_grid_iterator::difference_type difference_type;

        explicit movetk_grid_iterator(pointer value) : rit(value) {
            cit = rit->cbegin();
        }

        pointer base() const { return rit; }

        movetk_grid_iterator &operator++() {
            if (std::distance(rit->cbegin(), rit->cend()) == 0){
                rit++;
                cit = rit->cbegin();
                return *this;
            }
            if ( cit == (rit->cend() - 1) ) {
                rit++;
                cit = rit->cbegin();
            }
            else
                cit++;
            return *this;
        }


        movetk_grid_iterator operator++(int) {
            movetk_grid_iterator iter(this->base());
            ++(*this);
            return iter;
        }

        movetk_grid_iterator operator+(difference_type d) {
            movetk_grid_iterator iter(this->base());
            difference_type idx = 1;
            while (idx < ( d + 1)){
                iter++;
                idx++;
            }
            return iter;

        }

        typename Container::value_type::value_type operator*() const {return *cit;}

        typename Container::value_type::const_iterator operator->() const { return cit; }

        bool operator==(movetk_grid_iterator const &other) {
            return (this->base() == other.base());
        }

        bool operator!=(movetk_grid_iterator const &other) {
            return (this->base() != other.base());
        }

        std::size_t operator-(movetk_grid_iterator const &other) const {
            auto it = other.base();
            std::size_t count = 0;
            while (it != this->base()){
                auto jit = it->cbegin();
                while (jit != it->cend()){
                    count++;jit++;
                }
                it++;
            }
            return count;
        }

    };
}
#endif //MOVETK_ITERATORS_H
