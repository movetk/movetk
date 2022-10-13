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

#include <iostream>
#include <vector>
#include <tuple>
using std::tuple;
#include "movetk/io/TupleIterPrinter.h"
#include "movetk/io/TrajectoryTraits.h"


template <class... fields>
class ColumnarTrajectory {

public:
    // Total number of fields for each datapoint in the trajectory
    constexpr static int NUM_FIELDS = sizeof...(fields);
    // The value type
    typedef std::tuple<fields...> value_type;
    // Tuple of iterators for the fields
    using IteratorTuple = std::tuple<typename std::vector<fields>::iterator...>;

    using TrajectoryIterator = IteratorTuple;

    /**
     * @brief Returns the type of the field with the given index
     * @tparam FieldIdx The field index
     */
    template<int FieldIdx>
    using FieldType = std::tuple_element_t<FieldIdx, value_type>;

    /**
     * @brief Constructs a columnar trajectory from a tuple of lists of the fields
     * @param points The data points
     */
    explicit ColumnarTrajectory(std::tuple<std::vector<fields>...> points) : _points(points)
    {
    }

    ColumnarTrajectory() = default;

    /**
     * @brief Returns the size of the trajectory
     * This is the number of points in the trajectory
     * @return The size of the trajectory
     */
    std::size_t size()
    {
        return std::get<0>(_points).size();
    }

    constexpr static StorageScheme storage_scheme() {
        return StorageScheme::columnar;
    }

    constexpr std::size_t num_fields() {
        return sizeof...(fields);
    }

    template <int field_idx>
    auto get() -> const std::vector<typename std::tuple_element<field_idx, tuple<fields...>>::type>&
    {
        return std::get<field_idx>(_points);
    }

    const std::tuple<std::vector<fields>...>& data()
    {
        return _points;
    }

    template <int field_idx>
    void update_field(std::vector<typename std::tuple_element<field_idx, tuple<fields...>>::type> new_field_values)
    {
        std::get<field_idx>(_points) = new_field_values;
    }

    IteratorTuple row_begin()
    {
        return _field_iterators_begin(std::make_index_sequence<NUM_FIELDS>{});
    }

    /**
     * @brief Returns a tuple of end iterators for the fields of the trajectory
     * @return The tuple of end iterators.
     */
    IteratorTuple row_end()
    {
        return _field_iterators_end(std::make_index_sequence<NUM_FIELDS>{});
    }

    /**
     * @brief Increments the row iterators in place and returns the old tuple.
     * @param iters The tuple of current iterators
     * @return Old iterator tuple
     */
    IteratorTuple row_next(std::tuple<typename std::vector<fields>::iterator...>& iters)
    {
        return _field_iterators_increment(iters, std::make_index_sequence<NUM_FIELDS>{});
    }

    friend std::ostream& operator<<(std::ostream& os, ColumnarTrajectory<fields...>& trajectory)
    {
        auto iters = trajectory._field_iterators_begin(std::make_index_sequence<NUM_FIELDS>{});
        auto iters_end = trajectory._field_iterators_end(std::make_index_sequence<NUM_FIELDS>{});

        while (iters != iters_end) {
            print_iter_tuple(os, iters);
            trajectory._field_iterators_increment(iters, std::make_index_sequence<NUM_FIELDS>{});
        }
        return os;
    }

private:
    std::tuple<std::vector<fields>...> _points;

    template <std::size_t... idx>
    IteratorTuple _field_iterators_begin(std::index_sequence<idx...>)
    {
        return {std::get<idx>(_points).begin()...};
    }

    template <std::size_t... idx>
    IteratorTuple _field_iterators_end(std::index_sequence<idx...>)
    {
        return {std::get<idx>(_points).end()...};
    }

    /**
     * @brief Increments the field iterators in place, returns the old iterators.
     * @tparam idx IDs of the fields
     * @param iters The current iterators
     * @return The old tuple
     */
    template <std::size_t... idx>
    IteratorTuple _field_iterators_increment(std::tuple<typename std::vector<fields>::iterator...>& iters, std::index_sequence<idx...>) {
        return {(std::get<idx>(iters)++)...};
    }

};


template <class field_type, class... fields>
ColumnarTrajectory<fields..., field_type> concat_field(ColumnarTrajectory<fields...> t, std::vector<field_type> new_column)
{
    assert(new_column.size() == t.size());
    return ColumnarTrajectory<fields..., field_type>(std::tuple_cat(t.data(), std::make_tuple(new_column)));
}


#endif //MOVETK_COLUMNAR_TRAJECTORY_H
