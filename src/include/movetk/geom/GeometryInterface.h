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

#include <array>
#include <iostream>

#include "Intersections.h"
#include "ObjectCreation.h"
#include "movetk/utils/Asserts.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/Requirements.h"

#define PI 3.14159265
#define TWO_PI 6.2831853
#define LOG_TWO_PI 1.837877066409345

  // TODO  Concepts for the Interface
  /*!
   *
   *  @namespace movetk::utils
   *  @brief the core of movetk
   */
namespace movetk::geom {
    // the support library for Movetk

    /*!
     * @brief converts from degree to radians
     * @tparam NT
     * @param degrees
     * @return
     */
    template <class NT>
    NT deg2radians(const NT degrees) {
        return degrees * PI / 180.0;
    }

    /*!
     * @brief converts from radians to degrees
     * @tparam NT
     * @param radians
     * @return
     */
    template <class NT>
    NT rad2deg(const NT radians) {
        return radians * 180.0 / PI;
    }


    /*!
     *
     * @tparam GeometryTraits
     * @tparam Norm
     */
    template <utils::KernelSatisfying<utils::is_planar_geometry2> GeometryTraits,
        utils::L2Norm Norm>
        class Wedge {
        public:
            using MovetkVector = typename GeometryTraits::MovetkVector;
            using MovetkPoint = typename GeometryTraits::MovetkPoint;

        private:
            using NT = typename GeometryTraits::NT;
            Norm norm;
            geom::MakePoint<GeometryTraits> make_point;
            MovetkPoint ORIGIN = make_point({ 0, 0 });
            MovetkVector e1 = make_point({ 1, 0 }) - ORIGIN;
            MovetkVector e2 = make_point({ 0, 1 }) - ORIGIN;
            MovetkVector _slope = ORIGIN - ORIGIN;
            MovetkVector _intercept = ORIGIN - ORIGIN;
            bool horizontal = false, vertical = false;
            bool degenerate = false;
            bool upper_right = false, lower_left = false;
            bool lower_right = false, upper_left = false;

            void construct(const MovetkPoint& p, const MovetkPoint& center, NT radius) {
                NT m1, m2, c1, c2, tanA, tanB;
                NT squared_radius = radius * radius;
                auto _slope_ray = center - p;
                NT v_x = _slope_ray * e1;
                NT v_y = _slope_ray * e2;
                NT segment_squared_length = norm(_slope_ray);
                NT root = norm ^ 1;
                if (segment_squared_length < squared_radius) {
                    degenerate = true;
                }
                if (root < MOVETK_EPS) {
                    degenerate = true;
                }
                if (abs(v_x) < MOVETK_EPS) {
                    vertical = true;
                }
                if (abs(v_y) < MOVETK_EPS) {
                    horizontal = true;
                }
                if (v_x >= 0 && v_y >= 0) {
                    upper_right = true;
                }
                if (v_y <= 0 && v_x >= 0) {
                    lower_right = true;
                }
                if (v_y >= 0 && v_x <= 0) {
                    upper_left = true;
                }
                if (v_x <= 0 && v_y <= 0) {
                    lower_left = true;
                }
                NT tangent_squared_length = segment_squared_length - squared_radius;
                if (abs(tangent_squared_length) < MOVETK_EPS)
                    degenerate = true;
                else
                    tanB = radius / sqrt(tangent_squared_length);
                if (degenerate) {
                    _slope = ORIGIN - ORIGIN;
                }
                else if (horizontal) {
                    if (upper_right || lower_right)
                        _slope = make_point({ tanB, -tanB }) - ORIGIN;
                    else if (upper_left || lower_left)
                        _slope = make_point({ -tanB, tanB }) - ORIGIN;
                }
                else if (vertical) {
                    tanA = tan(PI / 2 - MOVETK_EPS * 0.001);
                    if ((tanA * tanB) == 1)
                        m1 = (tanA + tanB) / MOVETK_EPS;
                    else
                        m1 = (tanA + tanB) / (1 - tanA * tanB);
                    m2 = -m1;
                    _slope = make_point({ m1, m2 }) - ORIGIN;
                }
                else {
                    tanA = v_y / v_x;  // slope of the ray
                    if ((tanA * tanB) == 1) {
                        m1 = (tanA + tanB) / MOVETK_EPS;
                        m2 = (tanA - tanB) / (1 + tanA * tanB);
                    }
                    else if ((tanA * tanB) == -1) {
                        m1 = (tanA + tanB) / (1 - tanA * tanB);
                        m2 = (tanA - tanB) / (-1 * MOVETK_EPS);
                    }
                    else {
                        m1 = (tanA + tanB) / (1 - tanA * tanB);
                        m2 = (tanA - tanB) / (1 + tanA * tanB);
                    }

                    if (upper_right || lower_right)
                        _slope = make_point({ m1, m2 }) - ORIGIN;
                    else if (upper_left || lower_left)
                        _slope = make_point({ m2, m1 }) - ORIGIN;
                }

                c1 = (make_point({ -1 * (_slope * e1), 1 }) - ORIGIN) * (p - ORIGIN);  // (y - y0) = m * (x - x0)
                c2 = (make_point({ -1 * (_slope * e2), 1 }) - ORIGIN) * (p - ORIGIN);
                _intercept = make_point({ c1, c2 }) - ORIGIN;
            }

        public:
            Wedge() = default;

            Wedge(const MovetkPoint& p, const MovetkPoint& center, NT radius) { construct(p, center, radius); }

            Wedge(MovetkPoint&& p, MovetkPoint&& center, NT radius) { construct(p, center, radius); }

            Wedge(const MovetkVector& slope, const MovetkVector& intercept) : _slope(slope), _intercept(intercept) {}

            const MovetkVector& slope() const { return _slope; }
            const MovetkVector& intercept() const { return _intercept; }
            bool is_empty() const {
                if (_slope == (ORIGIN - ORIGIN)) {
                    if (_intercept == (ORIGIN - ORIGIN)) {
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
            Wedge operator*(const Wedge& w) const {
                constexpr std::size_t size = 2 * Norm::P;
                std::array<std::size_t, size> positions = { 0, 1, 2, 3 };
                std::size_t sum = *(positions.end() - 1) + *(positions.end() - 2);
                typename GeometryTraits::MovetkVector that_slope = w.slope();
                typename GeometryTraits::MovetkVector that_intercept = w.intercept();
                std::array<NT, size> slopes = { this->_slope * e1, this->_slope * e2, that_slope * e1, that_slope * e2 };
                std::array<NT, size> intercepts = { this->_intercept * e1,
                                                   this->_intercept * e2,
                                                   that_intercept * e1,
                                                   that_intercept * e2 };

                std::sort(std::begin(positions), std::end(positions), [&slopes](size_t i, size_t j) {
                    return slopes[i] < slopes[j];
                });

                auto start = positions.cbegin();
                auto end = positions.cend();
                auto sit = slopes.cbegin();
                auto it = intercepts.cbegin();

                NT diff_this = (slopes[0] - slopes[1]) / (1 + (slopes[0] * slopes[1]));
                NT diff_that = (slopes[2] - slopes[3]) / (1 + (slopes[2] * slopes[3]));
                NT WedgeAngle_this = 2 * movetk::geom::rad2deg(diff_this);
                NT WedgeAngle_that = 2 * movetk::geom::rad2deg(diff_that);

                if ((*start + *(start + 1) == sum) || (*(end - 1) + *(end - 2) == sum)) {
                    if ((abs(WedgeAngle_this) < 90) || (abs(WedgeAngle_that) < 90))
                        return Wedge();
                }

                if (slopes[0] > slopes[1]) {
                    that_slope = make_point({ *(sit + *(start + 2)), *(sit + *(start + 1)) }) - ORIGIN;
                    that_intercept = make_point({ *(it + *(start + 2)), *(it + *(start + 1)) }) - ORIGIN;
                }
                else {
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
            bool operator*(const MovetkPoint& p) const {
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
    std::ostream& operator<<(std::ostream& out, Wedge<GeometryTraits, Norm>& wedge) {
        MakePoint<GeometryTraits> make_point;
        const auto ORIGIN = make_point({ 0, 0 });
        const auto e1 = make_point({ 1, 0 }) - ORIGIN;
        const auto e2 = make_point({ 0, 1 }) - ORIGIN;
        const auto slope = wedge.slope();
        const auto intercept = wedge.intercept();
        const auto result_m1 = slope * e1;
        const auto result_m2 = slope * e2;
        const auto result_c1 = intercept * e1;
        const auto result_c2 = intercept * e2;
        out << "Line1 slope: " << result_m1 << ", Line1 intercept: " << result_c1 << "\n";
        out << "Line2 slope: " << result_m2 << ", Line2 intercept: " << result_c2 << "\n";
        return out;
    }

    template <utils::KernelSatisfying<utils::is_planar_geometry2> GeometryTraits,
        class Norm,
        typename = utils::requires_L2_norm<Norm>>
        class MBR {
        private:
            typedef typename GeometryTraits::NT NT;
            typedef typename GeometryTraits::MovetkPoint Point;
            typedef typename GeometryTraits::MovetkVector Vector;
            typedef typename GeometryTraits::MovetkSphere Sphere;
            typedef geom::IntersectionTraits<GeometryTraits, Norm, geom::sphere_sphere_intersection_tag> IntersectionTraits;
            typename IntersectionTraits::Norm norm;

            geom::MakePoint<GeometryTraits> make_point;
            geom::MakeSphere<GeometryTraits> make_sphere;
            geom::ComputeIntersections<IntersectionTraits> compute_intersections;

            Point ORIGIN = make_point({ 0, 0 });

            NT get_x(Vector& v) { return v * v.basis(0); }

            NT get_x(Point& p) {
                Vector v = p - ORIGIN;
                return v * v.basis(0);
            }

            NT get_y(Vector& v) { return v * v.basis(1); }

            NT get_y(Point& p) {
                Vector v = p - ORIGIN;
                return v * v.basis(1);
            }

            NT get_length(Point& p_u, Point& p_v) {
                Vector direction = p_v - p_u;
                norm(direction);
                NT length = norm ^ 1;
                return length;
            }

            NT get_length(Vector& direction) {
                norm(direction);
                NT length = norm ^ 1;
                return length;
            }

            Vector get_direction_vector(Point& p_u, Point& p_v) { return ((p_v - p_u) *= (1 / get_length(p_u, p_v))); }

            Vector get_direction_vector(Vector& v) {
                Vector direction = v;
                return (v *= (1 / get_length(direction)));
            }

            Point translate(Vector unit_vector, Point& start, NT translation_length) {
                unit_vector *= translation_length;
                return start + unit_vector;
            }

        public:
            std::pair<Point, Point> operator()(Point& p_u, Point& p_v, NT radius_u, NT radius_v) {
                Sphere sphere_u = make_sphere(p_u, radius_u);
                Sphere sphere_v = make_sphere(p_v, radius_v);
                Sphere intersection_sphere = compute_intersections(sphere_u, sphere_v);
                Point center = intersection_sphere.center();

                if (intersection_sphere.squared_radius() == 0)
                    return std::make_pair(center, center);

                NT half_length = std::pow(intersection_sphere.squared_radius(), 0.5);

                Vector direction_uv = get_direction_vector(p_u, p_v);
                Point point_sphere_u = translate(direction_uv, p_u, radius_u);
                Point point_sphere_v = translate(direction_uv, p_v, -radius_v);

                // counterclockwise rotation by pi/2
                Vector direction_p = make_point({ -1 * get_y(direction_uv), get_x(direction_uv) }) - ORIGIN;

                Point p1 = translate(direction_p, point_sphere_u, half_length);
                Point p2 = translate(direction_p, point_sphere_v, -half_length);

                return std::make_pair(p1, p2);
            }
    };

    template <class GeometryTraits, class Norm, class T>
    struct mbr_selector {
        using MinimumBoundingRectangle = T;
    };

    template <class GeometryTraits, class Norm>
    struct mbr_selector<GeometryTraits, Norm, void> {
        using MinimumBoundingRectangle = MBR<GeometryTraits, Norm>;
    };

    template <class GeometryTraits>
    struct Scaling {
        typename GeometryTraits::MovetkVector operator()(typename GeometryTraits::MovetkPoint p1,
            typename GeometryTraits::MovetkPoint p2,
            typename GeometryTraits::NT eps) {
            typename GeometryTraits::MovetkVector v = p2 - p1;
            v *= eps;
            return v;
        }

        typename GeometryTraits::MovetkVector operator()(typename GeometryTraits::MovetkVector v,
            typename GeometryTraits::NT eps) {
            v *= eps;
            return v;
        }
    };

    template <class GeometryTraits>
    struct Translation {
        typename GeometryTraits::MovetkPoint operator()(typename GeometryTraits::MovetkPoint p,
            typename GeometryTraits::MovetkVector v) {
            auto p1 = p + v;
            return p1;
        }
    };

    /*!@struct MovetkGeometryKernel
     * @brief A traits class that defines movetk geometry types
     * @tparam WrapperGeometryKernel - The traits class of a wrapper to a geometry
     * backend
     */
    template <class WrapperGeometryKernel>
    struct MovetkGeometryKernel {
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

}  // namespace movetk::geom

#endif  // MOVETK_INTERFACE_H
