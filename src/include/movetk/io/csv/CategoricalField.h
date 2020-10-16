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
// Created by onur on 10/22/18.
//

#ifndef MOVETK_CATEGORICALFIELD_H
#define MOVETK_CATEGORICALFIELD_H

#include <iostream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <vector>
#include <utility>

/**
 *
 * @tparam T type of the field
 * @tparam CRTP When subclassed, CRTP parameter should be given the name of the subclass.
 *         (Using the "Curiously recurring template pattern" to enforce separate static variables for each subclass)
 */
template <class T, class CRTP>
struct CategoricalField
{

    std::size_t _idx;
    static std::vector<T> _values;
    static std::unordered_map<T, std::size_t> _indexOf;

    inline std::size_t idx() const
    {
        return _idx;
    }
    double operator-(CategoricalField<T, CRTP> &rhs)
    {
        return _idx - rhs._idx; // time_end, time_beg
    }
    bool operator<(CategoricalField<T, CRTP> &rhs)
    {
        return _idx < rhs._idx;
    }
    bool operator>(CategoricalField<T, CRTP> &rhs)
    {
        return rhs < *this;
    }
    bool operator<=(CategoricalField<T, CRTP> &rhs)
    {
        return !(rhs < *this);
    }
    bool operator>=(CategoricalField<T, CRTP> &rhs)
    {
        return !(*this < rhs);
    }
    inline void add(T &&t)
    {
        if (_indexOf.find(t) == _indexOf.end())
        {
            _indexOf[t] = _values.size();
            _idx = _values.size();
            _values.push_back(t);
        }
        else
        {
            _idx = _indexOf[t];
        }
    }
    friend std::istream &operator>>(std::istream &is, CategoricalField<T, CRTP> &field)
    {
        T t;
        is >> t;
        field.add(std::move(t));
        return is;
    }

    friend std::ostream &operator<<(std::ostream &os, const CategoricalField<T, CRTP> &field)
    {
        os << field._values[field._idx];
        return os;
    }
};

template <class T, class CRTP>
std::vector<T> CategoricalField<T, CRTP>::_values;

template <class T, class CRTP>
std::unordered_map<T, std::size_t> CategoricalField<T, CRTP>::_indexOf;

#endif //MOVETK_CATEGORICALFIELD_H
