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
 // Created by Mitra, Aniket on 2019-04-03.
 //

#include <array>
#include <catch2/catch.hpp>

#include "movetk/geom/GeometryInterface.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "movetk/metric/Norm.h"
#include "movetk/algo/Simplification.h"

#include "test_includes.h"

template<typename Backend>
struct ImaiIriTests : public test_helpers::BaseTestFixture<Backend> {
    using Norm = movetk_support::FiniteNorm<MovetkGeometryKernel, 2>;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    using PolyLine = std::vector<MovetkPoint>;
    using Wedge = movetk_core::Wedge<MovetkGeometryKernel, Norm> ;
    using ChanChin = movetk_algorithms::ChanChin<MovetkGeometryKernel, Wedge>;
    using ImaiIri = movetk_algorithms::ImaiIri<MovetkGeometryKernel, ChanChin>;
};



MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(ImaiIriTests, "Check Imai-Iri Simplification 1", "[imai_iri_1]") {
    PolyLine polyline({
        make_point({1,-6}), make_point({4,-4}),
        make_point({5,-2}), make_point({6,-5}),
        make_point({7,-2}), make_point({8,-5}),
        make_point({9,-2}), make_point({10,-5}),
        make_point({11,-2}), make_point({13,-4})
        });
    std::cout << "Polyline to be simplified: ";
    std::cout << "{";
    for (auto& vertex : polyline) {
        cout << vertex;
        std::cout << ";";
    }
    std::cout << "}\n";

    ImaiIri simplification(2);
    std::vector<PolyLine::iterator> result;
    simplification(std::begin(polyline), std::end(polyline),
        movetk_core::movetk_back_insert_iterator(result));

    PolyLine ExpectedPolyline({
        make_point({1, -6}), make_point({4, -4}),
        make_point({13, -4})
        });
    REQUIRE(result.size() == ExpectedPolyline.size());
    auto eit = std::begin(ExpectedPolyline);
    for (auto reference : result) {
        MovetkGeometryKernel::MovetkVector v = *reference - *eit;
        REQUIRE((v * v) < MOVETK_EPS);
        eit++;
    }
}