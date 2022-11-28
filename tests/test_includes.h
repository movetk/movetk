/*
 * Copyright (C) 2018-2020
 * HERE Europe B.V.
 * Utrecht University (The Netherlands).
 * TU/e (The Netherlands).
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
// Created by Custers, Bram on 2020-02-08.
//

#ifndef TEST_INCLUDES_H
#define TEST_INCLUDES_H

#include <tuple>
#include <type_traits>

#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/Norm.h"

// Setup backend structures
#if MOVETK_WITH_CGAL_BACKEND
#include "movetk/geom/CGALTraits.h"
namespace movetk::test {
struct CGALBackend {
	using NT = long double;
	static constexpr size_t dimensions = 2;
	// Define the Geometry Backend
	using GeometryBackend = backends::cgal::CGALTraits<NT, dimensions>;
	// Using the Geometry Backend define the Movetk Geometry Kernel
	using MovetkGeometryKernel = typename GeometryBackend::Wrapper_CGAL_Geometry;
};
}  // namespace movetk::test
#else
#endif
#if MOVETK_WITH_BOOST_BACKEND
#include "movetk/geom/BoostGeometryTraits.h"
namespace movetk::test {
struct BoostBackend {
	using NT = long double;
	static constexpr size_t dimensions = 2;
	// Define the Geometry Backend
	using GeometryBackend = backends::boost::BoostGeometryTraits<NT, dimensions>;
	// Using the Geometry Backend define the Movetk Geometry Kernel
	using MovetkGeometryKernel = typename GeometryBackend::Wrapper_Boost_Geometry;
};
}  // namespace movetk::test
#endif

namespace movetk::test {
template <typename T>
struct remove_first_type;
template <typename T, typename... Ts>
struct remove_first_type<std::tuple<T, Ts...>> {
	using type = std::tuple<Ts...>;
};
struct noop {};

using AvailableBackends = typename remove_first_type<std::tuple<noop
#if MOVETK_WITH_BOOST_BACKEND
                                                                ,
                                                                BoostBackend
#endif
#if MOVETK_WITH_CGAL_BACKEND
                                                                ,
                                                                CGALBackend
#endif
                                                                >>::type;
}  // namespace movetk::test

namespace test_helpers {
template <typename Backend>
using NTFromBackend = typename Backend::MovetkGeometryKernel::NT;

template <typename Backend>
using MovetkKernelFromBackend = typename Backend::MovetkGeometryKernel;

// Helper struct for geometric constructors.
template <typename Backend>
struct GeometryConstructors {
	movetk::geom::MakePoint<typename Backend::MovetkGeometryKernel> make_point;
	movetk::geom::MakeLine<typename Backend::MovetkGeometryKernel> make_line;
	movetk::geom::MakeSegment<typename Backend::MovetkGeometryKernel> make_segment;
	movetk::geom::MakeSphere<typename Backend::MovetkGeometryKernel> make_sphere;

	using PolyLine = std::vector<typename Backend::MovetkGeometryKernel::MovetkPoint>;
	PolyLine make_polyline(std::initializer_list<typename Backend::MovetkGeometryKernel::MovetkPoint> points) {
		return PolyLine(points);
	}
};

template <typename Backend>
struct BaseTestFixture {
	using MovetkGeometryKernel = typename Backend::MovetkGeometryKernel;
	using NT = typename Backend::NT;
	// Commonly used types
	using MovetkPoint = typename MovetkGeometryKernel::MovetkPoint;
	using MovetkVector = typename MovetkGeometryKernel::MovetkVector;
	// Most commonly used norm
	using Norm = movetk::metric::FiniteNorm<MovetkGeometryKernel, 2>;

	template <typename T, typename... REST, typename = std::enable_if_t<std::conjunction_v<std::is_same<T, REST>...>>>
	static auto create_array_of(T&& arg, REST&&... args) {
		return std::array<T, sizeof...(REST) + 1>{std::forward<T>(arg), std::forward<REST>(args)...};
	}
	template <typename T, typename... REST, typename = std::enable_if_t<std::conjunction_v<std::is_same<T, REST>...>>>
	static auto create_vector_of(T&& arg, REST&&... args) {
		return std::vector<T>{std::forward<T>(arg), std::forward<REST>(args)...};
	}
};

namespace detail {
inline bool startsWith(const std::string& str, int offset, const std::string& toSearch, bool ignoreWs) {
	if (offset + toSearch.size() >= str.size())
		return false;
	std::size_t j = 0;
	for (std::size_t i = offset; i < str.size(); ++i) {
		if (ignoreWs && std::isspace(str[i]) && j == 0)
			continue;
		if (str[i] != toSearch[j])
			return false;
		++j;
		if (j == toSearch.size())
			break;
	}
	return j == toSearch.size();
}
}  // namespace detail

template <typename T>
inline void vector_from_string(const std::string& input, std::vector<T>& output, char delim = ',') {
	std::stringstream stream(input);
	std::string part;
	while (std::getline(stream, part, delim)) {
		std::stringstream part_stream(part);
		T t;
		part_stream >> t;
		output.push_back(t);
	}
}

/**
 * \brief Reads the first <path> element in an ipe string. The path is assumed to only contain 'm' and 'l' operators,
 * thus defining a polyline
 * \param pathData IPE string
 * \param points Output movetk points.
 */
template <typename MovetkGeometryKernel>
inline void parse_ipe_path_contents(const std::string& path_tag_contents,
                                    std::vector<typename MovetkGeometryKernel::MovetkPoint>& points) {
	using NT = typename MovetkGeometryKernel::NT;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;

	// Read <path> content
	std::vector<NT> buff;

	size_t prevStart = 0;
	for (size_t i = 0; i < path_tag_contents.size(); ++i) {
		if (std::isspace(path_tag_contents[i])) {
			if ((i > 0 && std::isspace(path_tag_contents[i - 1])) || (i == prevStart)) {
				prevStart = i + 1;
			} else {
				// Parse previous number
				std::stringstream ss(path_tag_contents.substr(prevStart, i - prevStart));
				NT datum;
				ss >> datum;
				buff.push_back(datum);
				prevStart = i + 1;
			}
		} else if (path_tag_contents[i] == 'm' || path_tag_contents[i] == 'l') {
			if (buff.size() != 2) {
				throw std::runtime_error("Malformed path data, expected 2 numbers before m/l operator");
			}
			points.push_back(make_point({buff[0], buff[1]}));
			buff.clear();
			prevStart = i + 1;
		}
	}
}

/**
 * \brief Reads the first <path> element in an ipe string. The path is assumed to only contain 'm' and 'l' operators,
 * thus defining a polyline
 * \param pathData IPE string
 * \param points Output movetk points.
 */
template <typename MovetkGeometryKernel>
inline void parseIpePath(const std::string& pathData, std::vector<typename MovetkGeometryKernel::MovetkPoint>& points) {
	using NT = typename MovetkGeometryKernel::NT;
	movetk::geom::MakePoint<MovetkGeometryKernel> make_point;
	std::size_t i = 0;
	for (; i < pathData.size(); ++i) {
		if (pathData[i] == '<') {
			if (!detail::startsWith(pathData, i + 1, "path", true)) {
				for (; i < pathData.size(); ++i) {
					if (pathData[i] == '>')
						break;
				}
			} else {
				for (; i < pathData.size(); ++i) {
					if (pathData[i] == '>')
						break;
				}
				// One past the last '>'
				++i;
				break;
			}
		}
	}

	for (; i < pathData.size(); ++i) {
		// Read <path> content
		std::vector<NT> buff;

		auto prevStart = i;
		for (; i < pathData.size(); ++i) {
			if (std::isspace(pathData[i])) {
				if ((i > 0 && std::isspace(pathData[i - 1])) || (i == prevStart)) {
					prevStart = i + 1;
				} else {
					std::stringstream ss(pathData.substr(prevStart, i - prevStart));
					NT datum;
					ss >> datum;
					buff.push_back(datum);
					prevStart = i + 1;
				}
			} else if (pathData[i] == 'm' || pathData[i] == 'l') {
				assert(buff.size() == 2);
				points.push_back(make_point({buff[0], buff[1]}));
				buff.clear();
				prevStart = i + 1;
			}
			// Start of endtag
			else if (pathData[i] == '<') {
				return;
			}
		}
	}
}

template <typename FieldsTuple, int XIdx, int YIdx, int TimeIdx>
std::vector<FieldsTuple> buildData(const std::vector<double>& xs,
                                   const std::vector<double>& ys,
                                   std::time_t timeIncrement,
                                   FieldsTuple defaultTuple) {
	assert(xs.size() == ys.size());
	std::vector<FieldsTuple> out(xs.size(), defaultTuple);
	std::time_t curr = std::get<TimeIdx>(defaultTuple);
	for (std::size_t i = 0; i < xs.size(); ++i) {
		if (i != 0) {
			std::get<TimeIdx>(out[i]) = curr + timeIncrement;
			curr += timeIncrement;
		}
		std::get<XIdx>(out[i]) = xs[i];
		std::get<YIdx>(out[i]) = ys[i];
	}
	return out;
}

template <typename FieldsTuple, int XIdx, int YIdx, int TimeIdx>
std::vector<FieldsTuple> buildData(const std::vector<double>& xs,
                                   const std::vector<double>& ys,
                                   const std::vector<std::time_t>& times,
                                   FieldsTuple defaultTuple) {
	assert(xs.size() == ys.size());
	assert(xs.size() == times.size());
	std::vector<FieldsTuple> out(xs.size(), defaultTuple);
	for (std::size_t i = 0; i < xs.size(); ++i) {
		std::get<TimeIdx>(out[i]) = times[i];
		std::get<XIdx>(out[i]) = xs[i];
		std::get<YIdx>(out[i]) = ys[i];
	}
	return out;
}

inline bool approximatelyZero(double val, double tolerance = 0.00001) {
	return std::abs(val) < tolerance;
}
inline bool approximatelyEqual(double v0, double v1, double tolerance = 0.00001) {
	return std::abs(v0 - v1) < tolerance;
}

template <typename InputIterator1, typename InputIterator2>
bool range_has_same_value(InputIterator1 start1,
                          InputIterator1 end1,
                          InputIterator2 start2,
                          InputIterator2 end2,
                          bool output_failure = false) {
	auto it1 = start1;
	auto it2 = start2;
	std::size_t index = 0;
	for (; it1 != end1 && it2 != end2; ++it1, ++it2, ++index) {
		if (*it1 != *it2) {
			if (output_failure) {
				std::cout << "Range were not equal at index " << index << '\n';
			}
			return false;
		}
	}
	if (it1 != end1 || it2 != end2) {
		std::cout << "Ranges are of unequal lenght\n";
		return false;
	}
	return true;
}
template <typename RangeLike1, typename RangeLike2>
bool range_has_same_value(RangeLike1 range1, RangeLike2 range2, bool output_failure = false) {
	return range_has_same_value(std::begin(range1),
	                            std::end(range1),
	                            std::begin(range2),
	                            std::end(range2),
	                            output_failure);
}
}  // namespace test_helpers
#endif