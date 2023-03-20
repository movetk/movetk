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
// Created by Mitra, Aniket on 2019-02-21.
//

#include "ExampleSetup.h"
#include "movetk/Simplification.h"
#include "movetk/utils/GeometryBackendTraits.h"
#include "movetk/utils/Iterators.h"

struct Example {
	static constexpr const char* NAME = "douglas_peucker_simplification";
	static constexpr const char* DESCRIPTION = "Computes statistics on a trajectory";

	void add_options(cxxopts::OptionAdder& adder) {
		adder("e,epsilon",
		      "Maximum allowed distance before requiring a more detailed simplification",
		      cxxopts::value<long double>()->default_value("10.0"));
	}

	template <typename Kernel>
	void run(cxxopts::ParseResult& arguments) {
		movetk::geom::MakePoint<Kernel> make_point;
		using PolyLine = std::vector<typename Kernel::MovetkPoint>;
		PolyLine polyline({make_point({-6.19, -3.46}),
		                   make_point({-4.99, 1.16}),
		                   make_point({-2.79, -2.22}),
		                   make_point({-1.87, 0.58}),
		                   make_point({0.77, 0.22}),
		                   make_point({-1.15, 3.06}),
		                   make_point({5.33, -1.12})});

		std::cout << "Polyline to be simplified: \n";
		std::cout << "{\n";
		for (auto vertex : polyline) {
			std::cout << vertex << ";\n";
		}
		std::cout << "}\n";

		std::vector<typename PolyLine::const_iterator> result;
		using Norm = movetk::metric::L2Norm<Kernel>;
		using FindFarthest = movetk::simplification::FindFarthest<Kernel, Norm>;
		const auto epsilon = static_cast<typename Kernel::NT>(arguments["epsilon"].as<long double>());
		movetk::simplification::DouglasPeucker<Kernel, FindFarthest> douglas_peucker(epsilon);
		douglas_peucker(std::begin(polyline), std::end(polyline), std::back_inserter(result));


		std::cout << "Simplified polyline has: " << result.size() << " vertices\n";
		std::cout << "Simplified Polyline: {\n";
		for (auto reference : result) {
			std::cout << *reference << ";\n";
		}
		std::cout << "}\n";
	}
};

int main(int argc, char** argv) {
	return movetk::examples::ExampleRunner().run_example<Example>(argc, argv);
}
