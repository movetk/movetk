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
// Created by Mitra, Aniket on 2020-10-13.
//

#ifndef DISTANCEINTERFACE_H
#define DISTANCEINTERFACE_H
#include "DistanceTraits.h"

namespace movetk::metric {
/**
 * @brief Interface for the squared distance algorithm
 *
 * @tparam Kernel The kernel to use
 * @tparam Norm The norm to use
 */
template <class Kernel, class Norm>
struct ComputeSquaredDistance {
	/**
	 * @brief Overloads the function operator to calculate the squared distance
	 * between two geometric objects
	 * @tparam T1  A geometric object
	 * @tparam T2  A geometric object
	 * @param object1
	 * @param object2
	 * @return GeometryTraits::NT
	 */
	template <class T1, class T2>
	typename Kernel::NT operator()(T1& object1, T2& object2) {
		using algorithm = squared_distance_algorithm<Kernel, Norm, typename Kernel::MovetkSquaredDistance>;
		typename algorithm::square_distance distance;
		return distance(object1, object2);
	}
};

/**
 * @brief Interface for the Discrete Hausdorff distance algorithm
 *
 * @tparam GeometryTraits
 * @tparam Norm
 */
template <class GeometryTraits, class Norm>
struct ComputeDiscreteHausdorffDistance {
	/**
	 * @brief Overloads the function operator to accept iterators over two
	 * point sets and returns the Discrete Hausdorff distance between them
	 * @tparam InputIterator
	 * @param polyline_a_first
	 * @param polyline_a_beyond
	 * @param polyline_b_first
	 * @param polyline_b_beyond
	 * @return GeometryTraits::NT
	 */
	template <utils::RandomAccessIterator<typename GeometryTraits::MovetkPoint> InputIterator>
	typename GeometryTraits::NT operator()(InputIterator polyline_a_first,
	                                       InputIterator polyline_a_beyond,
	                                       InputIterator polyline_b_first,
	                                       InputIterator polyline_b_beyond) {
		using algorithm = discrete_hausdorff_distance_algorithm<GeometryTraits,
		                                                        Norm,
		                                                        typename GeometryTraits::MovetkDiscreteHausdorffDistance>;
		typename algorithm::discrete_hausdorff_distance distance;
		return distance(polyline_a_first, polyline_a_beyond, polyline_b_first, polyline_b_beyond);
	}
};

/**
 * @brief Interface for the Discrete Frechet distance algorithm
 *
 * @tparam GeometryTraits
 * @tparam Norm
 */
template <class GeometryTraits, class Norm>
class ComputeDiscreteFrechetDistance {
public:
	/**
	 * @brief Overloads the function operator to accept iterators over two
	 * point sets and returns the Discrete Frechet distance between them
	 * @tparam InputIterator
	 * @param polyline_a_first
	 * @param polyline_a_beyond
	 * @param polyline_b_first
	 * @param polyline_b_beyond
	 * @return GeometryTraits::NT
	 */
	template <utils::RandomAccessIterator<typename GeometryTraits::MovetkPoint> InputIterator>
	typename GeometryTraits::NT operator()(InputIterator polyline_a_first,
	                                       InputIterator polyline_a_beyond,
	                                       InputIterator polyline_b_first,
	                                       InputIterator polyline_b_beyond) {
		typedef discrete_frechet_distance_algorithm<GeometryTraits,
		                                            Norm,
		                                            typename GeometryTraits::MovetkDiscreteFrechetDistance>
		    algorithm;
		typename algorithm::discrete_frechet_distance distance;
		return distance(polyline_a_first, polyline_a_beyond, polyline_b_first, polyline_b_beyond);
	}

	/**
	 * @brief Overloads the function operator to accept iterator over
	 * a collection of point sets and an upper traingular matrix to
	 * store the result of the pairwise discrete Frechet Distance computation
	 * @tparam DistanceMatrix
	 * @tparam InputIterator
	 * @tparam InputIterator
	 * @param first
	 * @param beyond
	 * @param upper_triag_mat
	 * @return GeometryTraits::NT
	 */
	template <utils::Iterable2D<typename GeometryTraits::NT> DistanceMatrix, std::random_access_iterator InputIterator>
		requires(utils::Iterable<std::iter_value_t<InputIterator>, typename GeometryTraits::MovetkPoint>)
	typename GeometryTraits::NT operator()(InputIterator first, InputIterator beyond, DistanceMatrix& upper_triag_mat) {
		using algorithm = discrete_frechet_distance_algorithm<GeometryTraits,
		                                                      Norm,
		                                                      typename GeometryTraits::MovetkDiscreteFrechetDistance>;
		typename algorithm::discrete_frechet_distance distance;
		auto bit = std::back_inserter(upper_triag_mat);
		auto it = first;
		for (; it != beyond; ++it) {
			auto nit = it + 1;
			typename DistanceMatrix::value_type node;
			auto node_bit = std::back_inserter(node);
			while (nit != beyond) {
				node_bit = distance(std::cbegin(*it), std::cend(*it), std::cbegin(*nit), std::cend(*nit));
				nit++;
			}
			bit = node;
		}
		auto rit = std::begin(upper_triag_mat);
		typename GeometryTraits::NT min_sum = 1 / MOVETK_EPS;
		std::size_t num_columns = std::distance(std::begin(*rit), std::end(*rit));
		std::size_t row_id = 0;
		while (rit != std::end(upper_triag_mat)) {
			typename GeometryTraits::NT max = MOVETK_EPS;
			typename GeometryTraits::NT second_max = MOVETK_EPS;
			auto vit = std::begin(*rit);
			std::size_t num_cols_right = std::distance(std::begin(*rit), std::end(*rit));
			std::size_t num_cols_left = num_columns - num_cols_right;
			std::size_t counter = 0;
			while (counter != num_cols_left) {
				auto row_it = std::begin(upper_triag_mat) + counter;
				std::size_t offset = row_id - counter - 1;
				auto column_it = std::begin(*row_it) + offset;
				if (*column_it > max) {
					second_max = max;
					max = *column_it;
				} else if (*column_it > second_max && *column_it < max) {
					second_max = *column_it;
				}
				counter++;
			}
			while (vit != std::end(*rit)) {
				if (*vit > max) {
					second_max = max;
					max = *vit;
				} else if (*vit > second_max && *vit < max) {
					second_max = *vit;
				}
				vit++;
			}
			typename GeometryTraits::NT sum = max + second_max;
			if (sum < min_sum) {
				min_sum = sum;
			}
			rit++;
			row_id++;
		}
		return min_sum;
	}
};

}  // namespace movetk::metric
#endif  // DISTANCEINTERFACE_H
