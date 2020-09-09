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

/*! @file Tree.h
 *  @brief  Trees
 *  @authors Aniket Mitra (aniket.mitra@here.com)
 */

#ifndef MOVETK_TREES_H
#define MOVETK_TREES_H

#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <array>
#include <iterator>
#include <algorithm>
#include "movetk/utils/Asserts.h"
#include "movetk/utils/Requirements.h"

namespace movetk_support {
    /*!
     *
     * @param v
     * @return
     */
    char f(char &v) {
        return v;
    }

    /*!
     *
     * @param v
     * @return
     */
    const char f(const char &v) {
        return v;
    }

    /*!
     *
     * @param v
     * @return
     */
    std::string f(int &v) {
        return std::to_string(v);
    }

    /*!
     *
     * @tparam K
     * @tparam V
     */
    template<class K, class V>
    class TrieNode {
    private:
        K key;
        std::unique_ptr<V> ValuePtr;
        typedef std::unique_ptr<TrieNode> NodePtr;
        std::map<K, NodePtr> links;
        typename std::map<K, NodePtr>::iterator it;

        TrieNode() = default;

    public:
        typedef NodePtr &reference;
        typedef typename V::Iterator ValuesIterator;
        typedef typename std::map<K, NodePtr>::iterator iterator;
        typedef typename std::map<K, NodePtr>::const_iterator const_iterator;

        /*!
         *
         * @param InKey
         */
        TrieNode(K &InKey) : key(InKey) {}

        /*!
         *
         * @param nd
         */
        void operator()(NodePtr nd) {
            links[(*nd)()] = std::move(nd);
        }


        /*!
         *
         * @param key
         * @return
         */
        bool operator()(K &key) {
            it = links.find(key);
            if (it != links.end())
                return true;
            else
                return false;
        }

        /*!
         *
         * @param nd
         * @return
         */
        bool operator<(reference nd) {
            if (this->key < (*nd)())
                return true;
            else
                return false;
        }

        /*!
         *
         * @param nd
         * @return
         */
        bool operator>(reference nd) {
            if (this->key > (*nd)())
                return true;
            else
                return false;
        }

        /*!
         *
         * @param nd
         * @return
         */
        bool operator==(reference nd) {
            if (this->key < (*nd)())
                return true;
            else
                return false;
        }

        /*!
         *
         * @return
         */
        K operator()() {
            return key;
        }

        /*!
         *
         * @param key
         * @return
         */
        NodePtr &operator[](K &key) {
            return links[key];
        }

        /*!
         *
         * @param iter
         * @return
         */
        NodePtr &operator()(iterator iter) {
            return iter->second;
        }

        /*!
         *
         * @return
         */
        iterator begin() {
            return links.begin();
        }

        /*!
         *
         * @return
         */
        iterator end() {
            return links.end();
        }

        /*!
         *
         * @tparam T
         * @param id
         * @param v
         */
        template<class T>
        void insert_value(std::string &id, T &v) {
            if (ValuePtr)
                (*ValuePtr)(v);
            else {
                ValuePtr = std::make_unique<V>(id);
                (*ValuePtr)(v);
            }
        }

        /*!
         *
         * @return
         */
        ValuesIterator v_begin() {
            assert(ValuePtr);
            return ValuePtr->begin();
        }

        /*!
         *
         * @return
         */
        ValuesIterator v_end() {
            assert(ValuePtr);
            return ValuePtr->end();
        }

        /*!
         *
         * @return
         */
        std::string &v_id() {
            assert(ValuePtr);
            return (*ValuePtr)();
        }

        ~TrieNode() {
            if (ValuePtr)
                ValuePtr.release();
        }

    };


    /*!
     *
     * @tparam K
     * @tparam V
     */
    template<class K, class V>
    class BinaryNode {
    private:
        K key;
        std::unique_ptr<V> ValuePtr;
        typedef std::unique_ptr<BinaryNode> NodePtr;
        std::array<NodePtr, 2> links = {nullptr};
        typename std::array<NodePtr, 2>::iterator it;
        NodePtr NullNode = nullptr;
        BinaryNode() = default;

        /*!
         *
         * @return
         */
        bool is_allocated() {
            return !std::all_of(std::begin(links), std::end(links),
                                [&](NodePtr &nd) { return nd == nullptr; });
        }

    public:
        typedef NodePtr &reference;
        typedef typename V::Iterator ValuesIterator;
        typedef typename std::array<NodePtr, 2>::const_iterator const_iterator;
        typedef typename std::array<NodePtr, 2>::iterator iterator;

        /*!
         *
         * @param InKey
         */
        BinaryNode(K &InKey) : key(InKey) {}

        /*!
         *
         * @param nd
         */
        void operator()(NodePtr nd) {
            if (!this->is_allocated()) {
                links[0] = std::move(nd);
            } else if (links.size() == 1) {
                if (links[0] > nd) {
                    links[1] = std::move(links[0]);
                    links[0] = std::move(nd);
                } else
                    links[1] = std::move(nd);
            }
        }


        /*!
         *
         * @param key
         * @return
         */
        bool operator()(K &key) {

            if (!this->is_allocated())
                return false;


            for (it = std::begin(links); it != std::end(links); it++) {
                if ((**it)() == key)
                    return true;
            }


            return false;
        }

        /*!
         *
         * @param nd
         * @return
         */
        bool operator<(reference nd) {
            if (this->key < (*nd)())
                return true;
            else
                return false;
        }

        /*!
         *
         * @param nd
         * @return
         */
        bool operator>(reference nd) {
            if (this->key > (*nd)())
                return true;
            else
                return false;
        }

        /*!
         *
         * @param nd
         * @return
         */
        bool operator==(reference nd) {
            if (this->key < (*nd)())
                return true;
            else
                return false;
        }

        /*!
         *
         * @return
         */
        K operator()() {
            return key;
        }

        /*!
         *
         * @param key
         * @return
         */
        NodePtr &operator[](K &key) {
            assert(this->is_allocated());
            for (it = std::begin(links); it != std::end(links); it++) {
                if ((**it)() == key)
                    return *it;
            }
            return NullNode;
        }

        /*!
         *
         * @param iter
         * @return
         */
        NodePtr &operator()(iterator iter) {
            return iter->second;
        }

        /*!
         *
         * @return
         */
        iterator begin() {
            return links.begin();
        }

        /*!
         *
         * @return
         */
        iterator end() {
            return links.end();
        }

        /*!
         *
         * @tparam T
         * @param id
         * @param v
         */
        template<class T>
        void insert_value(std::string &id, T &v) {
            if (ValuePtr)
                (*ValuePtr)(v);
            else {
                ValuePtr = std::make_unique<V>(id);
                (*ValuePtr)(v);
            }
        }

        /*!
         *
         * @return
         */
        ValuesIterator v_begin() {
            assert(ValuePtr);
            return ValuePtr->begin();
        }

        /*!
         *
         * @return
         */
        ValuesIterator v_end() {
            assert(ValuePtr);
            return ValuePtr->end();
        }

        /*!
         *
         * @return
         */
        std::string &v_id() {
            assert(ValuePtr);
            return (*ValuePtr)();
        }

        ~BinaryNode() {
            if (ValuePtr)
                ValuePtr.release();
        }

    };


    /*!
     *
     * @tparam NodeType
     */
    template<class NodeType>
    class Tree {
    private:
        //TODO Add Requirements for NodeType
        typedef typename NodeType::reference reference;
        std::unique_ptr<NodeType> root;
        size_t matched_branch_length_ = 0;
        std::string identifier = "";

        /*!
         *
         * @tparam InputIterator
         * @tparam ValueType
         * @param node
         * @param current
         * @param beyond
         * @param val
         */
        template<class InputIterator, class ValueType,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_arithmetic<
                        typename InputIterator::value_type >>
        void insert(reference node, InputIterator current, InputIterator beyond, ValueType &val) {
            //TODO Add Requirements for ValueType
            //ASSERT_RANDOM_ACCESS_ITERATOR(InputIterator);
            if (current == beyond) {
                node->insert_value(identifier, val);
                return;
            } else {
                if (!(*node)(*current)) {
                    (*node)(std::make_unique<NodeType>(*current));
                }
                InputIterator tmp = current;
                identifier = identifier + f(*current);
                current++;
                this->insert((*node)[*tmp], current, beyond, val);
            }
        }

        /*!
         *
         * @tparam InputIterator
         * @param node
         * @param current
         * @param beyond
         * @return
         */
        template<class InputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_arithmetic<
                        typename InputIterator::value_type >>
        reference find(reference node, InputIterator current, InputIterator beyond) {
            //ASSERT_RANDOM_ACCESS_ITERATOR(InputIterator);
            if (current == beyond)
                return node;
            if (!(*node)(*current))
                return node;
            else {
                matched_branch_length_++;
                InputIterator tmp = current;
                current++;
                return this->find((*node)[*tmp], current, beyond);
            }
        }

        /*!
         *
         * @tparam OutputIterator
         * @param node
         * @param iter
         */
        template<class OutputIterator,
                typename = movetk_core::requires_output_iterator<OutputIterator>,
                typename = movetk_core::requires_pair<
                typename OutputIterator::value_type >>
        void find(reference node, OutputIterator iter) {
            //ASSERT_OUTPUT_ITERATOR(OutputIterator);
            //ASSERT_IS_PAIR(OutputIterator);
            if (node->begin() == node->end()) {
                size_t size = std::distance(node->v_begin(), node->v_end());
                *iter = std::make_pair(node->v_id(), size);
                return;
            } else {
                for (auto it = node->begin(); it != node->end(); it++) {
                    this->find((*node)(it), iter);
                }
            }
        }

    public:

        reference get_root(){
            return root;
        }

        /*!
         *
         * @param nd
         */
        Tree(std::unique_ptr<NodeType> nd) {
            root = std::move(nd);
        }

        Tree() = default;

        Tree& operator=(Tree& rhs){
            this->root = std::move(rhs.get_root());
            return *this;
        }


        /*!
         *
         * @tparam InputIterator
         * @tparam ValueType
         * @param first
         * @param beyond
         * @param val
         */
        template<class InputIterator, class ValueType,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_arithmetic<
                        typename InputIterator::value_type >>
        void insert(InputIterator first, InputIterator beyond, ValueType &val) {
            //ASSERT_RANDOM_ACCESS_ITERATOR(InputIterator);
            identifier = "";
            this->insert(root, first, beyond, val);
        }

        /*!
         *
         * @tparam InputIterator
         * @param first
         * @param beyond
         * @return
         */
        template<class InputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_arithmetic<
                        typename InputIterator::value_type >>
        reference find(InputIterator first, InputIterator beyond) {
            //ASSERT_RANDOM_ACCESS_ITERATOR(InputIterator);
            matched_branch_length_ = 0;
            return this->find(root, first, beyond);
        }

        /*!
         *
         * @tparam OutputIterator
         * @param iter
         */
        template<class OutputIterator,
                typename = movetk_core::requires_output_iterator<OutputIterator>,
                typename = movetk_core::requires_pair<
                        typename OutputIterator::value_type >>
        void find(OutputIterator iter) {
            //ASSERT_OUTPUT_ITERATOR(OutputIterator);
            //ASSERT_IS_PAIR(OutputIterator);
            this->find(root, iter);
        }

        /*!
         *
         * @return
         */
        size_t get_match_size() {
            return matched_branch_length_;
        }

        ~Tree() {
            root.release();
        }
    };




    /*  template <class Tag, class T>
    class Node;


 *//*   template <class T>
    class Node<movetk_support::trie_tag, T> {
    private:
        typename T::K key;
        std::unique_ptr<typename T::container_type> ValuePtr;
        typedef std::unique_ptr<Node> NodePtr;
        typename T::template links_type<NodePtr> links;
        typename T::template links_type<NodePtr>::links_iterator it;
        Node() = default;

    public:
        typedef NodePtr &reference;
        typedef typename T::values_iterator values_iterator;
        typedef typename T::template Links<NodePtr>::links_iterator iterator;
        typedef typename T::template Links<NodePtr>::links_const_iterator const_iterator;


        Node(typename T::K &InKey) : key(InKey) {}


        void operator()(NodePtr nd) {
            links[(*nd)()] = std::move(nd);
        }


        bool operator()(typename T::K &key) {
            it = links.find(key);
            if (it != links.end())
                return true;
            else
                return false;
        }


        bool operator<(reference nd) {
            if (this->key < (*nd)())
                return true;
            else
                return false;
        }


        bool operator>(reference nd) {
            if (this->key > (*nd)())
                return true;
            else
                return false;
        }


        bool operator==(reference nd) {
            if (this->key < (*nd)())
                return true;
            else
                return false;
        }


        typename T::K operator()() {
            return key;
        }


        NodePtr &operator[](typename T::K &key) {
            return links[key];
        }


        NodePtr &operator()(iterator iter) {
            return iter->second;
        }


        iterator begin() {
            return links.begin();
        }


        iterator end() {
            return links.end();
        }


        void insert_value(typename T::value_type &id,
                          typename T::value_type  &v) {
            if (ValuePtr)
                (*ValuePtr)(v);
            else {
                ValuePtr = std::make_unique<typename T::container_type>(id);
                (*ValuePtr)(v);
            }
        }


        values_iterator v_begin() {
            assert(ValuePtr);
            return ValuePtr->begin();
        }


        values_iterator v_end() {
            assert(ValuePtr);
            return ValuePtr->end();
        }


        typename T::value_type &v_id() {
            assert(ValuePtr);
            return (*ValuePtr)();
        }

        ~Node() {
            if (ValuePtr)
                ValuePtr.release();
        }

    };

*//*

    template<class T>
    class Node<movetk_support::list_tag, T>{
    private:
        typename T::value_type value;
        typedef std::shared_ptr<Node> NodePtr;
        NodePtr prev, next;
        Node() = default;
    public:
       Node(T& input): value(input)
       {}

       Node(T&& input): value(input)
       {}

       Node(T input): value(input)
       {}

       void operator()(NodePtr& nd, std::size_t idx){
           assert(idx <= 1 );
           if (idx == 0)
               prev = nd;
           else if (idx == 1)
               next = nd;
       }

        NodePtr& operator[](std::size_t idx){
            assert(idx <= 1 );
            if (idx == 0)
                return prev;
            else if (idx == 1)
                return next;
       }

    };
*/
};

#endif //MOVETK_TREES_H
