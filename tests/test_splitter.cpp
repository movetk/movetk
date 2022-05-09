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


#include <catch2/catch.hpp>
#include <iostream>
#include <string>

#include "movetk/geo/geo.h"
#include "movetk/io/ParseDate.h"
#include "movetk/io/ProbeTraits.h"
#include "movetk/io/SplitByDifferenceThreshold.h"
#include "movetk/io/SplitByDistanceThreshold.h"
#include "movetk/io/SplitByField.h"
#include "movetk/io/Splitter.h"
#include "movetk/io/TuplePrinter.h"


TEST_CASE("Splitter", "[splitter]") {
	auto create_probe_parsedate = [](const std::string& str) {
		movetk::io::ParseDate date(0, "%Y-%m-%d %H:%M:%S");
		std::istringstream stream(str);
		stream >> date;
		return date;
	};

	auto d0 = create_probe_parsedate("2018-09-18 00:58:02");
	auto d1 = create_probe_parsedate("2018-09-18 00:58:12");
	auto d2 = create_probe_parsedate("2018-09-18 00:58:22");
	auto d3 = create_probe_parsedate("2018-09-18 00:58:42");
	auto d4 = create_probe_parsedate("2018-09-18 00:58:52");
	auto d5 = create_probe_parsedate("2018-09-18 00:58:57");

	using Row = std::tuple<std::string, std::string, int, float, movetk::io::ParseDate, float>;
	Row row0{"abc", "def", 5, 1.2, d0, 50.2};
	Row row1{"aaa", "jkl", 4, 2.5, d1, 50.3};
	Row row2{"aaa", "bbb", 3, 3.4, d2, 50.4};
	Row row3{"ddd", "ttt", 2, 4.3, d3, 50.5};
	Row row4{"eee", "vvv", 1, 5.4, d4, 50.6};
	Row row5{"eee", "vvv", 0, 6.6, d5, 50.7};

	std::array<Row, 6> probe_points = {row0, row1, row2, row3, row4, row5};

	SECTION("Split by field") {
		constexpr std::size_t expected_split_count = 4;
		std::array<std::string, expected_split_count> expected_field_values = {"abc", "aaa", "ddd", "eee"};
		std::array<std::size_t, expected_split_count> expected_split_size = {1, 2, 1, 2};

		using SplitByField0 = movetk::io::SplitByField<0, Row>;
		using ProbeInputIterator = decltype(probe_points.begin());
		movetk::io::Splitter<SplitByField0, ProbeInputIterator> splitter(probe_points.begin(), probe_points.end());

		size_t i = 0;
		for (const auto& rows : splitter) {
			REQUIRE(i < expected_split_count);
			REQUIRE(std::get<0>(rows[0]) == expected_field_values[i]);
			REQUIRE(rows.size() == expected_split_size[i]);
			i++;
		}
	}

	SECTION("Split by difference threshold") {
		std::array<std::string, 6> expected_field_values = {"abc", "aaa", "eee", "eee"};
		std::array<std::size_t, 6> expected_split_size = {1, 3, 1, 1};
		int expected_split_count = 4;

		using SplitByDiff3 = movetk::io::SplitByDifferenceThreshold<3, Row>;
		SplitByDiff3 split_by_diff3(1.0);
		using ProbeInputIterator = decltype(probe_points.begin());
		movetk::io::Splitter<SplitByDiff3, ProbeInputIterator> splitter(probe_points.begin(),
		                                                                probe_points.end(),
		                                                                split_by_diff3);

		int i = 0;
		for (const auto& rows : splitter) {
			REQUIRE(std::get<0>(rows[0]) == expected_field_values[i]);
			REQUIRE(rows.size() == expected_split_size[i]);
			i++;
		}
		REQUIRE(i == expected_split_count);
	}

	SECTION("Split by time difference threshold") {
		std::array<std::string, 6> expected_field_values = {"abc", "ddd"};
		std::array<std::size_t, 6> expected_split_size = {3, 3};
		int expected_split_count = 2;

		using SplitByDiff = movetk::io::SplitByDifferenceThreshold<4, Row>;
		SplitByDiff split_by_diff(15.0);
		using ProbeInputIterator = decltype(probe_points.begin());
		movetk::io::Splitter<SplitByDiff, ProbeInputIterator> splitter(probe_points.begin(),
		                                                               probe_points.end(),
		                                                               split_by_diff);

		int i = 0;
		for (const auto& rows : splitter) {
			REQUIRE(std::get<0>(rows[0]) == expected_field_values[i]);
			REQUIRE(rows.size() == expected_split_size[i]);
			i++;
		}
		REQUIRE(i == expected_split_count);
	}

	SECTION("Split by distance threshold") {
		using SplitByDist = movetk::io::SplitByDistanceThreshold<3, 5, Row>;
		std::function<double(float, float, float, float)> distancefn = movetk::geo::distance_exact;
		SplitByDist split_by_dist(110000.0, distancefn);
		using ProbeInputIterator = decltype(probe_points.begin());
		movetk::io::Splitter<SplitByDist, ProbeInputIterator> splitter(probe_points.begin(),
		                                                               probe_points.end(),
		                                                               split_by_dist);

		int i = 0;
		for ([[maybe_unused]] const auto& rows : splitter) {
			//            REQUIRE( std::get<0>(rows[0]) == expected_field_values[i] );
			//            REQUIRE( rows.size() == expected_split_size[i] );
			i++;
		}
		//        REQUIRE( i == expected_split_count );
	}
}