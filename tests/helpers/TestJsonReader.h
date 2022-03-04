#ifndef MOVETK_TEST_TESTJSONREADER_H
#define MOVETK_TEST_TESTJSONREADER_H

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/rapidjson.h>

#include <array>
#include <filesystem>
#include <fstream>

#include "movetk/geom/GeometryInterface.h"

namespace movetk::test::io {
using data_container_arg = const rapidjson::Value&;

template <typename T>
T construct(data_container_arg value) {}

template <typename T>
void load(data_container_arg value, T& t) {}

template <>
inline void load(data_container_arg value, bool& b) {
	b = value.GetBool();
}
template <>
inline void load(data_container_arg value, int& b) {
	b = value.GetInt();
}
template <>
inline void load(data_container_arg value, long& b) {
	b = value.GetInt64();
}
template <>
inline void load(data_container_arg value, long long& b) {
	b = value.GetInt64();
}
template <>
inline void load(data_container_arg value, std::string& b) {
	b = value.GetString();
}
template <>
inline void load(data_container_arg value, float& b) {
	b = static_cast<float>(value.GetDouble());
}
template <>
inline void load(data_container_arg value, double& b) {
	b = value.GetDouble();
}
template <>
inline void load(data_container_arg value, long double& b) {
	b = value.GetDouble();
}

template <typename T>
void load(data_container_arg value, std::vector<T>& output) {
	assert(value.IsArray());
	for (auto it = value.Begin(); it != value.End(); ++it) {
		if constexpr (std::is_default_constructible_v<T>) {
			T t;
			load(*it, t);
			output.emplace_back(std::move(t));

		} else {
			output.emplace_back(construct<T>(*it));
		}
	}
}
template <typename T, typename CONSTRUCT>
void load(data_container_arg value, std::vector<T>& output, CONSTRUCT&& constructor) {
	assert(value.IsArray());
	for (auto it = value.Begin(); it != value.End(); ++it) {
		output.emplace_back(constructor(*it));
	}
}

inline rapidjson::Document get_json_doc(const std::string& file_path) {
	rapidjson::Document output;
	std::ifstream ifs(file_path);
	if (!ifs.is_open()) {
		auto current_path = std::filesystem::current_path();
		std::string path_desc = ". Current path is:" + current_path.string();
		throw std::runtime_error("Could not open " + file_path + path_desc);
	}
	rapidjson::IStreamWrapper isw(ifs);

	output.ParseStream(isw);
	return output;
}

template <typename Kernel>
void read_point(data_container_arg object, typename Kernel::MovetkPoint& point) {
	movetk::geom::MakePoint<Kernel> make_point;
	point = make_point({object["x"].GetDouble(), object["y"].GetDouble()});
}
template <typename Kernel>
typename Kernel::MovetkPoint construct_point(data_container_arg object) {
	movetk::geom::MakePoint<Kernel> make_point;
	return make_point({object["x"].GetDouble(), object["y"].GetDouble()});
}
}  // namespace movetk::test::io

#endif