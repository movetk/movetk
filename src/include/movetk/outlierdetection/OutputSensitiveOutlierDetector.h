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

#ifndef MOVETK_OUTLIERDETECTION_OUTPUTSENSITIVEOUTLIERDETECTION_H
#define MOVETK_OUTLIERDETECTION_OUTPUTSENSITIVEOUTLIERDETECTION_H


#include "OutlierDetection.h"
#include "movetk/geo/geo.h"
#include "movetk/io/CartesianProbeTraits.h"
#include "movetk/utils/Requirements.h"

namespace movetk::outlierdetection {
struct output_sensitive_outlier_detector_tag;

/**
 * @brief Optimal outlier detection algorithm for concatenable consistency models, such as a speedbounded
 * model.
 * Based on B. Custers & M. van de Kerkhof & W. Meulemans and B. Speckmann & F. Staals (2019) .
 * Optimal physics-based outlier detection running in O(nk) time, where k is the number of outliers
 * @tparam GeometryKernel
 * @tparam Predicate Predicate to use for determining consistency between two probes.
 * Note that we assume that this predicate satistifies the concatenability property:
 * assuming we could apply the predicate to more than two probes, the following must hold:
 * \f$Predicate(p1,p2) \land Predicate(p2,p3) \iff Predicate(p1,p2,p3)\f$ for consecutive probes
 * \f$p1,p2,p3\f$. 
 * @complexity \f$O(nk)\f$ with \f$n\f$ the number of probes and \f$k\f$ the number of outliers.
 */
template <class GeometryKernel, class Predicate>
class OutlierDetection<GeometryKernel, Predicate, output_sensitive_outlier_detector_tag> {
private:
	using NT = typename GeometryKernel::NT;
	Predicate m_predicate;
	NT m_threshold;

	/**
	 * @brief A node in the consistency linked list
	 * @tparam InputIterator
	 */
	template <class InputIterator>
	struct Node {
	public:
		InputIterator value;         // Pointer to the underlying probe.
		size_t sequence_length = 1;  // Length of the largest consistent trajectory ending in this probe

		using NodePtr = std::shared_ptr<Node>;
		// Pointer to previous probe in Longest Consistent Trajectory, needed for backtracing
		NodePtr prev_consistent;
		NodePtr next;
		NodePtr prev;
	};

	template <class Node>
	class base_iterator
	    : public std::iterator<std::bidirectional_iterator_tag, Node, std::size_t, std::shared_ptr<Node>, Node &> {
	protected:
		std::shared_ptr<Node> it;

	public:
		base_iterator() = default;

		typedef typename base_iterator::reference reference;
		typedef typename base_iterator::pointer pointer;
		typedef typename base_iterator::difference_type difference_type;

		explicit base_iterator(pointer value) : it(value) {}

		pointer base() const { return it; }

		base_iterator &operator=(std::shared_ptr<Node> iter) {
			it = iter;
			return *this;
		}

		pointer operator()() const { return it; }

		reference operator*() const { return *it; }

		pointer operator->() const { return it; }

		bool operator!=(base_iterator const &other) const { return (this->base() != other.base()); }

		bool operator==(base_iterator const &other) const { return (this->base() == other.base()); }
	};

	template <class Node>
	class iterator : public base_iterator<Node> {
	public:
		iterator() = default;
		typedef typename base_iterator<Node>::pointer pointer;

		explicit iterator(pointer value) : base_iterator<Node>(value) {}


		iterator &operator++() {
			this->it = this->base()->next;
			return *this;
		}

		iterator &operator--() {
			this->it = this->base()->prev;
			return *this;
		}

		iterator operator++(int) {
			iterator iter(this->base());
			++(*this);
			return iter;
		}

		iterator operator--(int) {
			iterator iter(this->base());
			--(*this);
			return iter;
		}
	};


	template <class Node>
	class consistency_bactracker : public base_iterator<Node> {
	public:
		consistency_bactracker() = default;
		typedef typename base_iterator<Node>::pointer pointer;

		explicit consistency_bactracker(pointer value) : base_iterator<Node>(value) {}


		consistency_bactracker &operator--() {
			this->it = this->base()->prev_consistent;
			return *this;
		}


		consistency_bactracker operator--(int) {
			consistency_bactracker iter(this->base());
			--(*this);
			return iter;
		}
	};

	template <class InputIterator>
	class ConsistencyLinkedList {
	public:
		using ListNode = Node<InputIterator>;
		using node_iterator = iterator<ListNode>;
		using pointer = typename ListNode::NodePtr;
		using back_iterator = consistency_bactracker<ListNode>;

		ConsistencyLinkedList(InputIterator it) {
			last_ptr = std::make_shared<ListNode>(root);
			last_ptr->value = it;
			iter_first = node_iterator(last_ptr);
			// iter_beyond = iterator(nullptr);
		}

		node_iterator begin() { return iter_first; }

		node_iterator end() { return node_iterator(last_ptr); }

		node_iterator beyond() { return node_iterator(nullptr); }


		void insert(InputIterator it) {
			ListNode node;
			pointer curr_ptr = std::make_shared<ListNode>(node);
			curr_ptr->value = it;
			last_ptr->next = curr_ptr;
			curr_ptr->prev = last_ptr;
			last_ptr = curr_ptr;
		}


		void insert(node_iterator iter, InputIterator it) {
			this->insert(it);
			last_ptr->prev_consistent = iter();
			last_ptr->sequence_length = iter->sequence_length + 1;
		}

		back_iterator last() { return back_iterator(last_ptr); }

	private:
		ListNode root;
		std::shared_ptr<ListNode> last_ptr;
		iterator<ListNode> iter_first, iter_beyond;
	};


public:
	/*!
	 *@param InThreshold
	 */
	OutlierDetection(NT threshold) : m_threshold(threshold), m_predicate(threshold) {}

	/*!
	 *
	 * @tparam InputIterator
	 * @tparam OutputIterator
	 * @param first
	 * @param beyond
	 * @param result
	 */
	template <std::random_access_iterator InputIterator, utils::OutputIterator<InputIterator> OutputIterator>
	void operator()(InputIterator first, InputIterator beyond, OutputIterator result) {
		InputIterator it = first;
		ConsistencyLinkedList<InputIterator> list(it);
		for (; it != beyond; ++it) {
			bool consistent = false;
			auto iter = list.end();
			auto longest_sequence = list.begin();
			std::size_t longest_sequence_length = longest_sequence->sequence_length;
			while (iter != list.beyond()) {
				if (m_predicate(*it, *iter->value)) {
					std::size_t sequence_length = iter->sequence_length;
					if (longest_sequence_length < sequence_length) {
						longest_sequence = iter;
						longest_sequence_length = iter->sequence_length + 1;
					}
					consistent = true;
				}
				iter--;
			}
			if (consistent) {
				list.insert(longest_sequence, it);
			} else {
				list.insert(it);
			}
		}
		auto iter = list.last();
		while (iter != list.beyond()) {
			result = iter->value;
			iter--;
		}
	}
};


}  // namespace movetk::outlierdetection


#endif  // MOVETK_OUTLIERDETECTION_H
