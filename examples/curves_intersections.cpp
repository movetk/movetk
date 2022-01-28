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
// Created by Mitra, Aniket on 2019-03-05.
//

#include "movetk/utils/GeometryBackendTraits.h"
#include "movetk/utils/Iterators.h"


int main(int argc, char **argv) {
	using MovetkPoint = GeometryKernel::MovetkGeometryKernel::MovetkPoint;
	using PolyLine = std::vector<MovetkPoint>;
	if constexpr (!std::is_void_v<typename GeometryKernel::MovetkGeometryKernel::MovetkCurveIntersection>) {
		movetk::geom::MakePoint<GeometryKernel::MovetkGeometryKernel> make_point;
		using IntersectionTraits = movetk::geom::IntersectionTraits<GeometryKernel::MovetkGeometryKernel,
		                                                            GeometryKernel::Norm,
		                                                            movetk::geom::polylines_instersection_tag>;
		movetk::geom::ComputeIntersections<IntersectionTraits> compute_intersections;

		PolyLine polyline({make_point({3, 3}),
		                   make_point({3, 3}),
		                   make_point({6, 5}),
		                   make_point({6, 5}),
		                   make_point({9, 3}),
		                   make_point({2.94, 5.42}),
		                   make_point({3, 3}),
		                   make_point({5.92, 1.62}),
		                   make_point({8.8, 5.12}),
		                   make_point({1.26, 4.24})});

		std::cout << "Number of intersections: " << compute_intersections(std::begin(polyline), std::end(polyline));

		std::cout << "\n";

		polyline = PolyLine({make_point({-8228041.827950, 4975892.874943}),
		                     make_point({-8228041.827950, 4975892.874943}),
		                     make_point({-8228042.677250, 4975893.996076}),
		                     make_point({-8228039.280049, 4975892.874943}),
		                     make_point({-8228040.129349, 4975892.874943}),
		                     make_point({-8228034.184247, 4975892.874943}),
		                     make_point({-8228031.636346, 4975894.556643}),
		                     make_point({-8228032.485646, 4975892.874943}),
		                     make_point({-8228033.334946, 4975893.996076}),
		                     make_point({-8228033.334946, 4975893.996076}),
		                     make_point({-8228033.334946, 4975894.556643}),
		                     make_point({-8228033.334946, 4975893.996076}),
		                     make_point({-8228035.033547, 4975892.314376}),
		                     make_point({-8228035.033547, 4975892.874943}),
		                     make_point({-8228034.184247, 4975892.314376}),
		                     make_point({-8228034.184247, 4975892.314376})});
		std::cout << "Number of intersections: " << compute_intersections(std::begin(polyline), std::end(polyline));
		std::cout << "\n";
	} else {
		std::cout << "Selected backend doesn't support intersections" << '\n';
	}
}