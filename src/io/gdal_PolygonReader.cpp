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
// Created by onur on 10/9/18.
//

#include <iostream>
#include "movetk/io/gdal_PolygonReader.h"

std::vector<OGRPolygon> GdalPolygonReader::read_polygons(const std::string& file_name) {

    GDALAllRegister();
    auto poDS = static_cast<GDALDataset*>(
            GDALOpenEx( file_name.c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr ));
    if( poDS == nullptr )
    {
        printf( "Open failed.\n" );
        exit( 1 );
    }
    std::vector<OGRPolygon> polygons;

    std::cout << "Number of layers in shp: " << poDS->GetLayerCount() << std::endl;
//    OGRLayer  *poLayer = poDS->GetLayerByName( "point" );
    OGRLayer  *poLayer = poDS->GetLayer(0);
    std::cout << "Number of features in layer 0: " << poLayer->GetFeatureCount() << std::endl;
//    OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
    poLayer->ResetReading();
    OGRFeature *poFeature;
    while( (poFeature = poLayer->GetNextFeature()) != nullptr )
    {
        std::cout << "Feature:" << std::endl;
        OGRGeometry *poGeometry = poFeature->GetGeometryRef();
        if( poGeometry != nullptr
                && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon )
        {
            auto poPolygon = (OGRPolygon*) poGeometry;
            std::cout << "Area: " << poPolygon->get_Area() << std::endl;
            polygons.push_back(*(OGRPolygon*)poPolygon->clone());
        }

        OGRFeature::DestroyFeature( poFeature );
    }
    GDALClose( poDS );

    return polygons;
}
