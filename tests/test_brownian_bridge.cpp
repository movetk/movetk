/*
* Copyright (C) 2018-2020
* HERE Europe B.V.
* Utrecht University (The Netherlands).
* TU/e (The Netherlands).
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
// Created by Mitra, Aniket on 2019-06-20.
//


#include <array>
#include "catch2/catch.hpp"

#if CGAL_BACKEND_ENABLED
#include "movetk/geom/CGALTraits.h"
#else

#include "movetk/geom/BoostGeometryTraits.h"

#endif

#include "movetk/geom/GeometryInterface.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/TrajectoryUtils.h"
#include "movetk/metric/Norm.h"
#include "movetk/ds/TabularTrajectory.h"
#include "movetk/algo/BrownianBridge.h"


using namespace std;

#if CGAL_BACKEND_ENABLED
//==============================
    // Define the Number Type
    // For the CGAL backend,
    // One can choose from the
    // following number types
    typedef long double NT;
    //typedef CGAL::Mpzf NT;
    //typedef CGAL::Gmpfr NT;
    //typedef CGAL::Gmpq NT;
    //==============================

    //==============================
    // Define the Dimensions
    const size_t dimensions = 2;
    //==============================

    //==============================
    // Define the Geometry Backend
    typedef movetk_support::CGALTraits<NT, dimensions> CGAL_GeometryBackend;
    //Using the Geometry Backend define the Movetk Geometry Kernel
    typedef movetk_core::MovetkGeometryKernel<
            typename CGAL_GeometryBackend::Wrapper_CGAL_Geometry> MovetkGeometryKernel;
    //==============================
#else
//==============================
// Define the Number Type
// For the Boost Backend
typedef long double NT;
//==============================

//==============================
// Define the Dimensions
// It is possible to choose
// a higher dimension
// Note: Boost Geometry Adapter supports geometry in two
// dimensions only
const static size_t dimensions = 2;
//==============================

//==============================
// Define the Geometry Backend
typedef movetk_support::BoostGeometryTraits<long double, dimensions> Boost_Geometry_Backend;
//Using the Geometry Backend define the Movetk Geometry Kernel
typedef movetk_core::MovetkGeometryKernel<typename Boost_Geometry_Backend::Wrapper_Boost_Geometry> MovetkGeometryKernel;
//==============================
#endif

typedef movetk_support::FiniteNorm<MovetkGeometryKernel, 2> Norm;



TEST_CASE("brownian bridge mle 1", "[test brownian bridge mle 1]"){
    Norm norm;
    TabularTrajectory t = {};
    typedef movetk_algorithms::brownian_bridge::ParameterTraits<MovetkGeometryKernel, decltype(t.begin())> ParameterTraits;
    typedef std::vector<typename ParameterTraits::Parameters>::const_iterator BridgeIterator;
    std::vector<typename ParameterTraits::Parameters> bridges;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    ParameterTraits::Parameters p = {make_point({0, 1}), make_point({0, 0}), 0, t.begin(), t.begin()};
    bridges.push_back(p);
    typename MovetkGeometryKernel::MovetkVector v = std::get<ParameterTraits::ParameterColumns::POINT>(p)
                                                    - std::get<ParameterTraits::ParameterColumns::MU>(p);
    MovetkGeometryKernel::NT squared_length = norm(v);
    MovetkGeometryKernel::NT initial_estimate = squared_length / 2;
    MovetkGeometryKernel::NT upper_bound = squared_length;
    movetk_algorithms::brownian_bridge::MLE<MovetkGeometryKernel,
            ParameterTraits, Norm, BridgeIterator, 1000> mle(std::cbegin(bridges), std::cend(bridges),
                                                             initial_estimate, MOVETK_EPS, upper_bound, MOVETK_EPS);
    std::cout <<"Maximum Likelihood Estimate: "<< mle() << "\n";
    REQUIRE(std::abs(mle() - 0.5) < MOVETK_EPS);
}


TEST_CASE("brownian bridge mle 2", "[test brownian bridge mle 2]"){
    Norm norm;
    TabularTrajectory t = {};
    typedef movetk_algorithms::brownian_bridge::ParameterTraits<MovetkGeometryKernel, decltype(t.begin())> ParameterTraits;
    typedef std::vector<typename ParameterTraits::Parameters>::const_iterator BridgeIterator;
    std::vector<typename ParameterTraits::Parameters> bridges;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    ParameterTraits::Parameters p = {make_point({10, 20}), make_point({20, 10}), 0, t.begin(), t.begin()};
    bridges.push_back(p);
    typename MovetkGeometryKernel::MovetkVector v = std::get<ParameterTraits::ParameterColumns::POINT>(p)
                                                    - std::get<ParameterTraits::ParameterColumns::MU>(p);
    MovetkGeometryKernel::NT squared_length = norm(v);
    MovetkGeometryKernel::NT initial_estimate = squared_length / 2;
    MovetkGeometryKernel::NT upper_bound = squared_length;
    movetk_algorithms::brownian_bridge::MLE<MovetkGeometryKernel,
            ParameterTraits, Norm, BridgeIterator, 1000> mle(std::cbegin(bridges), std::cend(bridges),
                                                             initial_estimate, MOVETK_EPS, upper_bound, MOVETK_EPS);
    std::cout << "Maximum Likelihood Estimate: "<<mle() << "\n";
    REQUIRE(std::abs(mle() - 100) < MOVETK_EPS);
}

TEST_CASE("brownian bridge mle 3", "[test brownian bridge mle 3]"){
    Norm norm;
    TabularTrajectory t = {};
    typedef movetk_algorithms::brownian_bridge::ParameterTraits<MovetkGeometryKernel, decltype(t.begin())> ParameterTraits;
    std::vector<typename ParameterTraits::Parameters> bridges;
    movetk_core::MakePoint<MovetkGeometryKernel> make_point;
    MovetkGeometryKernel::NT sigma = 1.0;
    ParameterTraits::Parameters p = {make_point({0, 1}), make_point({0, 0}), sigma, t.begin(), t.begin()};
    bridges.push_back(p);
    typename MovetkGeometryKernel::MovetkVector v = std::get<ParameterTraits::ParameterColumns::POINT>(p)
                                                    - std::get<ParameterTraits::ParameterColumns::MU>(p);
    MovetkGeometryKernel::NT squared_length = norm(v);
    MovetkGeometryKernel::NT upper_bound = squared_length;
    p = {make_point({10, 20}), make_point({20, 10}), sigma, t.begin(), t.begin()};
    bridges.push_back(p);
    v = std::get<ParameterTraits::ParameterColumns::POINT>(p)
        - std::get<ParameterTraits::ParameterColumns::MU>(p);
    MovetkGeometryKernel::NT l = norm(v);
    squared_length += l;
    if (l > upper_bound)
        upper_bound = l;
    MovetkGeometryKernel::NT initial_estimate = squared_length / (2 * bridges.size());
    typedef std::vector<typename ParameterTraits::Parameters>::const_iterator BridgeIterator;
    movetk_algorithms::brownian_bridge::MLE<MovetkGeometryKernel,
            ParameterTraits, Norm, BridgeIterator, 1000> mle(std::cbegin(bridges), std::cend(bridges),
                                                             initial_estimate, MOVETK_EPS, upper_bound, MOVETK_EPS);
    std::cout <<"Maximum Likelihood Estimate: "<< mle() << "\n";
    REQUIRE(std::abs(mle() - 50.25) < MOVETK_EPS);
}


TEST_CASE("brownian bridge model 1","[test brownian bridge model 1]"){

   struct ProbeTraits{
       enum ProbeColumns {
           LAT, LON, SAMPLE_DATE
       };
   };


    using ProbePoint = std::tuple<long double, long double, std::size_t>;

    ProbePoint p1 = {51.443780998,5.47970146,1001};
    ProbePoint p2 = {51.44394316,5.479985774,1002};
    ProbePoint p3 = {51.444081916,5.480219126,1003};

    std::vector<ProbePoint> data = {p1, p2, p3};
    TabularTrajectory<long double, long double, std::size_t> t { data };
    Norm norm;
    typedef movetk_algorithms::brownian_bridge::ParameterTraits<MovetkGeometryKernel, decltype(t.begin())> ParameterTraits;
    typedef std::vector<typename ParameterTraits::Parameters>::const_iterator BridgeIterator;
    typedef LocalCoordinateReference<typename MovetkGeometryKernel::NT> Projection;
    std::vector<typename ParameterTraits::Parameters> bridges;

    typedef movetk_algorithms::brownian_bridge::Model<MovetkGeometryKernel, ProbeTraits,
            ParameterTraits, Norm, Projection> BBMM;

    BBMM bb(t.begin(), t.end(), movetk_core::movetk_back_insert_iterator(bridges));

    MovetkGeometryKernel::NT upper_bound = 0;
    MovetkGeometryKernel::NT squared_length = 0;
    std::cout<<"Parameters: \n";
    for (auto bridge: bridges){
        std::cout<<std::get<ParameterTraits::ParameterColumns::POINT>(bridge);
        std::cout<<";";
        std::cout<<std::get<ParameterTraits::ParameterColumns::MU>(bridge);
        std::cout<<"\n";
        typename MovetkGeometryKernel::MovetkVector v = std::get<ParameterTraits::ParameterColumns::POINT>(bridge)
                                                        - std::get<ParameterTraits::ParameterColumns::MU>(bridge);
        MovetkGeometryKernel::NT l = norm(v);
        squared_length += l;
        if (l > upper_bound)
            upper_bound = l;
    }

    MovetkGeometryKernel::NT initial_estimate = squared_length / (2 * bridges.size());
    movetk_algorithms::brownian_bridge::MLE<MovetkGeometryKernel,
            ParameterTraits, Norm, BridgeIterator, 1000> mle(std::cbegin(bridges), std::cend(bridges),
                                                             initial_estimate, MOVETK_EPS, upper_bound, MOVETK_EPS);
    std::cout <<"Maximum Likelihood Estimate: "<< mle() << "\n";
    REQUIRE(std::abs(mle() - 2.416917) < MOVETK_EPS);
}


TEST_CASE("brownian bridge model 2","[test brownian bridge model 2]"){

    struct ProbeTraits{
        enum ProbeColumns {
            LAT, LON, SAMPLE_DATE
        };
    };


    using ProbePoint = std::tuple<long double, long double, std::size_t>;

    ProbePoint p1 = {51.443780998,5.47970146,1001};
    ProbePoint p2 = {51.44394316,5.479985774,1002};
    ProbePoint p3 = {51.444081916,5.480219126,1003};
    ProbePoint p4 = {51.444402058,5.480742157,1004};
    ProbePoint p5 = {51.444943702,5.481090844,1005};
    ProbePoint p6 = {51.445498713,5.481675566,1006};

    std::vector<ProbePoint> data = {p1, p2, p3, p4, p5, p6};
    TabularTrajectory<long double, long double, std::size_t> t { data };

    Norm norm;
    typedef movetk_algorithms::brownian_bridge::ParameterTraits<MovetkGeometryKernel, decltype(t.begin())> ParameterTraits;
    typedef std::vector<typename ParameterTraits::Parameters>::const_iterator BridgeIterator;
    typedef LocalCoordinateReference<typename MovetkGeometryKernel::NT> Projection;
    std::vector<typename ParameterTraits::Parameters> bridges;

    typedef movetk_algorithms::brownian_bridge::Model<MovetkGeometryKernel, ProbeTraits,
            ParameterTraits, Norm, Projection> BBMM;


    BBMM bb(t.begin(), t.end(), movetk_core::movetk_back_insert_iterator(bridges));

    MovetkGeometryKernel::NT upper_bound = 0;
    MovetkGeometryKernel::NT squared_length = 0;
    std::cout<<"Parameters: \n";
    for (auto bridge: bridges){
        std::cout<<std::get<ParameterTraits::ParameterColumns::POINT>(bridge);
        std::cout<<";";
        std::cout<<std::get<ParameterTraits::ParameterColumns::MU>(bridge);
        std::cout<<"\n";
        typename MovetkGeometryKernel::MovetkVector v = std::get<ParameterTraits::ParameterColumns::POINT>(bridge)
                                                        - std::get<ParameterTraits::ParameterColumns::MU>(bridge);
        MovetkGeometryKernel::NT l = norm(v);
        squared_length += l;
        if (l > upper_bound)
            upper_bound = l;
    }

    MovetkGeometryKernel::NT initial_estimate = squared_length / (2 * bridges.size());
    movetk_algorithms::brownian_bridge::MLE<MovetkGeometryKernel,
            ParameterTraits, Norm, BridgeIterator, 1000> mle(std::cbegin(bridges), std::cend(bridges),
                                                             initial_estimate, MOVETK_EPS, upper_bound, MOVETK_EPS);
    std::cout <<"Maximum Likelihood Estimate: "<< mle() << "\n";
    REQUIRE(std::abs(mle() - 32.2321283) < MOVETK_EPS);
}



TEST_CASE("brownian bridge parameter selection 1","[test brownian bridge parameter selection 1]"){
    struct ProbeTraits{
        enum ProbeColumns {
            LAT, LON, SAMPLE_DATE
        };
    };


    using ProbePoint = std::tuple<long double, long double, std::size_t>;

    ProbePoint p0( 51.4443930124353, 5.47950342106722, 1550866709 );
    ProbePoint p1( 51.444393336164, 5.47950008089042, 1550866719);
    ProbePoint p2( 51.4443942186351, 5.47950781233749, 1550866729);
    ProbePoint p3( 51.4443943960143, 5.47950784270154, 1550866739);
    ProbePoint p4( 51.4443944277322, 5.47947849377084, 1550866749);
    ProbePoint p5( 51.4443944586718, 5.47950714413615, 1550866759);
    ProbePoint p6( 51.4443944790561, 5.47948123937032, 1550866769);
    ProbePoint p7( 51.4443945042884, 5.479478464752472, 1550866779);
    ProbePoint p8( 51.4443947175562, 5.47950683174872, 1550866789);
    ProbePoint p9( 51.4443949234094, 5.47948138337028, 1550866799);
    ProbePoint p10( 51.4443949362765, 5.47947914874741, 1550866809);
    ProbePoint p11( 51.4443949503156, 5.47950642352529, 1550866819);
    ProbePoint p12( 51.4443951830696, 5.47950328314741, 1550866829);
    ProbePoint p13( 51.4443954255127, 5.47948166958525, 1550866839);
    ProbePoint p14( 51.4443954402577, 5.47948047240938, 1550866849);
    ProbePoint p15( 51.4443955694185, 5.47950080933134, 1550866859);
    ProbePoint p16( 51.4443956476577, 5.4794815471467, 1550866869);
    ProbePoint p17( 51.4443957506309, 5.47948009708671, 1550866879);

    std::vector<ProbePoint> data = {p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10,
                                    p11, p12, p13, p14, p15, p16, p17};
    TabularTrajectory<long double, long double, std::size_t> t { data };
    Norm norm;
    typedef movetk_algorithms::brownian_bridge::ParameterTraits<MovetkGeometryKernel, decltype(t.begin()) > ParameterTraits;
    typedef std::vector<typename ParameterTraits::Parameters>::const_iterator BridgeIterator;
    typedef LocalCoordinateReference<typename MovetkGeometryKernel::NT> Projection;
    std::vector<typename ParameterTraits::Parameters> bridges;

    typedef movetk_algorithms::brownian_bridge::Model<MovetkGeometryKernel, ProbeTraits,
            ParameterTraits, Norm, Projection> BBMM;

    BBMM bb(t.begin(), t.end(), movetk_core::movetk_back_insert_iterator(bridges));

    std::vector<typename MovetkGeometryKernel::NT> selected_coeffs;

    auto bit = begin(bridges);
    while(bit != end(bridges)){
        movetk_algorithms::brownian_bridge::MLE<MovetkGeometryKernel,
                ParameterTraits, Norm, BridgeIterator, 1000> mle(bit, bit+1);
        std::get<ParameterTraits::ParameterColumns::SIGMA_SQUARED>(*bit) = mle();
        bit++;
    }

    movetk_algorithms::brownian_bridge::ParameterSelector<MovetkGeometryKernel, ParameterTraits> selector(4);
    selector(std::begin(bridges), std::end(bridges),
             movetk_core::movetk_back_insert_iterator(selected_coeffs));

    for (auto bridge: bridges){
        std::cout<< std::get<ParameterTraits::ParameterColumns::SIGMA_SQUARED>(bridge);
        std::cout<<"\n";
    }

    for (auto coeff: selected_coeffs){
        std::cout<<"Coefficient: "<<coeff<<"\n";
    }

}
