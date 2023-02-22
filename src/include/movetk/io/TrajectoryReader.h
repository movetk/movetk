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
// Created by onur on 10/11/18.
//

#ifndef MOVETK_TRAJECTORYREADER_H
#define MOVETK_TRAJECTORYREADER_H

#include <algorithm>
#include <iterator>
#include <memory>
#include <tuple>
#include <vector>

#include "SortByField.h"
#include "SplitByField.h"
#include "Splitter.h"
#include "movetk/io/TrajectoryTraits.h"
#include "movetk/io/TuplePrinter.h"
#include "movetk/utils/Transpose.h"

namespace movetk::io {
/**
 * @brief Trajectory reader class, that converts a range of probes into trajectories
 * @tparam TrajectoryTraits The traits for the reader
 * @tparam ProbeInputIterator Type of the probe range iterators
 * @tparam SortTrajectory Whether or not to sort the trajectory
 * @tparam RemoveDuplicates Whether or not to remove duplicates in the trajectory
*/
template <class TrajectoryTraits, class ProbeInputIterator, bool SortTrajectory = true, bool RemoveDuplicates = true>
class TrajectoryReader {
public:
	using ProbeTraits = typename TrajectoryTraits::ProbeTraits;
	constexpr static int SplitOnFieldIdx = TrajectoryTraits::SplitByFieldIdx;
	constexpr static int SortByFieldIdx = TrajectoryTraits::SortByFieldIdx;
	using ProbePoint = typename std::iterator_traits<ProbeInputIterator>::value_type;
	using value_type = typename TrajectoryTraits::trajectory_type;
	class iterator;
	using SplitByFieldIdx = SplitByField<SplitOnFieldIdx, ProbePoint>;

	TrajectoryReader(ProbeInputIterator start, ProbeInputIterator beyond) {
		// Split trajectory by attribute
		_splitter = std::make_unique<Splitter<SplitByFieldIdx, ProbeInputIterator>>(start, beyond);
		_splitit = std::begin(*_splitter);
		_splitit_end = std::end(*_splitter);
	}

	/// Status of the underlying stream
	/// @{
	inline bool good() { return _splitit != _splitit_end; }

	iterator begin() { return iterator(*this); }

	iterator end() { return iterator(); }

private:
	std::unique_ptr<Splitter<SplitByFieldIdx, ProbeInputIterator>> _splitter;
	typename Splitter<SplitByFieldIdx, ProbeInputIterator>::iterator _splitit;
	typename Splitter<SplitByFieldIdx, ProbeInputIterator>::iterator _splitit_end;

	inline value_type read_trajectory() {
		// Splitter<SplitByFieldIdx, ProbeInputIterator>::iterator::value_type
		auto segment = *_splitit;

		if constexpr (SortTrajectory || RemoveDuplicates) {
			// Sort trajectory by attribute
			SortByField<SortByFieldIdx, ProbePoint> sort_by_field_asc;
			std::sort(segment.begin(), segment.end(), sort_by_field_asc);
		}

		if constexpr (RemoveDuplicates) {
			// Eliminate if there are points with same field value.
			auto has_same_field_value = [](ProbePoint &p1, ProbePoint &p2) {
				return std::get<SortByFieldIdx>(p1) == std::get<SortByFieldIdx>(p2);
			};
			auto last = std::unique(segment.begin(), segment.end(), has_same_field_value);
			segment.erase(last, segment.end());
		}

		if constexpr (value_type::storage_scheme() == StorageScheme::columnar) {
			// Convert vector of tuples to tuple of vectors
			auto trans_segment = movetk::utils::Transpose(segment)();
			// Construct the trajectory
			value_type traj{trans_segment};
			return traj;
		} else {
			// Construct the trajectory
			value_type traj{segment};
			return traj;
		}
	}

	inline void increment_underlying_iterator() { _splitit++; }
};


/// Iterator; just calls recursively @ref TrajectoryReader::read_trajectory and stores the result.
template <class TrajectoryTraits, class ProbeInputIterator, bool SortTrajectory, bool RemoveDuplicates>
class TrajectoryReader<TrajectoryTraits, ProbeInputIterator, SortTrajectory, RemoveDuplicates>::iterator {
private:
	TrajectoryReader::value_type _trajectory;
	TrajectoryReader *_parent;

public:
	typedef std::input_iterator_tag iterator_category;
	typedef TrajectoryReader::value_type value_type;
	typedef std::size_t difference_type;
	typedef TrajectoryReader::value_type *pointer;
	typedef TrajectoryReader::value_type &reference;

	/// Construct an empty/end iterator
	inline iterator() : _parent(nullptr) {}
	/// Construct an iterator at the beginning of the @p parent object.
	inline iterator(TrajectoryReader &parent) : _parent(parent.good() ? &parent : nullptr) { ++(*this); }

	/// Read one segment, if possible. Set to end if parent is not good anymore.
	inline iterator &operator++() {
		if (_parent != nullptr) {
			_trajectory = _parent->read_trajectory();
			if (!_parent->good()) {
				_parent = nullptr;
			} else {
				_parent->increment_underlying_iterator();
			}
		}
		return *this;
	}

	inline iterator operator++(int) {
		iterator copy = *this;
		++(*this);
		return copy;
	}

	inline TrajectoryReader::value_type &operator*() { return _trajectory; }

	inline TrajectoryReader::value_type *operator->() { return &_trajectory; }

	bool operator==(iterator const &other) {
		return (this == &other) || (_parent == nullptr && other._parent == nullptr);
	}
	bool operator!=(iterator const &other) { return !(*this == other); }
};
}  // namespace movetk::io
#endif  // MOVETK_TRAJECTORYREADER_H
