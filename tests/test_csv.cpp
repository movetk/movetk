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

#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

#include "catch2/catch.hpp"
#include "movetk/io/csv/csv.h"


TEST_CASE("CSV is read", "[csv]") {
	namespace csv = movetk::io::csv;
	using Row = std::tuple<std::string, std::string, int, float, double>;
	Row row0{"abc", "def", 5, 1.2, 33.13};
	Row row1{"ghi", "jkl", 4, 2.5, 11.30};
	Row row2{"aaa", "bbb", 3, 3.4, 21.20};
	Row row3{"ddd", "ttt", 2, 4.3, 12.10};
	Row row4{"eee", "vvv", 1, 5.4, 44.14};
	std::array<Row, 5> expected = {row0, row1, row2, row3, row4};

	Row empty{"", "", 0, 0.0f, 0.0};

	SECTION("CSV is read") {
		std::stringstream in(
		    "abc, def, 5, 1.2, 33.13\n"
		    "ghi, jkl, 4, 2.5, 11.30\n"
		    "aaa, bbb, 3, 3.4, 21.20\n"
		    "ddd, ttt, 2, 4.3, 12.10\n"
		    "eee, vvv, 1, 5.4, 44.14\n");

		csv::csv<Row, 0, 1, 2, 3, 4> table(in, ',', false);

		int i = 0;
		for (const auto& row : table) {
			REQUIRE(row == expected[i]);
			i++;
		}
		REQUIRE(i == 5);
	}

	SECTION("Line without new line is not read") {
		std::stringstream in(
		    "abc, def, 5, 1.2, 33.13\n"
		    "ghi, jkl, 4, 2.5, 11.30\n"
		    "aaa, bbb, 3, 3.4, 21.20\n"
		    "ddd, ttt, 2, 4.3, 12.10\n"
		    "eee, vvv, 1, 5.4, 44.14");

		csv::csv<Row, 0, 1, 2, 3, 4> table(in, ',', false);

		int i = 0;
		for (const auto& row : table) {
			REQUIRE(i < expected.size());
			REQUIRE(row == expected[i]);
			i++;
		}
		REQUIRE(i == 4);
	}

	SECTION("CSV with header is read") {
		std::array<std::string, 5> expected_cols = {"col1", "col2", "col3", "col4", "col5"};
		std::stringstream in(
		    "col1,col2,col3,col4,col5\n"
		    "abc, def, 5, 1.2, 33.13\n"
		    "ghi, jkl, 4, 2.5, 11.30\n"
		    "aaa, bbb, 3, 3.4, 21.20\n"
		    "ddd, ttt, 2, 4.3, 12.10\n"
		    "eee, vvv, 1, 5.4, 44.14\n");

		csv::csv<Row, 0, 1, 2, 3, 4> table(in, ',', true);

		int i = 0;
		for (const auto& row : table) {
			REQUIRE(row == expected[i]);
			i++;
		}
		REQUIRE(i == 5);

		REQUIRE(table.columns() == expected_cols);
	}

	SECTION("CSV with missing values is read") {
		std::stringstream in(",,,,\n");

		csv::csv<Row, 0, 1, 2, 3, 4> table(in, ',', false);

		int i = 0;
		for (const auto& row : table) {
			REQUIRE(row == empty);
			i++;
		}
		REQUIRE(i == 1);
	}

	SECTION("Can get a subset of columns from CSV") {
		using ReducedRow = std::tuple<std::string, float, double>;
		ReducedRow rrow0{"def", 1.2, 33.13};
		ReducedRow rrow1{"jkl", 2.5, 11.30};
		ReducedRow rrow2{"bbb", 3.4, 21.20};
		ReducedRow rrow3{"ttt", 4.3, 12.10};
		ReducedRow rrow4{"vvv", 5.4, 44.14};
		std::array<ReducedRow, 5> expected = {rrow0, rrow1, rrow2, rrow3, rrow4};

		std::stringstream in(
		    "col1,col2,col3,col4,col5\n"
		    "abc, def, 5, 1.2, 33.13\n"
		    "ghi, jkl, 4, 2.5, 11.30\n"
		    "aaa, bbb, 3, 3.4, 21.20\n"
		    "ddd, ttt, 2, 4.3, 12.10\n"
		    "eee, vvv, 1, 5.4, 44.14\n");

		csv::csv<Row, 1, 3, 4> table(in, ',', true);

		std::array<std::string, 3> expected_header = {"col2", "col4", "col5"};
		auto cols = table.columns();
		REQUIRE(std::get<0>(cols) == expected_header[0]);
		REQUIRE(std::get<1>(cols) == expected_header[1]);
		REQUIRE(std::get<2>(cols) == expected_header[2]);

		int i = 0;
		for (const auto& row : table) {
			REQUIRE(row == expected[i]);
			i++;
		}
		REQUIRE(i == 5);
	}

	//    SECTION( "Types are correct" ) {
	//        using Row2 = std::tuple<std::string, std::string, int, std::string, double>;
	//        Row2 _row0 {"abc", "def", 5, "xyz", 33.13};
	//        Row2 _row1 {"ghi", "jkl", 4, "xyz", 11.30};
	//        Row2 _row2 {"aaa", "bbb", 3, "xyz", 21.20};
	//        Row2 _row3 {"ddd", "ttt", 2, "xyz", 12.10};
	//        Row2 _row4 {"eee", "vvv", 1, "xyz", 44.14};
	//        std::array<Row2, 5> expected2 = {_row0, _row1, _row2, _row3, _row4};
	//
	//
	//        std::stringstream in(
	//                "abc, def, 5, xyz, 33.13\n"
	//                "ghi, jkl, 4, xyz, 11.30\n"
	//                "aaa, bbb, 3, xyz, 21.20\n"
	//                "ddd, ttt, 2, xyz, 12.10\n"
	//                "eee, vvv, 1, xyz, 44.14\n"
	//        );
	//
	//        csv<std::string, std::string, int, float, double> table(in, ',', false);
	//
	//        int i = 0;
	//        for (const auto& row : table) {
	//            REQUIRE( row == expected2[i] );
	//            i++;
	//        }
	//        REQUIRE( i == 5 );
	//    }
}
