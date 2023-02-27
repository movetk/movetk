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

//
// Created by Wouter Jongeling (wouter.jongeling@gmail.com)
// Modified by Mitra, Aniket on 2019-06-18.
//

/*!
 * @file BrownianBridge.h
 * @brief
 * @authors Aniket Mitra (aniket.mitra@here.com)
 *  Wouter Jongeling (wouter.jongeling@gmail.com)
 */


#ifndef MOVETK_BROWNIANBRIDGE__H
#define MOVETK_BROWNIANBRIDGE__H

#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_min.h>

#include <cmath>
#include <iterator>

#include "movetk/geom/trajectory_to_interface.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/Requirements.h"

/*!
 * @file AlgorithmTraits.h
 * @brief The Brownian Bridge Movement Model (BBMM)
 * @authors Aniket Mitra (aniket.mitra@here.com)
 */

/*!
 *  @brief Collection of trajectory segmentation algorithms
 */
namespace movetk::segmentation {

// based on
// https://www.semanticscholar.org/paper/Computational-Movement-Analysis-Using-Brownian-Sijben/c6104979e609e12e1514b8b605a7d14fa6b5d81a

/*!
 *  @brief A collection of classes for BBMM
 */
namespace brownian_bridge {

/*!
 * @struct ParameterTraits
 * @brief This traits class serves as a collection of types
 * for parameterization of BBMM
 * @tparam _GeometryTraits -  This class is a collection of movetk
 *  geometry types. For example @refitem movetk::geom::MovetkGeometryKernel
 * @tparam TrajectoryIterator - An iterator type.
 * For example @refitem TabularTrajectory::TrajectoryIterator
 */
template <class _GeometryTraits, class TrajectoryIterator>
struct ParameterTraits {
	using GeometryTraits = _GeometryTraits;
	/*!*
	 * @typedef ::NT
	 * @brief typedef  of the number type defined in GeometryTraits.
	 * For example @refitem  movetk::geom::MovetkGeometryKernel::NT
	 * */
	using NT = typename GeometryTraits::NT;
	/*!*
	 * @typedef ::Point
	 * @brief typedef of the point type defined in GeometryTraits.
	 * For example @refitem movetk::geom::MovetkGeometryKernel::MovetkPoint
	 *  */
	using Point = typename GeometryTraits::MovetkPoint;
	/*!* @typedef ::Vector
	 * @brief typedef of the vector type defined in GeometryTraits.
	 * For example @refitem  movetk::geom::MovetkGeometryKernel::MovetkVector
	 * */
	using Vector = typename GeometryTraits::MovetkVector;
	/*!* @typedef ::Parameters
	 */
	using Parameters = std::tuple<Point, Point, NT, TrajectoryIterator, TrajectoryIterator>;
	/*!
	 * @enum ParameterColumns
	 *
	 */
	enum ParameterColumns {
		POINT,         /*!< global enum value 1 */
		MU,            /*!< global enum value 1 */
		SIGMA_SQUARED, /*!< global enum value 1 */
		BEGIN,         /*!< global enum value 1 */
		END            /*!< global enum value 1 */
	};
};

namespace concepts {
template <typename PARAMETER_TRAITS>
concept ParameterTraits = requires() {
	requires std::convertible_to<decltype(PARAMETER_TRAITS::ParameterColumns::POINT), size_t>;
	requires std::convertible_to<decltype(PARAMETER_TRAITS::ParameterColumns::MU), size_t>;
	requires std::convertible_to<decltype(PARAMETER_TRAITS::ParameterColumns::SIGMA_SQUARED), size_t>;
};
}  // namespace concepts

/*!
 * @brief The Maximum Likelihood Estimator
 * @tparam GeometryTraits -   This class is a collection of movetk
 *  geometry types.
 * @tparam ParameterTraits -  This traits class serves as a collection of types
 * for parameterization of BBMM
 * @tparam Norm - The type that models Euclidean distance
 * @tparam InputIterator - A random access iterator type over a collection of
 * tuples where each tuple is a model of type ParameterTraits::Parameters
 * @tparam MaxIter - A positive integer that bounds the number of iterations in the MLE
 * algorithm
 */
template <
    class GeometryTraits,
    concepts::ParameterTraits ParameterTraits,
    class Norm,
    utils::RandomAccessIteratorOfTupleWith<
        utils::TypeAt<ParameterTraits::ParameterColumns::POINT, typename GeometryTraits::MovetkPoint>,
        utils::TypeAt<ParameterTraits::ParameterColumns::MU, typename GeometryTraits::MovetkPoint>,
        utils::TypeAt<ParameterTraits::ParameterColumns::SIGMA_SQUARED, typename GeometryTraits::NT>> InputIterator,
    std::size_t MaxIter>
class MLE {
private:
	using NT = typename GeometryTraits::NT;
	const gsl_min_fminimizer_type *T = gsl_min_fminimizer_goldensection;
	gsl_min_fminimizer *s = gsl_min_fminimizer_alloc(T);
	gsl_function F;
	NT estimated_parameter;
	std::size_t iter = 0, status;

	struct fn_params {
		InputIterator first;
		InputIterator beyond;
	};

	static double fn(double sigma_squared, void *p) {
		Norm norm;
		struct fn_params *params = (fn_params *)p;
		InputIterator it = params->first;
		NT log_likelihood = 0;
		for (; it != params->beyond; ++it) {
			const auto v = std::get<ParameterTraits::ParameterColumns::POINT>(*it) -
			               std::get<ParameterTraits::ParameterColumns::MU>(*it);
			// Elements of the log of a Gaussian with sigma as standard deviation
			NT operand1 = -LOG_TWO_PI - log(static_cast<NT>(sigma_squared));
			NT operand2 = -norm(v) / (2 * sigma_squared);
			log_likelihood += operand1 + operand2;
		}
		return -1 * log_likelihood;
	}

	void operator()(InputIterator first, InputIterator beyond, NT result, NT x_lower, NT x_upper, NT eps) {
		fn_params params;
		params.first = first;
		params.beyond = beyond;
		F.function = &MLE::fn;
		F.params = &params;
		status = gsl_min_fminimizer_set(s, &F, result, x_lower, x_upper);
		do {
			iter++;
			status = gsl_min_fminimizer_iterate(s);
			result = gsl_min_fminimizer_x_minimum(s);
			x_lower = gsl_min_fminimizer_x_lower(s);
			x_upper = gsl_min_fminimizer_x_upper(s);

			status = gsl_min_test_interval(x_lower, x_upper, eps, 0.0);

			if (status == GSL_SUCCESS) {
				estimated_parameter = result;
				break;
			}
		} while (status == GSL_CONTINUE && iter < MaxIter);

		if (iter >= MaxIter)
			estimated_parameter = result;
	}

public:
	/*!
	 *
	 * @param first
	 * @param beyond
	 * @param result
	 * @param x_lower
	 * @param x_upper
	 * @param eps
	 */
	MLE(InputIterator first, InputIterator beyond, NT result, NT x_lower, NT x_upper, NT eps) {
		Norm norm;
		std::size_t num_elements = std::distance(first, beyond);
		if (num_elements == 1) {
			const auto v = std::get<ParameterTraits::ParameterColumns::POINT>(*first) -
			               std::get<ParameterTraits::ParameterColumns::MU>(*first);
			NT l = norm(v);
			if (l < MOVETK_EPS) {
				estimated_parameter = MOVETK_EPS;
				return;
			}
		}
		(*this)(first, beyond, result, x_lower, x_upper, eps);
	}

	/*!
	 *
	 * @param first
	 * @param beyond
	 */
	MLE(InputIterator first, InputIterator beyond) {
		Norm norm;
		InputIterator it = first;
		NT upper_bound = 0, squared_length = 0;
		std::size_t num_elements = std::distance(first, beyond);
		if (num_elements == 1) {
			const auto v = std::get<ParameterTraits::ParameterColumns::POINT>(*first) -
			               std::get<ParameterTraits::ParameterColumns::MU>(*first);
			NT l = norm(v);
			if (l < MOVETK_EPS) {
				estimated_parameter = MOVETK_EPS;
				return;
			}
		}
		for (; it != beyond; ++it) {
			const auto v = std::get<ParameterTraits::ParameterColumns::POINT>(*it) -
			               std::get<ParameterTraits::ParameterColumns::MU>(*it);
			NT l = norm(v);
			squared_length += l;
			if (l > upper_bound) {
				upper_bound = l;
			}
		}
		NT initial_estimate = squared_length / (2 * num_elements);
		(*this)(first, beyond, initial_estimate, MOVETK_EPS, upper_bound, MOVETK_EPS);
	}

	/*!
	 * Returns the estimated parameter
	 * @return The estimated parameter
	 */
	NT operator()() const { return estimated_parameter; }

	~MLE() { gsl_min_fminimizer_free(s); }
};


/*!
 *
 * @tparam GeometryTraits
 * @tparam ProbeTraits
 * @tparam ParameterTraits
 * @tparam Norm
 * @tparam GeoProjection
 */
template <class GeometryTraits, class ProbeTraits, class ParameterTraits, class Norm, class GeoProjection>
class Model {
private:
	typedef typename GeometryTraits::NT NT;
	typedef typename GeometryTraits::MovetkPoint Point;
	typedef typename GeometryTraits::MovetkVector Vector;
	geom::MakePoint<GeometryTraits> make_point;

	NT joint_log_likelihood = 0;

	static Point lerp(Point &p1, Point &p2, NT alpha) {
		Vector v = p2 - p1;
		v *= alpha;
		return (p1 + v);
	}

public:
	/*!
	 *
	 * @tparam TrajectoryIterator
	 * @tparam OutputIterator
	 * @param first
	 * @param beyond
	 * @param result
	 */
	template <std::random_access_iterator TrajectoryIterator,
	          utils::OutputIterator<std::tuple<Point, Point, int, TrajectoryIterator, TrajectoryIterator>> OutputIterator>
	/*typename = movetk::utils::requires_random_access_iterator<TrajectoryIterator>,
	typename = movetk::utils::requires_tuple<typename TrajectoryIterator::value_type>,
	typename = movetk::utils::requiresct_tuple_element_as_arithmetic<ProbeTraits::ProbeColumns::LAT,
	                                                               typename TrajectoryIterator::value_type>,
	typename = movetk::utils::requires_tuple_element_as_arithmetic<ProbeTraits::ProbeColumns::LON,
	                                                               typename TrajectoryIterator::value_type>,
	typename = movetk::utils::requires_tuple_element_as_size_t<ProbeTraits::ProbeColumns::SAMPLE_DATE,
	                                                           typename TrajectoryIterator::value_type>*/
	Model(TrajectoryIterator first, TrajectoryIterator beyond, OutputIterator result) {
		const auto reflat = std::get<ProbeTraits::ProbeColumns::LAT>(*first);
		const auto reflon = std::get<ProbeTraits::ProbeColumns::LON>(*first);
		GeoProjection ref(reflat, reflon);

		TrajectoryIterator tit = first;
		std::size_t NumPoints = std::distance(first, beyond);

		auto point_from_iterator = [this, &ref](auto iterator) {
			auto lat = std::get<ProbeTraits::ProbeColumns::LAT>(*iterator);
			auto lon = std::get<ProbeTraits::ProbeColumns::LON>(*iterator);
			auto projected_point = ref.project(lat, lon);
			return make_point(std::cbegin(projected_point), std::cend(projected_point));
		};
		if (NumPoints == 1) {
			Point p1 = point_from_iterator(first);
			*result = std::make_tuple(p1, p1, 0, first, first);
			return;
		}

		std::size_t multiple = (NumPoints - 1) / 2;
		std::size_t remainder = (NumPoints - 1) % 2;
		TrajectoryIterator last = first + 2 * multiple;

		while (tit != last) {
			Point p1 = point_from_iterator(tit);
			Point p2 = point_from_iterator(tit + 1);
			Point p3 = point_from_iterator(tit + 2);

			auto ts1 = std::get<ProbeTraits::ProbeColumns::SAMPLE_DATE>(*tit);
			auto ts2 = std::get<ProbeTraits::ProbeColumns::SAMPLE_DATE>(*(tit + 1));
			auto ts3 = std::get<ProbeTraits::ProbeColumns::SAMPLE_DATE>(*(tit + 2));
			NT alpha = static_cast<NT>(ts3 - ts2) / static_cast<NT>(ts3 - ts1);

			Point mu = lerp(p1, p3, alpha);
			*result = std::make_tuple(p2, mu, 0, tit, tit + 2);
			tit += 2;
		}

		if (remainder == 1) {
			Point p1 = point_from_iterator(last);
			Point p2 = point_from_iterator(last + 1);
			Point mu = lerp(p1, p2, 0.5);
			*result = std::make_tuple(mu, mu, 0, last, last + 1);
		}
	}
};


/*!
 *
 * @tparam Kernel
 * @tparam ParameterTraits
 */
template <class Kernel, class ParameterTraits>
class ParameterSelector {
	std::size_t SIZE;

public:
	using NT = typename Kernel::NT;
	/*!
	 *
	 * @param size
	 */
	explicit ParameterSelector(std::size_t size) : SIZE(size) {}

	/*!
	 *
	 * @tparam InputIterator
	 * @tparam OutputIterator
	 * @param first
	 * @param beyond
	 * @param result
	 */
	template <utils::RandomAccessIteratorOfTupleWith<utils::TypeAt<ParameterTraits::ParameterColumns::SIGMA_SQUARED, NT>>
	              InputIterator,
	          utils::OutputIterator<NT> OutputIterator>
	void operator()(InputIterator first, InputIterator beyond, OutputIterator result) {
		assert(static_cast<std::size_t>(std::distance(first, beyond)) >= SIZE);
		std::vector<typename ParameterTraits::NT> coeffs;
		std::transform(first, beyond, std::back_inserter(coeffs), [](auto a) {
			return std::get<ParameterTraits::ParameterColumns::SIGMA_SQUARED>(a);
		});
		std::sort(std::begin(coeffs), std::end(coeffs));
		std::size_t coeff_interval = std::distance(first, beyond) / SIZE;
		for (std::size_t i = 0; i < SIZE; i++) {
			std::size_t index = coeff_interval / 2 + coeff_interval * i;
			*result = coeffs[index];
		}
	}
};

/**
 * @brief Functor for computing the log likelihood for several parameter values
 * @tparam GeometryTraits The kernel to use
 * @tparam ParameterTraits Traits for the parameters?
 * @tparam Norm The norm to use
 */
template <class GeometryTraits, class ParameterTraits, class Norm>
class LogLikelihood {
	using NT = typename GeometryTraits::NT;
	using Parameters = typename ParameterTraits::Parameters;

public:
	LogLikelihood() = default;

	/**
	 * @brief Computes the log-likelihood for the given parameters, using a range of
	 * parameter values for the squared standard deviation
	 * @tparam InputIterator Type of the input iterator
	 * @tparam OutputIterator Type of the output iterator to write to.
	 * @param params The base parameters to use
	 * @param first Start of standard deviation value range
	 * @param beyond End of standard deviation value range
	 * @param result Output iterator for writing the result to
	 */
	template <utils::RandomAccessIterator<NT> InputIterator, utils::OutputIterator<NT> OutputIterator>
	void operator()(const Parameters &params, InputIterator first, InputIterator beyond, OutputIterator result) {
		Norm norm;
		const auto v = std::get<ParameterTraits::ParameterColumns::POINT>(params) -
		               std::get<ParameterTraits::ParameterColumns::MU>(params);
		const auto squared_length = norm(v);
		for (auto pit = first; pit != beyond; ++pit) {
			const auto operand1 = -LOG_TWO_PI - log(*pit);
			const auto operand2 = -squared_length / (2 * (*pit));
			const auto log_likelihood = operand1 + operand2;
			*result = log_likelihood;
		}
	}

	/**
	 * @brief Computes the log-likelihood for a single squared standard deviation
	 * @param params The base parameters
	 * @param sigma_squared The squared standard deviation
	 * @return The log-likelihood
	 */
	NT operator()(const Parameters &params, NT sigma_squared) {
		Norm norm;
		const auto v = std::get<ParameterTraits::ParameterColumns::POINT>(params) -
		               std::get<ParameterTraits::ParameterColumns::MU>(params);
		const auto squared_length = norm(v);
		const auto operand1 = -LOG_TWO_PI - log(sigma_squared);
		const auto operand2 = -squared_length / (2 * sigma_squared);
		return (operand1 + operand2);
	}
};


}  // namespace brownian_bridge


}  // namespace movetk::segmentation

#endif  // MOVETK_BROWNIANBRIDGE__H
