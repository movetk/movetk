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
// Created by Mitra, Aniket on 2019-02-19.
//

#include <cxxopts.hpp>

#include "ExampleSetup.h"
#include "movetk/metric/DistanceInterface.h"
#include "movetk/utils/GeometryBackendTraits.h"
#include "movetk/utils/Meta.h"

struct Example {
	static constexpr const char* NAME = "compute_squared_distance";
	static constexpr const char* DESCRIPTION =
	    "Example for computing squared distance between points and a point and segment";

	template <typename Kernel>
	void run(cxxopts::ParseResult& arguments) {
		movetk::geom::MakePoint<Kernel> make_point;
		auto pt1 = make_point({3, 3});

		auto pt2 = make_point({3, 1});
		auto pt3 = make_point({5, 3});
		movetk::geom::MakeSegment<Kernel> make_segment;
		auto seg = make_segment(pt2, pt3);

		// Compute squared distance using Euclidean norm
		using Norm = movetk::metric::FiniteNorm<Kernel,2>;
		movetk::metric::ComputeSquaredDistance<Kernel, Norm> squared_dist;
		std::cout << "Squared Distance from Point to Segment: " << squared_dist(pt1, seg) << std::endl;
	}
};
int main(int argc, char** argv) {
	return movetk::examples::ExampleRunner().run_example<Example>(argc, argv);
}
