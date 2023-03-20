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
		auto write_section_start = [](const std::string& title) { std::cout << "\n==== " << title << '\n'; };
		{
			write_section_start("Create points and a line");

			// Create the constructor functor for ponits
			movetk::geom::MakePoint<Kernel> make_point;
			// Create points by supplying coordinates
			auto p1 = make_point({1, 1});
			std::cout << "Point 1:\n" << p1 << '\n';

			auto p2 = make_point({2, 2});
			std::cout << "Point 2:\n" << p2 << '\n';

			movetk::geom::MakeLine<Kernel> make_line;
			auto line = make_line(p1, p2);
			// todo(bram): Output
		}
		// Reuse pointer constructor for the rest of the examples
		movetk::geom::MakePoint<Kernel> make_point;
		{
			write_section_start("Create a minimum enclosing ball for a list of points");
			//==============================
			// Declare the data structure that will hold a set of points
			// One can use any stl container that supports a random access
			// iterator
			std::vector<typename Kernel::MovetkPoint> points;

			points.push_back(make_point({1, 1}));
			points.push_back(make_point({2, 2}));
			points.push_back(make_point({3, 3}));
			std::cout << "Points:\n";
			for (size_t i = 0; i < points.size(); ++i) {
				std::cout << "Point " << i << ": " << points[i] << std::endl;
			}

			// Compute the minimum enclosing sphere/ball
			movetk::geom::MakeMinSphere<Kernel> min_sphere;
			std::vector<typename Kernel::NT> min_sphere_center_coordinates;
			const auto radius =
			    min_sphere(std::cbegin(points), std::cend(points), std::back_inserter(min_sphere_center_coordinates));
			const auto center = make_point(min_sphere_center_coordinates.begin(), min_sphere_center_coordinates.begin());

			std::cout << "Enclosing ball radius: \n" << radius << std::endl;
			std::cout << "Center: " << std::endl;
			std::cout << center << '\n';
			std::cout << "\n";
		}
		{
			write_section_start("Create a sphere");
			std::array<typename Kernel::NT, Kernel::dim> p;
			p = {3, 2};

			const auto center = make_point(std::cbegin(p), std::cend(p));
			std::cout << "Center\n" << center << "\n";

			movetk::geom::MakeSphere<Kernel> make_sphere;
			const auto sphere = make_sphere(center, 10);
			std::cout << "Sphere:\n" << sphere << '\n';
		}
		{
			write_section_start("Create a segment and compute its length");
			auto pt1 = make_point({1, 1});
			auto pt2 = make_point({2, 2});
			movetk::geom::MakeSegment<Kernel> make_segment;
			auto seg = make_segment(pt1, pt2);
			std::cout << "Segment: " << seg << std::endl;
			movetk::geom::ComputeLength<Kernel> compute_length;
			std::cout << "Length of the Segment: " << compute_length(seg) << std::endl;
		}
		{
			write_section_start("Create a Polygon");

			// Points for the polygon
			std::vector<typename Kernel::MovetkPoint> points;
			points.push_back(make_point({1, 1}));
			points.push_back(make_point({2, 2}));
			points.push_back(make_point({3, 3}));

			std::cout << "Points:\n";
			for (size_t i = 0; i < points.size(); ++i) {
				std::cout << "Point " << i << ": " << points[i] << std::endl;
			}

			// Create the polygon
			movetk::geom::MakePolygon<Kernel> make_polygon;
			const auto polygon = make_polygon(std::cbegin(points), std::cend(points));

			std::cout << "Polygon: \n" << polygon << std::endl;
		}
	}
};

int main(int argc, char** argv) {
	return movetk::examples::ExampleRunner().run_example<Example>(argc, argv);
}
