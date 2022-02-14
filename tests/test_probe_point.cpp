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
using std::string;
#include <tuple>

#include "movetk/io/CategoricalField.h"
#include "movetk/io/ParseDate.h"
#include "movetk/io/SortByField.h"
#include "movetk/io/TuplePrinter.h"

struct ProbePointTests {
	movetk::io::ParseDate create_date(const std::string& data) {
		std::istringstream stream(data);
		movetk::io::ParseDate date;
		stream >> date;
		return date;
	}
};

TEST_CASE_METHOD(ProbePointTests, "ProbePoint can be sorted", "[probepoint]") {
	using ProbePoint = std::tuple<string, movetk::io::ParseDate, float>;

	struct {
		bool operator()(ProbePoint a, ProbePoint b) const { return std::get<1>(a) < std::get<1>(b); }
	} customLess;
	// Initialize dates
	auto d1 = create_date("2017-05-22 16:33:33");
	auto d2 = create_date("2017-05-21 16:44:44");
	auto d3 = create_date("2017-05-24 16:44:44");
	auto d4 = create_date("2017-05-23 16:44:34");

	ProbePoint p1 = {"abc", d1, 5.4};
	ProbePoint p2 = {"def", d2, 4.5};
	ProbePoint p3 = {"gg", d3, -1.5};
	ProbePoint p4 = {"hh", d4, 2.5};
	std::vector<ProbePoint> v = {p1, p2, p3, p4};
	std::sort(begin(v), end(v), customLess);

	REQUIRE(std::get<0>(v[0]) == "def");
	REQUIRE(std::get<0>(v[1]) == "abc");
	REQUIRE(std::get<0>(v[2]) == "hh");
	REQUIRE(std::get<0>(v[3]) == "gg");
}

TEST_CASE_METHOD(ProbePointTests, "ProbePoint with custom date format can be sorted", "[probepointcustomdate]") {
	using ProbePoint = std::tuple<string, movetk::io::ParseDate, float>;

	struct {
		bool operator()(ProbePoint a, ProbePoint b) const { return std::get<1>(a) < std::get<1>(b); }
	} customLess;

	auto d1 = create_date("2017-05-22 16:33:33");
	auto d2 = create_date("2017-05-21 16:44:44");

	//    REQUIRE( d2 < d1 );

	ProbePoint p1 = {"abc", d1, 5.4};
	ProbePoint p2 = {"def", d2, 4.5};
	std::vector<ProbePoint> v = {p1, p2};
	std::sort(begin(v), end(v), customLess);

	//    REQUIRE( std::get<1>(v[0]) <  std::get<1>(v[1]) );
	REQUIRE(std::get<0>(v[0]) == "def");
	REQUIRE(std::get<0>(v[1]) == "abc");
}

TEST_CASE_METHOD(ProbePointTests,
                 "HERE ProbePoint with custom date format can be sorted",
                 "[hereprobepointcustomdate]") {
	auto local_categorical_tag = []() {};  // Lambda's are unique types, so can be used for tagging
	using ProviderCategoricalField = movetk::io::CategoricalField<std::string, decltype(local_categorical_tag)>;
	using ProbePoint =
	    typename std::tuple<std::string, movetk::io::ParseDate, double, double, double, double, ProviderCategoricalField>;

	auto customLess = [](ProbePoint a, ProbePoint b) { return std::get<2>(a) < std::get<2>(b); };

	auto d1 = create_date("2017-05-22 16:33:33");
	auto d2 = create_date("2017-05-21 16:44:44");

	ProviderCategoricalField c1;
	c1.add("Provider1");
	ProviderCategoricalField c2;
	c2.add("Provider2");
	ProviderCategoricalField c3;
	c3.add("Provider3");
	ProviderCategoricalField c4;
	c4.add("Provider4");
	ProviderCategoricalField c5;
	c5.add("Provider5");

	ProbePoint p1 = {"abc", d1, 5.4, 5.4, 5.4, 5.4, c1};
	ProbePoint p2 = {"def", d2, 4.5, 4.5, 4.5, 4.5, c2};
	std::vector<ProbePoint> v = {p1, p2};
	std::sort(begin(v), end(v), customLess);

	//    REQUIRE( std::get<2>(v[0]) <  std::get<2>(v[1]) );
	REQUIRE(std::get<0>(v[0]) == "def");
	REQUIRE(std::get<0>(v[1]) == "abc");
}

TEST_CASE_METHOD(ProbePointTests, "ProbePoint can be sorted by field", "[hereprobepointsortbyfield]") {
	auto local_categorical_tag = []() {};  // Lambda's are unique types, so can be used for tagging
	using ProviderCategoricalField = movetk::io::CategoricalField<std::string, decltype(local_categorical_tag)>;
	using ProbePoint =
	    typename std::tuple<std::string, movetk::io::ParseDate, double, double, double, double, ProviderCategoricalField>;
	constexpr int SAMPLE_DATE = 1;

	auto d1 = create_date("2017-05-22 16:33:33");
	auto d2 = create_date("2017-05-21 16:44:44");

	ProviderCategoricalField c1;
	c1.add("Provider1");
	ProviderCategoricalField c2;
	c2.add("Provider2");

	ProbePoint p1 = {"abc", d1, 5.4, 5.4, 5.4, 5.4, c1};
	ProbePoint p2 = {"def", d2, 4.5, 4.5, 4.5, 4.5, c2};
	std::vector<ProbePoint> v = {p1, p2};

	SECTION("sort by date in ascending order") {
		movetk::io::SortByField<SAMPLE_DATE, ProbePoint> asc_date;
		std::sort(begin(v), end(v), asc_date);

		REQUIRE(std::get<0>(v[0]) == "def");
		REQUIRE(std::get<0>(v[1]) == "abc");
	}

	SECTION("sort by date in descending order") {
		movetk::io::SortByField<SAMPLE_DATE, ProbePoint, false> desc_date;
		std::sort(begin(v), end(v), desc_date);

		REQUIRE(std::get<0>(v[1]) == "def");
		REQUIRE(std::get<0>(v[0]) == "abc");
	}
}
