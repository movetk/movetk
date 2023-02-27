/*
 * Copyright (C) 2018-2020
 * HERE Europe B.V.
 * Utrecht University (The Netherlands).
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


/*! @file Segmentation.h
 *  @brief  Trajectory Segmentation  Algorithms
 *  @authors Aniket Mitra (aniket.mitra@here.com), Wouter Jongeling (wouter.jongeling@gmail.com)
 */
#ifndef MOVETK_ALGO_SEGMENTATION_MODELBASEDSEGMENTATION_H
#define MOVETK_ALGO_SEGMENTATION_MODELBASEDSEGMENTATION_H
#include "movetk/Search.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/Requirements.h"

namespace movetk::segmentation {
/**
 * @brief Implements a model based segmentation algorithm.
 * 
 * This algorithm allows for segmenting a trajectory in subtrajectory that exhibit similar 
 * movement characteristics. The algorithm assumes that the movement model can be parameterized by 
 * a single parameter. 
 * @tparam GeometryTraits 
 * @tparam LogLikelihood 
 * @complexity \f$O(nm)\f$ for a trajectory of complexity \f$n\f$ and for \f$m\f$ model parameter values.
 * Based on \mtkCite{alewijnse2018model}.
*/
template <class GeometryTraits, class LogLikelihood>
class ModelBasedSegmentation {
	// based on https://doi.org/10.1007/s00453-017-0329-x

	using NT = typename GeometryTraits::NT;
	using Row = std::vector<typename GeometryTraits::NT>;
	using Table = std::vector<Row>;
	using TableIterator = typename Table::const_iterator;

	struct InformationCriteria {
		NT penalty_factor;
		NT operator()(NT log_likelihood) { return -2 * log_likelihood + penalty_factor; }
	} ic;

public:
	/**
	 * @brief Construct the segmentation algorithm
	 * @param penalty_factor The penalty factor for the information criterion.
	*/
	explicit ModelBasedSegmentation(NT penalty_factor) { ic.penalty_factor = penalty_factor; }

	/**
	 * @brief 
	 * @param rfirst 
	 * @param rbeyond 
	 * @param cfirst 
	 * @param cbeyond 
	 * @param result 
	*/
	template <std::random_access_iterator RowIterator,
	          std::random_access_iterator ColumnIterator,
	          utils::OutputIterator<RowIterator> OutputIterator>
	void operator()(RowIterator rfirst,
	                RowIterator rbeyond,
	                ColumnIterator cfirst,
	                ColumnIterator cbeyond,
	                OutputIterator result) {
		LogLikelihood ll;

		std::size_t num_rows = std::distance(rfirst, rbeyond);
		std::size_t num_columns = std::distance(cfirst, cbeyond);

		Row dp_row;
		Table dp_table(num_rows);

		// Fill up Dynamic Programming Table
		ll(*rfirst, cfirst, cbeyond, std::back_inserter(dp_row));

		std::transform(std::begin(dp_row), std::end(dp_row), std::begin(dp_row), ic);
		auto min_ic = std::min_element(std::cbegin(dp_row), std::cend(dp_row));
		std::size_t i = 0;
		dp_table[i] = std::move(dp_row);
		i++;
		RowIterator rit = rfirst + 1;
		while (rit != rbeyond) {
			ColumnIterator cit = cfirst;
			std::size_t j = 0;
			Row dp_row(num_columns);
			while (cit != cbeyond) {
				NT log_likelihood = ll(*rit, *cit);
				NT ic_extend = dp_table[i - 1][j];
				NT ic_append = *min_ic + ic.penalty_factor;
				dp_row[j] = std::min(ic_extend, ic_append) - 2 * log_likelihood;
				cit++;
				j++;
			}
			min_ic = std::min_element(std::cbegin(dp_row), std::cend(dp_row));
			dp_table[i] = std::move(dp_row);
			rit++;
			i++;
		}

		// Backtrack through the  Dynamic Programming Table
		TableIterator table_iterator = std::cend(dp_table) - 1;
		RowIterator parameter_iterator = rbeyond - 1;
		auto last_min_ic = std::min_element(std::cbegin(*table_iterator), std::cend(*table_iterator));
		std::size_t last_position = std::distance(std::cbegin(*table_iterator), last_min_ic);
		*result = parameter_iterator;

		if (num_rows == 1)
			return;

		table_iterator--;
		parameter_iterator--;

		while (table_iterator != std::cbegin(dp_table)) {
			min_ic = std::min_element(std::cbegin(*table_iterator), std::cend(*table_iterator));
			std::size_t position = std::distance(std::cbegin(*table_iterator), min_ic);
			if (last_position != position) {
				*result = parameter_iterator;
			}
			last_position = position;
			table_iterator--;
			parameter_iterator--;
		}

		min_ic = std::min_element(std::cbegin(*table_iterator), std::cend(*table_iterator));
		std::size_t position = std::distance(std::cbegin(*table_iterator), min_ic);

		if (last_position != position) {
			*result = parameter_iterator;
		}
	}
};


}  // namespace movetk::segmentation
#endif