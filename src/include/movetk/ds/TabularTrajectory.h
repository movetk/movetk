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
// Created by onur on 9/20/18.
// Modified by Custers, Bram on 2020-02-08.
//

#ifndef MOVETK_TABULAR_TRAJECTORY_H
#define MOVETK_TABULAR_TRAJECTORY_H

#include <algorithm>
#include <array>
#include <exception>
#include <iostream>
#include <tuple>
#include <utility>
#include <vector>

#include "movetk/Interpolation.h"
#include "movetk/PolylineUtils.h"
#include "movetk/io/TrajectoryTraits.h"
#include "movetk/io/TuplePrinter.h"
#include "movetk/utils/Meta.h"

namespace movetk::ds {


template <typename T>
concept IteratorCheck = requires(T t) {
	typename std::iter_difference_t<T>;
	requires std::is_signed_v<std::iter_difference_t<T>>;
	{ ++t } -> std::same_as<T&>;
	t++;
	{ *t } -> std::same_as<std::iter_reference_t<T>>;
	typename std::iter_value_t<T>;
	typename std::iter_reference_t<T>;
	typename std::iter_rvalue_reference_t<T>;
	{ std::ranges::iter_move(t) } -> std::same_as<std::iter_rvalue_reference_t<T>>;
}
&&std::common_reference_with<std::iter_reference_t<T>&&, std::iter_value_t<T>&>&&
    std::common_reference_with<std::iter_reference_t<T>&&, std::iter_rvalue_reference_t<T>&&>&&
        std::common_reference_with<std::iter_rvalue_reference_t<T>&&, const std::iter_value_t<T>&>;

/**
 * @brief Trajectory datastructure that stores fields in "row major" order:
 * each probe is stored as a tuple, the full trajecory is a list of these tuples.
 * @tparam ...fields The fields to store.
 */
template <class... FIELDS>
class TabularTrajectory {
public:
	constexpr static int NUM_FIELDS = sizeof...(FIELDS);
	using value_type = std::tuple<FIELDS...>;
	using ValueList = std::vector<value_type>;
	/*!
	 * TrajectoryIterator
	 */
	using TrajectoryIterator = typename ValueList::iterator;
	using ConstTrajectoryIterator = typename ValueList::const_iterator;
	using ReverseTrajectoryIterator = typename ValueList::reverse_iterator;

	template <int FieldIdx>
	using FieldType = std::tuple_element_t<FieldIdx, value_type>;

	/**
	 * @brief Construct a TabularTrajectory using the provided points
	 * @param points The points
	 */
	explicit TabularTrajectory(ValueList&& points) : _points(std::move(points)) {}
	/**
	 * @brief Construct a TabularTrajectory using the provided points
	 * @param points The points
	 */
	explicit TabularTrajectory(const ValueList& points) : _points(points) {}

	/**
	 * @brief Construct a TabularTrajectory from a range of row values
	 * @param first Start of the range
	 * @param last End of the range
	 */
	template <utils::InputIterator<value_type> ROW_ITERATOR>
	TabularTrajectory(ROW_ITERATOR first, ROW_ITERATOR last) {
		std::copy(first, last, std::back_inserter(_points));
	}

	/**
	 * @brief Construct a TabularTrajectory from a list of row elements
	 * @param points The row elements
	 */
	TabularTrajectory(std::initializer_list<TrajectoryIterator> points) {
		std::move(std::begin(points), std::end(points), std::back_inserter(_points));
	}

	TabularTrajectory() = default;
	// Support defaulted move and copy
	TabularTrajectory(const TabularTrajectory&) = default;
	TabularTrajectory(TabularTrajectory&&) = default;
	TabularTrajectory& operator=(const TabularTrajectory&) = default;
	TabularTrajectory& operator=(TabularTrajectory&&) = default;

	/**
	 * @brief Returns the number of probes(rows) in the trajectory
	 * @return The number of probes
	 */
	std::size_t size() const { return _points.size(); }

	/**
	 * @brief Returns the storage scheme
	 * @return The storage scheme
	 */
	constexpr static io::StorageScheme storage_scheme() { return io::StorageScheme::tabular; }

	/**
	 * @brief Returns the number of fields
	 * @return The number of feilds
	 */
	constexpr std::size_t num_fields() const { return sizeof...(FIELDS); }

	/**
	 * @brief Returns the values of the field with the given index
	 * @tparam field_idx Index of the field to return
	 * @return A copy of the values of the field
	 */
	template <int field_idx>
	auto get() const -> std::vector<FieldType<field_idx>> {
		std::vector<FieldType<field_idx>> field_vector;
		for (auto& p : _points) {
			field_vector.push_back(std::get<field_idx>(p));
		}
		return field_vector;
	}

	/**
	 * @brief Retrieves the vector of raw data points
	 * @return Const reference to the data points
	 */
	const ValueList& data() const { return _points; }

	/**
	 * @brief Updates all the values of the specified field with the new values
	 * @param new_field_values The new values
	 */
	template <int field_idx>
	void update_field(const std::vector<FieldType<field_idx>>& new_field_values) {
		assert(new_field_values.size() == _points.size());
		auto it = std::begin(_points);
		for (auto fv : new_field_values) {
			std::get<field_idx>(*it) = fv;
			++it;
		}
	}
	/**
	 * @brief Returns the begin of the range of probes
	 * @return The begin iterator
	 */
	TrajectoryIterator begin() { return _points.begin(); }

	/**
	 * @brief Returns the end of the range of probes
	 * @return The end iterator
	 */
	TrajectoryIterator end() { return _points.end(); }

	/**
	 * @brief Returns the begin of the reverse range of probes
	 * @return The begin iterator
	 */
	ReverseTrajectoryIterator rbegin() { return _points.rbegin(); }

	/**
	 * @brief Returns the End of the reverse range of probes
	 * @return The end iterator
	 */
	ReverseTrajectoryIterator rend() { return _points.rend(); }

	/**
	 * Insert points [first, last) before pos.
	 * @param pos
	 * @param first
	 * @param last
	 */
	template <int timestamp_idx>
	void insert(ConstTrajectoryIterator pos, TrajectoryIterator first, TrajectoryIterator last) {
		auto first_ts = std::get<timestamp_idx>(*first);
		auto last_ts = std::get<timestamp_idx>(*(last - 1));

		if (!_points.empty()) {
			if (pos == std::begin(_points)) {
				auto pos_ts = std::get<timestamp_idx>(*pos);
				if (last_ts > pos_ts) {
					throw std::invalid_argument(
					    "Insert (prepend) failed (timestamps of the prepended trajectory should be smaller or equal)!");
				}
			} else if (pos == std::end(_points)) {
				auto before_pos_ts = std::get<timestamp_idx>(*(pos - 1));
				if (first_ts < before_pos_ts) {
					throw std::invalid_argument(
					    "Insert (append) failed (timestamps of the appended trajectory should be greater or equal)!");
				}
			} else {
				auto before_pos_ts = std::get<timestamp_idx>(*(pos - 1));
				auto pos_ts = std::get<timestamp_idx>(*pos);
				if (first_ts < before_pos_ts || last_ts > pos_ts) {
					throw std::invalid_argument(
					    "Insert failed (timestamps of the inserted trajectory are not in the expected range)!");
				}
			}
		}

		_points.insert(pos, first, last);
	}

	/**
	 * Write points as CSV
	 * @param os
	 * @param trajectory
	 * @return
	 */
	friend std::ostream& operator<<(std::ostream& os, TabularTrajectory<FIELDS...>& trajectory) {
		for (auto row : trajectory) {
			movetk::io::print_tuple(os, row);
			os << "\r\n";
		}
		return os;
	}

	/**
	 * @brief Functor to lookup probes by an offset
	 * @tparam GeometryTraits The kernel to use
	 * @tparam LatIdx Index of the latitude coordinate in the probe
	 * @tparam LonIdx Index of the longitude coordinate in the probe
	 */
	template <class GeometryTraits, int LatIdx, int LonIdx>
	class LookupByOffsetFn {
	public:
		static_assert(std::is_same_v<typename GeometryTraits::NT, FieldType<LatIdx>>);
		static_assert(std::is_same_v<typename GeometryTraits::NT, FieldType<LonIdx>>);
		std::vector<double> offsets;

		inline LookupByOffsetFn(TabularTrajectory& parent) : _parent(&parent) {
			offsets = movetk::algo::geopolyline_offsets_m<GeometryTraits>(_parent->begin<LatIdx>(),
			                                                              _parent->end<LatIdx>(),
			                                                              _parent->begin<LonIdx>());
		}

		/**
		 * Returns an iterator to the point in the trajectory that has greater offset than the offset argument,
		 * or end iterator if offset is beyond trajectory length.
		 * @param offset
		 * @param offset_begin the offset at the start of the segment that contains the point at the given offset.
		 * @param offset_end the offset of the end of the segment that contains the point at the given offset.
		 * @return
		 */
		TrajectoryIterator operator()(double offset, double& offset_begin, double& offset_end) {
			auto lower = std::lower_bound(std::begin(offsets), std::end(offsets), offset);
			if (lower == std::begin(offsets)) {
				offset_begin = std::numeric_limits<double>::min();
				offset_end = *lower;
			} else if (lower == std::end(offsets)) {
				offset_begin = *(lower - 1);
				offset_end = std::numeric_limits<double>::max();
			} else {
				offset_begin = *(lower - 1);
				offset_end = *lower;
			}
			return _parent->begin() + std::distance(std::begin(offsets), lower);
		}

		/**
		 * Returns the offset value at the given trajectory point iterator,
		 * nullopt if the point does not belong to the trajectory.
		 */
		std::optional<double> reverse_lookup(TrajectoryIterator it) {
			auto traj_it = _parent->begin();
			for (auto& offset : offsets) {
				if (it == traj_it++)
					return offset;
			}
			return std::nullopt;
		}

	private:
		TabularTrajectory* _parent;
	};  // /nested class LookupByOffsetFn

	template <class GeometryTraits, int LatIdx, int LonIdx>
	LookupByOffsetFn<GeometryTraits, LatIdx, LonIdx> lookup_offset_fn() {
		return LookupByOffsetFn<GeometryTraits, LatIdx, LonIdx>(*this);
	}

	/**
	 * Return the interpolated point and a trajectory iterator that comes after the interpolated point.
	 * This iterator can be used as an argument to the insert method.
	 * @tparam LatIdx
	 * @tparam LonIdx
	 * @tparam Interpolator_2
	 * @param offset
	 * @param interpolator
	 * @return
	 */
	template <class GeometryTraits, int LatIdx, int LonIdx, class Interpolator_2>
	std::pair<value_type, TrajectoryIterator> getInterpolatedPoint(double offset, Interpolator_2 interpolator) {
		// TODO: assert Interpolator_2 value_type is same as value_type
		auto offset_fn = lookup_offset_fn<GeometryTraits, LatIdx, LonIdx>();
		double begin_offset, end_offset;
		auto seg_end_it = offset_fn(offset, begin_offset, end_offset);

		if (seg_end_it == std::end(*this)) {
			return std::make_pair(*(seg_end_it - 1),
			                      std::end(*this));  // don't interpolate, return the end point of the trajectory
		} else if (seg_end_it == std::begin(*this)) {
			return std::make_pair(*seg_end_it,
			                      std::begin(*this));  // don't interpolate, return the start point of the trajectory
		} else {
			// interpolator expects the offset between the interpolated points.
			auto interpolated_point = interpolator(*(seg_end_it - 1), *seg_end_it, offset - begin_offset);
			return std::make_pair(interpolated_point, seg_end_it);
		}
	}


	// FieldIterator nested class
	template <int field_idx>
	class FieldIterator {
	private:
		TabularTrajectory* _parent;
		TrajectoryIterator _it;

	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = std::tuple_element_t<field_idx, std::tuple<FIELDS...>>;
		using difference_type = long long;
		using pointer = value_type*;
		using reference = value_type&;

		FieldIterator() : _parent(nullptr), _it({}) {}

		/// Construct an iterator at the beginning of the @p parent object.
		FieldIterator(TabularTrajectory& parent) : _parent(&parent) { _it = _parent->begin(); }

		static FieldIterator end_it(TabularTrajectory& parent) {
			FieldIterator iterator(parent);
			iterator._it = parent.end();
			return iterator;
		}

		TrajectoryIterator& underlying_iterator() { return _it; }

		const TrajectoryIterator& underlying_iterator() const { return _it; }

		FieldIterator& operator++() {
			if (_parent != nullptr) {
				++_it;
				if (_it == _parent->end()) {
					_parent = nullptr;
				}
			}
			return *this;
		}

		FieldIterator operator++(int) {
			FieldIterator copy = *this;
			++(*this);
			return copy;
		}

		FieldIterator operator-(int n) const {
			FieldIterator copy = *this;
			std::advance(copy.underlying_iterator(), -n);
			return copy;
		}

		reference operator*() const { return std::get<field_idx>(*_it); }

		pointer operator->() {
			return &std::get<field_idx>(*_it);
			;
		}

		bool operator==(FieldIterator const& other) const {
			return (this->underlying_iterator() == other.underlying_iterator()) ||
			       (_parent == nullptr && other._parent == nullptr);
		}

		bool operator!=(FieldIterator const& other) const { return !(*this == other); }
	};  // /FieldIterator nested class

	template <int field_idx>
	FieldIterator<field_idx> begin() {
		return FieldIterator<field_idx>(*this);
	}

	template <int field_idx>
	FieldIterator<field_idx> end() {
		return FieldIterator<field_idx>::end_it(*this);
	}

private:
	std::vector<std::tuple<FIELDS...>> _points;
};

/**
 * @brief Convenience definition to get a TabularTrajectory with as fields the types of the PROBE_TYPE type,
 * which should be a tuple-like type
 * @tparam PROBE_TYPE The probe type
 */
template <typename PROBE_TYPE>
using TabularTrajectoryForProbeType = movetk::utils::transfer_types<PROBE_TYPE, TabularTrajectory>;

// Helper functions for concat_field
template <class... FIELDS, std::size_t... idx>
std::tuple<typename std::vector<FIELDS>::iterator...> field_iterators_increment(
    std::tuple<typename std::vector<FIELDS>::iterator...>& iters,
    std::index_sequence<idx...>) {
	return {(std::get<idx>(iters)++)...};
}

template <class... FIELDS>
std::tuple<typename std::vector<FIELDS>::iterator...> row_next(
    std::tuple<typename std::vector<FIELDS>::iterator...>& iters) {
	return field_iterators_increment<FIELDS...>(iters, std::make_index_sequence<sizeof...(FIELDS)>{});
}

template <class... FIELDS, std::size_t... idx>
typename std::tuple<FIELDS...> _read_row(std::tuple<typename std::vector<FIELDS>::iterator...>& iters,
                                         std::index_sequence<idx...>) {
	return std::make_tuple(*std::get<idx>(iters)...);
}

template <class... FIELDS>
typename std::tuple<FIELDS...> read_row(std::tuple<typename std::vector<FIELDS>::iterator...>& iters) {
	return _read_row<FIELDS...>(iters, std::make_index_sequence<sizeof...(FIELDS)>{});
}

template <class... FIELDS, class... new_fields>
TabularTrajectory<FIELDS..., new_fields...> concat_field(TabularTrajectory<FIELDS...> t,
                                                         std::vector<new_fields>... new_columns) {
	std::array<std::size_t, sizeof...(new_fields)> new_sizes = {new_columns.size()...};
	std::size_t size = t.size();
	assert(std::all_of(std::begin(new_sizes), std::end(new_sizes), [&size](std::size_t s) { return s == size; }));
	std::vector<std::tuple<FIELDS..., new_fields...>> new_data;
	new_data.reserve(t.size());
	auto new_it = std::make_tuple(std::begin(new_columns)...);
	for (auto row_it = std::begin(t); row_it != std::end(t); ++row_it) {
		auto new_slice = read_row<new_fields...>(new_it);
		row_next<new_fields...>(new_it);
		new_data.emplace_back(std::tuple_cat(*row_it, new_slice));
	}
	return TabularTrajectory<FIELDS..., new_fields...>(new_data);
}
}  // namespace movetk::ds
#endif  // MOVETK_TABULAR_TRAJECTORY_H
