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
// Created by Mitra, Aniket on 2019-06-03.
//

#ifndef MOVETK_DISTANCES_H
#define MOVETK_DISTANCES_H

#include <iostream>
#include <limits>
#include "movetk/utils/Asserts.h"

namespace movetk_support {

    // based on Eiter, T., & Mannila, H. (1994). Computing Discrete Fréchet Distance ∗.

    template<class Kernel, class Norm>
    struct squared_distance_d {

        typename Kernel::NT operator()(typename Kernel::MovetkPoint &p,
                                       typename Kernel::MovetkLine &l) {
            Norm n;
            typename Kernel::MovetkVector v = l[1] - l[0];
            typename Kernel::MovetkVector u = p - l[0];
            typename Kernel::NT uv = u * v;
            n(v);
            typename Kernel::NT vv = n ^2;
            v *= (uv / vv);
            typename Kernel::MovetkPoint Pb = l[0] + v;
            typename Kernel::MovetkVector v2 = p - Pb;
            return n(v2);
        }

        typename Kernel::NT operator()(typename Kernel::MovetkPoint &p,
                                       typename Kernel::MovetkSegment &s) {
            Norm n;
            typename Kernel::MovetkVector v = s[1] - s[0];
            typename Kernel::MovetkVector u = p - s[0];
            typename Kernel::MovetkVector v1 = p - s[1];
            typename Kernel::NT uv = u * v;
            if (uv <= 0) {
                return n(u);
            }
            n(v);
            typename Kernel::NT vv = n ^ Norm::P;
            if (vv <= uv) {
                return n(v1);
            }
            v *= (uv / vv);
            typename Kernel::MovetkPoint Pb = s[0] + v;
            typename Kernel::MovetkVector v2 = p - Pb;
            return n(v2);
        }

        typename Kernel::NT operator()(typename Kernel::MovetkPoint &p1,
                                       typename Kernel::MovetkPoint &p2) {
            Norm n;
            typename Kernel::MovetkVector v = p2 - p1;
            return n(v);
        }

    };

    template<class Kernel, class Norm>
    class Discrete_Hausdorff {
    private:

        template<class InputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_movetk_point<Kernel,
                        typename InputIterator::value_type> >
        typename Kernel::NT discrete_husdorff(InputIterator a_first, InputIterator a_beyond,
                                       InputIterator b_first, InputIterator b_beyond) {

            Norm norm;
            InputIterator it_a = a_first;
            typename Kernel::NT max_dist = 0;
            while (it_a !=  a_beyond){
                InputIterator it_b = b_first;
                typename Kernel::MovetkVector v = *it_b - *it_a;
                typename Kernel::NT distance = norm(v);
                typename Kernel::NT min_dist = distance;
                it_b++;
                while (it_b != b_beyond){
                    v = *it_b - *it_a;
                    distance = norm(v);
                    if (distance < min_dist){
                        min_dist = distance;
                    }
                    it_b++;
                }
                if (min_dist > max_dist){
                    max_dist = min_dist;
                }
                it_a++;
            }

            typename Kernel::NT n = 1 /static_cast<typename Kernel::NT>(Norm::P);
            return  std::pow(max_dist,n);
        }


    public:

        template<class InputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_movetk_point<Kernel,
                        typename InputIterator::value_type> >
        typename Kernel::NT operator()(InputIterator polyline_a_first, InputIterator polyline_a_beyond,
                                       InputIterator polyline_b_first, InputIterator polyline_b_beyond) {

            typename Kernel::NT hd_pq = discrete_husdorff(polyline_a_first, polyline_a_beyond,
                                                          polyline_b_first, polyline_b_beyond);


            typename Kernel::NT hd_qp = discrete_husdorff(polyline_b_first, polyline_b_beyond,
                                                          polyline_a_first, polyline_a_beyond);

            return std::max(hd_pq, hd_qp);

        }

    };

    template <class Kernel, class Norm >
    class Discrete_Frechet{
    private:

        typedef typename Kernel::NT NT;

        template <class InputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_movetk_point<Kernel,
                typename InputIterator::value_type>>
        NT distance(InputIterator iter_a, InputIterator iter_b){
            Norm norm;
            typename Kernel::MovetkVector v = *iter_b - *iter_a;
            return norm(v);
        }


    public:

        template <class InputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_movetk_point<Kernel,
                        typename InputIterator::value_type>>
        NT operator()(InputIterator polyline_a_first, InputIterator polyline_a_beyond,
                                       InputIterator polyline_b_first, InputIterator polyline_b_beyond){

            std::size_t size_polyline_b = std::distance(polyline_b_first, polyline_b_beyond);
            std::vector<NT> dp_row (size_polyline_b);
            std::fill( std::begin(dp_row), std::begin(dp_row) + size_polyline_b , -1 );
            InputIterator it_a = polyline_a_first;
            while ( it_a != polyline_a_beyond ){
                InputIterator it_b = polyline_b_first;
                std::size_t j = 0;
                typename Kernel::NT previous = -1, current = -1;
                while ( it_b != polyline_b_beyond){
                    if ( (it_a == polyline_a_first) && (it_b == polyline_b_first) ){
                        current = distance(it_a, it_b);
                        dp_row[0] = current;
                    }

                    else if ((it_a != polyline_a_first) && (it_b == polyline_b_first)  ){
                        current = std::max(dp_row[0] ,distance(it_a, it_b));
                        dp_row[0] = current;

                    }
                    else if ( (it_a == polyline_a_first) && (it_b != polyline_b_first) ){
                            current = std::max(previous, distance(it_a, it_b));
                            dp_row[j - 1] = previous;
                    }
                    else {
                            current = std::max(std::min(
                                                        std::min(dp_row[j], previous),
                                                        dp_row[j - 1]),
                                                                distance(it_a, it_b));
                            dp_row[j - 1] = previous;
                    }


                    previous = current;
                    j++;
                    it_b++;
                }
                dp_row[j - 1] = previous;
                it_a++;
            }

            NT dfd = dp_row.back();
            NT n = 1 /static_cast<NT>(Norm::P);
            return  std::pow(dfd,n);
        }

    };

    template<class Kernel, class Norm>
    class Dynamic_Time_Warping {

    //based on https://en.wikipedia.org/wiki/Dynamic_time_warping

    private:

        template<class InputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_movetk_point<Kernel,
                        typename InputIterator::value_type> >
        typename Kernel::NT dynamic_time_warping(InputIterator a_first, InputIterator a_beyond,
                                       InputIterator b_first, InputIterator b_beyond) {

            typedef typename Kernel::NT NT;
            Norm norm;

            std::size_t size_a = std::distance(a_first, a_beyond);
            std::size_t size_b = std::distance(b_first, b_beyond);
            std::vector< std::vector<NT> > DTW(size_a + 1, std::vector<NT>(size_b + 1));

            for(auto i = 0; i < size_a + 1; i++) {
                for(auto j = 0; j < size_b + 1; j++){
                    DTW[i][j] = std::numeric_limits<NT>::max();
                }
            }
            DTW[0][0] = 0;
            InputIterator it_a = a_first;
            it_a++;
            for(auto i = 1; i < size_a + 1; i++){
                InputIterator it_b = b_first;
                it_b++;
                for(auto j = 1; j < size_b + 1; j++){

                    typename Kernel::MovetkVector v = *it_b - *it_a;
                    NT cost = norm(v);
                    DTW[i][j] = cost + std::min({DTW[i-1][j],    // insertion
                                        DTW[i][j-1],    // deletion
                                        DTW[i-1][j-1]});    // match
                    it_b++;
                }
                it_a++;
            } 
            return  DTW[size_a][size_b];
        }


        template<class InputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_movetk_point<Kernel,
                        typename InputIterator::value_type> >
        typename Kernel::NT dynamic_time_warping(InputIterator a_first, InputIterator a_beyond,
                                       InputIterator b_first, InputIterator b_beyond, std::size_t window) {

            typedef typename Kernel::NT NT;
            Norm norm;

            std::size_t size_a = std::distance(a_first, a_beyond);
            std::size_t size_b = std::distance(b_first, b_beyond);
            std::vector< std::vector<NT> > DTW(size_a + 1, std::vector<NT>(size_b + 1));

            int diff = std::abs((int)size_a = (int)size_b);
            std::size_t w = std::max({window, (size_t) diff}); // adapt window size

            for(auto i = 0; i < size_a + 1; i++) {
                for(auto j = 0; j < size_b + 1; j++){
                    DTW[i][j] = std::numeric_limits<NT>::max();
                }
            }
            DTW[0][0] = 0;

            for(int i = 1; i < size_a + 1; i++) {
                for(int j = std::max<int>({1, i - w}); j < std::min<int>({size_b + 1, i + w}); j++){
                    DTW[i][j] = 0;
                }
            }

            InputIterator it_a = a_first;
            it_a++;
            for(int i = 1; i < size_a + 1; i++){
                InputIterator it_b = b_first;
                it_b++;
                for(int j = std::max<int>({1, i-w}); j < std::min<int>({size_b + 1, i + w}); j++){
                    typename Kernel::MovetkVector v = *it_b - *it_a;
                    NT cost = norm(v);
                    DTW[i][j] = cost + std::min({DTW[i-1][j],    // insertion
                                        DTW[i][j-1],    // deletion
                                        DTW[i-1][j-1]});    // match
                    it_b++;
                }
                it_a++;
            } 
            return  DTW[size_a][size_b];
        }



    public:

        template<class InputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_movetk_point<Kernel,
                        typename InputIterator::value_type> >
        typename Kernel::NT operator()(InputIterator polyline_a_first, InputIterator polyline_a_beyond,
                                       InputIterator polyline_b_first, InputIterator polyline_b_beyond) {

            return dynamic_time_warping(polyline_a_first, polyline_a_beyond,
                                                          polyline_b_first, polyline_b_beyond);

        }

        template<class InputIterator,
                typename = movetk_core::requires_random_access_iterator<InputIterator>,
                typename = movetk_core::requires_movetk_point<Kernel,
                        typename InputIterator::value_type> >
        typename Kernel::NT operator()(InputIterator polyline_a_first, InputIterator polyline_a_beyond,
                                       InputIterator polyline_b_first, InputIterator polyline_b_beyond, std::size_t window) {

            return dynamic_time_warping(polyline_a_first, polyline_a_beyond,
                                                          polyline_b_first, polyline_b_beyond, window);

        }



    };

}
#endif //MOVETK_DISTANCES_H
