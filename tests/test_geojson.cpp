#include <array>
#include <iostream>
#include <vector>

#include "catch2/catch.hpp"
#include "movetk/io/GeoJSON.h"
#include "helpers/CustomCatchTemplate.h"
using namespace rapidjson;

TEST_CASE("Test GeoJSON", "[test_geojson]") {
	std::string expected_geojson =
	    "{\"type\":\"FeatureCollection\",\"features\":\
[{\"type\":\"Feature\",\"geometry\":{\"coordinates\":[[102.0,0.0],[103.0,1.0],[104.0,0.0],[105.0,1.0]],\"type\"\
:\"LineString\"},\"properties\":{\"id\":\"a1398a11-d1ce-421c-bf66-a456ff525de9\",\"stroke\":\"red\",\"stroke-width\":\"4\"}},\
{\"type\":\"Feature\",\"geometry\":{\"coordinates\":[[100.0,0.0],[101.0,0.0],[101.0,1.0],[100.0,1.0]],\
\"type\":\"LineString\"},\"properties\":{\"id\":\"d1ce-a1398a11-421c-bf66-a456ff525de9\",\"stroke\":\"blue\",\"stroke-width\":\"6\"}}]}";

	rapidjson::StringBuffer strbuf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);

	movetk::io::GeoJSONGeometry geom;

	std::vector<double> lat({0.0, 1.0, 0.0, 1.0});
	std::vector<double> lon({102.0, 103.0, 104.0, 105.0});

	rapidjson::Document geometry1 = geom(std::begin(lat), std::end(lat), std::begin(lon));

	movetk::io::GeoJSONProperties prop;
	rapidjson::Document properties1 = prop("a1398a11-d1ce-421c-bf66-a456ff525de9", "red", 4);

	movetk::io::GeoJSONFeature feat;
	rapidjson::Document feature1 = feat(geometry1, properties1);

	lat.clear();
	lon.clear();

	lat = std::vector<double>({0.0, 0.0, 1.0, 1.0});
	lon = std::vector<double>({100.0, 101.0, 101.0, 100.0});

	rapidjson::Document geometry2 = geom(std::begin(lat), std::end(lat), std::begin(lon));

	rapidjson::Document properties2 = prop("d1ce-a1398a11-421c-bf66-a456ff525de9", "blue", 6);

	rapidjson::Document feature2 = feat(geometry2, properties2);

	rapidjson::Document root_doc;
	root_doc.SetObject();
	root_doc.AddMember("type", "FeatureCollection", root_doc.GetAllocator());
	rapidjson::Value feature_array(rapidjson::kArrayType);
	feature_array.PushBack(feature1, root_doc.GetAllocator());
	feature_array.PushBack(feature2, root_doc.GetAllocator());
	root_doc.AddMember("features", feature_array, root_doc.GetAllocator());

	root_doc.Accept(writer);

	std::string geojson = strbuf.GetString();
	std::cout << geojson << std::endl;

	REQUIRE(expected_geojson.compare(geojson) == 0);
}