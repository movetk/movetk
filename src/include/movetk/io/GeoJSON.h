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
// Created by Mitra, Aniket on 2020-10-16.
//

#ifndef MOVETK_GEOJSON_H
#define MOVETK_GEOJSON_H
#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
namespace movetk::io {
/**
 * @brief
 *
 */
struct GeoJSONPoint;

/**
 * @brief
 *
 */
struct GeoJSONLineString;

/**
 * @brief
 *
 */
struct GeoJSONColorProperty;

/**
 * @brief
 *
 */
struct GeoJSONGenericProperty;

/**
 * @brief
 *
 * @tparam T
 */
template <class T>
class GeoJSONCoordinates {};

/**
 * @brief
 *
 * @tparam
 */
template <>
struct GeoJSONCoordinates<GeoJSONPoint> {
	/**
	 * @brief
	 *
	 * @tparam CoordType
	 * @tparam Allocator
	 * @param lat
	 * @param lon
	 * @param geometry_allocator
	 * @return rapidjson::Value
	 */
	template <class CoordType, class Allocator>
	rapidjson::Value operator()(CoordType &lat, CoordType &lon, Allocator &geometry_allocator) {
		rapidjson::Value point(rapidjson::kArrayType);
		point.PushBack(lon, geometry_allocator);
		point.PushBack(lat, geometry_allocator);
		return point;
	}

	/**
	 * @brief
	 *
	 * @tparam CoordType
	 * @tparam TStype
	 * @tparam Allocator
	 * @param lat
	 * @param lon
	 * @param timestamp
	 * @param geometry_allocator
	 * @return rapidjson::Value
	 */
	template <class CoordType, class TStype, class Allocator>
	rapidjson::Value operator()(CoordType &lat, CoordType &lon, TStype &timestamp, Allocator &geometry_allocator) {
		rapidjson::Value point(rapidjson::kArrayType);
		point.PushBack(lon, geometry_allocator);
		point.PushBack(lat, geometry_allocator);
		point.PushBack(0, geometry_allocator);
		point.PushBack(timestamp, geometry_allocator);
		return point;
	}
};

/**
 * @brief
 *
 * @tparam
 */
template <>
struct GeoJSONCoordinates<GeoJSONLineString> {
	/**
	 * @brief
	 *
	 * @tparam LatIterator
	 * @tparam LonIterator
	 * @tparam Allocator
	 * @param lat_first
	 * @param lat_beyond
	 * @param lon_first
	 * @param geometry_allocator
	 * @return rapidjson::Value
	 */
	template <class LatIterator, class LonIterator, class Allocator>
	rapidjson::Value operator()(LatIterator lat_first,
	                            LatIterator lat_beyond,
	                            LonIterator lon_first,
	                            Allocator &geometry_allocator) {
		rapidjson::Value line_string(rapidjson::kArrayType);
		GeoJSONCoordinates<GeoJSONPoint> geojson_point;
		while (lat_first != lat_beyond) {
			line_string.PushBack(geojson_point(*lat_first++, *lon_first++, geometry_allocator), geometry_allocator);
		}
		return line_string;
	}

	/**
	 * @brief
	 *
	 * @tparam LatIterator
	 * @tparam LonIterator
	 * @tparam TSIterator
	 * @tparam Allocator
	 * @param lat_first
	 * @param lat_beyond
	 * @param lon_first
	 * @param ts_first
	 * @param geometry_allocator
	 * @return rapidjson::Value
	 */
	template <class LatIterator, class LonIterator, class TSIterator, class Allocator>
	rapidjson::Value operator()(LatIterator lat_first,
	                            LatIterator lat_beyond,
	                            LonIterator lon_first,
	                            TSIterator ts_first,
	                            Allocator &geometry_allocator) {
		rapidjson::Value line_string(rapidjson::kArrayType);
		GeoJSONCoordinates<GeoJSONPoint> geojson_point;
		while (lat_first != lat_beyond) {
			line_string.PushBack(geojson_point(*lat_first++, *lon_first++, *ts_first++, geometry_allocator),
			                     geometry_allocator);
		}
		return line_string;
	}
};

/**
 * @brief
 *
 */
struct GeoJSONGeometry {
	/**
	 * @brief
	 *
	 * @tparam T
	 * @param lat
	 * @param lon
	 * @return rapidjson::Document
	 */
	template <class T>
	rapidjson::Document operator()(T &lat, T &lon) {
		rapidjson::Document geometry;
		geometry.SetObject();
		GeoJSONCoordinates<GeoJSONPoint> geojson_point;
		rapidjson::Document::AllocatorType &geom_allocator = geometry.GetAllocator();
		geometry.AddMember("coordinates", geojson_point(lat, lon, geom_allocator), geom_allocator);
		geometry.AddMember("type", "Point", geom_allocator);
		return geometry;
	}

	/**
	 * @brief
	 *
	 * @tparam LatIterator
	 * @tparam LonIterator
	 * @param lat_first
	 * @param lat_beyond
	 * @param lon_first
	 * @return rapidjson::Document
	 */
	template <class LatIterator, class LonIterator>
	rapidjson::Document operator()(LatIterator lat_first, LatIterator lat_beyond, LonIterator lon_first) {
		rapidjson::Document geometry;
		geometry.SetObject();
		GeoJSONCoordinates<GeoJSONLineString> geojson_linestring;
		rapidjson::Document::AllocatorType &geom_allocator = geometry.GetAllocator();
		geometry.AddMember("coordinates",
		                   geojson_linestring(lat_first, lat_beyond, lon_first, geom_allocator),
		                   geom_allocator);
		geometry.AddMember("type", "LineString", geom_allocator);
		return geometry;
	}

	/**
	 * @brief
	 *
	 * @tparam LatIterator
	 * @tparam LonIterator
	 * @tparam TSIterator
	 * @param lat_first
	 * @param lat_beyond
	 * @param lon_first
	 * @param ts_first
	 * @return rapidjson::Document
	 */
	template <class LatIterator, class LonIterator, class TSIterator>
	rapidjson::Document operator()(LatIterator lat_first,
	                               LatIterator lat_beyond,
	                               LonIterator lon_first,
	                               TSIterator ts_first) {
		rapidjson::Document geometry;
		geometry.SetObject();
		GeoJSONCoordinates<GeoJSONLineString> geojson_linestring;
		rapidjson::Document::AllocatorType &geom_allocator = geometry.GetAllocator();
		geometry.AddMember("coordinates",
		                   geojson_linestring(lat_first, lat_beyond, lon_first, ts_first, geom_allocator),
		                   geom_allocator);
		geometry.AddMember("type", "LineString", geom_allocator);
		return geometry;
	}
};

/**
 * @brief
 *
 * @tparam T
 */
template <class T>
struct GeoJSONProperty {};

/**
 * @brief
 *
 * @tparam
 */
template <>
struct GeoJSONProperty<GeoJSONColorProperty> {
	/**
	 * @brief
	 *
	 * @tparam Document
	 * @tparam Allocator
	 * @param properties
	 * @param property_allocator
	 * @param identifier
	 * @param colour
	 * @param width
	 */
	template <class Document, class Allocator>
	void operator()(Document &properties,
	                Allocator &property_allocator,
	                std::string &identifier,
	                std::string &colour,
	                std::size_t width) {
		properties.AddMember("id", rapidjson::Value(identifier, property_allocator).Move(), property_allocator);
		properties.AddMember("stroke", rapidjson::Value(colour, property_allocator).Move(), property_allocator);
		properties.AddMember("stroke-width",
		                     rapidjson::Value(std::to_string(width), property_allocator).Move(),
		                     property_allocator);
	}
};

/**
 * @brief
 *
 * @tparam
 */
template <>
struct GeoJSONProperty<GeoJSONGenericProperty> {
	/**
	 * @brief
	 *
	 * @tparam Document
	 * @tparam Allocator
	 * @tparam Iterator
	 * @param properties
	 * @param property_allocator
	 * @param first
	 * @param beyond
	 */
	template <class Document, class Allocator, class Iterator>
	void operator()(Document &properties, Allocator &property_allocator, Iterator first, Iterator beyond) {
		auto it = first;
		while (it != beyond) {
			properties.AddMember(rapidjson::Value((*it).first, property_allocator).Move(),
			                     rapidjson::Value((*it).second, property_allocator).Move(),
			                     property_allocator);
			it++;
		}
	}
};

/**
 * @brief
 *
 */
struct GeoJSONProperties {
	/**
	 * @brief
	 *
	 * @param identifier
	 * @param colour
	 * @param width
	 * @return rapidjson::Document
	 */
	rapidjson::Document operator()(std::string identifier, std::string colour, std::size_t width) {
		rapidjson::Document properties;
		properties.SetObject();
		rapidjson::Document::AllocatorType &properties_allocator = properties.GetAllocator();
		GeoJSONProperty<GeoJSONColorProperty> colour_property;
		colour_property(properties, properties_allocator, identifier, colour, width);
		return properties;
	}

	/**
	 * @brief
	 *
	 * @return rapidjson::Document
	 */
	rapidjson::Document operator()() {
		rapidjson::Document properties;
		properties.SetObject();
		rapidjson::Document::AllocatorType &properties_allocator = properties.GetAllocator();
		return properties;
	}

	/**
	 * @brief
	 *
	 * @tparam Iterator
	 * @param first
	 * @param beyond
	 * @return rapidjson::Document
	 */
	template <class Iterator>
	rapidjson::Document operator()(Iterator first, Iterator beyond) {
		rapidjson::Document properties;
		properties.SetObject();
		rapidjson::Document::AllocatorType &properties_allocator = properties.GetAllocator();
		GeoJSONProperty<GeoJSONGenericProperty> generic_property;
		generic_property(properties, properties_allocator, first, beyond);
		return properties;
	}
};

/**
 * @brief
 *
 */
struct GeoJSONFeature {
	/**
	 * @brief
	 *
	 * @param geometry
	 * @param properties
	 * @return rapidjson::Document
	 */
	rapidjson::Document operator()(rapidjson::Document &geometry, rapidjson::Document &properties) {
		rapidjson::Document feature;
		feature.SetObject();
		rapidjson::Document::AllocatorType &feature_allocator = feature.GetAllocator();
		feature.AddMember("type", "Feature", feature_allocator);
		feature.AddMember("geometry", geometry, feature_allocator);
		feature.AddMember("properties", properties, feature_allocator);
		return feature;
	}
};
}  // namespace movetk::io
#endif