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

#ifndef MOVETK_HIGHFREQUENCYTRAJECTORYSPLITTER_H
#define MOVETK_HIGHFREQUENCYTRAJECTORYSPLITTER_H

#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <tuple>
#include <type_traits>
#include <vector>

#include "Splitter.h"
#include "movetk/ds/ColumnarTrajectory.h"
#include "movetk/geo/geo.h"
#include "movetk/io/SplitByDifferenceThreshold.h"
#include "movetk/io/SplitByDistanceThreshold.h"
#include "movetk/io/TuplePrinter.h"
namespace movetk::io {
template <class TrajectoryInputIterator, int DateIdx, int LatIdx, int LonIdx>
class HighFrequencyTrajectorySplitter {
public:
	using trajectory_type =
	    typename std::iterator_traits<TrajectoryInputIterator>::value_type;  // TabularTrajectory<fields...>
	using ProbePoint = typename trajectory_type::value_type;                 // tuple<fields...>
	class iterator;
	class SplitByTimeDiffOrDistance;

	HighFrequencyTrajectorySplitter(TrajectoryInputIterator start,
	                                TrajectoryInputIterator beyond,
	                                float time_diff_threshold_s,
	                                float distance_threshold_m,
	                                std::size_t min_points = 2)
	    : _traj_it(start)
	    , _traj_it_end(beyond)
	    , _time_diff_threshold_s(time_diff_threshold_s)
	    , _distance_threshold_m(distance_threshold_m)
	    , _min_points(min_points) {}

	/// Status of the underlying stream
	/// @{
	inline bool good() {
		// good if there are still trajectories to process or current trajectory has not been fully consumed
		return _traj_it != _traj_it_end || (_split_it != std::end(_splits));
	}

	iterator begin() { return iterator(*this); }

	iterator end() { return iterator(); }

private:
	TrajectoryInputIterator _traj_it;
	TrajectoryInputIterator _traj_it_end;
	double _time_diff_threshold_s;
	double _distance_threshold_m;
	std::size_t _min_points;  // min number of points required for a split to qualify as a trajectory

	// iteration state
	std::vector<trajectory_type> _splits;
	typename std::vector<trajectory_type>::iterator _split_it;

	inline std::optional<trajectory_type> read_trajectory() {
		if (_traj_it != _traj_it_end && _split_it == std::end(_splits)) {
			_splits.clear();  // reset for next use

			while (_splits.size() == 0 && _traj_it != _traj_it_end) {
				// std::cout << "Splitting... " << std::get<0>(_traj_it->data()[0]) << '\n';

				// move to the next trajectory, split it and return the first split segment
				using SplitByTimeDiff = SplitByDifferenceThreshold<DateIdx, ProbePoint>;
				SplitByTimeDiff split_by_time_diff(_time_diff_threshold_s);
				using SplitByDistance = SplitByDistanceThreshold<LatIdx, LonIdx, ProbePoint>;
				std::function<double(float, float, float, float)> distancefn = geo::distance_exact;
				SplitByDistance split_by_dist(_distance_threshold_m, distancefn);
				auto split_by_time_diff_or_distance = [&](const ProbePoint& p) {
					bool t = split_by_time_diff(p);
					bool d = split_by_dist(p);
					return (t || d);
				};

				using ProbeInputIterator = decltype(_traj_it->begin());
				Splitter<decltype(split_by_time_diff_or_distance), ProbeInputIterator> hifreq_splitter(
				    _traj_it->begin(),
				    _traj_it->end(),
				    split_by_time_diff_or_distance);


				for (auto& segment : hifreq_splitter) {
					if (segment.size() >= _min_points)
						_splits.emplace_back(trajectory_type{segment});
				}
				// std::cout << "Got " << _splits.size() << " splits\n";
				_split_it = std::begin(_splits);
				++_traj_it;
			}
		}

		if (_split_it != std::end(_splits)) {
			trajectory_type t = *_split_it;
			++_split_it;
			return t;
		} else
			return std::nullopt;
	}
};

/// Iterator; just calls iteratively @ref HighFrequencyTrajectorySplitter::read_trajectory and stores the result.
template <class TrajectoryInputIterator, int DateIdx, int LatIdx, int LonIdx>
class HighFrequencyTrajectorySplitter<TrajectoryInputIterator, DateIdx, LatIdx, LonIdx>::iterator {
private:
	HighFrequencyTrajectorySplitter::trajectory_type _trajectory;
	HighFrequencyTrajectorySplitter* _parent;

public:
	typedef std::input_iterator_tag iterator_category;
	typedef HighFrequencyTrajectorySplitter::trajectory_type value_type;
	typedef std::size_t difference_type;
	typedef HighFrequencyTrajectorySplitter::trajectory_type* pointer;
	typedef HighFrequencyTrajectorySplitter::trajectory_type& reference;

	/// Construct an empty/end iterator
	inline iterator() : _parent(nullptr) {}
	/// Construct an iterator at the beginning of the @p parent object.
	inline iterator(HighFrequencyTrajectorySplitter& parent) : _parent(parent.good() ? &parent : nullptr) { ++(*this); }

	/// Read one segment, if possible. Set to end if parent is not good anymore.
	inline iterator& operator++() {
		if (_parent != nullptr) {
			auto optional_trajectory = _parent->read_trajectory();
			if (!optional_trajectory) {
				_parent = nullptr;
			} else {
				_trajectory = *optional_trajectory;
			}
		}
		return *this;
	}

	inline iterator operator++(int) {
		iterator copy = *this;
		++(*this);
		return copy;
	}

	inline value_type const& operator*() const { return _trajectory; }

	inline value_type const* operator->() const { return &_trajectory; }

	bool operator==(iterator const& other) {
		return (this == &other) || (_parent == nullptr && other._parent == nullptr);
	}
	bool operator!=(iterator const& other) { return !(*this == other); }
};
}  // namespace movetk::io
#endif  // MOVETK_HIGHFREQUENCYTRAJECTORYSPLITTER_H
