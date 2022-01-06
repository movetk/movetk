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

 /*! @file Interface.h
  *  @brief  An interface for movetk  geometry
  *  @details A collection of classes that provide a generic
  *  interface for construction of different geometric types
  *  by decoupling the underlying geometry library
  *  (e.g CGAL, Boost etc.) from the interface
  *  @authors Aniket Mitra (aniket.mitra@here.com)
  */

#ifndef MOVETK_INTERFACE_H
#define MOVETK_INTERFACE_H

#include <math.h>
#include "movetk/utils/Requirements.h"
#include "movetk/utils/Asserts.h"
#include "movetk/utils/Iterators.h"
#include <iostream>
#include <array>

#define PI 3.14159265
#define TWO_PI 6.2831853
#define LOG_TWO_PI 1.837877066409345

  //TODO  Concepts for the Interface
  /*!
   *
   *  @namespace movetk_core
   *  @brief the core of movetk
   */
namespace movetk_core
{
    // the support library for Movetk

    /*!
   * @brief converts from degree to radians
   * @tparam NT
   * @param degrees
   * @return
   */
    template <class NT>
    NT deg2radians(const NT degrees) { return degrees * PI / 180.0; }

    /*!
     * @brief converts from radians to degrees
     * @tparam NT
     * @param radians
     * @return
     */
    template <class NT>
    NT rad2deg(const NT radians) { return radians * 180.0 / PI; }

    /*!@struct MakePoint
     * @brief Constructs a point from a set of input Cartesian coordinates
     * @tparam GeometryTraits - A traits class that defines movetk geometry types
     */
    template <class GeometryTraits>
    struct MakePoint
    {
        /*!
         * @details Infers the dimensions of the point from the input
         * and creates a point with Cartesian coordinates \f$[first,beyond)\f$
         * @tparam CoordinateIterator - A random access iterator over a set of
         * Cartesian coordinates where each coordinate is of type GeometryTraits::NT
         * @param first - Iterator to the first Cartesian coordinate
         * @param beyond - Iterator to the end of the last Cartesian coordinate
         * @return A movetk point
         */
        template <class CoordinateIterator,
            typename = movetk_core::requires_random_access_iterator<CoordinateIterator>>
            typename GeometryTraits::MovetkPoint operator()(CoordinateIterator first,
                CoordinateIterator beyond) const
        {
            //ASSERT_RANDOM_ACCESS_ITERATOR(CoordinateIterator);
            ASSERT_NUMBER_TYPE(GeometryTraits, first);
            typename GeometryTraits::MovetkPoint p(first, beyond);
            return p;
        }

        typename GeometryTraits::MovetkPoint operator()(std::initializer_list<
            typename GeometryTraits::NT>
            l) const
        {
            typename GeometryTraits::MovetkPoint p(l.begin(), l.end());
            return p;
        }
    };

    /*!@struct MakeLine
     * @brief Constructs a line form two input points
     * @tparam GeometryTraits - A traits class that defines movetk geometry types
     */
    template <class GeometryTraits>
    struct MakeLine
    {
        /*!
         *
         * @param p1 - A movetk point
         * @param p2 - A movetk point
         * @return A movetk line
         */
        typename GeometryTraits::MovetkLine operator()(typename GeometryTraits::MovetkPoint p1,
            typename GeometryTraits::MovetkPoint p2)
        {
            typename GeometryTraits::MovetkLine l(p1, p2);
            return l;
        }
    };

    /*!@struct ComputeLength
     * @brief  Computes the euclidean distance between two points
     * @tparam GeometryTraits - A traits class that defines movetk geometry types
     */
    template <class GeometryTraits>
    struct ComputeLength
    {
        /*!
         * @details Constructs a segment which has an associated measure of length
         * i.e euclidean distance between two points
         * @param p1 - A movetk point
         * @param p2 - A movetk point
         * @return Length of a segment
         */
        typename GeometryTraits::NT operator()(typename GeometryTraits::MovetkPoint p1,
            typename GeometryTraits::MovetkPoint p2)
        {
            typename GeometryTraits::MovetkSegment l(p1, p2);
            return sqrt(l());
        }

        typename GeometryTraits::NT operator()(typename GeometryTraits::MovetkSegment l)
        {
            return sqrt(l());
        }
    };

    template <class GeometryTraits>
    struct MakeSegment
    {
        typename GeometryTraits::MovetkSegment operator()(typename GeometryTraits::MovetkPoint p1,
            typename GeometryTraits::MovetkPoint p2)
        {
            typename GeometryTraits::MovetkSegment s(p1, p2);
            return s;
        }

        typename GeometryTraits::MovetkSegment operator()(std::initializer_list<typename GeometryTraits::NT> l1,
            std::initializer_list<typename GeometryTraits::NT> l2)
        {
            MakePoint<GeometryTraits> make_point;
            typename GeometryTraits::MovetkPoint p1 = make_point(l1);
            typename GeometryTraits::MovetkPoint p2 = make_point(l2);
            typename GeometryTraits::MovetkSegment s(p1, p2);
            return s;
        }
    };

    template <class GeometryTraits>
    struct MakeSphere
    {
        typename GeometryTraits::MovetkSphere
            operator()(typename GeometryTraits::MovetkPoint center, typename GeometryTraits::NT radius,
                bool square = true)
        {
            typename GeometryTraits::MovetkSphere s(center, radius, square);
            return s;
        }

        typename GeometryTraits::MovetkSphere
            operator()(std::initializer_list<typename GeometryTraits::NT> l,
                typename GeometryTraits::NT radius,
                bool square = true)
        {
            typename GeometryTraits::MovetkPoint center(l.begin(), l.end());
            typename GeometryTraits::MovetkSphere s(center, radius, square);
            return s;
        }
    };

    /*!@struct MakePolygon
     * @brief  Constructs a Polygon  from a set of Movetk points
     * @tparam GeometryTraits - A traits class that defines movetk geometry types
     */
    template <class GeometryTraits>
    struct MakePolygon
    {

        /*!
         *
         * @tparam PointIterator - A random access iterator over a set of points
         * @param first -  Iterator to the first point in a set of Movetk points
         * @param beyond - Iterator to the last point in a set of Movetk points
         * @return A movetk polygon
         */
        template <class PointIterator,
            typename = movetk_core::requires_random_access_iterator<PointIterator>,
            typename = movetk_core::requires_movetk_point<GeometryTraits,
            typename PointIterator::value_type>>
            typename GeometryTraits::MovetkPolygon operator()(PointIterator first,
                PointIterator beyond)
        {
            //ASSERT_RANDOM_ACCESS_ITERATOR(PointIterator);
            //ASSERT_MOVETK_POINT_TYPE(GeometryTraits, first);
            typename GeometryTraits::MovetkPolygon polygon(first, beyond);
            return polygon;
        }
    };

    /*! @struct MakeMinSphere
     *  @brief Constructs a minimum enclolsing ball from a set of Movetk points
     *  @tparam GeometryTraits - A traits class that defines movetk geometry types
     */
    template <class GeometryTraits>
    struct MakeMinSphere
    {
        typename GeometryTraits::MovetkMinSphere make_min_sphere;

        /*!
         *
         * @tparam PointIterator - A random access iterator over a set of points
         * @param first - Iterator to the first point in a set of Movetk points
         * @param beyond - Iterator to the last point in a set of Movetk points
         * @return Radius of the Minimum Enclosing Ball
         */
        template <class PointIterator,
            typename = movetk_core::requires_random_access_iterator<PointIterator>,
            typename = movetk_core::requires_movetk_point<GeometryTraits,
            typename PointIterator::value_type>>
            typename GeometryTraits::NT operator()(PointIterator first, PointIterator beyond)
        {
            //ASSERT_RANDOM_ACCESS_ITERATOR(PointIterator);
            //ASSERT_MOVETK_POINT_TYPE(GeometryTraits, first);
            typename GeometryTraits::NT Radius = make_min_sphere(first, beyond);
            return Radius;
        }

        /*!
         *
         * @tparam PointIterator -  A random access iterator over a set of points
         * @tparam CenterIterator -  A forward iterator that points to the location
         * where a value has to be inserted
         * @param first - Iterator to the first point in a set of Movetk points
         * @param beyond - Iterator to the last point in a set of Movetk points
         * @param iter - Iterator to the location where a value has to be inserted
         * @return Radius of the Minimum Enclosing Ball
         */
        template <class PointIterator, class CenterIterator,
            typename = movetk_core::requires_random_access_iterator<PointIterator>,
            typename = movetk_core::requires_movetk_point<GeometryTraits,
            typename PointIterator::value_type>,
            typename = movetk_core::requires_output_iterator<CenterIterator>,
            typename = movetk_core::requires_NT<GeometryTraits,
            typename CenterIterator::value_type>>
            typename GeometryTraits::NT operator()(PointIterator first,
                PointIterator beyond, CenterIterator iter)
        {
            //ASSERT_RANDOM_ACCESS_ITERATOR(PointIterator);
            //ASSERT_MOVETK_POINT_TYPE(GeometryTraits, first);
            //ASSERT_OUTPUT_ITERATOR(CenterIterator);
            typename GeometryTraits::NT Radius = make_min_sphere(first, beyond, iter);
            return Radius;
        }
    };

    struct sphere_segment_intersection_tag;

    struct polylines_instersection_tag;

    struct sphere_sphere_intersection_tag;

    template <class _GeometryTraits, class _Norm, class Tag>
    struct IntersectionTraits
    {
    };

    template <class _GeometryTraits, class _Norm>
    struct IntersectionTraits<_GeometryTraits, _Norm, sphere_segment_intersection_tag>
    {
        enum Attributes
        {
            ID,
            SIGN_DISCRIMINANT,
            SQUARED_RATIO,
            POINT
        };
        typedef std::tuple<std::size_t, int,
            typename _GeometryTraits::NT,
            typename _GeometryTraits::MovetkPoint>
            value_type;
        typedef _GeometryTraits GeometryTraits;
        typedef _Norm Norm;
    };

    template <class _GeometryTraits, class _Norm>
    struct IntersectionTraits<_GeometryTraits, _Norm, polylines_instersection_tag>
    {
        typedef _GeometryTraits GeometryTraits;
        typedef _Norm Norm;
    };

    template <class _GeometryTraits, class _Norm>
    struct IntersectionTraits<_GeometryTraits, _Norm, sphere_sphere_intersection_tag>
    {
        typedef _GeometryTraits GeometryTraits;
        typedef _Norm Norm;
    };

    template <class IntersectionTraits>
    class ComputeIntersections
    {

    public:
        template <class PointIterator,
            typename = movetk_core::requires_random_access_iterator<PointIterator>,
            typename = movetk_core::requires_movetk_point<
            typename IntersectionTraits::GeometryTraits,
            typename PointIterator::value_type>>
            std::size_t operator()(PointIterator first, PointIterator beyond)
        {
            typename IntersectionTraits::GeometryTraits::MovetkCurveIntersection compute_curve_intersections;
            return compute_curve_intersections(first, beyond);
        }

        template <class OutputIterator,
            typename = movetk_core::requires_output_iterator<OutputIterator>,
            typename = movetk_core::requires_tuple<
            typename OutputIterator::value_type>,
            typename = movetk_core::requires_L2_norm<typename IntersectionTraits::Norm>>
            void operator()(typename IntersectionTraits::GeometryTraits::MovetkSphere& sphere,
                typename IntersectionTraits::GeometryTraits::MovetkSegment& segment, OutputIterator result)
        {
            typename IntersectionTraits::Norm norm;
            typedef typename IntersectionTraits::GeometryTraits::NT NT;
            typedef typename IntersectionTraits::GeometryTraits::MovetkVector MovetkVector;
            MovetkVector v1 = segment[1] - segment[0];
            NT squared_length_v1 = norm(v1);
            MovetkVector v2 = segment[0] - sphere.center();
            NT squared_length_v2 = norm(v2);
            NT q1 = std::pow(v1 * v2, 2) * (1 / squared_length_v1);
            NT q2 = squared_length_v2 - sphere.squared_radius();
            if (q1 < q2)
                result = std::make_tuple(0, -1, -1.0, sphere.center());
            else if ((q1 - q2) > (MOVETK_EPS * 1000))
            {
                NT n = 0.5;
                NT root = std::pow(squared_length_v1 * (q1 - q2), n);
                NT translation1 = -1 * (v1 * v2) + root;
                NT translation2 = -1 * (v1 * v2) - root;
                MovetkVector v3 = v1;
                v3 *= (translation1 / squared_length_v1);
                NT direction = v3 * v1;
                NT length = norm(v3);
                NT ratio = length / squared_length_v1;
                if ((direction > 0) && (ratio <= 1))
                {
                    result = std::make_tuple(0, 1, ratio, segment[0] + v3);
                }
                else if (direction < 0)
                {
                    result = std::make_tuple(0, -1, ratio, segment[0]);
                }
                else if (ratio > 1)
                {
                    result = std::make_tuple(0, -1, ratio, segment[1]);
                }

                v3 *= (squared_length_v1 / translation1);
                v3 *= (translation2 / squared_length_v1);
                direction = v3 * v1;
                length = norm(v3);
                ratio = length / squared_length_v1;
                if ((direction > 0) && (ratio <= 1))
                {
                    result = std::make_tuple(0, 1, ratio, segment[0] + v3);
                }
                else if (direction < 0)
                {
                    result = std::make_tuple(0, -1, ratio, segment[0]);
                }
                else if (ratio > 1)
                {
                    result = std::make_tuple(0, -1, ratio, segment[1]);
                }
            }
            else
            {
                NT translation = -1 * (v1 * v2);
                MovetkVector v3 = v1;
                v3 *= (translation / squared_length_v1);
                NT direction = v3 * v1;
                NT length = norm(v3);
                NT ratio = length / squared_length_v1;
                if ((direction > 0) && (ratio <= 1))
                {
                    result = std::make_tuple(0, 0, ratio, segment[0] + v3);
                }
            }
        }

        template <typename = movetk_core::requires_L2_norm<typename IntersectionTraits::Norm>>
        typename IntersectionTraits::GeometryTraits::MovetkSphere
            operator()(typename IntersectionTraits::GeometryTraits::MovetkSphere& sphere_a,
                typename IntersectionTraits::GeometryTraits::MovetkSphere& sphere_b)
        {
            // based on https://hal.archives-ouvertes.fr/hal-01955983/document
            typename IntersectionTraits::Norm norm;
            typedef typename IntersectionTraits::GeometryTraits GeometryTraits;
            typedef typename GeometryTraits::NT NT;
            typedef typename GeometryTraits::MovetkVector MovetkVector;
            typedef typename GeometryTraits::MovetkPoint MovetkPoint;
            movetk_core::MakePoint<GeometryTraits> make_point;
            MakeSphere<GeometryTraits> make_sphere;
            MovetkPoint ORIGIN = make_point({ 0, 0 });
            NT squared_r_a = sphere_a.squared_radius();
            NT r_a = std::pow(squared_r_a, 0.5);
            NT squared_r_b = sphere_b.squared_radius();
            NT r_b = std::pow(squared_r_b, 0.5);
            MovetkVector v = sphere_b.center() - sphere_a.center();
            MovetkVector v1 = (sphere_b.center() - ORIGIN) +
                (sphere_a.center() - ORIGIN);
            NT squared_length = norm(v);
            NT sum_squared_radius = squared_r_a + squared_r_b;
            NT squared_diff_radius = sum_squared_radius - 2 * r_a * r_b;
            NT squared_sum_radius = sum_squared_radius + 2 * r_a * r_b;
            bool disjoint = (squared_sum_radius - squared_length < 0) || (squared_diff_radius - squared_length > 0);
            if (!disjoint)
            {
                v *= (squared_r_a - squared_r_b) / (2 * squared_length);
                v1 *= 0.5;
                MovetkVector v2 = v1 + v;
                MovetkPoint center = ORIGIN + v2;
                NT op1 = (squared_r_a + squared_r_b) * 0.5;
                NT numerator = (squared_r_a - squared_r_b) * (squared_r_a - squared_r_b);
                NT op2 = 0.25 * numerator / squared_length;
                NT squared_radius = op1 - op2 - 0.25 * squared_length;
                return make_sphere(center, squared_radius, false);
            }
            else
            {
                return make_sphere(ORIGIN, 0);
            }
        }
    };

    /*!
     *
     * @tparam GeometryTraits
     * @tparam Norm
     */
    template <class GeometryTraits,
        class Norm,
        typename = movetk_core::requires_planar_geometry<GeometryTraits>,
        typename = movetk_core::requires_L2_norm<Norm>>
        class Wedge
    {
    public:
        using MovetkVector = typename GeometryTraits::MovetkVector;
        using MovetkPoint = typename GeometryTraits::MovetkPoint;
    private:
        using NT = typename GeometryTraits::NT;
        Norm norm;
        movetk_core::MakePoint<GeometryTraits> make_point;
        MovetkPoint ORIGIN = make_point({ 0, 0 });
        MovetkVector e1 = make_point({ 1, 0 }) - ORIGIN;
        MovetkVector e2 = make_point({ 0, 1 }) - ORIGIN;
        MovetkVector _slope = ORIGIN - ORIGIN;
        MovetkVector _intercept = ORIGIN - ORIGIN;
        bool horizontal = false, vertical = false;
        bool degenerate = false;
        bool upper_right = false, lower_left = false;
        bool lower_right = false, upper_left = false;

        void construct(const MovetkPoint& p,
            const MovetkPoint& center,
            NT radius)
        {
            NT m1, m2, c1, c2, tanA, tanB;
            NT squared_radius = radius * radius;
            auto _slope_ray = center - p;
            NT v_x = _slope_ray * e1;
            NT v_y = _slope_ray * e2;
            NT segment_squared_length = norm(_slope_ray);
            NT root = norm ^ 1;
            if (segment_squared_length < squared_radius)
            {
                degenerate = true;
            }
            if (root < MOVETK_EPS)
            {
                degenerate = true;
            }
            if (abs(v_x) < MOVETK_EPS)
            {
                vertical = true;
            }
            if (abs(v_y) < MOVETK_EPS)
            {
                horizontal = true;
            }
            if (v_x >= 0 && v_y >= 0)
            {
                upper_right = true;
            }
            if (v_y <= 0 && v_x >= 0)
            {
                lower_right = true;
            }
            if (v_y >= 0 && v_x <= 0)
            {
                upper_left = true;
            }
            if (v_x <= 0 && v_y <= 0)
            {
                lower_left = true;
            }
            NT tangent_squared_length = segment_squared_length - squared_radius;
            if (abs(tangent_squared_length) < MOVETK_EPS)
                degenerate = true;
            else
                tanB = radius / sqrt(tangent_squared_length);
            if (degenerate)
            {
                _slope = ORIGIN - ORIGIN;
            }
            else if (horizontal)
            {
                if (upper_right || lower_right)
                    _slope = make_point({ tanB, -tanB }) - ORIGIN;
                else if (upper_left || lower_left)
                    _slope = make_point({ -tanB, tanB }) - ORIGIN;
            }
            else if (vertical)
            {
                tanA = tan(PI / 2 - MOVETK_EPS * 0.001);
                if ((tanA * tanB) == 1)
                    m1 = (tanA + tanB) / MOVETK_EPS;
                else
                    m1 = (tanA + tanB) / (1 - tanA * tanB);
                m2 = -m1;
                _slope = make_point({ m1, m2 }) - ORIGIN;
            }
            else
            {
                tanA = v_y / v_x; // slope of the ray
                if ((tanA * tanB) == 1)
                {
                    m1 = (tanA + tanB) / MOVETK_EPS;
                    m2 = (tanA - tanB) / (1 + tanA * tanB);
                }
                else if ((tanA * tanB) == -1)
                {
                    m1 = (tanA + tanB) / (1 - tanA * tanB);
                    m2 = (tanA - tanB) / (-1 * MOVETK_EPS);
                }
                else
                {
                    m1 = (tanA + tanB) / (1 - tanA * tanB);
                    m2 = (tanA - tanB) / (1 + tanA * tanB);
                }

                if (upper_right || lower_right)
                    _slope = make_point({ m1, m2 }) - ORIGIN;
                else if (upper_left || lower_left)
                    _slope = make_point({ m2, m1 }) - ORIGIN;
            }

            c1 = (make_point({ -1 * (_slope * e1), 1 }) - ORIGIN) * (p - ORIGIN); // (y - y0) = m * (x - x0)
            c2 = (make_point({ -1 * (_slope * e2), 1 }) - ORIGIN) * (p - ORIGIN);
            _intercept = make_point({ c1, c2 }) - ORIGIN;
        }

    public:
        Wedge() = default;

        Wedge(const MovetkPoint& p,
            const MovetkPoint& center,
            NT radius)
        {
            construct(p, center, radius);
        }

        Wedge(MovetkPoint&& p,
            MovetkPoint&& center,
            NT radius)
        {
            construct(p, center, radius);
        }

        Wedge(const MovetkVector& slope,
            const MovetkVector& intercept) : _slope(slope),
            _intercept(intercept) {}

        const MovetkVector& slope() const
        {
            return _slope;
        }

        const MovetkVector& intercept() const
        {
            return _intercept;
        }

        bool is_empty() const
        {
            if (_slope == (ORIGIN - ORIGIN))
            {
                if (_intercept == (ORIGIN - ORIGIN))
                {
                    return true;
                }
            }
            return false;
        }

        /*!
         *
         * @param w
         * @return
         */
        Wedge operator*(const Wedge& w) const
        {
            constexpr std::size_t size = 2 * Norm::P;
            std::array<std::size_t, size> positions = { 0, 1, 2, 3 };
            std::size_t sum = *(positions.end() - 1) + *(positions.end() - 2);
            typename GeometryTraits::MovetkVector that_slope = w.slope();
            typename GeometryTraits::MovetkVector that_intercept = w.intercept();
            std::array<NT, size> slopes = { this->_slope * e1, this->_slope * e2,
                                           that_slope * e1, that_slope * e2 };
            std::array<NT, size> intercepts = { this->_intercept * e1, this->_intercept * e2,
                                               that_intercept * e1, that_intercept * e2 };

            std::sort(std::begin(positions), std::end(positions), [&slopes](size_t i, size_t j) {
                return slopes[i] < slopes[j];
            });

            auto start = positions.cbegin();
            auto end = positions.cend();
            auto sit = slopes.cbegin();
            auto it = intercepts.cbegin();

            NT diff_this = (slopes[0] - slopes[1]) / (1 + (slopes[0] * slopes[1]));
            NT diff_that = (slopes[2] - slopes[3]) / (1 + (slopes[2] * slopes[3]));
            NT WedgeAngle_this = 2 * movetk_core::rad2deg(diff_this);
            NT WedgeAngle_that = 2 * movetk_core::rad2deg(diff_that);

            if ((*start + *(start + 1) == sum) || (*(end - 1) + *(end - 2) == sum))
            {
                if ((abs(WedgeAngle_this) < 90) || (abs(WedgeAngle_that) < 90))
                    return Wedge();
            }

            if (slopes[0] > slopes[1])
            {
                that_slope = make_point({ *(sit + *(start + 2)), *(sit + *(start + 1)) }) - ORIGIN;
                that_intercept = make_point({ *(it + *(start + 2)), *(it + *(start + 1)) }) - ORIGIN;
            }
            else
            {
                that_slope = make_point({ *(sit + *(start + 1)), *(sit + *(start + 2)) }) - ORIGIN;
                that_intercept = make_point({ *(it + *(start + 1)), *(it + *(start + 2)) }) - ORIGIN;
            }
            return Wedge(that_slope, that_intercept);
        }

        /*!
         *
         * @param p
         * @return
         */
        bool operator*(const MovetkPoint& p) const
        {
            NT mx1 = (make_point({ this->_slope * this->e1, -1 }) - ORIGIN) * (p - ORIGIN);
            NT mx2 = (make_point({ this->_slope * this->e2, -1 }) - ORIGIN) * (p - ORIGIN);
            typename GeometryTraits::MovetkVector v = make_point({ mx1, mx2 }) - ORIGIN;
            typename GeometryTraits::MovetkVector result = v + this->_intercept;

            if ((result * this->e1) >= 0 && (result * this->e2) <= 0)
                return true;

            if ((result * this->e1) <= 0 && (result * this->e2) >= 0)
                return true;

            return false;
        }
    };

    template <class GeometryTraits, class Norm>
    std::ostream& operator<<(std::ostream& out, Wedge<GeometryTraits, Norm>& wedge)
    {
        MakePoint<GeometryTraits> make_point;
        typename GeometryTraits::MovetkPoint ORIGIN = make_point({ 0, 0 });
        typename GeometryTraits::MovetkVector e1 = make_point({ 1, 0 }) - ORIGIN;
        typename GeometryTraits::MovetkVector e2 = make_point({ 0, 1 }) - ORIGIN;
        typename GeometryTraits::MovetkVector slope = wedge.slope();
        typename GeometryTraits::MovetkVector intercept = wedge.intercept();
        typename GeometryTraits::NT result_m1 = slope * e1;
        typename GeometryTraits::NT result_m2 = slope * e2;
        typename GeometryTraits::NT result_c1 = intercept * e1;
        typename GeometryTraits::NT result_c2 = intercept * e2;
        out << "Line1 slope: " << result_m1 << ", Line1 intercept: " << result_c1 << "\n";
        out << "Line2 slope: " << result_m2 << ", Line2 intercept: " << result_c2 << "\n";
        return out;
    }

    template <class GeometryTraits, class Norm,
        typename = movetk_core::requires_planar_geometry<GeometryTraits>,
        typename = movetk_core::requires_L2_norm<Norm>>
        class MBR
    {
    private:
        typedef typename GeometryTraits::NT NT;
        typedef typename GeometryTraits::MovetkPoint Point;
        typedef typename GeometryTraits::MovetkVector Vector;
        typedef typename GeometryTraits::MovetkSphere Sphere;
        typedef movetk_core::IntersectionTraits<GeometryTraits, Norm,
            movetk_core::sphere_sphere_intersection_tag>
            IntersectionTraits;
        typename IntersectionTraits::Norm norm;

        movetk_core::MakePoint<GeometryTraits> make_point;
        movetk_core::MakeSphere<GeometryTraits> make_sphere;
        movetk_core::ComputeIntersections<IntersectionTraits> compute_intersections;

        Point ORIGIN = make_point({ 0, 0 });

        NT get_x(Vector& v)
        {
            return v * v.basis(0);
        }

        NT get_x(Point& p)
        {
            Vector v = p - ORIGIN;
            return v * v.basis(0);
        }

        NT get_y(Vector& v)
        {
            return v * v.basis(1);
        }

        NT get_y(Point& p)
        {
            Vector v = p - ORIGIN;
            return v * v.basis(1);
        }

        NT get_length(Point& p_u, Point& p_v)
        {
            Vector direction = p_v - p_u;
            norm(direction);
            NT length = norm ^ 1;
            return length;
        }

        NT get_length(Vector& direction)
        {
            norm(direction);
            NT length = norm ^ 1;
            return length;
        }

        Vector get_direction_vector(Point& p_u, Point& p_v)
        {
            return ((p_v - p_u) *= (1 / get_length(p_u, p_v)));
        }

        Vector get_direction_vector(Vector& v)
        {
            Vector direction = v;
            return (v *= (1 / get_length(direction)));
        }

        Point translate(Vector unit_vector, Point& start, NT translation_length)
        {
            unit_vector *= translation_length;
            return start + unit_vector;
        }

    public:
        std::pair<Point, Point> operator()(Point& p_u, Point& p_v, NT radius_u, NT radius_v)
        {
            Sphere sphere_u = make_sphere(p_u, radius_u);
            Sphere sphere_v = make_sphere(p_v, radius_v);
            Sphere intersection_sphere = compute_intersections(sphere_u,
                sphere_v);
            Point center = intersection_sphere.center();

            if (intersection_sphere.squared_radius() == 0)
                return std::make_pair(center, center);

            NT half_length = std::pow(intersection_sphere.squared_radius(), 0.5);

            Vector direction_uv = get_direction_vector(p_u, p_v);
            Point point_sphere_u = translate(direction_uv, p_u, radius_u);
            Point point_sphere_v = translate(direction_uv, p_v, -radius_v);

            //counterclockwise rotation by pi/2
            Vector direction_p = make_point({ -1 * get_y(direction_uv), get_x(direction_uv) }) - ORIGIN;

            Point p1 = translate(direction_p, point_sphere_u, half_length);
            Point p2 = translate(direction_p, point_sphere_v, -half_length);

            return std::make_pair(p1, p2);
        }
    };

    template <class GeometryTraits, class Norm, class T>
    struct mbr_selector
    {
        typedef T MinimumBoundingRectangle;
    };

    template <class GeometryTraits, class Norm>
    struct mbr_selector<GeometryTraits, Norm, void>
    {
        typedef MBR<GeometryTraits, Norm> MinimumBoundingRectangle;
    };

    template <class GeometryTraits>
    struct Scaling
    {

        typename GeometryTraits::MovetkVector operator()(typename GeometryTraits::MovetkPoint p1,
            typename GeometryTraits::MovetkPoint p2,
            typename GeometryTraits::NT eps)
        {
            typename GeometryTraits::MovetkVector v = p2 - p1;
            v *= eps;
            return v;
        }

        typename GeometryTraits::MovetkVector operator()(typename GeometryTraits::MovetkVector v,
            typename GeometryTraits::NT eps)
        {
            v *= eps;
            return v;
        }
    };

    template <class GeometryTraits>
    struct Translation
    {

        typename GeometryTraits::MovetkPoint operator()(typename GeometryTraits::MovetkPoint p,
            typename GeometryTraits::MovetkVector v)
        {
            typename GeometryTraits::MovetkPoint p1 = p + v;
            return p1;
        }
    };

    /*!@struct MovetkGeometryKernel
     * @brief A traits class that defines movetk geometry types
     * @tparam WrapperGeometryKernel - The traits class of a wrapper to a geometry
     * backend
     */
    template <class WrapperGeometryKernel>
    struct MovetkGeometryKernel
    {
    public:
        /*!*
         * @typedef ::NT
         * */
        typedef typename WrapperGeometryKernel::NT NT;
        constexpr static size_t dim = WrapperGeometryKernel::dim;
        /*!*
         * @typedef ::MovetkPoint
         * */
        typedef typename WrapperGeometryKernel::Wrapper_Point MovetkPoint;
        /*!*
         * @typedef ::MovetkLine
         * */
        typedef typename WrapperGeometryKernel::Wrapper_Line MovetkLine;
        /*!*
         * @typedef ::MovetkSegment
         * */
        typedef typename WrapperGeometryKernel::Wrapper_Segment MovetkSegment;
        /*!*
         * @typedef ::MovetkVector
         * */
        typedef typename WrapperGeometryKernel::Wrapper_Vector MovetkVector;
        /*!*
         * @typedef ::MovetkMinSphere
         * */
        typedef typename WrapperGeometryKernel::Wrapper_MinSphere MovetkMinSphere;
        /*!*
         * @typedef ::MovetkPolygon
         * */
        typedef typename WrapperGeometryKernel::Wrapper_Polygon MovetkPolygon;
        /*!*
         * @typedef ::MovetkSphere
         * */
        typedef typename WrapperGeometryKernel::Wrapper_Sphere MovetkSphere;
        /*!*
         * @typedef ::MovetkIntersectionVisitor
         * */
        typedef typename WrapperGeometryKernel::Intersection_visitor MovetkIntersectionVisitor;
        /*!*
         * @typedef ::MovetkSquaredDistance
         * */
        typedef typename WrapperGeometryKernel::Wrapper_Squared_Distance MovetkSquaredDistance;
        /*!*
         * @typedef ::MovetkCurveIntersection
         * */
        typedef typename WrapperGeometryKernel::Wrapper_Curve_Intersection MovetkCurveIntersection;
        /*!*
         * @typedef ::MovetkDiscreteHausdorffDistance
         * */
        typedef typename WrapperGeometryKernel::Wrapper_Discrete_Hausdorff_Distance MovetkDiscreteHausdorffDistance;
        /*!*
         * @typedef ::MovetkDiscreteFrechetDistance
         * */
        typedef typename WrapperGeometryKernel::Wrapper_Discrete_Frechet_Distance MovetkDiscreteFrechetDistance;
        /*!*
        * @typedef ::MinimumBoundingRectangle
        * */
        typedef typename WrapperGeometryKernel::Wrapper_Minimum_Bounding_Rectangle MovetkMinimumBoundingRectangle;
    };

}; // namespace movetk_core

#endif //MOVETK_INTERFACE_H
