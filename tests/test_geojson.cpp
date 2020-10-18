#include "catch2/catch.hpp"
#include "movetk/io/GeoJSON.h"
#include "test_includes.h"
#include <iostream>
#include <array>
#include <vector>
using namespace rapidjson;

TEST_CASE("Test GeoJSON", "[test_geojson]")
{

    std::string expected_geojson = "{\"type\":\"FeatureCollection\",\"features\":\
[{\"type\":\"Feature\",\"geometry\":{\"coordinates\":[[102.0,0.0],[103.0,1.0],[104.0,0.0],[105.0,1.0]],\"type\"\
:\"LineString\"},\"properties\":{\"stroke\":\"red\",\"stroke-width\":\"4\"}},\
{\"type\":\"Feature\",\"geometry\":{\"coordinates\":[[100.0,0.0],[101.0,0.0],[101.0,1.0],[100.0,1.0]],\
\"type\":\"LineString\"},\"properties\":{\"stroke\":\"blue\",\"stroke-width\":\"6\"}}]}";

    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);

    GeoJSONGeometry geom;

    std::vector<double> lat({0.0, 1.0, 0.0, 1.0});
    std::vector<double> lon({102.0, 103.0, 104.0, 105.0});

    rapidjson::Document geometry1 = geom(std::begin(lat),
                                         std::end(lat),
                                         std::begin(lon));

    GeoJSONProperties prop;
    rapidjson::Document properties1 = prop("red", 4);

    GeoJSONFeature feat;
    rapidjson::Document feature1 = feat(geometry1, properties1);

    lat.clear();
    lon.clear();

    lat = std::vector<double>({0.0, 0.0, 1.0, 1.0});
    lon = std::vector<double>({100.0, 101.0, 101.0, 100.0});

    rapidjson::Document geometry2 = geom(std::begin(lat),
                                         std::end(lat),
                                         std::begin(lon));

    rapidjson::Document properties2 = prop("blue", 6);

    rapidjson::Document feature2 = feat(geometry2, properties2);

    std::string geojson = "{\"type\":\"FeatureCollection\",\"features\":[";
    feature1.Accept(writer);
    geojson = geojson + strbuf.GetString() + ",";
    strbuf.Clear();
    feature2.Accept(writer);
    geojson = geojson + strbuf.GetString();
    geojson = geojson + "]}";
    std::cout << geojson << std::endl;

    REQUIRE(expected_geojson.compare(geojson) == 0);
}