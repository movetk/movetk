// Copyright 2015 Pietro Saccardi
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//


//
//  csv.h
//
//  Created by Pietro Saccardi on 05/12/15.
//  http://stackoverflow.com/questions/1120140/how-can-i-read-and-parse-csv-files-in-c
//

#ifndef csv_h
#define csv_h

#include <array>
#include <iostream>  // for debug
#include <iterator>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace movetk::io::csv {

template <class Tuple, int... idx>
auto subset_tuple(Tuple t) -> std::tuple<typename std::tuple_element<idx, Tuple>::type...> {
	return std::make_tuple(std::get<idx>(t)...);
}
template <class Tuple, int... idx>
auto subset_tuple(Tuple &&t, std::integer_sequence<int, idx...>)
    -> std::tuple<typename std::tuple_element<idx, Tuple>::type...> {
	return std::make_tuple(std::get<idx>(std::forward<Tuple>(t))...);
}

template <class Array, int... idx>
auto subset_array(const Array &t, std::integer_sequence<int, idx...>)
    -> std::array<typename Array::value_type, sizeof...(idx)> {
	return std::array<typename Array::value_type, sizeof...(idx)>{std::get<idx>(t)...};
}


namespace csvtools {
/// Read the last element of the tuple without calling recursively
template <std::size_t idx, class Tuple>
typename std::enable_if<idx >= std::tuple_size<Tuple>::value - 1>::type read_tuple(std::istream &in,
                                                                                   Tuple &out,
                                                                                   const char delimiter) {
	std::string cell;
	std::getline(in, cell, delimiter);
	std::stringstream cell_stream(cell);
	cell_stream >> std::get<idx>(out);
}

/// Read the @p idx-th element of the tuple and then calls itself with @p idx + 1 to
/// read the next element of the tuple. Automatically falls in the previous case when
/// reaches the last element of the tuple thanks to enable_if
template <std::size_t idx, class Tuple>
    typename std::enable_if <
    idx<std::tuple_size<Tuple>::value - 1>::type read_tuple(std::istream &in, Tuple &out, const char delimiter) {
	std::string cell;
	std::getline(in, cell, delimiter);
	std::stringstream cell_stream(cell);
	cell_stream >> std::get<idx>(out);
	read_tuple<idx + 1, Tuple>(in, out, delimiter);
}
}  // namespace csvtools

/**
 * Iterable csv wrapper around a stream. @p fields the list of types that form up a row.
 * @tparam Tuple
 * @tparam selidx an integer pack denoting the column indices to include in the read tuples
 */
template <class Tuple, int... selidx>
class csv {
public:
	constexpr static size_t _num_columns = sizeof...(selidx);
	using header_tuple = std::array<std::string, _num_columns>;
	using selected_indices_t = std::integer_sequence<int, selidx...>;
	using value_type = std::tuple<typename std::tuple_element<selidx, Tuple>::type...>;
	class iterator;

	/// Construct from a stream.
	inline csv(std::istream &in, const char delim, const bool header) : _in(in), _delim(delim), _header(header) {
		if (_header) {
			// if there is a header line, store column names
			std::string row;
			std::getline(_in, row);
			std::stringstream row_stream(row);
			auto all_columns = std::array<std::string, std::tuple_size_v<Tuple>>();
			csvtools::read_tuple<0>(row_stream, all_columns, _delim);
			_columns = subset_array(all_columns, selected_indices_t{});
		}
	}

	/// Status of the underlying stream
	/// @{
	inline bool good() const { return _in.good(); }
	inline const std::istream &underlying_stream() const { return _in; }
	/// @}

	inline const header_tuple &columns() { return _columns; }

	constexpr static size_t num_columns() { return _num_columns; }

	inline iterator begin();
	inline iterator end();

private:
	std::istream &_in;
	const char _delim;
	bool _header = false;
	header_tuple _columns;

	/// Reads a line into a stringstream, and then reads the line into a tuple, that is returned
	inline value_type read_row() {
		std::string line;
		std::getline(_in, line);
		//        std::cout << line << std::endl;
		if (line.empty()) {
			value_type empty;
			_in.setstate(std::ios_base::eofbit);
			return empty;
		}
		std::stringstream line_stream(line);
		Tuple retval;
		csvtools::read_tuple<0>(line_stream, retval, _delim);
		return subset_tuple<Tuple, selidx...>(retval);
	}
};

/// Iterator; just calls recursively @ref csv::read_row and stores the result.
template <class Tuple, int... selidx>
class csv<Tuple, selidx...>::iterator {
	csv::value_type _row;
	csv *_parent;

public:
	typedef std::input_iterator_tag iterator_category;
	typedef csv::value_type value_type;
	typedef std::size_t difference_type;
	typedef csv::value_type *pointer;
	typedef csv::value_type &reference;

	/// Construct an empty/end iterator
	inline iterator() : _parent(nullptr) {}
	/// Construct an iterator at the beginning of the @p parent csv object.
	inline iterator(csv &parent) : _parent(parent.good() ? &parent : nullptr) { ++(*this); }

	inline iterator(iterator &other) {
		//        std::cout << "copy ctor" << std::endl;
		_row = other._row;
		_parent = other._parent;
	}

	inline iterator(iterator &&other) {
		//        std::cout << "move ctor" << std::endl;
		_row = std::move(other._row);
		_parent = other._parent;
	}

	/// Read one row, if possible. Set to end if parent is not good anymore.
	inline iterator &operator++() {
		if (_parent != nullptr) {
			_row = _parent->read_row();
			if (!_parent->good()) {
				_parent = nullptr;
			}
		}
		return *this;
	}

	inline iterator operator++(int) {
		iterator copy = std::move(*this);
		++(*this);
		return copy;
	}

	inline csv::value_type const &operator*() const { return _row; }

	inline csv::value_type const *operator->() const { return &_row; }

	bool operator==(iterator const &other) {
		return (this == &other) || (_parent == nullptr && other._parent == nullptr);
	}
	bool operator!=(iterator const &other) { return !(*this == other); }
};

template <class Tuple, int... selidx>
typename csv<Tuple, selidx...>::iterator csv<Tuple, selidx...>::begin() {
	return iterator(*this);
}

template <class Tuple, int... selidx>
typename csv<Tuple, selidx...>::iterator csv<Tuple, selidx...>::end() {
	return iterator();
}
}  // namespace movetk::io::csv
#endif /* csv_h */
