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

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "movetk/utils/Requirements.h"

namespace movetk::ds {
/*!
 *
 * @param v
 * @return
 */
char f(char& v) {
	return v;
}

/*!
 *
 * @param v
 * @return
 */
const char f(const char& v) {
	return v;
}

/*!
 *
 * @param v
 * @return
 */
std::string f(int& v) {
	return std::to_string(v);
}

/*!
 *
 * @tparam K
 * @tparam V
 */
template <class K, class V>
class TrieNode {
private:
	K key;
	std::unique_ptr<V> ValuePtr;
	typedef std::unique_ptr<TrieNode> NodePtr;
	std::map<K, NodePtr> links;
	typename std::map<K, NodePtr>::iterator it;

	TrieNode() = default;

public:
	typedef NodePtr& reference;
	typedef typename V::Iterator ValuesIterator;
	typedef typename std::map<K, NodePtr>::iterator iterator;
	typedef typename std::map<K, NodePtr>::const_iterator const_iterator;

	/*!
	 *
	 * @param InKey
	 */
	TrieNode(K& InKey) : key(InKey) {}

	/*!
	 *
	 * @param nd
	 */
	void operator()(NodePtr nd) { links[(*nd)()] = std::move(nd); }


	/*!
	 *
	 * @param key
	 * @return
	 */
	bool operator()(K& key) {
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
	K operator()() { return key; }

	/*!
	 *
	 * @param key
	 * @return
	 */
	NodePtr& operator[](K& key) { return links[key]; }

	/*!
	 *
	 * @param iter
	 * @return
	 */
	NodePtr& operator()(iterator iter) { return iter->second; }

	/*!
	 *
	 * @return
	 */
	iterator begin() { return links.begin(); }

	/*!
	 *
	 * @return
	 */
	iterator end() { return links.end(); }

	/*!
	 *
	 * @tparam T
	 * @param id
	 * @param v
	 */
	template <class T>
	void insert_value(std::string& id, T& v) {
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
	const std::string& v_id() const {
		assert(ValuePtr);
		return (*ValuePtr)();
	}

	~TrieNode() {}
};


/*!
 *
 * @tparam K
 * @tparam V
 */
template <class K, class V>
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
		return !std::all_of(std::begin(links), std::end(links), [&](NodePtr& nd) { return nd == nullptr; });
	}

public:
	typedef NodePtr& reference;
	typedef typename V::Iterator ValuesIterator;
	typedef typename std::array<NodePtr, 2>::const_iterator const_iterator;
	typedef typename std::array<NodePtr, 2>::iterator iterator;

	/*!
	 *
	 * @param InKey
	 */
	BinaryNode(K& InKey) : key(InKey) {}

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
	bool operator()(K& key) {
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
	K operator()() { return key; }

	/*!
	 *
	 * @param key
	 * @return
	 */
	NodePtr& operator[](K& key) {
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
	NodePtr& operator()(iterator iter) { return iter->second; }

	/*!
	 *
	 * @return
	 */
	iterator begin() { return links.begin(); }

	/*!
	 *
	 * @return
	 */
	iterator end() { return links.end(); }

	/*!
	 *
	 * @tparam T
	 * @param id
	 * @param v
	 */
	template <class T>
	void insert_value(std::string& id, T& v) {
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
	std::string& v_id() {
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
template <class NodeType>
class Tree {
private:
	// TODO Add Requirements for NodeType
	typedef typename NodeType::reference reference;
	std::unique_ptr<NodeType> root;
	size_t matched_branch_length_ = 0;
	std::string identifier = "";

	using node_id_t = std::remove_cvref_t<decltype(root->v_id())>;
	/*!
	 *
	 * @tparam InputIterator
	 * @tparam ValueType
	 * @param node
	 * @param current
	 * @param beyond
	 * @param val
	 */
	template <std::random_access_iterator InputIterator,
	          class ValueType>
	          /*typename = utils::requires_random_access_iterator<InputIterator>,
	          typename = utils::requires_arithmetic<typename InputIterator::value_type>>*/
	void insert(reference node, InputIterator current, InputIterator beyond, ValueType& val) {
		// TODO Add Requirements for ValueType
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
	template <std::random_access_iterator InputIterator>
	          //typename = utils::requires_arithmetic<typename InputIterator::value_type>>
	reference find(reference node, InputIterator current, InputIterator beyond) {
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
	template <utils::OutputIterator<std::pair<node_id_t,size_t>> OutputIterator>
	void find(reference node, OutputIterator iter) {
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
	reference get_root() { return root; }

	/*!
	 *
	 * @param nd
	 */
	Tree(std::unique_ptr<NodeType> nd) { root = std::move(nd); }

	Tree() = default;

	Tree& operator=(Tree&& rhs) {
		this->root = std::move(rhs.get_root());
		return *this;
	}
	Tree& operator=(const Tree& rhs) {
		this->root = std::make_unique<NodeType>(*rhs.get_root());
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
	template <std::random_access_iterator InputIterator,
	          class ValueType>
	void insert(InputIterator first, InputIterator beyond, ValueType& val) {
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
	template <std::random_access_iterator InputIterator>
	reference find(InputIterator first, InputIterator beyond) {
		matched_branch_length_ = 0;
		return this->find(root, first, beyond);
	}

	/*!
	 *
	 * @tparam OutputIterator
	 * @param iter
	 */
	template <utils::OutputIterator<std::pair<node_id_t, size_t>> OutputIterator>
	void find(OutputIterator iter) {
		this->find(root, iter);
	}

	/*!
	 *
	 * @return
	 */
	size_t get_match_size() { return matched_branch_length_; }

	~Tree() { root.release(); }
};
};  // namespace movetk::ds

#endif  // MOVETK_TREES_H
