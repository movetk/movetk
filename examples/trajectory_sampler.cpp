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
// Created by Mitra, Aniket on 29/10/2019.
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

using MovetkGeometryKernel = typename GeometryKernel::MovetkGeometryKernel;
using Norm = typename GeometryKernel::Norm;
using namespace std;

class ParseInput {
public:
	static void show_usage(std::string &name) {
		std::cerr << "Usage: " << name << " <option(s)>\n"
		          << "Description: to be filled \n"
		          << "Input Format: TS, LAT, LON, SPEED, HEADING\n"
		          << "Pre-Conditions:\n"
		          << "\t - Input sorted by Timestamp\n"
		          << "Options:\n"
		          << "\t-h,--help\t\tShow this help message\n"
		          << "\t-tr,--trajectory\t\tTrajectories file\n"
		          << "\t--head\t\t\tSpecify this when the input file has header\n"
		          << "\t-idx,--indexes\t\tPosition of columns in the input\n"
		          << "\t-t,--speed_threshold\t\tThreshold\n";
	}

	bool operator()(int argc, char **argv) {
		std::string executable = argv[0];
		if ((argc >= MinArgs) && (argc <= MaxArgs)) {
			auto it = argv;
			it++;
			while (it != (argv + argc)) {
				bool Matched = false;
				auto eit = eargs.cbegin();
				while (eit != eargs.cend()) {
					if ((std::get<0>(*eit) == *it) || (std::get<1>(*eit) == *it)) {
						Matched = true;
						break;
					}
					eit++;
				}
				if (Matched) {
					if (std::get<2>(*eit)) {
						params[std::get<0>(*eit)] = *(it + 1);
						it = it + 2;
					} else
						it++;
					set_flags(std::get<0>(*eit));
					eargs.erase(eit);
				} else {
					show_usage(executable);
					return false;
				}
			}
			return true;
		}
		show_usage(executable);
		return false;
	}

	std::string &get_parameter(std::string &key) { return params[key]; }

	bool has_header() { return header; }

	bool is_stream() { return stream; }

private:
	static const int MinArgs = 4;
	static const int MaxArgs = 9;
	bool header = false, stream = true;
	typedef std::tuple<std::string, std::string, bool> earg;
	std::vector<earg> eargs{std::make_tuple("--head", "--head", false),
	                        std::make_tuple("-tr", "--trajectory", true),
	                        std::make_tuple("-idx", "--indexes", true),
	                        std::make_tuple("-t", "--speed_threshold", true)};

	std::map<std::string, std::string> params{{"-tr", ""}, {"-t", ""}, {"-idx", ""}, {"--head", ""}};

	void set_flags(std::string arg) {
		if (arg == "--head")
			header = true;
		if (arg == "-tr" || (arg == "--trajectory"))
			stream = false;
	}
};


struct ProbeTraits {
	enum ProbeColumns { SAMPLE_DATE, LAT, LON, HEADING, SPEED };
	typedef MovetkGeometryKernel::NT NT;
	typedef std::tuple<movetk::io::ParseDate, NT, NT, NT, NT> ProbePoint;
};


int main(int argc, char **argv) {
#if CGAL_BACKEND_ENABLED
	std::cerr << "Using CGAL Backend for Geometry\n";
#else
	std::cerr << "Using Boost Backend for Geometry\n";
#endif

	std::ios_base::sync_with_stdio(false);
	std::cout.setf(std::ios::fixed);
	string date_format = "%Y-%m-%d %H:%M:%S";
	ParseInput parse;
	if (!parse(argc, argv))
		return 0;

	std::cerr << "Has Header?: " << parse.has_header() << "\n";


	std::vector<string> tokens;
	std::string key = "-idx";
	string line;
	line = parse.get_parameter(key);
	movetk::utils::split(line, std::back_inserter(tokens));
	assert(tokens.size() == 5);

	std::size_t ts_idx = stold(tokens[0]);
	std::size_t lat_idx = stold(tokens[1]);
	std::size_t lon_idx = stold(tokens[2]);
	std::size_t speed_idx = stold(tokens[3]);
	std::size_t heading_idx = stold(tokens[4]);

	std::vector<std::size_t> col_idx;
	std::transform(std::cbegin(tokens), std::cend(tokens), std::back_insert_iterator(col_idx), [](auto i) {
		return static_cast<std::size_t>(std::stoul(i)) - 1;
	});

	key = "-t";
	MovetkGeometryKernel::NT threshold = std::stold(parse.get_parameter(key)) * 0.277778;

	std::cerr << "Threshold: " << threshold << "\n";


	Norm norm;
	typedef movetk::geo::LocalCoordinateReference<typename MovetkGeometryKernel::NT> Projection;
	typedef movetk::interpolation::InterpolationTraits<MovetkGeometryKernel, Projection, ProbeTraits, Norm>
	    InterpolationTraits;
	/*typedef movetk::algo::Interpolator<movetk::algo::kinematic_interpolator_tag,
	        InterpolationTraits> Interpolator;*/
	typedef movetk::interpolation::Interpolator<movetk::interpolation::random_trajectory_generator_tag,
	                                            InterpolationTraits,
	                                            ProbeTraits::ProbeColumns::LAT,
	                                            ProbeTraits::ProbeColumns::LON,
	                                            ProbeTraits::ProbeColumns::SAMPLE_DATE,
	                                            ProbeTraits::ProbeColumns::SPEED,
	                                            ProbeTraits::ProbeColumns::HEADING>
	    Interpolator;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	std::size_t line_count = 0;
	std::vector<std::string> input;
	std::vector<ProbeTraits::ProbePoint> trajectory;
	if (parse.is_stream()) {
		while (getline(cin, line)) {
			tokens.clear();
			if (line_count == 0) {
				if (parse.has_header()) {
					line_count++;
					continue;
				}
			}
			input.push_back(line);
			movetk::utils::split(line, std::back_inserter(tokens));
			MovetkGeometryKernel::NT lat = std::stold(tokens[lat_idx]);
			MovetkGeometryKernel::NT lon = std::stold(tokens[lon_idx]);
			movetk::io::ParseDate dt(0, date_format);
			std::istringstream is(tokens[ts_idx]);
			is >> dt;
			MovetkGeometryKernel::NT heading = std::stold(tokens[heading_idx]);
			MovetkGeometryKernel::NT speed = std::stold(tokens[speed_idx]) * 0.277778;
			trajectory.push_back(make_tuple(dt, lat, lon, heading, speed));
			line_count++;
		}

	} else {
		key = "-tr";
		std::string trajfile = parse.get_parameter(key);
		ifstream infile;
		infile.open(trajfile);
		while (getline(infile, line)) {
			tokens.clear();
			if (line_count == 0) {
				if (parse.has_header()) {
					line_count++;
					continue;
				}
			}
			input.push_back(line);
			movetk::utils::split(line, std::back_inserter(tokens));
			MovetkGeometryKernel::NT lat = std::stold(tokens[lat_idx]);
			MovetkGeometryKernel::NT lon = std::stold(tokens[lon_idx]);
			movetk::io::ParseDate dt(0, date_format);
			std::istringstream is(tokens[ts_idx]);
			is >> dt;
			MovetkGeometryKernel::NT heading = std::stold(tokens[heading_idx]);
			MovetkGeometryKernel::NT speed = std::stold(tokens[speed_idx]) * 0.277778;
			trajectory.push_back(make_tuple(dt, lat, lon, heading, speed));
			line_count++;
		}
	}

	auto pit = std::cbegin(trajectory);
	MovetkGeometryKernel::NT lat0 = std::get<ProbeTraits::ProbeColumns::LAT>(*pit);
	MovetkGeometryKernel::NT lon0 = std::get<ProbeTraits::ProbeColumns::LON>(*pit);
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
		auto result = std::back_inserter(interpolated_pts);
		// result = p_u;
		// interpolator(p_u, p_v, std::begin(ts), std::end(ts), result);
		interpolator(p_u, p_v, std::begin(ts), std::end(ts), threshold, result);
		/* if ( pit == std::cend(trajectory) - 1 ){
		     result = p_v;
		 }*/

		auto it = std::begin(interpolated_pts);
		while (it != std::end(interpolated_pts) - 1) {
			std::cout << std::get<ProbeTraits::ProbeColumns::SAMPLE_DATE>(*it) << ",";
			std::cout << std::get<ProbeTraits::ProbeColumns::LAT>(*it) << ",";
			std::cout << std::get<ProbeTraits::ProbeColumns::LON>(*it) << ",";
			std::cout << std::get<ProbeTraits::ProbeColumns::HEADING>(*it) << ",";
			std::cout << std::get<ProbeTraits::ProbeColumns::SPEED>(*it) * 3.6 << "\n";
			it++;
		}
	}
}