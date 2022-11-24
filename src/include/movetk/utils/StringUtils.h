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

/*! @file StringUtils.h
 *  @brief  A collection of utility functions for strings
 *  @authors Aniket Mitra (aniket.mitra@here.com)
 */

#ifndef MOVETK_STRINGUTILS_H
#define MOVETK_STRINGUTILS_H

#include <algorithm>
#include <iterator>
#include <sstream>

#include "Asserts.h"
#include "Requirements.h"


/*!
 * @namespace movetk_support
 * @brief support system of movetk
 */
namespace movetk::utils {


/*!
 * @brief joins a set of input values
 * @tparam IteratorType - Iterator over a set of values
 * @param first - Iterator to the first value
 * @param beyond - Iterator to the last value
 * @param delim
 * @return the concatenated string
 */
template <std::random_access_iterator IteratorType>
std::string join(IteratorType first, IteratorType beyond, char delim = ',') {
	std::stringstream output;
	output << std::to_string(*first);
	IteratorType it = first;
	it++;
	for (; it != beyond; ++it) {
		output << delim << std::to_string(*it);
	}
	return output.str();
}

/*!
 * @brief splits a string into tokens
 * @tparam OutputIterator - an insert iterator
 * @param in - input string
 * @param iter - Iterator to where the value is to be inserted
 * @param delim
 */
template <utils::OutputIterator<std::string> OutputIterator>
void split(std::string &in, OutputIterator iter, char delim = ',') {
	unsigned long int prevIdx = 0;
	std::string::size_type currIdx;
	currIdx = in.find(delim, prevIdx);
	if (currIdx == std::string::npos) {
		*iter = in;
		return;
	}
	while (currIdx != std::string::npos) {
		*iter = in.substr(prevIdx, (currIdx - prevIdx));
		prevIdx = currIdx + 1;
		currIdx = in.find(delim, prevIdx);
	}
	*iter = in.substr(prevIdx);
}

/*!
 *
 * @tparam NT
 */
template <class NT>
struct cast {
	// NT operator()(std::string &str){}
};

/*!
 *
 */
template <>
struct cast<long double> {
	long double operator()(std::string &str) { return std::stold(str); }
};


}  // namespace movetk::utils

#endif  // MOVETK_STRINGUTILS_H
