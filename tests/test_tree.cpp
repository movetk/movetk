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
// Created by Mitra, Aniket on 09/01/2019.
//

#include <catch2/catch.hpp>

#include "movetk/ds/Tree.h"
#include "movetk/utils/Iterators.h"

/*!
 *
 * @brief values stored by each node of the tree
 */
template <class T>
class Values {
private:
	std::string id;
	std::vector<T> values;

public:
	typedef typename std::vector<T>::const_iterator Iterator;

	Values(const std::string& identifier) : id(identifier) {}

	void operator()(const T& value) { values.push_back(value); }

	const std::string& operator()() { return id; }
	Iterator begin() const { return cbegin(values); }

	Iterator end() const { return cend(values); }
};

TEST_CASE("Create and search in a Trie", "[test_trie") {
	typedef movetk::ds::TrieNode<const char, Values<int>> _Node;
	movetk::ds::Tree<_Node> tree(std::make_unique<_Node>('1'));
	std::vector<std::pair<std::string, size_t>> leaves;
	{
		int value = 1;
		const std::string str = "aabc";
		//using str_it = std::remove_reference_t<std::remove_cv_t<decltype(begin(str))>>;
		// static_assert(std::same_as<str_it, typename std::string::const_iterator>);
		// static_assert(std::random_access_iterator<str_it>);
		// static_assert(std::same_as<typename std::iterator_traits<str_it>::value_type,char>);
		// static_assert(movetk::utils::RandomAccessIteratorWithType<str_it, const char>);
		tree.insert(begin(str), end(str), value);
	}
	{
		const std::string str = "aabda";
		int value = 2;
		tree.insert(begin(str), end(str), value);
	}
	{
		const std::string str = "aabda";
		int value = 4;
		tree.insert(begin(str), end(str), value);
	}
	std::string search_str = "aabcaj";
	auto& result = tree.find(cbegin(search_str), cend(search_str));

	REQUIRE(tree.get_match_size() == 4);

	REQUIRE((*result)() == 'c');

	REQUIRE(std::distance(result->v_begin(), result->v_end()) == 1);

	auto it = result->v_begin();

	REQUIRE(*it == 1);

	// counts number of leaves in each branch of the tree
	tree.find(std::back_inserter(leaves));

	REQUIRE(leaves.size() == 2);

	REQUIRE(leaves[0].first.compare("aabc") == 0);

	REQUIRE(leaves[0].second == 1);

	REQUIRE(leaves[1].first.compare("aabda") == 0);

	REQUIRE(leaves[1].second == 2);
}

// TEST_CASE("Create and search in a binary tree", "[test_btree") {
////    typedef movetk_support::BinaryNode<int, Values<char> > Node;
////    int rkey = 0;
////    movetk_support::Tree<Node> tree( std::make_unique<Node >(rkey) );
////    std::vector< std::pair<std::string, size_t> > leaves;
////    std::vector<int> input1{0,1,0,1,1};
////    char value = 'a';
////    tree.insert( begin(input1), end(input1), value );
////
////    value = 'b';
////    std::vector<int> input2{0,1,1,1,0,1};
////    tree.insert( begin(input2), end(input2), value );
//
////
////    Node::reference result = tree.find( cbegin(search_str), cend(search_str) );
////
////    REQUIRE (tree.get_match_size() == 4);
////
////    REQUIRE ((*result)() == 'c');
////
////    REQUIRE (std::distance(result->v_begin(), result->v_end()) == 1);
////
////    auto it = result->v_begin();
////
////    REQUIRE (*it == 1);
////
////    // counts number of leaves in each branch of the tree
////    tree.find(std::back_inserter(leaves));
////
////    REQUIRE (leaves.size() == 2);
////
////    REQUIRE ( leaves[0].first.compare("aabc") == 0 );
////
////    REQUIRE (leaves[0].second == 1);
////
////    REQUIRE ( leaves[1].first.compare("aabda") == 0 );
////
////    REQUIRE (leaves[1].second == 2);
//
//
//}
