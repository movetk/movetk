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
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

struct GeoJSONPoint;
struct GeoJSONLineString;
struct GeoJSONColorProperty;

template <class T>
class GeoJSONCoordinates
{
};

template <>
struct GeoJSONCoordinates<GeoJSONPoint>
{
    template <class CoordType, class Allocator>
    rapidjson::Value
    operator()(CoordType &lat, CoordType &lon,
               Allocator &geometry_allocator)
    {
        rapidjson::Value point(rapidjson::kArrayType);
        point.PushBack(lon, geometry_allocator);
        point.PushBack(lat, geometry_allocator);
        return point;
    }
};

template <>
struct GeoJSONCoordinates<GeoJSONLineString>
{
    template <class LatIterator, class LonIterator, class Allocator>
    rapidjson::Value
    operator()(LatIterator lat_first, LatIterator lat_beyond,
               LonIterator lon_first, Allocator &geometry_allocator)
    {
        rapidjson::Value line_string(rapidjson::kArrayType);
        GeoJSONCoordinates<GeoJSONPoint> geojson_point;
        while (lat_first != lat_beyond)
        {
            line_string.PushBack(geojson_point(*lat_first++, *lon_first++, geometry_allocator),
                                 geometry_allocator);
        }
        return line_string;
    }
};

struct GeoJSONGeometry
{
    template <class T>
    rapidjson::Document
    operator()(T &lat, T &lon)
    {
        rapidjson::Document geometry;
        geometry.SetObject();
        GeoJSONCoordinates<GeoJSONPoint> geojson_point;
        rapidjson::Document::AllocatorType &geom_allocator = geometry.GetAllocator();
        geometry.AddMember("coordinates", geojson_point(lat, lon, geom_allocator),
                           geom_allocator);
        geometry.AddMember("type", "Point", geom_allocator);
        return geometry;
    }

    template <class LatIterator, class LonIterator>
    rapidjson::Document
    operator()(LatIterator lat_first, LatIterator lat_beyond,
               LonIterator lon_first)
    {

        rapidjson::Document geometry;
        geometry.SetObject();
        GeoJSONCoordinates<GeoJSONLineString> geojson_linestring;
        rapidjson::Document::AllocatorType &geom_allocator = geometry.GetAllocator();
        geometry.AddMember("coordinates", geojson_linestring(lat_first, lat_beyond, lon_first, geom_allocator),
                           geom_allocator);
        geometry.AddMember("type", "LineString", geom_allocator);
        return geometry;
    }
};

template <class T>
struct GeoJSONProperty
{
};

template <>
struct GeoJSONProperty<GeoJSONColorProperty>
{
    template <class Document, class Allocator>
    void operator()(Document &properties, Allocator &property_allocator,
                    std::string &colour, std::size_t width)
    {
        properties.AddMember("stroke", rapidjson::Value(colour, property_allocator).Move(), property_allocator);
        properties.AddMember("stroke-width", rapidjson::Value(std::to_string(width), property_allocator).Move(), property_allocator);
    }
};

struct GeoJSONProperties
{

    rapidjson::Document
    operator()(std::string colour, std::size_t width)
    {
        rapidjson::Document properties;
        properties.SetObject();
        rapidjson::Document::AllocatorType &properties_allocator = properties.GetAllocator();
        GeoJSONProperty<GeoJSONColorProperty> colour_property;
        colour_property(properties, properties_allocator, colour, width);
        return properties;
    }

    rapidjson::Document
    operator()()
    {
        rapidjson::Document properties;
        properties.SetObject();
        rapidjson::Document::AllocatorType &properties_allocator = properties.GetAllocator();
        return properties;
    }
};

struct GeoJSONFeature
{
    rapidjson::Document
    operator()(rapidjson::Document &geometry, rapidjson::Document &properties)
    {
        rapidjson::Document feature;
        feature.SetObject();
        rapidjson::Document::AllocatorType &feature_allocator = feature.GetAllocator();
        feature.AddMember("type", "Feature", feature_allocator);
        feature.AddMember("geometry", geometry, feature_allocator);
        feature.AddMember("properties", properties, feature_allocator);
        return feature;
    }
};
#endif