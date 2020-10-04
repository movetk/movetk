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
// Created by Mitra, Aniket 17/11/2019
//

#include "GeometryBackendTraits.h"
#include "movetk/algo/SegmentationTraits.h"
#include "movetk/ds/StartStopMatrix.h"
#include "movetk/io/csv/ParseDate.h"
#include "movetk/geo/geo.h"
#include "movetk/utils/TrajectoryUtils.h"
#include <algorithm>

#include <GeographicLib/LocalCartesian.hpp>
#include <GeographicLib/Geodesic.hpp>
#include "movetk/algo/BrownianBridge.h"
#include "movetk/algo/Simplification.h"
#include "movetk/algo/AlgorithmTraits.h"
#include "movetk/algo/Clustering.h"
#include "movetk/algo/Interpolation.h"

typedef GeometryKernel::MovetkGeometryKernel MovetkGeometryKernel;
typedef GeometryKernel::Norm Norm;
typedef GeometryKernel::SphSegIntersectionTraits SphSegIntersectionTraits;
using IntersectionAttributes = SphSegIntersectionTraits::Attributes;
typedef GeometryKernel::SphSphIntersectionTraits SphSphIntersectionTraits;
typedef GeometryKernel::FreeSpaceCellTraits FreeSpaceCellTraits;
typedef GeometryKernel::FreeSpaceDiagram FreeSpaceDiagram;

using namespace GeographicLib;

double distance_exact(double lat0, double lon0, double lat1, double lon1)
{
    const Geodesic &geod = Geodesic::WGS84();
    double s12;
    geod.Inverse(lat0, lon0, lat1, lon1, s12);
    //    std::cout.setf(std::ios::fixed);
    //    std::cout << "distance: " << std::setprecision(6) << lat0 << " " << std::setprecision(6) << lon0 << " " << std::setprecision(6) << lat1 << " " << std::setprecision(6) << lon1 << " ==> " << s12 << '\n';
    return s12;
}

void destination_exact(double lat0, double lon0, double x, double y, double &lat, double &lon)
{
    const Geocentric &earth = Geocentric::WGS84();
    LocalCartesian proj(lat0, lon0, 0, earth);
    double h; // unused
    proj.Reverse(x, y, 0, lat, lon, h);
}

void destination_by_bearing_exact(double lat0, double lon0, double bearing, double distance, double &lat1, double &lon1)
{
    const Geodesic &geod = Geodesic::WGS84();
    geod.Direct(lat0, lon0, bearing, distance, lat1, lon1);
}

double bearing_exact(double lat0, double lon0, double lat1, double lon1)
{
    const Geodesic &geod = Geodesic::WGS84();
    double s12, azi1, azi2;
    geod.Inverse(lat0, lon0, lat1, lon1, s12, azi1, azi2);
    return azi1;
}

void meters_per_degree(double lat0, double lon0, double &meters_per_lat_degree, double &meters_per_lon_degree)
{
    double lat, lon;
    destination_exact(lat0, lon0, 1, 1, lat, lon);
    meters_per_lat_degree = 1.0 / (lat - lat0);
    meters_per_lon_degree = 1.0 / (lon - lon0);
}

double euclidean_distance(double x0, double y0, double x1, double y1)
{
    auto dx = x1 - x0;
    auto dy = y1 - y0;
    return sqrt(dx * dx + dy * dy);
}

double euclidean_distance_3d(double x0, double y0, double z0, double x1, double y1, double z1)
{
    auto dx = x1 - x0;
    auto dy = y1 - y0;
    auto dz = z1 - z0;
    return sqrt(dx * dx + dy * dy + dz * dz);
}

typedef movetk_algorithms::SegmentationTraits<MovetkGeometryKernel::NT, MovetkGeometryKernel, MovetkGeometryKernel::dim> SegmentationTraits;
typedef std::vector<std::size_t> Segments;
typedef movetk_support::StartStopDiagram<SsdType::compressed, MovetkGeometryKernel, Segments> StartStopDiagram;

template <class GeometryTraits,
          class Norm,
          class TrajectoryIterator,
          class ProbeTraits>
struct BBMMTraits
{
    typedef movetk_algorithms::brownian_bridge::ParameterTraits<GeometryTraits, TrajectoryIterator> ParameterTraits;
    typedef typename std::vector<typename ParameterTraits::Parameters>::const_iterator BridgeIterator;
    typedef LocalCoordinateReference<typename GeometryTraits::NT> Projection;
    typedef movetk_algorithms::brownian_bridge::Model<GeometryTraits, ProbeTraits,
                                                      ParameterTraits, Norm, Projection>
        BBMM;
    typedef movetk_algorithms::brownian_bridge::MLE<GeometryTraits, ParameterTraits, Norm, BridgeIterator, 1000> MLE;
    typedef movetk_algorithms::brownian_bridge::ParameterSelector<GeometryTraits, ParameterTraits> ParameterSelector;
    typedef movetk_algorithms::brownian_bridge::LogLikelihood<GeometryTraits, ParameterTraits, Norm> LogLikelihood;
    typedef movetk_algorithms::ModelBasedSegmentation<GeometryTraits, LogLikelihood> ModelBasedSegmentation;
};

typedef movetk_algorithms::FindFarthest<MovetkGeometryKernel, Norm> FindFarthest;
typedef movetk_algorithms::DouglasPeucker<MovetkGeometryKernel, FindFarthest> DouglasPeucker;

typedef movetk_core::Wedge<MovetkGeometryKernel, Norm> Wedge;
typedef movetk_algorithms::ChanChin<MovetkGeometryKernel, Wedge> ChanChin;
typedef movetk_algorithms::ImaiIri<MovetkGeometryKernel, ChanChin> ImaiIri;

typedef movetk_algorithms::ClusteringTraits<FreeSpaceDiagram> ClusteringTraits;
typedef movetk_algorithms::SubTrajectoryClustering<ClusteringTraits> SubTrajectoryClustering;

template <class ProbeTraits>
struct IPTraits
{
    typedef LocalCoordinateReference<typename MovetkGeometryKernel::NT> Projection;
    typedef movetk_algorithms::InterpolationTraits<MovetkGeometryKernel, Projection, ProbeTraits, Norm> InterpolationTraits;
    typedef movetk_algorithms::Interpolator<movetk_algorithms::linear_interpolator_tag, InterpolationTraits,
                                            ProbeTraits::ProbeColumns::LAT,
                                            ProbeTraits::ProbeColumns::LON, ProbeTraits::ProbeColumns::SAMPLE_DATE,
                                            ProbeTraits::ProbeColumns::SPEED, ProbeTraits::ProbeColumns::HEADING>
        LinearInterpolator;
    typedef movetk_algorithms::Interpolator<movetk_algorithms::kinematic_interpolator_tag, InterpolationTraits,
                                            ProbeTraits::ProbeColumns::LAT,
                                            ProbeTraits::ProbeColumns::LON, ProbeTraits::ProbeColumns::SAMPLE_DATE,
                                            ProbeTraits::ProbeColumns::SPEED, ProbeTraits::ProbeColumns::HEADING>
        KinematicInterpolator;
};

#pragma cling load("libGeographic.so")
#pragma cling load "libgslcblas.so")
#pragma cling load("libgsl.so")
