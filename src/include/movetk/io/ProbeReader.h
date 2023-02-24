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
// Created by onur on 9/21/18.
//

#ifndef MOVETK_PROBEREADER_H
#define MOVETK_PROBEREADER_H

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>  // for move

#include "movetk/io/csv/csv.h"
#include "movetk/utils/text.h"  // for ends_with

/**
 * @namespace movetk::io
 * @brief Contains functionality for reading and writing trajectories
 */
namespace movetk::io {

namespace concepts {
template <typename TRAITS>
concept ProbeReaderTraits = requires() {
	typename TRAITS::ProbeCsv;
	typename TRAITS::ProbeInputIterator;
	requires std::input_iterator<typename TRAITS::ProbeInputIterator>;
};
}  // namespace concepts

template <class ProbeTraits>
class ProbeReader {
public:
	typedef typename ProbeTraits::ProbeCsv ProbeCsv;
	typedef typename ProbeTraits::ProbeInputIterator ProbeInputIterator;

	explicit ProbeReader(std::unique_ptr<std::istream> in) : _in(std::move(in)) {}

	virtual ProbeInputIterator begin() = 0;

	inline virtual ProbeInputIterator end() { return ProbeInputIterator(); }

	inline auto columns() { return _table->columns(); }

	virtual ~ProbeReader() = default;

protected:
	std::unique_ptr<std::istream> _in;
	std::unique_ptr<ProbeCsv> _table = nullptr;
};

template <class ProbeTraits>
class CsvProbeReader : public ProbeReader<ProbeTraits> {
public:
	typedef typename ProbeTraits::ProbeCsv ProbeCsv;
	typedef typename ProbeTraits::ProbeInputIterator ProbeInputIterator;

	explicit CsvProbeReader(std::unique_ptr<std::istream> in) : ProbeReader<ProbeTraits>(std::move(in)) {
		this->_table = std::make_unique<ProbeCsv>(*this->_in, ProbeTraits::delimiter, ProbeTraits::header);
	}

	inline virtual ProbeInputIterator begin() { return ProbeInputIterator(*this->_table); }
};

template <class ProbeTraits>
class ZippedProbeReader : public ProbeReader<ProbeTraits> {
public:
	typedef typename ProbeTraits::ProbeCsv ProbeCsv;
	typedef typename ProbeTraits::ProbeInputIterator ProbeInputIterator;

	explicit ZippedProbeReader(std::unique_ptr<std::istream> in) : ProbeReader<ProbeTraits>(std::move(in)) {
		flt_in.push(boost::iostreams::gzip_decompressor());
		flt_in.push(*this->_in);
		this->_table = std::make_unique<ProbeCsv>(flt_in, ProbeTraits::delimiter, ProbeTraits::header);
	}

	inline virtual ProbeInputIterator begin() { return ProbeInputIterator(*this->_table); }

private:
	boost::iostreams::filtering_stream<boost::iostreams::input> flt_in;
};

class ProbeReaderFactory {
public:
	static constexpr std::string_view CSV_EXT = ".csv";
	static constexpr std::string_view GZ_EXT = ".gz";
	template <class ProbeTraits>
	static std::unique_ptr<movetk::io::ProbeReader<ProbeTraits>> create(const std::string& file_name) {
		// TODO: check if file exists
		if (file_name.ends_with(CSV_EXT)) {
			auto fin = std::make_unique<std::ifstream>(file_name);
			return std::make_unique<CsvProbeReader<ProbeTraits>>(std::move(fin));
		} else if (file_name.ends_with(GZ_EXT)) {
			auto fin = std::make_unique<std::ifstream>(file_name, std::ios_base::in | std::ios_base::binary);
			return std::make_unique<ZippedProbeReader<ProbeTraits>>(std::move(fin));
		} else {
			throw std::invalid_argument("Unsupported probe file extension!");
		}
	}

	template <class ProbeTraits>
	static std::unique_ptr<movetk::io::ProbeReader<ProbeTraits>> create_from_string(const char* csv_string) {
		auto ss = std::make_unique<std::istringstream>(csv_string);
		return std::make_unique<CsvProbeReader<ProbeTraits>>(std::move(ss));
	}
};
}  // namespace movetk::io
#endif  // MOVETK_PROBEREADER_H
