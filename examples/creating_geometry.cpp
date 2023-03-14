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
// Created by Mitra, Aniket on 08/01/2019.
//

#include <vector>

#include "ExampleSetup.h"
#include "HereTrajectoryTraits.h"
#include "movetk/Statistics.h"
#include "movetk/geo/geo.h"
#include "movetk/io/ProbeReader.h"
#include "movetk/io/TrajectoryReader.h"
#include "movetk/utils/GeometryBackendTraits.h"
#include "test_data.h"

struct Example {
	static constexpr const char* NAME = "creating_geometry";
	static constexpr const char* DESCRIPTION = "Creating geometry for a Kernel";

	template <typename Kernel>
	void run(cxxopts::ParseResult& arguments) {
		auto write_section_start = [](const std::string& title) { std::cout << "==== " << title << '\n'; };
		{
			write_section_start("Creating points and a line");

			movetk::geom::MakePoint<Kernel> make_point;
			auto P1 = make_point({1, 1});
			std::cout << "Point 1:\n" << P1 << '\n';

			auto P2 = make_point({2, 2});
			std::cout << "Point 2:\n" << P1 << '\n';

			movetk::geom::MakeLine<Kernel> make_line;
			auto line = make_line(P1, P2);
			//todo(bram): Output
		}
		{
			write_section_start("Creating minimum enclosing ball");
			//==============================
			// Declare the data structure that will hold a set of points
			// One can use any stl container that supports a random access
			// iterator
			std::vector<typename Kernel::MovetkPoint> points;
			std::vector<typename Kernel::NT> CenterMinSphere;
			//===================================
			// Create a  Movetk min sphere and print it's center and radius
			movetk::geom::MakePoint<Kernel> make_point;


			points.push_back(make_point({1, 1}));
			std::cout << "Point 1:\n" << points.back() << std::endl;
			points.push_back(make_point({2, 2}));
			std::cout << "Point 2:\n" << points.back() << std::endl;
			points.push_back(make_point({3, 3}));
			std::cout << "Point 3:\n" << points.back() << std::endl;

			movetk::geom::MakeMinSphere<GeometryKernel::MovetkGeometryKernel> min_sphere;

			GeometryKernel::MovetkGeometryKernel::NT radius =
			    min_sphere(std::cbegin(points), std::cend(points), std::back_inserter(CenterMinSphere));

			std::cout << "Enclosing ball radius: \n" << radius << std::endl;

			std::cout << "Center: " << std::endl;
			for (auto it = std::cbegin(CenterMinSphere);; it != std::cend(CenterMinSphere); ++it) {
				std::cout << *it << ' ';
			}
			std::cout << "\n";
		}
	}
};

int main(int argc, char** argv) {
	return movetk::examples::ExampleRunner().run_example<Example>(argc, argv);
}
