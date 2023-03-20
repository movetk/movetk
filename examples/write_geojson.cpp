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
// Created by Mitra, Aniket on 08/11/2020.
//

#include <iostream>
#include <sstream>
#include <vector>

#include "ExampleSetup.h"
#include "HereTrajectoryTraits.h"
#include "movetk/Statistics.h"
#include "movetk/io/GeoJSON.h"
#include "movetk/io/ProbeReader.h"
#include "movetk/io/TrajectoryReader.h"
#include "movetk/utils/GeometryBackendTraits.h"
#include "test_data.h"

constexpr int LON_Idx = here::c2d::raw::ProbeColumns::LON;
constexpr int LAT_Idx = here::c2d::raw::ProbeColumns::LAT;
constexpr int TS_Idx = here::c2d::raw::ProbeColumns::SAMPLE_DATE;
constexpr int DEVICEID_Idx = here::c2d::raw::ProbeColumns::PROBE_ID;
constexpr int SPEED_Idx = here::c2d::raw::ProbeColumns::SPEED;

template <class Trajectory_t, class PropertyIterator>
void write_geojson(Trajectory_t &trajectory, PropertyIterator first, PropertyIterator beyond, std::ofstream &ofjson) {
	// write GeoJSON
	movetk::io::GeoJSONGeometry geom;
	movetk::io::GeoJSONProperties prop;
	movetk::io::GeoJSONFeature feat;

	auto lats = trajectory.template get<LAT_Idx>();
	auto lons = trajectory.template get<LON_Idx>();
	auto ts = trajectory.template get<TS_Idx>();

	std::vector<int> timestamps;
	std::transform(std::begin(ts), std::end(ts), std::back_inserter(timestamps), [](auto val) -> int {
		return static_cast<int>(val.ts());
	});

	rapidjson::Document geometry = geom(std::begin(lats), std::end(lats), std::begin(lons), std::begin(timestamps));

	rapidjson::Document properties = prop(first, beyond);
	rapidjson::Document feature = feat(geometry, properties);
	rapidjson::StringBuffer strbuf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	feature.Accept(writer);
	ofjson << strbuf.GetString();
	strbuf.Clear();
}


struct Example : public movetk::examples::BaseExample {
	static constexpr const char *NAME = "write_geojson";
	static constexpr const char *DESCRIPTION = "Writing a GeoJSON file for the provided trajectory, augmented with speed statistics";

	void add_options(cxxopts::OptionAdder &adder) {
		adder("t,trajectory", "Trajectory file", cxxopts::value<std::string>());
	}

	template <typename Kernel>
	class ComputeStatistics {
	private:
		std::string device_id, trajectory_id;
		std::size_t num_points;
		typename Kernel::NT length, traj_duration;
		typename Kernel::NT mean_speed, median_speed, min_speed;
		typename Kernel::NT max_speed, var_speed;
		typename Kernel::NT time_mode;
		std::vector<std::pair<std::string, std::string>> trajectory_properties;
		typedef std::vector<std::pair<std::string, std::string>>::const_iterator iterator;

	public:
		template <class Trajectory_t>
		ComputeStatistics(Trajectory_t &trajectory, std::size_t count) {
			using NT = typename Kernel::NT;
			device_id = trajectory.template get<DEVICEID_Idx>()[0];
			trajectory_properties.push_back(std::make_pair("device_id", device_id));
			trajectory_id = device_id + "_" + std::to_string(count);
			trajectory_properties.push_back(std::make_pair("trajectory_id", trajectory_id));
			num_points = trajectory.size();
			trajectory_properties.push_back(std::make_pair("num_points", std::to_string(num_points)));

			// Retrieve spatio-temporal data
			auto lons = trajectory.template get<LON_Idx>();
			auto lats = trajectory.template get<LAT_Idx>();
			auto timeStamps = trajectory.template get<TS_Idx>();

			// Compute length
			movetk::statistics::TrajectoryLength<Kernel, Distance<Kernel>> lenCalc;
			length = lenCalc(lons.begin(), lons.end(), lats.begin(), lats.end());
			trajectory_properties.push_back(std::make_pair("length", std::to_string(length)));

			// Compute duration
			movetk::statistics::TrajectoryDuration duration;
			traj_duration = duration(timeStamps.begin(), timeStamps.end());
			trajectory_properties.push_back(std::make_pair("duration", std::to_string(traj_duration)));

			using SpeedStat = movetk::statistics::TrajectorySpeedStatistic<Kernel, Distance<Kernel>>;
			SpeedStat speedStat;
			using Stat = typename SpeedStat::Statistic;

			using CoordItPair = std::pair<decltype(lons.begin()), decltype(lons.begin())>;

			auto lonLatPoints = movetk::utils::point_iterators_from_coordinates<Kernel>(
			    std::array<CoordItPair, 2>{std::make_pair(lons.begin(), lons.end()),
			                               std::make_pair(lats.begin(), lats.end())});

			// Compute statistics
			std::vector<Stat> statsToCompute = {Stat::Mean, Stat::Median, Stat::Min, Stat::Max, Stat::Variance};
			std::vector<NT> stats =
			    speedStat(lonLatPoints.first, lonLatPoints.second, timeStamps.begin(), timeStamps.end(), statsToCompute);
			const std::array<std::string, 5> NAMES{"mean_speed", "median_speed,", "min_speed", "max_speed", "var_speed"};
			for (size_t i = 0; i < NAMES.size(); ++i) {
				trajectory_properties.push_back(std::make_pair(NAMES[i], std::to_string(stats[i])));
			}
			// Show time mode
			movetk::statistics::ComputeDominantDifference timeMode;

			time_mode = timeMode(timeStamps.begin(), timeStamps.end(), 0);
			trajectory_properties.push_back(std::make_pair("time_mode", std::to_string(time_mode)));
		}

		void set_property(std::string &key, std::string &val) { trajectory_properties.push_back(std::make_pair(key, val)); }

		iterator begin() { return std::cbegin(trajectory_properties); }

		iterator end() { return std::cend(trajectory_properties); }

		auto get_num_points() { return num_points; }
		auto get_length() { return length; }
		auto get_duration() { return traj_duration; }
		auto get_mean_speed() { return mean_speed; }
		auto get_median_speed() { return median_speed; }
		auto get_min_speed() { return min_speed; }
		auto get_max_speed() { return max_speed; }
		auto get_var_speed() { return var_speed; }
		auto get_time_mode() { return time_mode; }
	};

	template <typename Kernel>
	struct Distance {
		typename Kernel::NT operator()(const typename Kernel::MovetkPoint &p0,
		                               const typename Kernel::MovetkPoint &p1) const {
			std::vector<typename Kernel::NT> ll0;
			std::copy(p0.begin(), p0.end(), std::back_inserter(ll0));
			std::vector<typename Kernel::NT> ll1;
			std::copy(p1.begin(), p1.end(), std::back_inserter(ll1));
			return movetk::geo::distance_exact(ll0[1], ll0[0], ll1[1], ll1[0]);
		}
	};


	template <typename Kernel>
	void run(cxxopts::ParseResult &arguments) {
		std::ios_base::sync_with_stdio(false);

		// Specializations for the Commit2Data raw probe format
		using TrajectoryTraits = here::c2d::raw::ColumnarTrajectoryTraits;
		using ProbeTraits = typename TrajectoryTraits::ProbeTraits;

		// Create trajectory reader
		std::unique_ptr<movetk::io::ProbeReader<ProbeTraits>> probe_reader =
		    get_probe_reader<ProbeTraits>(arguments, "trajectory", testdata::c2d_raw_csv);
		using ProbeInputIterator = decltype(probe_reader->begin());
		auto trajectory_reader =
		    movetk::io::create_trajectory_reader<TrajectoryTraits>(probe_reader->begin(), probe_reader->end());

		std::ofstream ofjson("output_trajectories.geojson");
		ofjson.setf(std::ios::fixed);

		ofjson << "{\"type\":\"FeatureCollection\",\"features\":[";

		std::size_t count = 0;
		for (auto tit = trajectory_reader.begin(); tit != trajectory_reader.end();) {
			auto trajectory = *tit;
			if (trajectory.size() == 1) {
				++tit;
				continue;
			}

			ComputeStatistics<Kernel> statistics(trajectory, count);
			auto property_iterator_first = statistics.begin();
			auto property_iterator_beyond = statistics.end();
			write_geojson(trajectory, property_iterator_first, property_iterator_beyond, ofjson);
			if (++tit == trajectory_reader.end())
				continue;
			else
				ofjson << ",";

			count++;
		}

		ofjson << "]}" << std::endl;
	}
};

int main(int argc, char **argv) {
	return movetk::examples::ExampleRunner().run_example<Example>(argc, argv);
}