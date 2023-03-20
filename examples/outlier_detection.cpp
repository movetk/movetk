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

#include "ExampleSetup.h"
#include "movetk/OutlierDetection.h"
#include "movetk/ds/TabularTrajectory.h"
#include "movetk/io/CartesianProbeTraits.h"
#include "movetk/metric/Norm.h"
#include "movetk/outlierdetection/OutlierDetectionPredicates.h"
#include "movetk/utils/GeometryBackendTraits.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"

struct Example {
	static constexpr const char *NAME = "outlier_detection";
	static constexpr const char *DESCRIPTION = R"(Detection of outliers within a trajectory
Input Format: X,Y,TS
Pre-Conditions:
- Input sorted by Timestamp
)";

	void add_options(cxxopts::OptionAdder &adder) {
		adder("t,trajectory", "Trajectory file", cxxopts::value<std::string>());
		adder("head", "Specify this when the input file has a header");
		adder("i,indexes", "Position of columns in the input", cxxopts::value<std::vector<size_t>>());
		adder("e,epsilon", "Threshold for outlier detection", cxxopts::value<double>());
	}


	template <typename Kernel>
	void run(cxxopts::ParseResult &arguments) {
		using NT = typename Kernel::NT;

		std::ios_base::sync_with_stdio(false);
		std::cout.setf(std::ios::fixed);

		const bool has_header = arguments.count("head");
		const bool is_stream = arguments.count("trajectory") == 0;

		std::cerr << "Has Header?: " << has_header << "\n";

		std::ios_base::sync_with_stdio(false);
		std::cout.setf(std::ios::fixed);

		std::vector<std::string> tokens;
		std::string line;
		const auto column_indices = arguments["indexes"].as<std::vector<size_t>>();
		assert(column_indices.size() == 3);

		const auto threshold = arguments["epsilon"].as<NT>();

		std::cerr << "Threshold: " << threshold << "\n";

		using CartesianProbeTraits = movetk::io::ProbeTraits<Kernel>;
		using Norm = typename Kernel::Norm;
		namespace od = movetk::outlierdetection;

		using Probe = CartesianProbeTraits::ProbePoint;
		using Trajectory = std::vector<CartesianProbeTraits::ProbePoint>;
		using Trajectories = std::vector<Trajectory>;
		using OutlierDetectionTraits = movetk::outlierdetection::OutlierDetectionTraits<CartesianProbeTraits, Kernel, Norm>;
		using Predicate =
		    od::TEST<od::linear_speed_bounded_test_tag, movetk::algo::cartesian_coordinates_tag, OutlierDetectionTraits>;
		const auto test_val = static_cast<long double>(0.5);
		Predicate test(test_val);
		using OutlierDetector =
		    od::OutlierDetection<OutlierDetectionTraits, Predicate, od::output_sensitive_outlier_detector_tag>;
		movetk::geom::MakePoint<Kernel> make_point;


		NT ts, x, y;
		Trajectory trajectory;
		std::size_t line_count = 0;

		std::vector<std::string> input;
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
				x = std::stold(tokens[column_indices[0]]);
				y = std::stold(tokens[column_indices[1]]);
				ts = static_cast<std::size_t>(std::stoul(tokens[column_indices[2]]));
				trajectory.push_back(std::make_tuple(make_point({x, y}), ts));
				line_count++;
			}

		} else {
			std::string trajfile = arguments["trajectory"].as<std::string>();
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
				x = std::stold(tokens[column_indices[0]]);
				y = std::stold(tokens[column_indices[1]]);
				ts = static_cast<std::size_t>(std::stoul(tokens[column_indices[2]]));
				trajectory.push_back(std::make_tuple(make_point({x, y}), ts));
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
};

int main(int argc, char **argv) {
	return movetk::examples::ExampleRunner().run_example<Example>(argc, argv);
}