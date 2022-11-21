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
using namespace std;

class ParseInput {
public:
	static void show_usage(std::string& name) {
		std::cerr
		    << "Usage: " << name << " <option(s)>\n"
		    << "Description: Model based Segmentation of a Trajectory \n"
		    << "The model used is a Brownian Bridge Movement Model (BBMM) \n"
		    << "Therefore, given a trajectory T = {(x_0,t_0), (x_1,t_1), (x_2,t_2), ... ,(x_n, t_n)} \n"
		    << "where x is a position vector. The trajectory is partitioned into two sets \n"
		    << "The even-numbered points are used \n"
		    << "to define Brownian bridges, and the odd-numbered points are use to \n "
		    << "estimate the diffusion coefficient and are assumed to be independent. \n"
		    << "The BBMM is defined with parameters mu(t) and sigma^2(t) \n"
		    << "where,\n"
		    << "      mu(i) =  x(i) + alpha * ( x(i+1) - x(i) ) \n"
		    << "      sigma^2(i) =  (t(i+1) - t(i)) * alpha * (1 - alpha) * diffusion_coefficient^2 \n"
		    << "      alpha = ( t(i+1) - t'(i) ) / ( t(i+1) - t(i) ) \n"
		    << "      where (x(i),t(i)) and (x(i+1), t(i+1)) are the even numbered points in the trajectory \n"
		    << "      and (x'(i),t'(i)) are the off numbered points in the trajectory \n"
		    << "Therefore, the likelihood of a candidate diffusion coefficient given a Brownian bridge T[i,i'] is \n"
		    << "      L(diffusion_coefficient^2 | T[i, i'] ) = ( 1/(2 * pi * diffusion_coefficient^2 )) * exp(- || x'(i) - "
		       "mu(i) || / 2 * diffusion_coefficient^2 )  \n"
		    << "Using Maximum Likelihood Estimation, the diffusion coefficient for each bridge is estimated \n"
		    << "Finally, given the bridges and their diffusion coefficients , the segmentation algorithm  \n"
		    << "segments that trajectory in such a way that the information criteria of the segmentation is minimized \n"
		    << "Input Format: Lon, Lat, TS\n"
		    << "Output Format: Lon, Lat, TS, SegID\n"
		    << "Pre-Conditions:\n"
		    << "\t - Input sorted by Timestamp\n"
		    << "Options:\n"
		    << "\t-h,--help\t\tShow this help message\n"
		    << "\t-tr,--trajectory\t\tTrajectories file\n"
		    << "\t--head\t\t\tSpecify this when the input file has header\n"
		    << "\t-idx,--indexes\t\tPosition of columns in the input\n"
		    << "\t-s, --size\t\t Size of the model (number diffusion coefficinets to be selected. Should be between 1 and "
		       "(n-1)/2 )\n"
		    << "\t-p,--penalty\t\tPenalty factor\n";
	}

	bool operator()(int argc, char** argv) {
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

	std::string& get_parameter(std::string& key) { return params[key]; }

	bool has_header() { return header; }

	bool is_stream() { return stream; }

private:
	static const int MinArgs = 7;
	static const int MaxArgs = 10;
	bool header = false, stream = true;
	typedef std::tuple<std::string, std::string, bool> earg;
	std::vector<earg> eargs{std::make_tuple("--head", "--head", false),
	                        std::make_tuple("-tr", "--trajectory", true),
	                        std::make_tuple("-idx", "--indexes", true),
	                        std::make_tuple("-s", "--size", true),
	                        std::make_tuple("-p", "--penalty", true)};

	std::map<std::string, std::string> params{{"-tr", ""}, {"-p", ""}, {"-s", ""}, {"-idx", ""}, {"--head", ""}};

	void set_flags(std::string arg) {
		if (arg == "--head")
			header = true;
		if (arg == "-tr" || (arg == "--trajectory"))
			stream = false;
	}
};

using MovetkGeometryKernel = typename GeometryKernel::MovetkGeometryKernel;

struct ProbeTraits {
	enum ProbeColumns { LAT, LON, SAMPLE_DATE };
};


int main(int argc, char** argv) {
#if CGAL_BACKEND_ENABLED
	std::cerr << "Using CGAL Backend for Geometry\n";
#else
	std::cerr << "Using Boost Backend for Geometry\n";
#endif

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


	key = "-s";
	MovetkGeometryKernel::NT num_coefficients = static_cast<std::size_t>(std::stold(parse.get_parameter(key)));
	std::cerr << "Number of coefficients: " << num_coefficients << "\n";
	assert(num_coefficients > 0);

	key = "-p";
	MovetkGeometryKernel::NT penalty_factor = std::stold(parse.get_parameter(key));


	std::size_t line_count = 0;

	MovetkGeometryKernel::NT lat, lon;
	std::size_t ts;
	using ProbePoint = std::tuple<long double, long double, std::size_t>;
	std::vector<ProbePoint> trajectory;

	if (parse.is_stream()) {
		while (getline(cin, line)) {
			tokens.clear();
			if (line_count == 0) {
				if (parse.has_header()) {
					line_count++;
					continue;
				}
			}
			movetk::utils::split(line, std::back_inserter(tokens));
			lon = std::stold(tokens[col_idx[0]]);
			lat = std::stold(tokens[col_idx[1]]);
			ts = static_cast<std::size_t>(std::stoul(tokens[col_idx[2]]));
			trajectory.push_back(make_tuple(lat, lon, ts));
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
			movetk::utils::split(line, std::back_inserter(tokens));
			lon = std::stold(tokens[col_idx[0]]);
			lat = std::stold(tokens[col_idx[1]]);
			ts = static_cast<std::size_t>(std::stoul(tokens[col_idx[2]]));
			trajectory.push_back(make_tuple(lat, lon, ts));
			line_count++;
		}
	}

	typedef movetk::segmentation::brownian_bridge::ParameterTraits<MovetkGeometryKernel, decltype(trajectory.begin())>
	    ParameterTraits;
	typedef std::vector<typename ParameterTraits::Parameters>::const_iterator BridgeIterator;
	typedef movetk::geo::LocalCoordinateReference<typename MovetkGeometryKernel::NT> Projection;
	std::vector<typename ParameterTraits::Parameters> bridges;

	typedef movetk::segmentation::brownian_bridge::
	    Model<MovetkGeometryKernel, ProbeTraits, ParameterTraits, GeometryKernel::Norm, Projection>
	        BBMM;

	BBMM bb(trajectory.begin(), trajectory.end(), std::back_inserter(bridges));

	std::vector<typename MovetkGeometryKernel::NT> selected_coeffs;

	movetk::segmentation::brownian_bridge::
	    MLE<MovetkGeometryKernel, ParameterTraits, GeometryKernel::Norm, BridgeIterator, 1000>
	        mle(std::cbegin(bridges), std::cend(bridges));

	auto bit = begin(bridges);
	while (bit != end(bridges)) {
		movetk::segmentation::brownian_bridge::
		    MLE<MovetkGeometryKernel, ParameterTraits, GeometryKernel::Norm, BridgeIterator, 1000>
		        mle(bit, bit + 1);
		std::get<ParameterTraits::ParameterColumns::SIGMA_SQUARED>(*bit) = mle();
		bit++;
	}


	movetk::segmentation::brownian_bridge::ParameterSelector<MovetkGeometryKernel, ParameterTraits> selector(
	    num_coefficients);

	selector(std::cbegin(bridges), std::cend(bridges), std::back_inserter(selected_coeffs));


	typedef movetk::segmentation::brownian_bridge::
	    LogLikelihood<MovetkGeometryKernel, ParameterTraits, GeometryKernel::Norm>
	    LogLikelihood;

	typedef movetk::segmentation::ModelBasedSegmentation<MovetkGeometryKernel, LogLikelihood> ModelBasedSegmentation;

	ModelBasedSegmentation segmentation(penalty_factor);

	std::vector<BridgeIterator> segments;

	segmentation(std::cbegin(bridges),
	             std::cend(bridges),
	             std::cbegin(selected_coeffs),
	             std::cend(selected_coeffs),
	             std::back_inserter(segments));


	std::reverse(std::begin(segments), std::end(segments));

	movetk::utils::SegmentIdGenerator make_segment(std::begin(segments), std::end(segments));

	bit = std::begin(bridges);
	std::size_t id = 0;
	while (bit != std::end(bridges)) {
		auto first = std::get<ParameterTraits::ParameterColumns::BEGIN>(*bit);
		auto beyond = std::get<ParameterTraits::ParameterColumns::END>(*bit);
		auto it = first;
		while (it != beyond) {
			std::cout << std::get<1>(*it) << "," << std::get<0>(*it) << "," << std::get<2>(*it) << "," << id << "\n";
			it++;
		}
		id = make_segment.getSegmentID(bit);
		bit++;
	}
	make_segment.reset();

	return 0;
}