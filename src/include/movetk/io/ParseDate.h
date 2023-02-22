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
// Created by onur on 9/22/18.
// Modified by Custers, Bram on 2020-02-08.
//

#ifndef MOVETK_PARSEDATE_H
#define MOVETK_PARSEDATE_H

#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

namespace movetk::io {
class ParseDate {
protected:
	std::time_t _ts;
	std::string _date_format;

public:
	explicit ParseDate(std::time_t ts = 0, std::string date_format = "%Y-%m-%d")
	    : _ts(ts)
	    , _date_format(std::move(date_format)) {}

	/**
	 * @brief (Implicitly) Construct a ParseData from a timestamp
	 * @param timestamp The timestamp
	 */
	ParseDate(std::time_t timestamp) : _ts(timestamp) {}

	//    ParseDate(const ParseDate&) = default;
	//    ParseDate(ParseDate&&) = default;
	//    ParseDate& operator=(const ParseDate&) = default;
	//    ParseDate& operator=(ParseDate&&) = default;
	//    ~ParseDate() = default;

	inline void ts(std::time_t ts) { this->_ts = ts; }
	inline std::time_t ts() const { return _ts; }
	double operator-(const ParseDate &rhs) const {
		return std::difftime(_ts, rhs.ts());  // time_end, time_beg
	}

	ParseDate operator+(std::size_t delta) const {
		return ParseDate(_ts + delta, this->_date_format);  // time_end, time_beg
	}

	bool operator<(const ParseDate &rhs) const { return _ts < rhs._ts; }
	bool operator>(const ParseDate &rhs) const { return rhs < *this; }
	bool operator<=(const ParseDate &rhs) const { return !(rhs < *this); }
	bool operator>=(const ParseDate &rhs) const { return !(*this < rhs); }
	bool operator==(const ParseDate &rhs) const {
		return *this - rhs ==
		       0;  // On POSIX, time_t is int, so should work. Otherwise may be comparing doubles for equality.
	}

	friend std::istream &operator>>(std::istream &is, ParseDate &date) {
		if (!date._date_format.empty()) {
			std::tm _tm = {};
			is >> std::get_time(&_tm, date._date_format.c_str());
			if (is.fail()) {
				std::cout << "Parse date failed\n";
				return is;
			}
			date.ts(std::mktime(&_tm));
		}

		return is;
	}

	friend std::ostream &operator<<(std::ostream &os, const ParseDate &date) {
		if (!date._date_format.empty()) {
			auto ts = date.ts();
			os << std::put_time(std::gmtime(&ts), date._date_format.c_str());  // "%c"
		} else {
			os << date._ts;
		}
		return os;
	}
};
}  // namespace movetk::io
#endif  // MOVETK_PARSEDATE_H
