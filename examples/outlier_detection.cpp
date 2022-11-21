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
// Created by Mitra, Aniket on 2019-08-14.
//


#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "movetk/OutlierDetection.h"
#include "movetk/ds/TabularTrajectory.h"
#include "movetk/io/CartesianProbeTraits.h"
#include "movetk/metric/Norm.h"
#include "movetk/outlierdetection/OutlierDetectionPredicates.h"
#include "movetk/utils/GeometryBackendTraits.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"


using namespace std;

class ParseInput {
public:
	static void show_usage(std::string &name) {
		std::cerr << "Usage: " << name << " <option(s)>\n"
		          << "Description: to be filled \n"
		          << "Input Format: X,Y,TS\n"
		          << "Pre-Conditions:\n"
		          << "\t - Input sorted by Timestamp\n"
		          << "Options:\n"
		          << "\t-h,--help\t\tShow this help message\n"
		          << "\t-tr,--trajectory\t\tTrajectories file\n"
		          << "\t--head\t\t\tSpecify this when the input file has header\n"
		          << "\t-idx,--indexes\t\tPosition of columns in the input\n"
		          << "\t-eps,--epsilon\t\tThreshold\n";
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
	                        std::make_tuple("-eps", "--epsilon", true)};

	std::map<std::string, std::string> params{{"-tr", ""}, {"-eps", ""}, {"-idx", ""}, {"--head", ""}};

	void set_flags(std::string arg) {
		if (arg == "--head")
			header = true;
		if (arg == "-tr" || (arg == "--trajectory"))
			stream = false;
	}
};

using MovetkGeometryKernel = typename GeometryKernel::MovetkGeometryKernel;
using Norm = typename GeometryKernel::Norm;

int main(int argc, char **argv) {
#if CGAL_BACKEND_ENABLED
	std::cerr << "Using CGAL Backend for Geometry\n";
#else
	std::cerr << "Using Boost Backend for Geometry\n";
#endif

	std::ios_base::sync_with_stdio(false);
	std::cout.setf(std::ios::fixed);

	ParseInput parse;
	if (!parse(argc, argv))
		return 0;

	std::cerr << "Has Header?: " << parse.has_header() << "\n";

	std::ios_base::sync_with_stdio(false);
	std::cout.setf(std::ios::fixed);

	std::vector<string> tokens;
	std::string key = "-idx";
	string line;
	line = parse.get_parameter(key);
	movetk::utils::split(line, std::back_inserter(tokens));
	assert(tokens.size() == 3);

	std::vector<std::size_t> col_idx;
	std::transform(std::cbegin(tokens), std::cend(tokens), std::back_insert_iterator(col_idx), [](auto i) {
		return static_cast<std::size_t>(std::stoul(i)) - 1;
	});

	key = "-eps";
	MovetkGeometryKernel::NT threshold = std::stold(parse.get_parameter(key));

	std::cerr << "Threshold: " << threshold << "\n";

	using CartesianProbeTraits = movetk::io::ProbeTraits<MovetkGeometryKernel>;

	namespace od = movetk::outlierdetection;

	using Probe = CartesianProbeTraits::ProbePoint;
	using Trajectory = std::vector<CartesianProbeTraits::ProbePoint>;
	using Trajectories = std::vector<Trajectory>;
	using OutlierDetectionTraits =
	    movetk::outlierdetection::OutlierDetectionTraits<CartesianProbeTraits, MovetkGeometryKernel, Norm>;
	using Test =
	    od::TEST<od::linear_speed_bounded_test_tag, movetk::algo::cartesian_coordinates_tag, OutlierDetectionTraits>;
	static_assert(std::is_same_v<typename Test::NT, long double>);
	const auto test_val = static_cast<long double>(0.5);
	Test test(test_val);
	using OutlierDetector = od::OutlierDetection<OutlierDetectionTraits, Test, od::output_sensitive_outlier_detector_tag>;
	/* typedef movetk::algo::OutlierDetection<movetk::algo::greedy_outlier_detector_tag,
	         Test, OutlierDetectionTraits> OutlierDetector;*/
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;


	MovetkGeometryKernel::NT ts, x, y;
	Trajectory trajectory;
	std::size_t line_count = 0;

	std::vector<std::string> input;
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
			x = std::stold(tokens[col_idx[0]]);
			y = std::stold(tokens[col_idx[1]]);
			ts = static_cast<std::size_t>(std::stoul(tokens[col_idx[2]]));
			trajectory.push_back(make_tuple(make_point({x, y}), ts));
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
			x = std::stold(tokens[col_idx[0]]);
			y = std::stold(tokens[col_idx[1]]);
			ts = static_cast<std::size_t>(std::stoul(tokens[col_idx[2]]));
			trajectory.push_back(make_tuple(make_point({x, y}), ts));
			line_count++;
		}
	}


	std::vector<Trajectory::const_iterator> result;
	OutlierDetector outlier_detector(threshold);
	outlier_detector(std::cbegin(trajectory), std::cend(trajectory), std::back_inserter(result));
	std::reverse(std::begin(result), std::end(result));
	std::size_t num_outliers = trajectory.size() - result.size();
	std::cerr << "Number of Outliers Detected: " << num_outliers;
	std::cerr << "\n";
	auto cit = std::cbegin(input);
	auto prev = *std::cbegin(result);
	for (auto traj_it : result) {
		// std::cout << std::get<0>(*traj_it);
		// std::cout << "\n";
		std::size_t dist = std::distance(traj_it, std::cbegin(trajectory));
		if (!(dist == 0)) {
			dist = std::distance(prev, traj_it);
		}
		cit = cit + dist;
		prev = traj_it;
		std::cout << *cit << std::endl;
	}
}
