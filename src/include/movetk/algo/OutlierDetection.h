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

//
// Created by Mees van de Kerkhof (m.a.vandekerkhof@uu.nl),
// Bram Custers (b.a.custers@tue.nl),
// Kevin Verbeek (k.a.b.verbeek@tue.nl)
// Modified by Aniket Mitra (aniket.mitra@here.com)
//

#ifndef MOVETK_OUTLIERDETECTION_H
#define MOVETK_OUTLIERDETECTION_H


#include "movetk/utils/Requirements.h"
#include "movetk/algo/AlgorithmTraits.h"
#include "movetk/CartesianProbeTraits.h"
#include "movetk/geo/geo.h"

using namespace std;

namespace movetk_algorithms
{

    // based on B. Custers & M. van de Kerkhof & W. Meulemans and B. Speckmann & F. Staals (2019) .
    // Maximum Physically Consistent Trajectories
    // published in SIGSPATIAL 2019

    template<class AlgorithmTag,
            class Test,
            class T>
    class OutlierDetection{

    };

	//Physics-based outlier detection running in O(n) time
    template<class T, class Test>
    class OutlierDetection<movetk_algorithms::greedy_outlier_detector_tag,
            Test, T>{
    private:
        typedef typename T::NT NT;
        typedef Test TEST;
        TEST test;
        NT threshold;

    public:

        /*!
        *@param InThreshold
        */
        OutlierDetection(NT InThreshold) {
            threshold = InThreshold;
            test = TEST(InThreshold);
        }

        /*!
         *
         * @tparam TrajectoryIterator
         * @tparam OutputIterator
         * @param first
         * @param beyond
         * @param result
         */
        template<class InputIterator, class OutputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_output_iterator<OutputIterator>,
                typename = movetk_core::requires_equality<typename InputIterator::value_type,
                        typename OutputIterator::value_type::value_type>>
        void operator()(InputIterator first, InputIterator beyond, OutputIterator result) {
            auto it = first;
            *result = it;
            auto prev = *it;
            it++;
            for (; it != beyond; it++) {
                if (test(prev, *it)) {
                    *result = it;
                    prev = *it;
                }
            }
        } //operator()
    };

	//Physics-based outlier detection running in O(nk) time, where k is the number of outliers
    template <class T, class Test>
    class OutlierDetection<movetk_algorithms::smart_greedy_outlier_detector_tag,
            Test, T> {
    private:
        typedef typename T::NT NT;
        typedef Test TEST;
        TEST test;
        NT threshold;
    public:

        /*!
        *@param InThreshold
        */
        OutlierDetection(NT InThreshold) { threshold = InThreshold; test = TEST(InThreshold); };

        /*!
         *
         * @tparam InputIterator
         * @tparam OutputIterator
         * @param first
         * @param beyond
         * @param result
         */
        template<class InputIterator, class Container,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_equality<
                        InputIterator,
                        typename Container::value_type::value_type> >
        typename Container::const_iterator operator()(InputIterator first, InputIterator beyond, Container& sequences){
            auto it = first;
            movetk_core::movetk_back_insert_iterator bit(sequences);
            for (; it != beyond; it++)
            {
                bool extend_subsequence = false;
                for (auto sit = std::begin(sequences); sit != std::end(sequences); sit++)
                {
                    InputIterator prev = *(sit->cend() - 1);
                    if (test(*prev, *it))
                    {
                        movetk_core::movetk_back_insert_iterator node_bit(*sit);
                        node_bit = it;
                        extend_subsequence = true;
                    }
                }
                if (!extend_subsequence)
                {
                    typename Container::value_type node;
                    movetk_core::movetk_back_insert_iterator node_bit(node);
                    node_bit = it;
                    bit =  node;
                }
            }

            std::sort(std::begin(sequences), std::end(sequences), [](auto a, auto b){
                std::size_t size_a = std::distance(std::begin(a), std::end(a));
                std::size_t size_b = std::distance(std::begin(b), std::end(b));
                return size_a > size_b;
            });

            auto sit = std::cbegin(sequences);
            std::size_t max_size = sit->size();

            auto eit = std::remove_if(std::begin(sequences), std::end(sequences), [max_size](auto a){
                std::size_t size_a = std::distance(std::begin(a), std::end(a));
               return  size_a != max_size;
            });

            return eit;

        } //operator()
    };


	//Physics-based outlier detection running in O(n) time
    template <class T, class Test>
    class OutlierDetection<movetk_algorithms::zheng_outlier_detector_tag,
            Test, T>{
    private:
        typedef typename T::NT NT;
        typedef Test TEST;
        TEST test;
        NT threshold, min_seg_size;

    public:

        /*!
        *@param InThreshold
        */
        OutlierDetection(NT InThreshold, NT InMinSegSize) { threshold = InThreshold; test = TEST(InThreshold); min_seg_size = InMinSegSize; };

        /*!
         *
         * @tparam InputIterator
         * @tparam OutputIterator
         * @param first
         * @param beyond
         * @param result
         */
        template <class InputIterator, class OutputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_output_iterator<OutputIterator>,
                typename = movetk_core::requires_equality<typename InputIterator::value_type,
                        typename OutputIterator::value_type::value_type >>
        void operator()(InputIterator first, InputIterator beyond, OutputIterator result)
        {
            vector<size_t> segments;
            size_t segment_size = 1;
            auto it = first;
            auto prev = *it;
            it++;
            for (; it != beyond; it++)
            {
                if (test(prev, *it))
                    segment_size++;
                else
                {
                    segments.push_back(segment_size);
                    segment_size = 1;
                }
                prev = *it;
            }
            segments.push_back(segment_size);
            it = first;
            for (auto segment : segments)
            {
                if (segment >= min_seg_size)
                    for (size_t i = 0; i < segment; i++)
                    {
                        *result = it;
                        it++;
                    }
                else it = next(it, segment);
            }
        } //operator()
    };

	//Optimal physics-based outlier detection running in O(nk) time, where k is the number of outliers 
    template <class T, class Test>
    class OutlierDetection<movetk_algorithms::output_sensitive_outlier_detector_tag,
            Test, T>{
    private:
        typedef typename T::NT NT;
        typedef Test TEST;
        TEST test;
        NT threshold;

        template <class InputIterator>
        struct Node {
        public:
            InputIterator value; //Pointer to the underlying probe.
            //size_t k; //Length of the largest consistent trajectory ending in this probe
            size_t sequence_length = 1; //Length of the largest consistent trajectory ending in this probe

            //shared_ptr<LinkedListNode<InputIterator>> prev_probe; //Pointer to previous probe in Longest Consistent Trajectory, needed for backtracing
            shared_ptr<Node> prev_consistent = nullptr; //Pointer to previous probe in Longest Consistent Trajectory, needed for backtracing
            shared_ptr<Node> next = nullptr;
            shared_ptr<Node> prev = nullptr;
        };

        template <class Node>
        class __base_iterator : public std::iterator <std::bidirectional_iterator_tag,
                Node,
                std::size_t,
                std::shared_ptr<Node>,
                Node &>{
        protected:
            std::shared_ptr<Node> it;
        public:
            __base_iterator() = default;

            typedef typename __base_iterator::reference reference;
            typedef typename __base_iterator::pointer pointer;
            typedef typename __base_iterator::difference_type  difference_type;

            explicit __base_iterator(pointer value):  it(value) {}

            pointer base() const {return it;}

            __base_iterator& operator=(std::shared_ptr<Node> iter){
                it = iter;
                return *this;
            }

            pointer operator()() const {return it;}

            reference operator*() const { return *it; }

            pointer operator->() const { return it; }

            bool operator!=(__base_iterator const& other) const {
                return (this->base() != other.base());
            }

            bool operator==(__base_iterator const& other) const{
                return (this->base() == other.base());
            }

        };

        template <class Node>
        class iterator : public __base_iterator<Node>{

        public:
            iterator() = default;
            typedef typename __base_iterator<Node>::pointer pointer;

            explicit iterator(pointer value): __base_iterator<Node>(value) {}


            iterator &operator++() {
                this->it = this->base()->next;
                return *this;
            }

            iterator &operator--(){
                this->it = this->base()->prev;
                return *this;
            }

            iterator operator++(int){
                iterator iter(this->base());
                ++(*this);
                return iter;
            }

            iterator operator--(int){
                iterator iter(this->base());
                --(*this);
                return iter;
            }

        };


        template <class Node>
        class consistency_bactracker : public __base_iterator<Node>{

        public:
            consistency_bactracker() = default;
            typedef typename __base_iterator<Node>::pointer pointer;

            explicit consistency_bactracker(pointer value): __base_iterator<Node>(value) {}


            consistency_bactracker &operator--(){
                this->it = this->base()->prev_consistent;
                return *this;
            }


            consistency_bactracker operator--(int){
                consistency_bactracker iter(this->base());
                --(*this);
                return iter;
            }

        };

        template <class InputIterator>
        class Container{
        private:
            typedef Node<InputIterator> _Node;

            _Node root;
            std::shared_ptr<_Node> last_ptr;
            iterator<_Node> iter_first, iter_beyond;

        public:
            typedef iterator<_Node> node_iterator;
            typedef std::shared_ptr<_Node> pointer;
            typedef consistency_bactracker<_Node> back_iterator;

            Container(InputIterator it){
                last_ptr = std::make_shared<_Node>(root);
                last_ptr->value = it;
                iter_first = node_iterator(last_ptr);
                //iter_beyond = iterator(nullptr);
            }

            node_iterator begin(){
                return iter_first;
            }

            node_iterator end(){
                return node_iterator(last_ptr);
            }

            node_iterator beyond(){
                return  node_iterator(nullptr);
            }


            void insert(InputIterator it){
                _Node node;
                pointer curr_ptr = std::make_shared<_Node>(node);
                curr_ptr->value = it;
                last_ptr->next = curr_ptr;
                curr_ptr->prev = last_ptr;
                last_ptr = curr_ptr;
            }


            void insert(node_iterator iter, InputIterator it){
                this->insert(it);
                last_ptr->prev_consistent = iter();
                last_ptr->sequence_length = iter->sequence_length + 1;
            }

            back_iterator last(){
                return back_iterator(last_ptr);
            }
        };


    public:
        /*!
        *@param InThreshold
        */
        OutlierDetection(NT InThreshold) { threshold = InThreshold; test = TEST(InThreshold);};

        /*!
         *
         * @tparam InputIterator
         * @tparam OutputIterator
         * @param first
         * @param beyond
         * @param result
         */
        template <class InputIterator, class OutputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_output_iterator<OutputIterator>,
                typename = movetk_core::requires_equality<typename InputIterator::value_type,
                        typename OutputIterator::value_type::value_type >>
        void operator()(InputIterator first, InputIterator beyond, OutputIterator result)
        {
            InputIterator it = first;
            Container<InputIterator> list(it);
            while(++it != beyond){
                bool consistent = false;
                auto iter = list.end();
                auto longest_sequence = list.begin();
                std::size_t longest_sequence_length = longest_sequence->sequence_length;
                while (iter != list.beyond()){
                    if (test(*it, *iter->value)){
                        std::size_t sequence_length = iter->sequence_length;
                        if ( longest_sequence_length < sequence_length ){
                            longest_sequence = iter;
                            longest_sequence_length = iter->sequence_length + 1;
                        }
                        consistent = true;
                    }
                    iter--;
                }
                if (consistent){
                    list.insert(longest_sequence, it);
                }
                if (!consistent){
                    list.insert(it);
                }
            }
            auto iter = list.last();
            while (iter != list.beyond()){
                result = iter->value;
                iter--;
            }
        }
    };


}


#endif //MOVETK_OUTLIERDETECTION_H
