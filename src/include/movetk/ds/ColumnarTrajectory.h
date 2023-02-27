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

#ifndef MOVETK_COLUMNAR_TRAJECTORY_H
#define MOVETK_COLUMNAR_TRAJECTORY_H

#include <cassert>
#include <iostream>
#include <tuple>
#include <vector>

#include "movetk/io/TrajectoryTraits.h"
#include "movetk/io/TupleIterPrinter.h"
#include "movetk/utils/Meta.h"

namespace movetk::ds {
/**
 * @brief Data structure for storing a trajectory, where each field of the trajectory,
 * e.g. location, time, heading, etc. is stored as a column, a single vector of values
 * @tparam ...fields Types of the fields in the trajectory.
 */
template <typename... FIELDS>
class ColumnarTrajectory {
public:
	// Total number of fields for each datapoint in the trajectory
	static constexpr size_t NUM_FIELDS = sizeof...(FIELDS);
	// The value type
	using value_type = std::tuple<FIELDS...>;
	// std::tuple of iterators for the fields
	using IteratorTuple = std::tuple<typename std::vector<FIELDS>::iterator...>;

	using TrajectoryIterator = IteratorTuple;

	/**
	 * @brief Returns the type of the field with the given index
	 * @tparam FieldIdx The field index
	 */
	template <int FieldIdx>
	using FieldType = std::tuple_element_t<FieldIdx, value_type>;

	/**
	 * @brief Constructs a columnar trajectory from a std::tuple of lists of the fields
	 * @param points The data points
	 */
	explicit ColumnarTrajectory(std::tuple<std::vector<FIELDS>...> points) : _points(points) {}

	ColumnarTrajectory() = default;

	/**
	 * @brief Returns the size of the trajectory
	 * This is the number of points in the trajectory
	 * @return The size of the trajectory
	 */
	std::size_t size() const { return std::get<0>(_points).size(); }

	/**
	 * @brief Returns the storage scheme
	 * @return The storage scheme
	 */
	constexpr static io::StorageScheme storage_scheme() { return io::StorageScheme::columnar; }

	/**
	 * @brief Returns the number of fields (columns)
	 * @return The number of fields in this data structure
	 */
	constexpr std::size_t num_fields() { return NUM_FIELDS; }

	/**
	 * @brief Returns the column for the given field index.
	 * @return The column data
	 */
	template <int field_idx>
	auto get() const -> const std::vector<typename std::tuple_element<field_idx, std::tuple<FIELDS...>>::type>& {
		return std::get<field_idx>(_points);
	}

	/**
	 * @brief Returns a reference to all data in the data structure
	 * @return Reference to all data
	 */
	const std::tuple<std::vector<FIELDS>...>& data() const { return _points; }

	/**
	 * @brief Updates a single field (column), replacing all values with the supplied values
	 * @param new_field_values The new field values.
	 */
	template <int field_idx>
	void update_field(std::vector<typename std::tuple_element<field_idx, std::tuple<FIELDS...>>::type> new_field_values) {
		std::get<field_idx>(_points) = std::move(new_field_values);
	}

	/**
	 * @brief Returns a std::tuple of begin iterators for the fields of the trajectory
	 * @return The std::tuple of begin iterators.
	 */
	IteratorTuple row_begin() { return _field_iterators_begin(std::make_index_sequence<NUM_FIELDS>{}); }

	/**
	 * @brief Returns a std::tuple of end iterators for the fields of the trajectory
	 * @return The std::tuple of end iterators.
	 */
	IteratorTuple row_end() { return _field_iterators_end(std::make_index_sequence<NUM_FIELDS>{}); }

	/**
	 * @brief Increments the row iterators in place and returns the old std::tuple.
	 * @param iters The std::tuple of current iterators
	 * @return Old iterator std::tuple
	 */
	IteratorTuple row_next(std::tuple<typename std::vector<FIELDS>::iterator...>& iters) {
		return _field_iterators_increment(iters, std::make_index_sequence<NUM_FIELDS>{});
	}

	/**
	 * @brief Stream write for the data structure
	 * @param os Output stream to write to
	 * @param trajectory Data structure to write
	 * @return Reference to the output stream
	 */
	friend std::ostream& operator<<(std::ostream& os, ColumnarTrajectory<FIELDS...>& trajectory) {
		auto iters = trajectory._field_iterators_begin(std::make_index_sequence<NUM_FIELDS>{});
		auto iters_end = trajectory._field_iterators_end(std::make_index_sequence<NUM_FIELDS>{});

		while (iters != iters_end) {
			movetk::io::print_iter_tuple(os, iters);
			trajectory._field_iterators_increment(iters, std::make_index_sequence<NUM_FIELDS>{});
		}
		return os;
	}

private:
	std::tuple<std::vector<FIELDS>...> _points;

	template <std::size_t... idx>
	IteratorTuple _field_iterators_begin(std::index_sequence<idx...>) {
		return {std::get<idx>(_points).begin()...};
	}

	template <std::size_t... idx>
	IteratorTuple _field_iterators_end(std::index_sequence<idx...>) {
		return {std::get<idx>(_points).end()...};
	}

	/**
	 * @brief Increments the field iterators in place, returns the old iterators.
	 * @tparam idx IDs of the fields
	 * @param iters The current iterators
	 * @return The old std::tuple
	 */
	template <std::size_t... idx>
	IteratorTuple _field_iterators_increment(std::tuple<typename std::vector<FIELDS>::iterator...>& iters,
	                                         std::index_sequence<idx...>) {
		return {(std::get<idx>(iters)++)...};
	}
};

/**
 * @brief Convenience definition to get a ColumnarTrajectory with as fields the types of the PROBE_TYPE type,
 * which should be a tuple-like type
 * @tparam PROBE_TYPE The probe type
 */
template <typename PROBE_TYPE>
using ColumnarTrajectoryForProbeType = movetk::utils::transfer_types<PROBE_TYPE, ColumnarTrajectory>;

/**
 * @brief Concatenate/Append a column to a ColumnarTrajectory data structure.
 * Note that the data structure is copied into the new data structure.
 * @tparam field_type The type of the column to append
 * @tparam ...fields The field types of the present ColumnarTrajectory
 * @param t The trajectory data structure
 * @param new_column The new column
 * @return A new datastructure with the column added to it.
 */
template <class field_type, class... FIELDS>
ColumnarTrajectory<FIELDS..., field_type> concat_field(const ColumnarTrajectory<FIELDS...>& t,
                                                       std::vector<field_type> new_column) {
	assert(new_column.size() == t.size());
	return ColumnarTrajectory<FIELDS..., field_type>(std::tuple_cat(t.data(), std::make_tuple(new_column)));
}
}  // namespace movetk::ds

#endif  // MOVETK_COLUMNAR_TRAJECTORY_H
