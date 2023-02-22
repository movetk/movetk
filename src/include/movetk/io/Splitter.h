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
// Created by onur on 9/23/18.
//

#ifndef MOVETK_SPLITTER_H
#define MOVETK_SPLITTER_H

#include <iostream>
#include <iterator>
#include <tuple>
#include <vector>

#include "movetk/io/TuplePrinter.h"

namespace movetk::io {
/**
 * @brief Range adaptor that takes a range of probes as input and outputs trajectories
 * in the form of a list of probe points
 * @tparam Predicate The predicate to apply for splitting
 * @tparam ProbeInputIterator The type of the input iterator
*/
template <class Predicate, class ProbeInputIterator>
class Splitter {
public:
	using ProbePoint = typename std::iterator_traits<ProbeInputIterator>::value_type;
	using value_type = std::vector<ProbePoint>;
	class iterator;

	/**
	 * @brief Construct a Splitter using the provided probe range
	 * @param start The start of the probe range
	 * @param beyond The end of the probe range
	*/
	Splitter(ProbeInputIterator start, ProbeInputIterator beyond) : _start(start), _beyond(beyond) {
		_state = State::Uninitialized;
		_exists_carry = false;
	}

	/**
	 * @brief Construct a Splitter using the provided probe range, and use the provided
	 * predicate for determining when to split the range into separate trajectories
	 * @param start The start of the probe range
	 * @param beyond The end of the probe range
	 * @param predicate The predicate to apply
	*/
	Splitter(ProbeInputIterator start, ProbeInputIterator beyond, Predicate predicate)
	    : _start(start)
	    , _beyond(beyond)
	    , _predicate(predicate) {
		_state = State::Uninitialized;
		_exists_carry = false;
	}

	/**
	 * @brief Return whether we can iterate the splitter
	 * @return Can we iterate the splitter
	*/
	inline bool good() { return _start != _beyond; }

	/**
	 * @brief Returns the begin iterator of the splitter, returning the trajectories
	 * @return The begin iterator
	*/
	iterator begin() { return iterator(*this); }

	/**
	 * @brief Returns the end iterator of the splitter, returning the trajectories
	 * @return The end iterator
	 */
	iterator end() { return iterator(); }

private:
	ProbeInputIterator _start;
	ProbeInputIterator _beyond;
	Predicate _predicate;  // predicate functor returns True for the first element of each segment in the stream
	enum class State { Uninitialized, Start_of_Segment, In_Segment };
	State _state;  // segment construction state
	ProbePoint _carry;
	bool _exists_carry;

	/**
	 * @brief Read a single (sub)trajectory from the probes
	 * @return The trajectory.
	*/
	inline value_type read_segment() {
		bool segment_done = false;
		value_type _segment;
		State next_state = State::Uninitialized;

		if (_state == State::Start_of_Segment && _exists_carry) {
			_segment.clear();
			_segment.push_back(_carry);
			_exists_carry = false;
		}

		while (!segment_done && _start != _beyond) {
			auto &input = *_start;

			switch (_state) {
				case State::Uninitialized:
					if (_predicate(input)) {
						_segment.push_back(input);
						next_state = State::Start_of_Segment;
					} else {
						_segment.clear();
						next_state = State::Uninitialized;
					}
					break;
				case State::Start_of_Segment:
					if (_predicate(input)) {
						segment_done = true;
						_carry = input;
						_exists_carry = true;
						next_state = State::Start_of_Segment;
					} else {
						_segment.push_back(input);
						next_state = State::In_Segment;
					}
					break;
				case State::In_Segment:
					if (_predicate(input)) {
						segment_done = true;
						_carry = input;
						_exists_carry = true;
						next_state = State::Start_of_Segment;
					} else {
						_segment.push_back(input);
						next_state = State::In_Segment;
					}
					break;
			}
			_state = next_state;
			_start++;
		}

		return _segment;
	}
};


/// Iterator; just calls recursively @ref Splitter::read_segment and stores the result.
template <class Predicate, class ProbeInputIterator>
class Splitter<Predicate, ProbeInputIterator>::iterator {
private:
	Splitter::value_type _segment;
	Splitter *_parent;

public:
	typedef std::input_iterator_tag iterator_category;
	typedef Splitter::value_type value_type;
	typedef std::size_t difference_type;
	typedef Splitter::value_type *pointer;
	typedef Splitter::value_type &reference;

	/// Construct an empty/end iterator
	inline iterator() : _parent(nullptr) {}
	/// Construct an iterator at the beginning of the @p parent object.
	inline iterator(Splitter &parent) : _parent(parent.good() ? &parent : nullptr) { ++(*this); }

	/// Read one segment, if possible. Set to end if parent is not good anymore.
	inline iterator &operator++() {
		if (_parent != nullptr) {
			_segment = _parent->read_segment();
			if (_segment.empty()) {  // if (!_parent->good()) {
				_parent = nullptr;
			}
		}
		return *this;
	}

	inline iterator operator++(int) {
		iterator copy = *this;
		++(*this);
		return copy;
	}

	inline Splitter::value_type const &operator*() const { return _segment; }

	inline Splitter::value_type const *operator->() const { return &_segment; }

	bool operator==(iterator const &other) {
		return (this == &other) || (_parent == nullptr && other._parent == nullptr);
	}
	bool operator!=(iterator const &other) { return !(*this == other); }
};
}  // namespace movetk::io
#endif  // MOVETK_SPLITTER_H
