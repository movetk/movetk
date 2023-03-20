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
// Created by Mitra, Aniket on 2019-07-04.
//


#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "movetk/Segmentation.h"
#include "movetk/ds/TabularTrajectory.h"
#include "movetk/metric/Norm.h"
#include "movetk/segmentation/BrownianBridge.h"
#include "movetk/utils/GeometryBackendTraits.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"

struct Example {
	static constexpr const char* NAME = "kinematic_interpolation";
	static constexpr const char* DESCRIPTION = R"(Model based Segmentation of a Trajectory
The model used is a Brownian Bridge Movement Model (BBMM)
Therefore, given a trajectory T = {(x_0,t_0), (x_1,t_1), (x_2,t_2), ... ,(x_n, t_n)}
where x is a position vector. The trajectory is partitioned into two sets
The even-numbered points are used 
to define Brownian bridges, and the odd-numbered points are use to 
estimate the diffusion coefficient and are assumed to be independent. 
The BBMM is defined with parameters mu(t) and sigma^2(t) 
where,
		mu(i) =  x(i) + alpha * ( x(i+1) - x(i) ) 
		sigma^2(i) =  (t(i+1) - t(i)) * alpha * (1 - alpha) * diffusion_coefficient^2 
		alpha = ( t(i+1) - t'(i) ) / ( t(i+1) - t(i) ) 
		where (x(i),t(i)) and (x(i+1), t(i+1)) are the even numbered points in the trajectory 
		and (x'(i),t'(i)) are the off numbered points in the trajectory 
Therefore, the likelihood of a candidate diffusion coefficient given a Brownian bridge T[i,i'] is 
		L(diffusion_coefficient^2 | T[i, i'] ) = ( 1/(2 * pi * diffusion_coefficient^2 )) * exp(- || x'(i) -
mu(i) || / 2 * diffusion_coefficient^2 ) 
Using Maximum Likelihood Estimation, the diffusion coefficient for each bridge is estimated 
Finally, given the bridges and their diffusion coefficients , the segmentation algorithm  
segments that trajectory in such a way that the information criteria of the segmentation is minimized 
Input Format: Lon, Lat, TS
Output Format: Lon, Lat, TS, SegID
Pre-Conditions:
	- Input sorted by Timestamp
)";
	void add_options(cxxopts::OptionAdder& adder) {
		adder("t,trajectory", "Trajectories file", cxxopts::value<std::string>());
		adder("head", "Specify this when the input file has a header");
		adder("i,indexes", "Position of the columns in the input", cxxopts::value<std::vector<size_t>>());
		adder("s,size",
		      "Size of the model (number diffusion coefficients to be selected. Should be between 1 and "
		      "(n-1)/2 ",
		      cxxopts::value<size_t>());
		adder("p,penalty", "Penalty factor", cxxopts::value<double>());
	}


	struct ProbeTraits {
		enum ProbeColumns { LAT, LON, SAMPLE_DATE };
	};

	template <typename Kernel>
	void run(cxxopts::ParseResult& arguments) {
		std::cerr << "Has Header?: " << arguments.count("head") << "\n";
		std::cout.setf(std::ios::fixed);

		std::vector<std::string> tokens;
		std::string line;
		std::vector<std::size_t> columns = arguments["indexes"].as<std::vector<size_t>>();
		assert(columns.size() == 3);
		using NT = typename Kernel::NT;
		NT num_coefficients = arguments["size"].as<size_t>();
		std::cerr << "Number of coefficients: " << num_coefficients << "\n";
		assert(num_coefficients > 0);

		NT penalty_factor = arguments["penalty_factory"].as<double>();
		std::size_t line_count = 0;
		NT lat, lon;
		std::size_t ts;
		using ProbePoint = std::tuple<long double, long double, std::size_t>;
		std::vector<ProbePoint> trajectory;
		bool is_stream = arguments.count("trajectory") == 0;
		bool has_header = arguments.count("head");
		if (is_stream) {
			while (std::getline(std::cin, line)) {
				tokens.clear();
				if (line_count == 0) {
					if (has_header) {
						line_count++;
						continue;
					}
				}
				movetk::utils::split(line, std::back_inserter(tokens));
				lon = std::stold(tokens[columns[0]]);
				lat = std::stold(tokens[columns[1]]);
				ts = static_cast<std::size_t>(std::stoul(tokens[columns[2]]));
				trajectory.push_back(std::make_tuple(lat, lon, ts));
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
				movetk::utils::split(line, std::back_inserter(tokens));
				lon = std::stold(tokens[columns[0]]);
				lat = std::stold(tokens[columns[1]]);
				ts = static_cast<std::size_t>(std::stoul(tokens[columns[2]]));
				trajectory.push_back(std::make_tuple(lat, lon, ts));
				line_count++;
			}
		}

		using ParameterTraits =
		    movetk::segmentation::brownian_bridge::ParameterTraits<Kernel, decltype(trajectory.begin())>;
		using BridgeIterator = std::vector<typename ParameterTraits::Parameters>::const_iterator;
		using Projection = movetk::geo::LocalCoordinateReference<NT>;
		std::vector<typename ParameterTraits::Parameters> bridges;
		using Norm = typename Kernel::Norm;

		using BBMM = movetk::segmentation::brownian_bridge::Model<Kernel, ProbeTraits, ParameterTraits, Norm, Projection>;

		BBMM bb(trajectory.begin(), trajectory.end(), std::back_inserter(bridges));

		std::vector<NT> selected_coeffs;

		// Maximum likelihood estimator
		movetk::segmentation::brownian_bridge::MLE<Kernel, ParameterTraits, Norm, BridgeIterator, 1000> mle(
		    std::cbegin(bridges),
		    std::cend(bridges));

		for (auto bit = begin(bridges); bit != end(bridges); ++bit) {
			movetk::segmentation::brownian_bridge::MLE<Kernel, ParameterTraits, Norm, BridgeIterator, 1000>
			    mle(bit, bit + 1);
			std::get<ParameterTraits::ParameterColumns::SIGMA_SQUARED>(*bit) = mle();
		}


		movetk::segmentation::brownian_bridge::ParameterSelector<Kernel, ParameterTraits> selector(
		    num_coefficients);

		selector(std::cbegin(bridges), std::cend(bridges), std::back_inserter(selected_coeffs));


		using LogLikelihood =
		    movetk::segmentation::brownian_bridge::LogLikelihood<Kernel, ParameterTraits, Norm>;

		using ModelBasedSegmentation = movetk::segmentation::ModelBasedSegmentation<Kernel, LogLikelihood>;

		ModelBasedSegmentation segmentation(penalty_factor);

		std::vector<BridgeIterator> segments;

		segmentation(std::cbegin(bridges),
		             std::cend(bridges),
		             std::cbegin(selected_coeffs),
		             std::cend(selected_coeffs),
		             std::back_inserter(segments));


		std::reverse(std::begin(segments), std::end(segments));

		movetk::utils::SegmentIdGenerator make_segment(std::begin(segments), std::end(segments));


		std::size_t id = 0;
		for (auto bit = std::begin(bridges); bit != std::end(bridges); ++bit) {
			auto first = std::get<ParameterTraits::ParameterColumns::BEGIN>(*bit);
			auto beyond = std::get<ParameterTraits::ParameterColumns::END>(*bit);

			for (auto it = first; it != beyond; ++it) {
				std::cout << std::get<1>(*it) << "," << std::get<0>(*it) << "," << std::get<2>(*it) << "," << id << "\n";
			}
			id = make_segment.getSegmentID(bit);
		}
		make_segment.reset();
	}
};

int main(int argc, char** argv) {
	return movetk::examples::ExampleRunner().run_example<Example>(argc, argv);
}
