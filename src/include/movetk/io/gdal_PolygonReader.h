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
// Created by onur on 10/10/18.
//

#ifndef MOVETK_GDAL_POLYGONREADER_H
#define MOVETK_GDAL_POLYGONREADER_H

#include "ogrsf_frmts.h"
#include <vector>
#include <string>


class GdalPolygonReader
{
public:
    std::vector<OGRPolygon> read_polygons(const std::string& file_name);
};

#endif //MOVETK_GDAL_POLYGONREADER_H
