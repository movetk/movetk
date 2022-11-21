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

#include "movetk/utils/GeometryBackendTraits.h"
#include "movetk/utils/Iterators.h"

class ParseInput {
public:
	static void show_usage(std::string &name) {
		std::cerr << "Usage: " << name << " <option(s)>\n"
		          << "Description: Simplifies a trajectory\n"
		          << "Input Format: Id,TS,Lon,Lat,X,Y\n"
		          << "Output Format: Id,TS,Lon,Lat,X,Y\n"
		          << "Pre-Conditions:\n"
		          << "\t - Input sorted by Timestamp\n"
		          << "Options:\n"
		          << "\t-h,--help\t\tShow this help message\n"
		          << "\t-eps,--epsilon\t\tSimplification threshold\n";
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
					if (strcmp(*it, std::get<0>(*eit).c_str()) == 0 || strcmp(*it, std::get<1>(*eit).c_str()) == 0) {
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

private:
	static const int MinArgs = 3;
	static const int MaxArgs = 3;

	typedef std::tuple<std::string, std::string, bool> earg;
	std::vector<earg> eargs{
	    std::make_tuple("-eps", "--epsilon", true) /*,
	        std::make_tuple("-p", "--polygon", true),
	        std::make_tuple("-c", "--centroids", true),
	        std::make_tuple("-r", "--resolutions", true),
	        std::make_tuple("-t", "--thresholds", true),
	        std::make_tuple("-f", "--outfile", true)*/
	};

	std::map<std::string, std::string> params{
	    {"-eps", ""}
	    /*{"-tr", ""},
	        {"-p",  ""},
	        {"-c",  ""},
	        {"-r",  ""},
	        {"-t",  ""},
	        {"-f",  ""}*/
	};
};

int main(int argc, char **argv) {
#if CGAL_BACKEND_ENABLED
	std::cerr << "Using CGAL Backend for Geometry\n";
#else
	std::cerr << "Using Boost Backend for Geometry\n";
#endif

	ParseInput parse;
	if (!parse(argc, argv))
		return 0;

	std::ios_base::sync_with_stdio(false);
	std::cout.setf(std::ios::fixed);
	std::string line;
	movetk::geom::MakePoint<GeometryKernel::MovetkGeometryKernel> make_point;
	using PolyLine = std::vector<GeometryKernel::MovetkGeometryKernel::MovetkPoint>;
	PolyLine polyline;
	std::vector<std::string> tokens, input;
	std::size_t LonIdx = 2, LatIdx = 3, XIdx = 4, YIdx = 5;
	GeometryKernel::NT X, Y;
	std::string key = "-eps";
	GeometryKernel::NT epsilon = std::stold(parse.get_parameter(key));

	while (getline(std::cin, line)) {
		input.push_back(line);
		movetk::utils::split(line, std::back_inserter(tokens));
		X = stold(tokens[XIdx]);
		Y = stold(tokens[YIdx]);
		polyline.push_back(make_point({X, Y}));
		tokens.clear();
	}
	std::vector<PolyLine::const_iterator> result;
	using FindFarthest = movetk::simplification::FindFarthest<GeometryKernel::MovetkGeometryKernel, GeometryKernel::Norm>;
	movetk::simplification::DouglasPeucker<GeometryKernel::MovetkGeometryKernel, FindFarthest> DouglasPeucker(epsilon);
	DouglasPeucker(std::begin(polyline), std::end(polyline), std::back_inserter(result));
	std::cerr << "Simplified polyline has: " << result.size() << " vertices\n";
	std::cout << "Id,TS,Lon,Lat,X,Y\n";
	auto cit = std::cbegin(input);
	auto rit = std::cbegin(result);
	rit++;
	std::cout << *cit << std::endl;
	while (rit != std::cend(result)) {
		std::size_t dist = std::distance(*(rit - 1), *rit);
		cit = cit + dist;
		std::cout << *cit << std::endl;
		rit++;
	}
	return 0;
}