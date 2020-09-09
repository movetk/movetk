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

/*!
 * @file AlgorithmTraits.h
 * @brief A collection of traits classes for various algorithms in movetk
 * @authors Aniket Mitra (aniket.mitra@here.com)
 */

#ifndef MOVETK_ALGORITHMTRAITS_H
#define MOVETK_ALGORITHMTRAITS_H

#include <boost/graph/named_function_params.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>

/*!
 *  @brief a collection of algorithms in movetk
 */
namespace movetk_algorithms {
    /*!
     * @struct geo_coordinates_tag
     * @brief This tag is used to specify whether the input will be a collection of
     * geo coordinates
     */
    struct geo_coordinates_tag;
    /*!
     * @struct cartesian_coordinates_tag
     * @brief This tag is used to specify whether the input will be a collection of
     * cartesian coordinates
     */
    struct cartesian_coordinates_tag;
    /*!
     * @struct linear_speed_bounded_test_tag
     * @brief This tag is used to select the type of outlier detection predicate
     */
    struct linear_speed_bounded_test_tag;
    /*!
     *@struct greedy_outlier_detector_tag
     * @brief This tag is used to select the type of outlier detection algorithm
     */
    struct greedy_outlier_detector_tag;
    /*!
     *@struct smart_greedy_outlier_detector_tag
     * @brief This tag is used to select the type of outlier detection algorithm
     */
    struct smart_greedy_outlier_detector_tag;
    /*!
     *@struct output_sensitive_outlier_detector_tag
     * @brief This tag is used to select the type of outlier detection algorithm
     */
    struct output_sensitive_outlier_detector_tag;
    /*!
     *@struct zheng_outlier_detector_tag
     * @brief This tag is used to select the type of outlier detection algorithm
     */
    struct zheng_outlier_detector_tag;

    struct linear_interpolator_tag;

    struct kinematic_interpolator_tag;

    struct random_trajectory_generator_tag;

    /*!
     * @struct OutlierDetectionTraits
     * @brief This traits class serves as a collection of
     * types required by various outlier detection algorithms
     * @tparam _ProbeTraits - This class is a collection of types
     * required to define a Probe. For example @refitem _ProbeTraits
     * @tparam _GeometryTraits - This class is a collection of movetk
     * geometry types. For example @refitem movetk_core::MovetkGeometryKernel
     * @tparam _Norm - The type that models Euclidean distance
     * For example @refitem movetk_support::FiniteNorm
     * @tparam _ContainerTraits - This class models the data structure used
     * by the outlier detection algorithnms
     */
    template<class _ProbeTraits,
            class _GeometryTraits,
            class _Norm,
            class _ContainerTraits = void>
    struct OutlierDetectionTraits {
        typedef _Norm Norm;
        typedef _ProbeTraits ProbeTraits;
        typedef _GeometryTraits GeometryTraits;
        typedef _ContainerTraits ContainerTraits;
        typedef typename _GeometryTraits::NT NT;
        typedef typename _GeometryTraits::MovetkPoint Point;
        typedef typename _GeometryTraits::MovetkVector Vector;
    };

    /*!
     * @struct ClusteringTraits
     * @brief This traits class serves as a collection of types
     * required for clustering with Discrete Frechet Distance
     * @tparam _FreeSpaceDiagram - A model of the Free Space Diagram.
     * For example @refitem movetk_support::FreeSpaceDiagram
     */
    template<class _FreeSpaceDiagram>
    struct ClusteringTraits {
        typedef _FreeSpaceDiagram FreeSpaceDiagram;
        typedef typename FreeSpaceDiagram::FreeSpaceDiagramTraits FreeSpaceDiagramTraits;
        typedef typename FreeSpaceDiagramTraits::FreeSpaceCellTraits FreeSpaceCellTraits;
        typedef typename FreeSpaceCellTraits::IntersectionTraits IntersectionTraits;
        typedef typename FreeSpaceDiagramTraits::GeometryTraits GeometryTraits;
        typedef typename GeometryTraits::NT NT;
        typedef typename FreeSpaceCellTraits::vertex_orientation vertex_orientation;
        typedef typename boost::property<boost::vertex_index_t, std::size_t> VertexProperty;
        typedef typename boost::property<boost::edge_name_t, std::size_t> EdgeProperty;
        typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS,
                VertexProperty, EdgeProperty> Graph;
        typedef typename boost::graph_traits<Graph>::vertex_iterator vertex_iterator;
        typedef typename boost::graph_traits<Graph>::edge_iterator edge_iterator;
        typedef typename boost::graph_traits<Graph>::out_edge_iterator out_edge_iterator;
        typedef typename boost::graph_traits<Graph>::edge_descriptor edge_descriptor;
    };


    /*!
         * @struct BBParameterTraits
         * @brief This traits class serves as a collection of types
         * for parameterization of BBMM
         * @tparam _GeometryTraits -  This class is a collection of movetk
         *  geometry types. For example @refitem movetk_core::MovetkGeometryKernel
         * @tparam TrajectoryIterator - An iterator type.
         * For example @refitem TabularTrajectory::TrajectoryIterator
         */
    template<class _GeometryTraits, class TrajectoryIterator , class _Norm>
    struct BBMMParameterTraits {
        typedef _Norm Norm;
        typedef _GeometryTraits GeometryTraits;
        /*!*
         * @typedef ::NT
         * @brief typedef  of the number type defined in GeometryTraits.
         * For example @refitem  movetk_core::MovetkGeometryKernel::NT
         * */
        typedef typename GeometryTraits::NT NT;
        /*!*
         * @typedef ::Point
         * @brief typedef of the point type defined in GeometryTraits.
         * For example @refitem movetk_core::MovetkGeometryKernel::MovetkPoint
         *  */
        typedef typename GeometryTraits::MovetkPoint Point;
        /*!* @typedef ::Vector
         * @brief typedef of the vector type defined in GeometryTraits.
         * For example @refitem  movetk_core::MovetkGeometryKernel::MovetkVector
         * */
        typedef typename GeometryTraits::MovetkVector Vector;
        /*!* @typedef ::Parameters
         */
        typedef std::tuple<Point, Point, NT, TrajectoryIterator, TrajectoryIterator> Parameters;
        /*!
         * @enum ParameterColumns
         *
         */
        enum ParameterColumns {
            POINT,  /*!< global enum value 1 */
            MU,     /*!< global enum value 1 */
            SIGMA_SQUARED,  /*!< global enum value 1 */
            BEGIN,  /*!< global enum value 1 */
            END    /*!< global enum value 1 */
        };

    };




}

#endif //MOVETK_ALGORITHMTRAITS_H
