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
// Created by Mitra, Aniket on 19/12/2018.
//

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "ExampleSetup.h"
#include "movetk/ds/StartStopMatrix.h"
#include "movetk/segmentation/SegmentationTraits.h"
#include "movetk/utils/GeometryBackendTraits.h"
#include "movetk/utils/Iterators.h"


struct Example : public movetk::examples::BaseExample {
	static constexpr const char* NAME = "monotone_segmentation";
	static constexpr const char* DESCRIPTION = R"(Description: Trajectory Segmentation using Monotone Criteria
Input Format: <Id,TS,Lon,Lat,X,Y,Speed,Heading> / <Id,TS,Lon,Lat,X,Y>
Output Format:
Id,Lat,Lon,Timestamp,Speed,Heading,X,Y,Distance,Velocity,LocationSegments,SpeedSegments,HeadingSegments,
	VelocitySegments,Distance_or_VelocitySegments>
Threshold Format: <TSThreshold,RadiusThreshold,DifferenceThreshold,RangeThreshold>
Column Idx:
IDColumnIdx,TSColumnIdx,LonColumnIdx,LatColumnIdx,XColumnIdx,YColumnIdx,SpeedColumnIdx,HeadingColumnIdx>
Pre-Conditions:
- Input sorted by Timestamp
- Timestamp should be unix timestamp
)";

	void add_options(cxxopts::OptionAdder& adder) {
		adder("f,file", "Input file", cxxopts::value<std::string>());
		adder("t,trajectory", "Trajectories file", cxxopts::value<std::string>());
		adder("c,compute", "Compute speed and heading");
		adder("thresholds", "Comma separated segmentation thresholds", cxxopts::value<std::vector<double>>());
		adder("i,indexes", "Indices of positions of columns in the input", cxxopts::value<std::vector<size_t>>());
	}


	template <typename Kernel>
	void run(cxxopts::ParseResult& arguments) {
		using NT = typename Kernel::NT;
		using SegmentationTraits = movetk::segmentation::SegmentationTraits<NT, Kernel, Kernel::dimensions>;
		// Atribute Containers defined here
		using DiscretePosValuedAttributes = std::vector<size_t>;
		using GeoAttributes = std::vector<std::pair<NT, NT>>;
		using NonGeoAttributes = std::vector<NT>;
		using Points = std::vector<typename SegmentationTraits::Point>;
		using PointsReference = std::vector<typename Points::const_iterator>;
		using NonGeoAttributesReference = std::vector<typename NonGeoAttributes::const_iterator>;
		using DiscretePosValuedAttributesReference = std::vector<typename DiscretePosValuedAttributes::const_iterator>;

		// Atribute  Iterators defined here
		using SSD = movetk::ds::StartStopDiagram<movetk::ds::SsdType::compressed, Kernel, DiscretePosValuedAttributes>;
		using TimeAttributesConstIterator = DiscretePosValuedAttributes::const_iterator;
		using TimeDiffsConstIterator = DiscretePosValuedAttributes::const_iterator;
		using GeoAttributesConstIterator = GeoAttributes::const_iterator;
		using NonGeoAttributesConstIterator = NonGeoAttributes::const_iterator;
		using PointsConstIterator = Points::const_iterator;
		using PointsReferenceConstIter = PointsReference::const_iterator;
		using NonGeoAttributesReferenceConstIter = NonGeoAttributesReference::const_iterator;
		using DiscretePosValuedAttributesReferenceConstIter = DiscretePosValuedAttributesReference::const_iterator;
		using ssd_const_iterator = SSD::const_iterator;

		std::ios_base::sync_with_stdio(false);
		std::cout.setf(std::ios::fixed);
		NT lat, lon;
		std::string line;
		std::vector<std::string> tokens;

		auto thresholds = arguments["thresholds"].as<std::vector<NT>>();
		assert(thresholds.size() == 4);

		NT TSThreshold, RadiusThreshold, DifferenceThreshold, RangeThreshold;
		assign_values(thresholds, TSThreshold, RadiusThreshold, DifferenceThreshold, RangeThreshold);
		assert(TSThreshold > 0);
		assert(RadiusThreshold > 0);
		assert(DifferenceThreshold > 0);
		assert(RangeThreshold > 0 && RangeThreshold < 360);

		auto indices = arguments["indexes"].as<std::vector<size_t>>();
		const bool compute_heading_and_speed = arguments.count("compute");
		if (compute_heading_and_speed)
			assert(indices.size() == 6);
		else
			assert(indices.size() == 8);

		size_t IDColumnIdx, TSColumnIdx, LonColumnIdx, LatColumnIdx, XColumnIdx, YColumnIdx;
		assign_values(indices, IDColumnIdx, TSColumnIdx, LonColumnIdx, LatColumnIdx, XColumnIdx, YColumnIdx);
		size_t SpeedColumnIdx, HeadingColumnIdx;
		if (!compute_heading_and_speed) {
			SpeedColumnIdx = indices[6];
			HeadingColumnIdx = indices[7];
		}


		// Attribute Iterators defined here

		// Attribute Containers are declared here
		std::array<NT, 2> pt;
		Points poly, velocities;
		DiscretePosValuedAttributes ts;
		GeoAttributes GeoCoords;
		NonGeoAttributes headings, distances, speeds;
		std::vector<std::string> ids;
		DiscretePosValuedAttributes tdiffs;


		movetk::geom::MakePoint<Kernel> make_point;

		PointsReference points_ref;
		NonGeoAttributesReference speed_ref, heading_ref;
		DiscretePosValuedAttributesReference ts_ref;

		SSD location_segments_ssd, speed_segments_ssd, heading_segments_ssd, ts_segments_ssd;
		SSD conjunction_segments_ts_location_ssd, conjunction_segments_ts_speed_ssd, conjunction_segments_ts_heading_ssd;
		SSD conjunction_segments_ssd, disjunction_segments_ssd;
		size_t disjunction_segment_idx = 0, conjunction_segment_idx = 0, conjunction_segment_ts_loc_idx = 0;
		size_t conjunction_segment_ts_speed_idx = 0, conjunction_segment_ts_heading_idx = 0;

		const bool is_stream = arguments.count("trajectory") == 0;
		const bool has_header = arguments.count("head");
		tokens.clear();
		std::size_t linecount = 0;
		if (is_stream) {
			while (std::getline(std::cin, line)) {
				if (linecount == 0) {
					if (has_header) {
						linecount++;
						continue;
					}
				}
				linecount++;

				movetk::utils::split(line, std::back_inserter(tokens));
				if (compute_heading_and_speed) {
					assert(tokens.size() >= 6);
				} else {
					assert(tokens.size() >= 8);

					headings.push_back(stold(tokens[HeadingColumnIdx]));
					speeds.push_back(stold(tokens[SpeedColumnIdx]));
				}

				NT X = stold(tokens[XColumnIdx]);
				NT Y = stold(tokens[YColumnIdx]);
				ts.push_back(stold(tokens[TSColumnIdx]));
				lat = stold(tokens[LatColumnIdx]);
				lon = stold(tokens[LonColumnIdx]);
				GeoCoords.push_back(std::make_pair(lat, lon));
				pt[0] = X;
				pt[1] = Y;
				auto P = make_point(std::cbegin(pt), std::cend(pt));
				poly.push_back(P);
				std::string id = tokens[IDColumnIdx];
				ids.push_back(id);
				tokens.clear();
			}
		} else {
			std::string trajfile = arguments["trajectory"].as<std::string>();
			std::ifstream infile;
			infile.open(trajfile);
			while (std::getline(infile, line)) {
				if (linecount == 0) {
					if (has_header) {
						linecount++;
						continue;
					}
				}
				linecount++;

				movetk::utils::split(line, std::back_inserter(tokens));
				if (compute_heading_and_speed) {
					assert(tokens.size() >= 6);
				} else {
					assert(tokens.size() >= 8);
					headings.push_back(stold(tokens[HeadingColumnIdx]));
					speeds.push_back(stold(tokens[SpeedColumnIdx]));
				}

				NT X = stold(tokens[XColumnIdx]);
				NT Y = stold(tokens[YColumnIdx]);
				ts.push_back(stold(tokens[TSColumnIdx]));
				lat = stold(tokens[LatColumnIdx]);
				lon = stold(tokens[LonColumnIdx]);
				GeoCoords.push_back(std::make_pair(lat, lon));
				pt[0] = X;
				pt[1] = Y;
				auto P = make_point(std::cbegin(pt), std::cend(pt));
				poly.push_back(P);
				std::string id = tokens[IDColumnIdx];
				ids.push_back(id);
				tokens.clear();
			}
		}

		size_t NumPoints = std::distance(std::begin(ts), std::end(ts));

		movetk::utils::get_distances<Kernel>(std::begin(poly), std::end(poly), std::back_inserter(distances));
		assert(distances.size() == NumPoints);

		movetk::utils::get_time_diffs(std::begin(ts), std::end(ts), std::back_inserter(tdiffs));
		assert(tdiffs.size() == NumPoints);

		if (compute_heading_and_speed) {
			movetk::utils::get_speeds<Kernel>(std::begin(tdiffs),
			                                  std::end(tdiffs),
			                                  std::begin(distances),
			                                  std::back_inserter(speeds));
			assert(speeds.size() == NumPoints);


			movetk::utils::get_headings<Kernel>(std::begin(GeoCoords),
			                                    std::end(GeoCoords),
			                                    std::begin(poly),
			                                    std::back_inserter(headings));
			assert(headings.size() == NumPoints);
		}

		movetk::utils::get_velocities<Kernel>(std::begin(speeds),
		                                      std::end(speeds),
		                                      std::begin(headings),
		                                      std::back_inserter(velocities));
		assert(velocities.size() == NumPoints);


		typename SegmentationTraits::TSSegmentation segment_by_ts(TSThreshold);
		segment_by_ts(std::cbegin(ts), std::cend(ts), std::back_inserter(ts_ref));
		auto make_ts_segment = movetk::utils::SegmentIdGenerator(std::begin(ts_ref), std::end(ts_ref));
		ts_segments_ssd(std::begin(ts), std::end(ts), make_ts_segment);

		typename SegmentationTraits::LocationSegmentation segment_by_meb(RadiusThreshold);
		segment_by_meb(std::cbegin(poly), std::cend(poly), std::back_inserter(points_ref));
		auto make_location_segment = movetk::utils::SegmentIdGenerator(std::begin(points_ref), std::end(points_ref));
		location_segments_ssd(std::begin(poly), std::end(poly), make_location_segment);

		typename SegmentationTraits::SpeedSegmentation segment_by_diff(DifferenceThreshold);
		segment_by_diff(std::begin(speeds), std::end(speeds), std::back_inserter(speed_ref));
		auto make_speed_segment = movetk::utils::SegmentIdGenerator(std::begin(speed_ref), std::end(speed_ref));
		speed_segments_ssd(std::begin(speeds), std::end(speeds), make_speed_segment);

		typename SegmentationTraits::HeadingSegmentation segment_by_range(RangeThreshold);
		segment_by_range(std::begin(headings), std::end(headings), std::back_inserter(heading_ref));
		auto make_heading_segment = movetk::utils::SegmentIdGenerator(std::begin(heading_ref), std::end(heading_ref));
		heading_segments_ssd(std::begin(headings), std::end(headings), make_heading_segment);

		conjunction_segments_ts_location_ssd = ts_segments_ssd * location_segments_ssd;

		conjunction_segments_ts_speed_ssd = ts_segments_ssd * speed_segments_ssd;

		conjunction_segments_ts_heading_ssd = ts_segments_ssd * heading_segments_ssd;

		conjunction_segments_ssd = conjunction_segments_ts_speed_ssd * conjunction_segments_ts_heading_ssd;

		disjunction_segments_ssd = conjunction_segments_ts_location_ssd + conjunction_segments_ssd;


		ssd_const_iterator disjunction_ssd_it = disjunction_segments_ssd.cbegin();
		ssd_const_iterator conjunction_ssd_it = conjunction_segments_ssd.cbegin();
		ssd_const_iterator disjunction_prev_ssd_it = disjunction_ssd_it;
		ssd_const_iterator conjunction_prev_ssd_it = conjunction_ssd_it;
		ssd_const_iterator conjunction_ts_location_ssd_it = conjunction_segments_ts_location_ssd.cbegin();
		ssd_const_iterator conjunction_ts_location_ssd_prev_it = conjunction_ts_location_ssd_it;
		ssd_const_iterator conjunction_ts_speed_ssd_it = conjunction_segments_ts_speed_ssd.cbegin();
		ssd_const_iterator conjunction_ts_speed_ssd_prev_it = conjunction_ts_speed_ssd_it;
		ssd_const_iterator conjunction_ts_heading_ssd_it = conjunction_segments_ts_heading_ssd.cbegin();
		ssd_const_iterator conjunction_ts_heading_ssd_prev_it = conjunction_ts_heading_ssd_it;


		GeoAttributesConstIterator GeoCoordsIterator = std::cbegin(GeoCoords);
		TimeAttributesConstIterator TimeStampIterator = std::cbegin(ts);
		PointsConstIterator PolyLineIterator = std::cbegin(poly);
		TimeDiffsConstIterator TDiffsIterator = std::cbegin(tdiffs);
		NonGeoAttributesConstIterator DistanceIterator = std::cbegin(distances);
		NonGeoAttributesConstIterator SpeedIterator = std::cbegin(speeds);
		NonGeoAttributesConstIterator HeadingIterator = std::cbegin(headings);
		PointsConstIterator VelocityIterator = std::cbegin(velocities);
		auto id_iterator = std::cbegin(ids);


		std::cout << "ID,Lat,Lon,Timestamp,Speed,Heading,Tdiff,Distance,X,Y,Velocity_X,Velocity_Y,TSSegments,";
		std::cout << "LocationSegments,SpeedSegments,HeadingSegments,VelocitySegments,Distance_or_VelocitySegments\n";
		while (disjunction_ssd_it != disjunction_segments_ssd.cend()) {
			if (!movetk::utils::is_sequence(disjunction_prev_ssd_it, (disjunction_ssd_it + 1))) {
				disjunction_segment_idx++;
				disjunction_prev_ssd_it = disjunction_ssd_it;
			}

			if (!movetk::utils::is_sequence(conjunction_prev_ssd_it, (conjunction_ssd_it + 1))) {
				conjunction_segment_idx++;
				conjunction_prev_ssd_it = conjunction_ssd_it;
			}
			if (!movetk::utils::is_sequence(conjunction_ts_location_ssd_prev_it, (conjunction_ts_location_ssd_it + 1))) {
				conjunction_segment_ts_loc_idx++;
				conjunction_ts_location_ssd_prev_it = conjunction_ts_location_ssd_it;
			}
			if (!movetk::utils::is_sequence(conjunction_ts_speed_ssd_prev_it, (conjunction_ts_speed_ssd_it + 1))) {
				conjunction_segment_ts_speed_idx++;
				conjunction_ts_speed_ssd_prev_it = conjunction_ts_speed_ssd_it;
			}
			if (!movetk::utils::is_sequence(conjunction_ts_heading_ssd_prev_it, (conjunction_ts_heading_ssd_it + 1))) {
				conjunction_segment_ts_heading_idx++;
				conjunction_ts_heading_ssd_prev_it = conjunction_ts_heading_ssd_it;
			}
			std::cout << *id_iterator << ",";
			std::cout << GeoCoordsIterator->first << "," << GeoCoordsIterator->second;
			std::cout << "," << *TimeStampIterator << "," << *SpeedIterator << "," << *HeadingIterator << ",";
			std::cout << *TDiffsIterator << "," << *DistanceIterator << ",";
			std::cout << *PolyLineIterator;
			std::cout << ",";
			std::cout << *VelocityIterator;
			std::cout << "," << make_ts_segment.getSegmentID(TimeStampIterator) << ",";
			std::cout << conjunction_segment_ts_loc_idx << ",";
			std::cout << conjunction_segment_ts_speed_idx << ",";
			std::cout << conjunction_segment_ts_heading_idx << ",";
			std::cout << conjunction_segment_idx << "," << disjunction_segment_idx << "\n";

			conjunction_ts_location_ssd_it++;
			conjunction_ts_speed_ssd_it++;
			conjunction_ts_heading_ssd_it++;
			disjunction_ssd_it++;
			conjunction_ssd_it++;
			PolyLineIterator++;
			TimeStampIterator++;
			TDiffsIterator++;
			DistanceIterator++;
			SpeedIterator++;
			HeadingIterator++;
			GeoCoordsIterator++;
			VelocityIterator++;
			id_iterator++;
		}
	}
};

int main(int argc, char** argv) {
	return movetk::examples::ExampleRunner().run_example<Example>(argc, argv);
}