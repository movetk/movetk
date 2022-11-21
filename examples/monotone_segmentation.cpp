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

#include "movetk/ds/StartStopMatrix.h"
#include "movetk/segmentation/SegmentationTraits.h"
#include "movetk/utils/GeometryBackendTraits.h"
#include "movetk/utils/Iterators.h"


using namespace std;


typedef movetk::segmentation::SegmentationTraits<typename GeometryKernel::MovetkGeometryKernel::NT,
                                         typename GeometryKernel::MovetkGeometryKernel,
                                         GeometryKernel::dimensions>
    SegmentationTraits;
typedef typename GeometryKernel::MovetkGeometryKernel::NT NT;
// Atribute Containers defined here
typedef std::vector<size_t> DiscretePosValuedAttributes;
typedef std::vector<std::pair<NT, NT>> GeoAttributes;
typedef std::vector<NT> NonGeoAttributes;
typedef std::vector<SegmentationTraits::Point> Points;
typedef std::vector<Points::const_iterator> PointsReference;
typedef std::vector<NonGeoAttributes::const_iterator> NonGeoAttributesReference;
typedef std::vector<DiscretePosValuedAttributes::const_iterator> DiscretePosValuedAttributesReference;

// Atribute  Iterators defined here
typedef movetk::ds::StartStopDiagram<movetk::ds::SsdType::compressed,
                                     typename GeometryKernel::MovetkGeometryKernel,
                                     DiscretePosValuedAttributes>
    SSD;
// typedef DiscretePosValuedAttributes::iterator TimeAttributesIterator;
typedef DiscretePosValuedAttributes::const_iterator TimeAttributesConstIterator;
// typedef DiscretePosValuedAttributes::iterator TimeDiffsIterator;
typedef DiscretePosValuedAttributes::const_iterator TimeDiffsConstIterator;
typedef GeoAttributes::const_iterator GeoAttributesConstIterator;
// typedef GeoAttributes::iterator GeoAttributesIterator;
typedef NonGeoAttributes::const_iterator NonGeoAttributesConstIterator;
// typedef NonGeoAttributes::iterator NonGeoAttributesIterator;
typedef Points::const_iterator PointsConstIterator;
// typedef Points::iterator PointsIterator;
typedef PointsReference::const_iterator PointsReferenceConstIter;
// typedef PointsReference::iterator PointsReferenceIter;
typedef NonGeoAttributesReference::const_iterator NonGeoAttributesReferenceConstIter;
// typedef NonGeoAttributesReference::iterator NonGeoAttributesReferenceIter;
typedef DiscretePosValuedAttributesReference::const_iterator DiscretePosValuedAttributesReferenceConstIter;
// typedef DiscretePosValuedAttributesReference::iterator DiscretePosValuedAttributesReferenceIter;
typedef SSD::const_iterator ssd_const_iterator;


class ParseInput {
public:
	static void show_usage(std::string& name) {
		std::cerr
		    << "Usage: cat Trajectory | " << name << " <option(s)>\n"
		    << "OR"
		    << "Usage:  " << name << " <option(s)>\n"
		    << "Description: Trajectory Segmentation using Monotone Criteria\n"
		    << "Input Format: <Id,TS,Lon,Lat,X,Y,Speed,Heading> / <Id,TS,Lon,Lat,X,Y> \n"
		    << "Output Format: "
		       "<Id,Lat,Lon,Timestamp,Speed,Heading,X,Y,Distance,Velocity,LocationSegments,SpeedSegments,HeadingSegments,"
		       "VelocitySegments,Distance_or_VelocitySegments> \n"
		    << "Threshold Format: <TSThreshold,RadiusThreshold,DifferenceThreshold,RangeThreshold> \n"
		    << "Column Idx: "
		       "<IDColumnIdx,TSColumnIdx,LonColumnIdx,LatColumnIdx,XColumnIdx,YColumnIdx,SpeedColumnIdx,HeadingColumnIdx>"
		    << "Pre-Conditions:\n"
		    << "\t - Input sorted by Timestamp\n"
		    << "\t - Timestamp should be unix timestamp\n"
		    << "Options:\n"
		    << "\t-h,--help\t\t\tShow this help message\n"
		    << "\t--head\t\t\tSpecify this when the input file has header\n"
		    << "\t-tr,--trajectory\t\tTrajectories file\n"
		    << "\t-c,--compute\t\tCompute Speed and Heading\n"
		    << "\t-t,--thresholds\t\tComma Separated Segmentation Thresholds\n"
		    << "\t-idx,--indexes\t\tPosition of columns in the input\n"
		    << std::endl;
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

	bool compute_attributes() { return compute; }

	bool is_stream() { return stream; }

	bool has_indexes() { return has_idx; }

private:
	static const size_t MinArgs = 5;
	static const size_t MaxArgs = 9;
	bool header = false, compute = false, stream = true, has_idx = false;
	std::string s1 = "--head";
	std::string s2 = "-c";
	std::string s3 = "--compute";
	std::string s4 = "-tr";
	std::string s5 = "--trajectory";
	std::string s6 = "-idx";
	std::string s7 = "--indexes";
	typedef std::tuple<std::string, std::string, bool> earg;
	std::vector<earg> eargs{
	    std::make_tuple(s1, "", false),
	    std::make_tuple(s2, s3, false),
	    std::make_tuple(s4, s5, true),
	    std::make_tuple("-t", "--thresholds", true),
	    std::make_tuple(s6, s7, true),
	};

	std::map<std::string, std::string> params{{"-t", ""}, {"-tr", ""}, {"-idx", ""}};

	void set_flags(std::string arg) {
		if (s1.compare(arg) == 0)
			header = true;
		if ((s2.compare(arg) == 0) || (s3.compare(arg) == 0))
			compute = true;
		if ((s4.compare(arg) == 0) || (s5.compare(arg) == 0))
			stream = false;
		if ((s6.compare(arg) == 0) || (s7.compare(arg) == 0))
			has_idx = true;
	}
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

	std::ios_base::sync_with_stdio(false);
	std::cout.setf(std::ios::fixed);
	NT lat, lon;
	string line;
	std::string key = "-t";
	line = parse.get_parameter(key);
	std::vector<string> tokens;
	movetk::utils::split(line, std::back_inserter(tokens));

	assert(tokens.size() == 4);

	NT TSThreshold = stold(tokens[0]);
	assert(TSThreshold > 0);

	NT RadiusThreshold = stold(tokens[1]);
	assert(RadiusThreshold > 0);
	NT DifferenceThreshold = stold(tokens[2]);
	assert(DifferenceThreshold > 0);
	NT RangeThreshold = stold(tokens[3]);
	assert(RangeThreshold > 0 && RangeThreshold < 360);

	assert(parse.has_indexes() == 1);

	tokens.clear();
	key = "-idx";
	line = parse.get_parameter(key);
	movetk::utils::split(line, std::back_inserter(tokens));

	if (parse.compute_attributes())
		assert(tokens.size() == 6);
	else
		assert(tokens.size() == 8);

	std::cerr << tokens.size() << std::endl;

	NT IDColumnIdx = stold(tokens[0]);
	NT TSColumnIdx = stold(tokens[1]);
	NT LonColumnIdx = stold(tokens[2]);
	NT LatColumnIdx = stold(tokens[3]);
	NT XColumnIdx = stold(tokens[4]);
	NT YColumnIdx = stold(tokens[5]);
	NT SpeedColumnIdx, HeadingColumnIdx;

	if (!parse.compute_attributes()) {
		SpeedColumnIdx = stold(tokens[6]);
		HeadingColumnIdx = stold(tokens[7]);
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


	movetk::geom::MakePoint<typename GeometryKernel::MovetkGeometryKernel> make_point;

	PointsReference points_ref;
	NonGeoAttributesReference speed_ref, heading_ref;
	DiscretePosValuedAttributesReference ts_ref;

	movetk::utils::SegmentIdGenerator<PointsReferenceConstIter> make_location_segment;
	movetk::utils::SegmentIdGenerator<NonGeoAttributesReferenceConstIter> make_speed_segment, make_heading_segment;
	movetk::utils::SegmentIdGenerator<DiscretePosValuedAttributesReferenceConstIter> make_ts_segment;
	SSD location_segments_ssd, speed_segments_ssd, heading_segments_ssd, ts_segments_ssd;
	SSD conjunction_segments_ts_location_ssd, conjunction_segments_ts_speed_ssd, conjunction_segments_ts_heading_ssd;
	SSD conjunction_segments_ssd, disjunction_segments_ssd;
	size_t disjunction_segment_idx = 0, conjunction_segment_idx = 0, conjunction_segment_ts_loc_idx = 0;
	size_t conjunction_segment_ts_speed_idx = 0, conjunction_segment_ts_heading_idx = 0;

	tokens.clear();
	std::size_t linecount = 0;
	if (parse.is_stream()) {
		while (getline(cin, line)) {
			if (linecount == 0) {
				if (parse.has_header()) {
					linecount++;
					continue;
				}
			}
			linecount++;

			movetk::utils::split(line, std::back_inserter(tokens));
			if (parse.compute_attributes()) {
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
			GeoCoords.push_back(make_pair(lat, lon));
			pt[0] = X;
			pt[1] = Y;
			SegmentationTraits::Point P = make_point(std::cbegin(pt), std::cend(pt));
			poly.push_back(P);
			std::string id = tokens[IDColumnIdx];
			ids.push_back(id);
			tokens.clear();
		}
	} else {
		key = "-tr";
		std::string trajfile = parse.get_parameter(key);
		ifstream infile;
		infile.open(trajfile);
		while (getline(infile, line)) {
			if (linecount == 0) {
				if (parse.has_header()) {
					linecount++;
					continue;
				}
			}
			linecount++;

			movetk::utils::split(line, std::back_inserter(tokens));
			if (parse.compute_attributes()) {
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
			GeoCoords.push_back(make_pair(lat, lon));
			pt[0] = X;
			pt[1] = Y;
			SegmentationTraits::Point P = make_point(std::cbegin(pt), std::cend(pt));
			poly.push_back(P);
			std::string id = tokens[IDColumnIdx];
			ids.push_back(id);
			tokens.clear();
		}
	}

	size_t NumPoints = std::distance(std::begin(ts), std::end(ts));

	movetk::utils::get_distances<typename GeometryKernel::MovetkGeometryKernel>(
	    std::begin(poly),
	    std::end(poly),
	    std::back_inserter(distances));
	assert(distances.size() == NumPoints);

	movetk::utils::get_time_diffs(std::begin(ts), std::end(ts), std::back_inserter(tdiffs));
	assert(tdiffs.size() == NumPoints);

	if (parse.compute_attributes()) {
		movetk::utils::get_speeds<typename GeometryKernel::MovetkGeometryKernel>(
		    std::begin(tdiffs),
		    std::end(tdiffs),
		    std::begin(distances),
		    std::back_inserter(speeds));
		assert(speeds.size() == NumPoints);


		movetk::utils::get_headings<typename GeometryKernel::MovetkGeometryKernel>(
		    std::begin(GeoCoords),
		    std::end(GeoCoords),
		    std::begin(poly),
		    std::back_inserter(headings));
		assert(headings.size() == NumPoints);
	}

	movetk::utils::get_velocities<typename GeometryKernel::MovetkGeometryKernel>(
	    std::begin(speeds),
	    std::end(speeds),
	    std::begin(headings),
	    std::back_inserter(velocities));
	assert(velocities.size() == NumPoints);


	SegmentationTraits::TSSegmentation segment_by_ts(TSThreshold);
	segment_by_ts(std::cbegin(ts), std::cend(ts), std::back_inserter(ts_ref));
	make_ts_segment = movetk::utils::SegmentIdGenerator<DiscretePosValuedAttributesReferenceConstIter>(std::begin(ts_ref),
	                                                                                                   std::end(ts_ref));
	ts_segments_ssd(std::begin(ts), std::end(ts), make_ts_segment);
	//    cerr<<"TS Segments\n";
	//    cerr<<movetk::utils::join(ts_segments_ssd.cbegin(), ts_segments_ssd.cend());


	SegmentationTraits::LocationSegmentation segment_by_meb(RadiusThreshold);
	segment_by_meb(std::cbegin(poly), std::cend(poly), std::back_inserter(points_ref));
	make_location_segment =
	    movetk::utils::SegmentIdGenerator<PointsReferenceConstIter>(std::begin(points_ref), std::end(points_ref));
	location_segments_ssd(std::begin(poly), std::end(poly), make_location_segment);
	//    cerr<<"Location Segments\n";
	//    cerr<<movetk::utils::join(location_segments_ssd.cbegin(), location_segments_ssd.cend());

	SegmentationTraits::SpeedSegmentation segment_by_diff(DifferenceThreshold);
	segment_by_diff(std::begin(speeds), std::end(speeds), std::back_inserter(speed_ref));
	make_speed_segment =
	    movetk::utils::SegmentIdGenerator<NonGeoAttributesReferenceConstIter>(std::begin(speed_ref), std::end(speed_ref));
	speed_segments_ssd(std::begin(speeds), std::end(speeds), make_speed_segment);
	//    cerr<<"Speed Segments\n";
	//    cerr<<TSL::join(speed_segments_ssd.cbegin(), speed_segments_ssd.cend());

	SegmentationTraits::HeadingSegmentation segment_by_range(RangeThreshold);
	segment_by_range(std::begin(headings), std::end(headings), std::back_inserter(heading_ref));
	make_heading_segment = movetk::utils::SegmentIdGenerator<NonGeoAttributesReferenceConstIter>(std::begin(heading_ref),
	                                                                                             std::end(heading_ref));
	heading_segments_ssd(std::begin(headings), std::end(headings), make_heading_segment);
	//    cerr<<"Heading Segments\n";
	//    cerr<<TSL::join(heading_segments_ssd.cbegin(), heading_segments_ssd.cend());

	conjunction_segments_ts_location_ssd = ts_segments_ssd * location_segments_ssd;

	conjunction_segments_ts_speed_ssd = ts_segments_ssd * speed_segments_ssd;

	conjunction_segments_ts_heading_ssd = ts_segments_ssd * heading_segments_ssd;

	// conjunction_segments_ssd = speed_segments_ssd * heading_segments_ssd;

	conjunction_segments_ssd = conjunction_segments_ts_speed_ssd * conjunction_segments_ts_heading_ssd;

	// disjunction_segments_ssd = location_segments_ssd + conjunction_segments_ssd;

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


	cout << "ID,Lat,Lon,Timestamp,Speed,Heading,Tdiff,Distance,X,Y,Velocity_X,Velocity_Y,TSSegments,";
	cout << "LocationSegments,SpeedSegments,HeadingSegments,VelocitySegments,Distance_or_VelocitySegments\n";
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
		// cerr<<make_ts_segment.getSegmentID(TimeStampIterator)<<endl;
		cout << *id_iterator << ",";
		cout << GeoCoordsIterator->first << "," << GeoCoordsIterator->second;
		cout << "," << *TimeStampIterator << "," << *SpeedIterator << "," << *HeadingIterator << ",";
		cout << *TDiffsIterator << "," << *DistanceIterator << ",";
		cout << *PolyLineIterator;
		cout << ",";
		cout << *VelocityIterator;
		cout << "," << make_ts_segment.getSegmentID(TimeStampIterator) << ",";
		// cout << make_location_segment.getSegmentID(PolyLineIterator) << ",";
		cout << conjunction_segment_ts_loc_idx << ",";
		// cout << make_speed_segment.getSegmentID(SpeedIterator) << ",";
		cout << conjunction_segment_ts_speed_idx << ",";
		// cout << make_heading_segment.getSegmentID(HeadingIterator) << ",";
		// cout << make_heading_segment.getSegmentID(HeadingIterator) << ",";
		cout << conjunction_segment_ts_heading_idx << ",";
		cout << conjunction_segment_idx << "," << disjunction_segment_idx << "\n";
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
