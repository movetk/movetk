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

/*! @file Iterators.h
 *  @brief  A collection of iterators for movetk
 *  @details A collection of custom iterators for movetk that are
 *  models of std::iterator_traits
 *  @authors Aniket Mitra (aniket.mitra@here.com)
 */

#ifndef MOVETK_ITERATORS_H
#define MOVETK_ITERATORS_H

#include <iostream>
#include <iterator>

#include "movetk/utils/Requirements.h"

/*!
 * @brief Collection of utilities used in parts of the MoveTK library
 */
namespace movetk::utils {
/*!
 * @brief an adaptor to std::iterator for for inserting at the end of a container
 * @details unlike the std::back_inserter, ths iterator has a value_type.
 * Increment operator has no effect. The assignment operator extend the size of the container
 * by inserting an element at the end and returns an iterator to the end
 * @tparam Container
 */
template <class Container>
class movetk_back_insert_iterator : public std::iterator<std::output_iterator_tag, void, void, void, void> {
protected:
	Container *container;

public:
	typedef Container container_type;
	typedef typename Container::value_type value_type;
	typedef void difference_type;
	typedef void reference;
	typedef void pointer;

	/*!
	 *
	 * @param x
	 */
	explicit movetk_back_insert_iterator(Container &x) : container(std::addressof(x)) {}

	/*!
	 *
	 * @param value
	 * @return
	 */
	movetk_back_insert_iterator &operator=(const typename Container::value_type &value) {
		container->push_back(value);
		return *this;
	}

	movetk_back_insert_iterator &operator-=(const typename Container::value_type &value) {
		container->pop_back();
		container->push_back(value);
		return *this;
	}

	/*!
	 *
	 * @return
	 */
	movetk_back_insert_iterator &operator*() { return *this; }

	/*!
	 *
	 * @return
	 */
	movetk_back_insert_iterator &operator++() { return *this; }

	/*!
	 *
	 * @return
	 */
	movetk_back_insert_iterator operator++(int) { return *this; }
};


template <class Container>
auto make_back_inserter(Container &container) {
	return movetk_back_insert_iterator<Container>(container);
}
/*!
 * @brief an adaptor to std::iterator for inserting into a container at a given position
 * @details unlike the std::inserter, ths iterator has a value_type.
 * Increment operator has no effect. The assignment operator extends the size of the container
 * by inserting an element at a given position
 * @tparam Container
 */
template <class Container>
class movetk_insert_iterator : public std::iterator<std::output_iterator_tag, void, void, void, void> {
protected:
	Container *container;
	typename Container::iterator iter;

public:
	typedef Container container_type;
	typedef typename Container::value_type value_type;
	typedef void difference_type;
	typedef void reference;
	typedef void pointer;

	explicit movetk_insert_iterator(Container &x, typename Container::iterator _iter)
	    : container(std::addressof(x))
	    , iter(_iter) {}

	movetk_insert_iterator &operator=(const typename Container::value_type &value) {
		iter = container->insert(iter, value);
		++iter;
		return *this;
	}

	movetk_insert_iterator &operator*() { return *this; }

	movetk_insert_iterator &operator++() { return *this; }

	movetk_insert_iterator operator++(int) { return *this; }
};

template <class T>
class movetk_basic_iterator : public std::iterator<std::random_access_iterator_tag, T, std::size_t, T *, T &> {
protected:
	T *it;

public:
	typedef typename movetk_basic_iterator::reference reference;
	typedef typename movetk_basic_iterator::pointer pointer;
	typedef typename movetk_basic_iterator::difference_type difference_type;
	typedef typename movetk_basic_iterator::value_type value_type;
	typedef typename movetk_basic_iterator::iterator iterator;

	explicit movetk_basic_iterator(pointer value) : it(value) {}

	pointer base() const { return it; }

	movetk_basic_iterator &operator=(const T *iter) {
		it = iter;
		return *this;
	}

	reference operator*() const { return *it; }

	pointer operator->() const { return it; }

	movetk_basic_iterator &operator++() {
		it++;
		return *this;
	}

	movetk_basic_iterator operator+(difference_type d) {
		movetk_basic_iterator iter(this->base() + d);
		return iter;
	}

	movetk_basic_iterator operator++(int) {
		movetk_basic_iterator iter(this->base());
		++(*this);
		return iter;
	}

	bool operator==(movetk_basic_iterator const &other) const { return (this->base() == other.base()); }

	bool operator!=(movetk_basic_iterator const &other) const { return (this->base() != other.base()); }

	auto operator-(movetk_basic_iterator const &other) const { return (this->base() - other.base()); }
};

namespace concepts {
template <typename T>
concept RandomAccessIterable = requires(const T &t) {
	typename T::const_iterator;
	{ t.begin() } -> std::random_access_iterator;
	{ t.end() } -> std::random_access_iterator;
};
}  // namespace concepts

template <concepts::RandomAccessIterable Container>
class movetk_grid_iterator : public std::iterator<std::random_access_iterator_tag,
                                                  Container,
                                                  std::size_t,
                                                  typename Container::const_iterator,
                                                  typename Container::reference> {
protected:
	typename Container::const_iterator rit;
	typename Container::const_iterator rit_end;
	typename Container::value_type::const_iterator cit;

public:
	using column_iterator = typename Container::value_type::const_iterator;
	using reference = typename movetk_grid_iterator::reference;
	using pointer = typename movetk_grid_iterator::pointer;
	using difference_type = typename movetk_grid_iterator::difference_type;

	movetk_grid_iterator(const Container &container, bool is_end = false) {
		rit_end = container.cend();

		if (!is_end) {
			rit = container.cbegin();
			cit = rit->cbegin();
			move_iterators_to_valid_state();
		} else {
			rit = container.cend();
		}
	}
	movetk_grid_iterator(const movetk_grid_iterator &other) = default;
	movetk_grid_iterator(movetk_grid_iterator &&other) = default;

	pointer base() const { return rit; }

	movetk_grid_iterator &operator++() {
		if (rit == rit_end) {
			throw std::runtime_error("Incrementing end iterator");
		}
		++cit;
		move_iterators_to_valid_state();
		return *this;
	}

	movetk_grid_iterator operator++(int) {
		movetk_grid_iterator iter(*this);
		++(*this);
		return iter;
	}

	movetk_grid_iterator operator+(difference_type d) const {
		movetk_grid_iterator iter(*this);
		std::advance(iter, d);
		return iter;
	}

	typename Container::value_type::value_type operator*() const { return *cit; }

	typename Container::value_type::const_iterator operator->() const { return cit; }

	bool operator==(movetk_grid_iterator const &other) const {
		return rit == other.rit && (rit == rit_end || cit == other.cit);
	}

	bool operator!=(const movetk_grid_iterator &other) const { return !(*this == other); }

	std::size_t operator-(movetk_grid_iterator const &other) const {
		auto it = other.base();
		std::size_t count = 0;
		while (it != this->base()) {
			auto jit = it->cbegin();
			while (jit != it->cend()) {
				count++;
				jit++;
			}
			it++;
		}
		return count;
	}

private:
	void move_iterators_to_valid_state() {
		if (cit == rit->cend()) {
			++rit;
			while (rit != rit_end) {
				if (rit->cbegin() == rit->cend()) {
					++rit;
				} else {
					break;
				}
			}
			if (rit != rit_end) {
				cit = rit->cbegin();
			}
		}
	}
};


/**
 * @brief An output iterator that will accumulate the assigned values.
 * @tparam ValueType The type of the value to accumulate
 */
template <typename ValueType>
class movetk_accumulator_output_iterator : public std::iterator<std::output_iterator_tag, void, void, void, void> {
protected:
	ValueType m_data;
	bool m_skip_first = false;
	bool m_accept_first = false;

public:
	using value_type = ValueType;
	using difference_type = void;
	using reference = void;
	using pointer = void;

	/*!
	 *
	 * @param x
	 */
	explicit movetk_accumulator_output_iterator(ValueType initial) : m_data(initial) {}

	/**
	 * @brief Returns the accumulated value
	 * @return The accumulated value
	 */
	ValueType value() const { return m_data; }
	/*!
	 *
	 * @param value
	 * @return
	 */
	movetk_accumulator_output_iterator &operator=(const value_type &value) {
		m_data += value;
		return *this;
	}

	movetk_accumulator_output_iterator &operator-=(const value_type &value) {
		m_data -= value;
		return *this;
	}

	/*!
	 *
	 * @return
	 */
	movetk_accumulator_output_iterator &operator*() { return *this; }

	/*!
	 *
	 * @return
	 */
	movetk_accumulator_output_iterator &operator++() { return *this; }

	/*!
	 *
	 * @return
	 */
	movetk_accumulator_output_iterator operator++(int) { return *this; }
};

}  // namespace movetk::utils
#endif  // MOVETK_ITERATORS_H
