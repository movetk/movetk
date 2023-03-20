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
// Created by Mitra, Aniket on 2019-03-03.
//

#include "movetk/Simplification.h"

#include <iostream>
#include <map>
#include <vector>

#include "ExampleSetup.h"
#include "movetk/utils/GeometryBackendTraits.h"
#include "movetk/utils/Iterators.h"

struct Example {
	static constexpr const char *NAME = "simplification";
	static constexpr const char *DESCRIPTION =
	    R"(Simplifies a trajectory using Douglas-Peucker, read from the standard input.
Input Format: Id,TS,Lon,Lat,X,Y
Output Format: Id,TS,Lon,Lat,X,Y
Pre-Conditions:
- Input sorted by Timestamp
)";

	void add_options(cxxopts::OptionAdder &adder) {
		adder("e,epsilon", "Simplification threshold", cxxopts::value<double>());
	}


	template <typename Kernel>
	void run(cxxopts::ParseResult &arguments) {
		using NT = typename Kernel::NT;
		std::ios_base::sync_with_stdio(false);
		std::cout.setf(std::ios::fixed);
		std::string line;
		movetk::geom::MakePoint<Kernel> make_point;
		using PolyLine = std::vector<typename Kernel::MovetkPoint>;
		PolyLine polyline;
		std::vector<std::string> tokens, input;
		std::size_t LonIdx = 2, LatIdx = 3, XIdx = 4, YIdx = 5;
		NT X, Y;
		NT epsilon = arguments["epsilon"].as<NT>();

		while (getline(std::cin, line)) {
			input.push_back(line);
			movetk::utils::split(line, std::back_inserter(tokens));
			X = stold(tokens[XIdx]);
			Y = stold(tokens[YIdx]);
			polyline.push_back(make_point({X, Y}));
			tokens.clear();
		}
		std::vector<typename PolyLine::const_iterator> result;
		using FindFarthest = movetk::simplification::FindFarthest<Kernel, typename Kernel::Norm>;
		movetk::simplification::DouglasPeucker<Kernel, FindFarthest> douglas_peucker(epsilon);
		douglas_peucker(std::begin(polyline), std::end(polyline), std::back_inserter(result));

		std::cout << "Simplified polyline has: " << result.size() << " vertices\n";
		std::cout << "Id,TS,Lon,Lat,X,Y\n";
		auto cit = std::cbegin(input);
		auto rit = std::cbegin(result);
		rit++;
		std::cout << *cit << std::endl;
		for (; rit != std::cend(result); ++rit) {
			std::size_t dist = std::distance(*(rit - 1), *rit);
			cit = cit + dist;
			std::cout << *cit << std::endl;
			rit++;
		}
	}
};

int main(int argc, char **argv) {
	return movetk::examples::ExampleRunner().run_example<Example>(argc, argv);
}