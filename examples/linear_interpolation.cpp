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
// Created by Mitra, Aniket on 08/11/2019.
//

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "movetk/Interpolation.h"
#include "movetk/geo/geo.h"
#include "movetk/io/ParseDate.h"
#include "movetk/utils/GeometryBackendTraits.h"

struct Example {
	static constexpr const char *NAME = "linear_interpolation";
	static constexpr const char *DESCRIPTION = R"(Computes the linear interpolation between points of a trajectory.
Description: to be filled 
Input Format: TS, LAT, LON, SPEED, HEADING
Pre-Conditions:
- Input sorted by Timestamp
)";
	void add_options(cxxopts::OptionAdder &adder) {
		adder("f,trajectory_file", "Trajectories file", cxxopts::value<std::string>());
		adder("head", "Specify if the file has a header");
		adder("i,indexes", "Position of columns in the input", cxxopts::value<std::vector<std::size_t>>());
		adder("t,speed_threshold", "Threshold", cxxopts::value<double>());
	}

	template <typename Kernel>
	void run(cxxopts::ParseResult &arguments) {
		std::cout.setf(std::ios::fixed);
		std::string date_format = "%Y-%m-%d %H:%M:%S";
		std::cerr << "Has Header?: " << arguments.count("head") << "\n";


		std::vector<std::string> tokens;
		std::string line;
		auto columns = arguments["indexes"].as<std::vector<std::size_t>>();
		assert(columns.size() == 5);

		std::size_t ts_idx = columns[0];
		std::size_t lat_idx = columns[1];
		std::size_t lon_idx = columns[2];
		std::size_t speed_idx = columns[3];
		std::size_t heading_idx = columns[4];

		using Norm = typename Kernel::Norm;
		using NT = typename Kernel::NT;
		struct ProbeTraits {
			enum ProbeColumns { SAMPLE_DATE, LAT, LON, HEADING, SPEED };
			using NT = typename Kernel::NT;
			using ProbePoint = std::tuple<movetk::io::ParseDate, NT, NT, NT, NT>;
		};
		Norm norm;
		using Projection = movetk::geo::LocalCoordinateReference<typename Kernel::NT>;
		using InterpolationTraits = movetk::interpolation::InterpolationTraits<Kernel, Projection, ProbeTraits, Norm>;
		using Interpolator = movetk::interpolation::Interpolator<movetk::interpolation::linear_interpolator_tag,
		                                                         InterpolationTraits,
		                                                         ProbeTraits::ProbeColumns::LAT,
		                                                         ProbeTraits::ProbeColumns::LON,
		                                                         ProbeTraits::ProbeColumns::SAMPLE_DATE,
		                                                         ProbeTraits::ProbeColumns::SPEED,
		                                                         ProbeTraits::ProbeColumns::HEADING>;
		std::size_t line_count = 0;
		std::vector<std::string> input;
		std::vector<typename ProbeTraits::ProbePoint> trajectory;

		NT threshold = arguments["speed_threshold"].as<double>() * 0.277778;
		std::cerr << "Threshold: " << threshold << "\n";
		bool has_header = arguments.count("head");
		bool is_stream = arguments.count("trajectory_file") == 0;
		if (is_stream) {
			while (std::getline(std::cin, line)) {
				tokens.clear();
				if (line_count == 0) {
					if (has_header) {
						line_count++;
						continue;
					}
				}
				input.push_back(line);
				movetk::utils::split(line, std::back_inserter(tokens));
				NT lat = std::stold(tokens[lat_idx]);
				NT lon = std::stold(tokens[lon_idx]);
				movetk::io::ParseDate dt(0, date_format);
				std::istringstream is(tokens[ts_idx]);
				is >> dt;
				NT heading = std::stold(tokens[heading_idx]);
				NT speed = std::stold(tokens[speed_idx]) * 0.277778;
				trajectory.push_back(std::make_tuple(dt, lat, lon, heading, speed));
				line_count++;
			}

		} else {
			std::string trajfile = arguments["trajectory_file"].as<std::string>();
			std::ifstream infile;
			infile.open(trajfile);
			while (std::getline(infile, line)) {
				tokens.clear();
				if (line_count == 0) {
					if (has_header) {
						line_count++;
						continue;
					}
				}
				input.push_back(line);
				movetk::utils::split(line, std::back_inserter(tokens));
				NT lat = std::stold(tokens[lat_idx]);
				NT lon = std::stold(tokens[lon_idx]);
				movetk::io::ParseDate dt(0, date_format);
				std::istringstream is(tokens[ts_idx]);
				is >> dt;
				NT heading = std::stold(tokens[heading_idx]);
				NT speed = std::stold(tokens[speed_idx]) * 0.277778;
				trajectory.push_back(std::make_tuple(dt, lat, lon, heading, speed));
				line_count++;
			}
		}

		auto pit = std::cbegin(trajectory);
		NT lat0 = std::get<ProbeTraits::ProbeColumns::LAT>(*pit);
		NT lon0 = std::get<ProbeTraits::ProbeColumns::LON>(*pit);
		Interpolator interpolator(lat0, lon0);
		std::cout << "SAMPLE_DATE, LAT, LON, HEADING, SPEED\n";
		while (++pit != std::cend(trajectory)) {
			std::vector<typename ProbeTraits::ProbePoint> interpolated_pts;
			std::vector<movetk::io::ParseDate> ts;
			auto p_u = *(pit - 1);
			auto p_v = *pit;
			auto ts_u = std::get<ProbeTraits::ProbeColumns::SAMPLE_DATE>(p_u);
			auto ts_v = std::get<ProbeTraits::ProbeColumns::SAMPLE_DATE>(p_v);
			std::size_t tdiff = ts_v - ts_u;
			std::size_t idx = 1;
			ts.push_back(ts_u);
			while (idx != tdiff) {
				ts.push_back(ts_u + idx);
				idx++;
			}
			ts.push_back(ts_v);
			interpolator(p_u, p_v, std::begin(ts), std::end(ts), std::back_inserter(interpolated_pts));
			auto it = std::begin(interpolated_pts);
			while (it != std::end(interpolated_pts)) {
				std::cout << std::get<ProbeTraits::ProbeColumns::SAMPLE_DATE>(*it) << ",";
				std::cout << std::get<ProbeTraits::ProbeColumns::LAT>(*it) << ",";
				std::cout << std::get<ProbeTraits::ProbeColumns::LON>(*it) << ",";
				std::cout << std::get<ProbeTraits::ProbeColumns::HEADING>(*it) << ",";
				std::cout << std::get<ProbeTraits::ProbeColumns::SPEED>(*it) * 3.6 << "\n";
				it++;
			}
		}
	}
};


int main(int argc, char **argv) {
	return movetk::examples::ExampleRunner().run_example<Example>(argc, argv);
}