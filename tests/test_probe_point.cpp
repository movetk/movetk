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

#include "movetk/io/SortByField.h"
#include "movetk/io/TuplePrinter.h"
#include "movetk/io/csv/ParseDate.h"
#include "movetk/utils/HereProbeTraits.h"

struct ProbePointTests {
	ParseDate create_date(const std::string& data) {
		std::istringstream stream(data);
		ParseDate date;
		stream >> date;
		return date;
	}
};

TEST_CASE_METHOD(ProbePointTests, "ProbePoint can be sorted", "[probepoint]") {
	using ProbePoint = std::tuple<string, ParseDate, float>;

	struct {
		bool operator()(ProbePoint a, ProbePoint b) const { return std::get<1>(a) < std::get<1>(b); }
	} customLess;
	// Initialize dates
	ParseDate d1 = create_date("2017-05-22 16:33:33");
	ParseDate d2 = create_date("2017-05-21 16:44:44");
	ParseDate d3 = create_date("2017-05-24 16:44:44");
	ParseDate d4 = create_date("2017-05-23 16:44:34");

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

TEST_CASE("ProbePoint with custom date format can be sorted", "[probepointcustomdate]") {
	using here::c2d::raw::ProbeParseDate;
	using ProbePoint = std::tuple<string, ProbeParseDate, float>;

	struct {
		bool operator()(ProbePoint a, ProbePoint b) const { return std::get<1>(a) < std::get<1>(b); }
	} customLess;

	ProbeParseDate d1;
	std::istringstream dis1("2017-05-22 16:33:33");
	dis1 >> d1;
	ProbeParseDate d2;
	std::istringstream dis2("2017-05-21 16:44:44");
	dis2 >> d2;

	//    REQUIRE( d2 < d1 );

	ProbePoint p1 = {"abc", d1, 5.4};
	ProbePoint p2 = {"def", d2, 4.5};
	std::vector<ProbePoint> v = {p1, p2};
	std::sort(begin(v), end(v), customLess);

	//    REQUIRE( std::get<1>(v[0]) <  std::get<1>(v[1]) );
	REQUIRE(std::get<0>(v[0]) == "def");
	REQUIRE(std::get<0>(v[1]) == "abc");
}

TEST_CASE("HERE ProbePoint with custom date format can be sorted", "[hereprobepointcustomdate]") {
	using here::c2d::raw::ProbeParseDate;
	using here::c2d::raw::ProbePoint;
	using here::c2d::raw::ProviderCategoricalField;

	struct {
		bool operator()(ProbePoint a, ProbePoint b) const { return std::get<2>(a) < std::get<2>(b); }
	} customLess;

	ProbeParseDate d1;
	std::istringstream dis1("2017-05-22 16:33:33");
	dis1 >> d1;
	ProbeParseDate d2;
	std::istringstream dis2("2017-05-21 16:44:44");
	dis2 >> d2;

	ProviderCategoricalField c1;
	std::istringstream cis1("Provider1");
	cis1 >> c1;
	ProviderCategoricalField c2;
	std::istringstream cis2("Provider2");
	cis2 >> c2;
	ProviderCategoricalField c3;
	std::istringstream cis3("Provider3");
	cis3 >> c3;
	ProviderCategoricalField c4;
	std::istringstream cis4("Provider4");
	cis4 >> c4;
	ProviderCategoricalField c5;
	std::istringstream cis5("Provider5");
	cis5 >> c5;

	ProbePoint p1 = {"abc", d1, 5.4, 5.4, 5.4, 5.4, c1};
	ProbePoint p2 = {"def", d2, 4.5, 4.5, 4.5, 4.5, c2};
	std::vector<ProbePoint> v = {p1, p2};
	std::sort(begin(v), end(v), customLess);

	//    REQUIRE( std::get<2>(v[0]) <  std::get<2>(v[1]) );
	REQUIRE(std::get<0>(v[0]) == "def");
	REQUIRE(std::get<0>(v[1]) == "abc");
}

TEST_CASE("HERE ProbePoint can be sorted by field", "[hereprobepointsortbyfield]") {
	using here::c2d::raw::ProbeParseDate;
	using here::c2d::raw::ProbePoint;
	using here::c2d::raw::ProviderCategoricalField;
	constexpr int SAMPLE_DATE = here::c2d::raw::ProbeColumns::SAMPLE_DATE;

	ProbeParseDate d1;
	std::istringstream dis1("2017-05-22 16:33:33");
	dis1 >> d1;
	ProbeParseDate d2;
	std::istringstream dis2("2017-05-21 16:44:44");
	dis2 >> d2;

	ProviderCategoricalField c1;
	std::istringstream cis1("Provider1");
	cis1 >> c1;
	ProviderCategoricalField c2;
	std::istringstream cis2("Provider2");
	cis2 >> c2;

	ProbePoint p1 = {"abc", d1, 5.4, 5.4, 5.4, 5.4, c1};
	ProbePoint p2 = {"def", d2, 4.5, 4.5, 4.5, 4.5, c2};
	std::vector<ProbePoint> v = {p1, p2};

	SECTION("sort by date in ascending order") {
		movetk::io::SortByField<SAMPLE_DATE, here::c2d::raw::ProbeTraits::ProbePoint> asc_date;
		std::sort(begin(v), end(v), asc_date);

		//    REQUIRE( std::get<SAMPLE_DATE>(v[0]) <  std::get<SAMPLE_DATE>(v[1]) );
		REQUIRE(std::get<0>(v[0]) == "def");
		REQUIRE(std::get<0>(v[1]) == "abc");
	}

	SECTION("sort by date in descending order") {
		movetk::io::SortByField<SAMPLE_DATE, here::c2d::raw::ProbeTraits::ProbePoint, false> desc_date;
		std::sort(begin(v), end(v), desc_date);

		//    REQUIRE( std::get<SAMPLE_DATE>(v[0]) <  std::get<SAMPLE_DATE>(v[1]) );
		REQUIRE(std::get<0>(v[1]) == "def");
		REQUIRE(std::get<0>(v[0]) == "abc");
	}
}
