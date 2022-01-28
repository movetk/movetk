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

/*! @file CGALAdapter.h
 *  @brief  An adapter for the CGAL library
 *  @details A collection of adapter classes for all
 *  CGAL geometric objects and CGAL algorithms that are used by movetk.
 *  @authors Aniket Mitra (aniket.mitra@here.com)
 */

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cstdlib>
#include <CGAL/Cartesian_d.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Homogeneous_d.h>
#include <CGAL/Exact_rational.h>
#include <CGAL/Min_sphere_of_spheres_d.h>
#include <CGAL/Min_sphere_of_points_d_traits_d.h>
#include <CGAL/intersections_d.h>
#include <CGAL/Kernel_d/Sphere_d.h>
#include <CGAL/Gmpfr.h>
#include <CGAL/Arrangement_2.h>
#include <CGAL/squared_distance_2.h>
#include "movetk/utils/StringUtils.h"
#include "movetk/utils/Asserts.h"
#include "movetk/utils/Requirements.h"
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Surface_sweep_2_algorithms.h>
#include <CGAL/IO/io.h>
#include <list>


namespace movetk::geom {
    // the support library for Movetk

    /*!@struct is_valid_NT
    * @brief A class to checking the number type of CGALTraits
    * @tparam Type
    */
    template<class Type>
    struct is_valid_NT {
        static const bool value = false;
        static const bool can_cast_to_string = false;
    };

    /*!
     * @brief a specialization of is_valid_NT
     * @details sets value to true iff \c Type == \c long double
     */
    template<>
    struct is_valid_NT<long double> {
        static const bool value = true;
        static const bool can_cast_to_string = true;
    };

    /*!
    * @brief a specialization of is_valid_NT
     * @details sets value to true iff \c Type == \c CGAL::MP_Float
     */
    template<>
    struct is_valid_NT<CGAL::MP_Float> {
        static const bool value = true;
        static const bool can_cast_to_string = false;
    };

    /*!
     * @brief a specialization of is_valid_NT
     * @details sets value to true iff \c Type == \c CGAL::Gmpfr
     */
    template<>
    struct is_valid_NT<CGAL::Gmpfr> {
        static const bool value = true;
        static const bool can_cast_to_string = false;
    };

    /*!
     * @brief a specialization of is_valid_NT
     * @details sets value to true iff \c Type == \c CGAL::Gmpfq
     */
    template<>
    struct is_valid_NT<CGAL::Gmpq> {
        static const bool value = true;
        static const bool can_cast_to_string = false;
    };

    /*!
     * @brief a specialization of is_valid_NT
     * @details sets value to true iff \c Type == \c CGAL::Mpzf
     */
    template<>
    struct is_valid_NT<CGAL::Mpzf> {
        static const bool value = true;
        static const bool can_cast_to_string = false;
    };


    template<class T = void>
    using requires_valid_CGAL_NT = typename std::enable_if<is_valid_NT<T>::value>::type;



    template<bool, class Kernel, class Type>
    struct OutputRep {
    };

    template<class Kernel>
    struct OutputRep<true, Kernel, typename Kernel::Wrapper_Point> {
        std::ostream& operator()(std::ostream &out, const typename Kernel::Wrapper_Point &pt) {
            return (out << movetk_support::join(pt.begin(), pt.end()));
        }
    };

    template<class Kernel>
    struct OutputRep<false, Kernel, typename Kernel::Wrapper_Point> {
        std::ostream& operator()(std::ostream &out, const typename Kernel::Wrapper_Point &pt) {
            return ( out << pt.get() );
        }
    };

    template<class Kernel>
    struct OutputRep<true, Kernel, typename Kernel::Wrapper_Vector>{
        std::ostream& operator()(std::ostream &out, const typename Kernel::Wrapper_Vector &v) {
            return (out << movetk_support::join(v.begin(), v.end()));
        }
    };

    template<class Kernel>
    struct OutputRep<false, Kernel, typename Kernel::Wrapper_Vector> {
        std::ostream& operator()(std::ostream &out, const typename Kernel::Wrapper_Point &v) {
            return ( out << v.get() );
        }
    };

    template<class Kernel>
    struct OutputRep<true, Kernel, typename Kernel::Wrapper_Sphere> {
        std::ostream& operator()(std::ostream &out, const typename Kernel::Wrapper_Sphere &s) {
            typename Kernel::Wrapper_Point center = s.center();
            out << movetk_support::join(center.begin(), center.end()) + ";";
            out << s.squared_radius();
            return out;
        }
    };

    template<class Kernel>
    struct OutputRep<false, Kernel, typename Kernel::Wrapper_Sphere> {
        std::ostream& operator()(std::ostream &out, const typename Kernel::Wrapper_Sphere &s) {
            typename Kernel::Wrapper_Point center = s.center();
            out << center.get();
            out << ",";
            out << s.squared_radius();
            return  out;
        }
    };


    template<class Kernel>
    struct OutputRep<true, Kernel, typename Kernel::Wrapper_Polygon> {
        std::ostream& operator()(std::ostream &out, typename Kernel::Wrapper_Polygon &poly) {
            auto it = poly.get().vertices_begin();
            auto beyond = poly.get().vertices_end();
            //out << "Number of vertices: " << std::distance(it, beyond) << "\n";
            std::string mergedTokens = movetk_support::join(it->cartesian_begin(), it->cartesian_end(), ';');
            it++;
            while (it != beyond) {
                mergedTokens += ',';
                mergedTokens += movetk_support::join(it->cartesian_begin(), it->cartesian_end(), ';');
                it++;
            }
            out << mergedTokens;
            return out;
        }
    };

    template<class Kernel>
    struct OutputRep<false, Kernel, typename Kernel::Wrapper_Polygon> {
        std::ostream& operator()(std::ostream &out, typename Kernel::Wrapper_Polygon &poly) {
            auto it = poly.get().vertices_begin();
            auto beyond = poly.get().vertices_end();
            //out << "Number of vertices: " << std::distance(it, beyond) << "\n";
            out << *it;
            it++;
            while (it != beyond) {
                out << ",";
                out << *it;
                it++;
            }
            //out << mergedTokens;
            return out;
        }
    };





    /*! @class Wrapper_CGAL_Point
         *  @brief A wrapper class for CGAL Point.
         *  @details This class acts as a wrapper to a CGAL point by encapsulating
         *  CGAL specific details of a point. Provides a generic interface for a Point.
         * @tparam Kernel - A traits class that defines CGAL geometry types
         */
        template<class Kernel>
        class Wrapper_CGAL_Point {

        private:
            typedef typename Kernel::CGAL_Point_ CGAL_Point;
            typedef typename Kernel::Wrapper_Vector Wrapper_Vector;
            CGAL_Point pt;

        public:

            /*!
             * 	Creates a CGAL point and a generic interface to a Point as defined
             * 	by this class.
             */
            Wrapper_CGAL_Point() = default;

            /*!
             * Creates a CGAL point and a generic interface to a Point as defined
             * by this class.
             * @param p - A CGAL point
             */
            Wrapper_CGAL_Point(const CGAL_Point &p) : pt(p) {}

            /*!
             * Creates a CGAL point and a generic interface to a Point as defined
             * by this class.
             * @details Infers the dimensions of the point from the input
             * and creates a point with Cartesian coordinates \f$[first,beyond)\f$
             * @pre InputIterator should be a random access iterator where
             * each coordinate is of type GeometryTraits::NT
             * @param first - Iterator to the first Cartesian coordinate
             * @param beyond - Iterator to the end of the last Cartesian coordinate
             */
            template<class CoordinateIterator,
                    typename = movetk::utils::requires_random_access_iterator<CoordinateIterator >>
            Wrapper_CGAL_Point(CoordinateIterator first,
                               CoordinateIterator beyond) {
                //ASSERT_RANDOM_ACCESS_ITERATOR(CoordinateIterator);
                ASSERT_NUMBER_TYPE(Kernel, first);
                size_t dimensions = std::distance(first, beyond);
                pt = CGAL_Point(dimensions, first, beyond);
            }

            /*!
             *
             * @return Iterator to the first coordinate of the CGAL point
             */
            auto begin() const {
                return pt.cartesian_begin();
            }

            /*!
             *
             * @return Iterator to the end of the last coordinate of the CGAL point
             */
            auto end() const {
                return pt.cartesian_end();
            }

            /*!
             * Compares two wrapped CGAL points for less than
             * @param point - A point of type Wrapper_CGAL_Point<Kernel>
             * @return  True / False
             */
            bool operator<(const Wrapper_CGAL_Point<Kernel> &point) {
                return this->get() < point.get();
            }



            /*!
             * Compares two wrapped CGAL points for greater than
             * @param point - A point of type Wrapper_CGAL_Point<Kernel>
             * @return  True / False
             */
            bool operator>(const Wrapper_CGAL_Point<Kernel> &point) {
                return this->get() > point.get();
            }

            /*!
             * Compares two wrapped CGAL points for less than equality
             * @param point -  A point of type Wrapper_CGAL_Point<Kernel>
             * @return True / False
             */
            bool operator<=(const Wrapper_CGAL_Point<Kernel> &point) const{
                return this->get() <= point.get();
            }

            /*!
             * Compares two wrapped CGAL points for greater than equality
             * @param point - A point of type Wrapper_CGAL_Point<Kernel>
             * @return  True / False
             */
            bool operator>=(const Wrapper_CGAL_Point<Kernel> &point) {
                return this->get() >= point.get();
            }

            Wrapper_Vector  operator-(const Wrapper_CGAL_Point<Kernel> &point) const{
                return this->get() - point.get();
            }

            Wrapper_Vector  operator-(const Wrapper_CGAL_Point<Kernel> &&point) const{
                return this->get() - point.get();
            }

            Wrapper_CGAL_Point<Kernel>  operator+(Wrapper_Vector &v) const{
                return this->get() + v.get();
            }

            Wrapper_CGAL_Point<Kernel>  operator+(Wrapper_Vector &&v) const{
                return this->operator+(v);
            }

            Wrapper_CGAL_Point<Kernel>  operator-(Wrapper_Vector &v) const{
                return this->get() - v.get();
            }

            /*!
             * @return  A CGAL point
             */
            CGAL_Point get() const{
                return pt;
            }


        };

        /*!
        * Prints a point whose coordinates are separated by comma
        * @param out - OutputStream
        * @param point - A point of type Wrapper_CGAL_Point<Kernel>
        */
        template <class Kernel>
        std::ostream &operator<<(std::ostream &out, const Wrapper_CGAL_Point<Kernel> &point) {
            OutputRep<is_valid_NT<typename Kernel::NT>::can_cast_to_string,
                Kernel, Wrapper_CGAL_Point<Kernel> > output;
            return output(out, point);
        }




    /*! @class Wrapper_CGAL_Line
         * @brief A wapper class for CGAL line
         * @details This class acts as a wrapper to a CGAL line by
         * encapsulating CGAL specific details of a line.
         * Provides a generic inteface for a Line.
         * @tparam Kernel - A traits class that defines CGAL geometry types
         */
        template<class Kernel>
        class Wrapper_CGAL_Line {
        private:
            typedef typename Kernel::CGAL_Point_ CGAL_Point;
            typedef typename Kernel::CGAL_Line_ CGAL_Line;
            typedef typename Kernel::Wrapper_Point Wrapper_Point;
            CGAL_Line line;
        public:
            /*!
             * Creates a CGAL line and a generic interface to a Line as defined
             * by this class.
             */
            Wrapper_CGAL_Line() = default;

            /*!
             * Creates a wrapped CGAL line
             * @param l - A CGAL line
             */
            Wrapper_CGAL_Line(const CGAL_Line &l) : line(l) {}

            /*!
             * Creates a CGAL line and a generic interface to a Line as defined
             * by this class.
             * @param p1 - A wrapped CGAL point through which the wrapped CGAL line passes
             * @param p2 - A wrapped CGAL point through which the wrapped CGAL line passes
             */
            Wrapper_CGAL_Line(Wrapper_CGAL_Point<Kernel> &p1, Wrapper_CGAL_Point<Kernel> &p2) {
                line = CGAL_Line(p1.get(), p2.get());
            }

            Wrapper_Point operator[](size_t idx) {
               return line[idx];
            }

            /*!
             *
             * @return A CGAL line
             */
            CGAL_Line get() const {
                return line;
            }
        };

        /*! @class Wrapper_CGAL_Segment
         *  @brief A wrapper class for CGAL Segment
         *  @details This class acts as a wrapper to a CGAL segment by encapsulating
         *  CGAL specific details of segment.
         *  Provides a generic inteface for a segment.
         * @tparam Kernel  - A traits class that defines CGAL geometry types
         */
        template<class Kernel>
        class Wrapper_CGAL_Segment {
        private:
            typedef typename Kernel::CGAL_Point_ CGAL_Point;
            typedef typename Kernel::CGAL_Segment_ CGAL_Segment;
            typedef typename Kernel::Wrapper_Point Wrapper_Point;
            typedef typename Kernel::NT NT;
            CGAL_Segment seg;
        public:
            /*!
            * Creates a CGAL segment and a generic interface to a Segment as defined
             * by this class.
             */
            Wrapper_CGAL_Segment() = default;

            /*!
             * Creates a CGAL segment and a generic interface to a Segment as defined
             * by this class.
             * @param s  - A CGAL segment
             */
            Wrapper_CGAL_Segment(const CGAL_Segment &s) : seg(s) {}

            /*!
             * Creates a CGAL segment and a generic interface to a Segment as defined
             * by this class.
             * @param p1 - A wrapped CGAL point that defines one end-point of
             * a wrapped CGAL segment
             * @param p2 - A wrapped CGAL point that defines the other end-point of
             * a wrapped CGAL segment
             */
            Wrapper_CGAL_Segment(Wrapper_CGAL_Point<Kernel> &p1, Wrapper_CGAL_Point<Kernel> &p2) {
                seg = CGAL_Segment(p1.get(), p2.get());
            }

            /*!
              * Creates a CGAL segment and a generic interface to a Segment as defined
              * by this class.
              * @param p1 - A wrapped CGAL point that defines one end-point of
              * a wrapped CGAL segment
              * @param p2 - A wrapped CGAL point that defines the other end-point of
              * a wrapped CGAL segment
              */
            Wrapper_CGAL_Segment(Wrapper_CGAL_Point<Kernel>&& p1, Wrapper_CGAL_Point<Kernel>&& p2) {
                seg = CGAL_Segment(p1.get(), p2.get());
            }

            Wrapper_Point operator[](size_t idx) const{
                  return seg[idx];
            }

            /*!
             *
             * @return Length of the segment
             */
            NT operator()() {
                return seg.squared_length();
            }

            /*!
            *
            * @return A CGAL segment
            */
            CGAL_Segment get() const {
                return seg;
            }
        };

        template <class Kernel>
        std::ostream& operator<<(std::ostream &out, Wrapper_CGAL_Segment<Kernel> &seg) {
            Wrapper_CGAL_Point<Kernel> p1 = seg[0];
            Wrapper_CGAL_Point<Kernel> p2 = seg[1];
            OutputRep<is_valid_NT<typename Kernel::NT>::can_cast_to_string,
                    Kernel,  Wrapper_CGAL_Point<Kernel> > output;
            out << output(out, p1);
            out << ";";
            out << output(out, p2);
            return out;
        }


    /*! @class Wrapper_CGAL_Vector
         * @brief A wrapper class for CGAL vector
         * @details This class acts as a wrapper to a CGAL vector by encapsulating
         *  CGAL specific details of vector.
         *  Provides a generic inteface for a vector.
         * @tparam Kernel  - A traits class that defines CGAL geometry types
         */
        template<class Kernel>
        class Wrapper_CGAL_Vector {
        private:
            typedef typename Kernel::CGAL_Point_ CGAL_Point;
            typedef typename Kernel::CGAL_Vector_ CGAL_Vector;
            typedef std::array<typename Kernel::NT, Kernel::dim> container;
            CGAL_Vector vec;
        public:

            /*!
            * Creates a CGAL vector and a generic interface to a Vector as defined
             * by this class.
             * @param pt - A wrapped CGAL point
             */
            explicit Wrapper_CGAL_Vector(Wrapper_CGAL_Point<Kernel> &p) {
                vec = p.get() - CGAL::ORIGIN;
            }

            Wrapper_CGAL_Vector(const CGAL_Vector &v) : vec(v) {}

            typename Kernel::NT operator*(const Wrapper_CGAL_Vector<Kernel>& vector) const {
                return this->get() * vector.get();
            }

            typename Kernel::NT operator*(const Wrapper_CGAL_Vector<Kernel>&& vector) const {
                return this->get() * vector.get();
            }

            Wrapper_CGAL_Vector<Kernel>& operator*=(typename Kernel::NT scalar){
                vec *= scalar;
                return *this;
            }

            Wrapper_CGAL_Vector<Kernel> operator+(const Wrapper_CGAL_Vector<Kernel>& vector) const{
                return this->get() + vector.get();
            }


            Wrapper_CGAL_Vector<Kernel> operator-(const Wrapper_CGAL_Vector<Kernel>& vector) const {
                return this->get() - vector.get();
            }

            bool operator==(const Wrapper_CGAL_Vector<Kernel>&& vector ) const{
                return std::equal(this->begin(),this->end(),vector.begin());

            }

            Wrapper_CGAL_Vector<Kernel> basis(std::size_t i) const{
                container e = {0};
                e[i] = 1;
                CGAL_Vector basis_vec(Kernel::dim ,std::begin(e), std::end(e));
                return  Wrapper_CGAL_Vector(basis_vec);
            }

            /*!
             *
             * @return Iterator to the first coordinate of the CGAL vector
             */
            auto begin() const {
                return vec.cartesian_begin();
            }

            /*!
             *
             * @return Iterator to the end of the last coordinate of the CGAL vector
             */
            auto end() const {
                return vec.cartesian_end();
            }


            /*!
           *
           * @return A CGAL vector
           */
            CGAL_Vector get() const {
                return vec;
            }
        };


    /*!
  * Prints a vector whose components are separated by comma
  * @param out - OutputStream
  * @param point - A point of type Wrapper_CGAL_Vector<Kernel>
  */
    template<class Kernel>
    std::ostream &operator<<(std::ostream &out, const Wrapper_CGAL_Vector<Kernel> &vec) {
        OutputRep<is_valid_NT<typename Kernel::NT>::can_cast_to_string,
                Kernel, Wrapper_CGAL_Vector<Kernel> > output;
        return output(out, vec);
    }



    /*! @class Wrapper_CGAL_Polygon
         *  @brief A wrapper class for CGAL polygon
         *  @details This class acts as a wrapper to a CGAL polygon by encapsulating
         *  CGAL specific details of polygon.
         *  Provides a generic inteface for a polygon.
         *  @note - Constructs a polygon in 2D
         * @tparam Kernel - A traits class that defines CGAL geometry types
         */
        template<class Kernel>
        class Wrapper_CGAL_Polygon {
        private:
            typedef typename Kernel::Wrapper_CGAL_Kernel_2::GeometryType_2 K;
            typedef typename K::Point_2 Point_2;
            typedef CGAL::Polygon_2<K> CGAL_Polygon;
            CGAL_Polygon polygon;
        public:

            /*!
             * 	Creates a wrapped CGAL polygon
             */
            Wrapper_CGAL_Polygon() = default;


            /*!
             * Creates a CGAL polygon and a generic interface to a polygon as defined
             * by this class.
             * @tparam PointIterator - A random access iterator over a set of points
             * where each point is of type Wrapper_CGAL_Point<Kernel>
             * @param first - Iterator to the first point in a set of points
             * where each point is of type Wrapper_CGAL_Point<Kernel>
             * @param beyond - Iterator to the end of the last point in a set of points
             * where each point is of type Wrapper_CGAL_Point<Kernel>
             */
            template<class PointIterator,
                    typename = movetk::utils::requires_random_access_iterator<PointIterator>,
                    typename = movetk::utils::requires_wrapper_point<Kernel,
                            typename PointIterator::value_type >>
            Wrapper_CGAL_Polygon(PointIterator first, PointIterator beyond) {
                //ASSERT_RANDOM_ACCESS_ITERATOR(PointIterator);
                //ASSERT_WRAPPER_POINT_TYPE(Kernel, first);
                PointIterator it = first;
                while (it != beyond) {
                    auto CoordinateIter = it->begin();
                    polygon.push_back(Point_2(*CoordinateIter, *(CoordinateIter + 1)));
                    it++;
                }
                if (polygon[0] == polygon[polygon.size() - 1])
                    polygon.erase(polygon.vertices_end() - 1);
            }

            /*!
             *
             * @return  A CGAL polygon in 2D
             */
            CGAL_Polygon &get() {
                return polygon;
            }

            /*!
             *
             * @return Iterator to the first vertex in a set of points
             */
            auto v_begin() {
                return polygon.vertices_begin();
            }

            /*!
             *
             * @return Iterator to the end of the last vertex in a set of points
             */
            auto v_end() {
                return polygon.vertices_end();
            }

            /*!
             *
             * @return Iterator to the first edge in a set of points
             */
            auto e_begin() {
                return polygon.edges_begin();
            }

            /*!
             * @return Iterator to the end of the last edge in a set of points
             */
            auto e_end() {
                return polygon.edges_end();
            }


        };

        /*!
             * Prints a polygon whose vertices are separated by comma. Since
             * each of these vertices are points, their coordinates are separated
             * by semicolon
             * @param out - OutputStream
             * @param poly - A polygon of type Wrapper_CGAL_Polygon<Kernel>
             */
        template <class Kernel>
        std::ostream& operator<<(std::ostream &out, Wrapper_CGAL_Polygon<Kernel> &poly) {
            OutputRep<is_valid_NT<typename Kernel::NT>::can_cast_to_string,
            Kernel,  Wrapper_CGAL_Polygon<Kernel> > output;
            return ( output(out, poly) );
        }

    template<class Kernel>
    class Wrapper_CGAL_Sphere {
    private:
        typedef typename Kernel::CGAL_Sphere_ CGAL_Sphere;
        typedef typename Kernel::CGAL_Point_ CGAL_Point;
        typedef typename Kernel::Wrapper_Point Wrapper_Point;
        typedef typename Kernel::Wrapper_Vector Wrapper_Vector;
        typedef typename Kernel::NT NT;
        std::set<CGAL_Point> points;
        CGAL_Sphere sphere;
        Wrapper_Point _center;
        NT _squared_radius;
    public:
        Wrapper_CGAL_Sphere(Wrapper_Point& center,
                NT radius, bool square = true):_center(center){
            if (square)
                _squared_radius = std::pow(radius,2);
            else
                _squared_radius = radius;
        }

        Wrapper_Point center() const{
            return _center;
        }

        NT squared_radius() const{
            return _squared_radius;
        }

    };

    template<class Kernel>
    std::ostream &operator<<(std::ostream &out, const Wrapper_CGAL_Sphere<Kernel> &sphere) {
        OutputRep<is_valid_NT<typename Kernel::NT>::can_cast_to_string,
                Kernel, Wrapper_CGAL_Sphere<Kernel> > output;
        return output(out, sphere);
    }

    namespace CGAL_Algorithms {

        /*!
         * @class Intersection_visitor
         * @brief A visitor class for intersection between two geometric objects
         * @details An intersection of two geometric objects can result in a geometric object that
         *  varies based on how two objects intersect. This class provides an interface for
         *  manipulating an object from different geometry types in a uniform manner
         *  For further details , please see: https://www.boost.org/doc/libs/1_64_0/doc/html/variant.html
         * @tparam Kernel - A traits class that defines CGAL geometry types and a variant type
         *  e.g: boost::variant
         */
        template<class Kernel>
        class Intersection_visitor : public boost::static_visitor<typename Kernel::IntersectionVariant> {
        private:
            typedef typename Kernel::CGAL_Point_ CGAL_Point;
            typedef typename Kernel::CGAL_Segment_ CGAL_Segment;
            typedef typename Kernel::CGAL_Line_ CGAL_Line;
        public:

            /*!
             *
             * @param p - A CGAL point
             * @return  A wrapped CGAL point
             */
            typename Kernel::IntersectionVariant operator()(const CGAL_Point &p) const {
                typename Kernel::Wrapper_Point wrapper_point(p);
                return wrapper_point;
            }

            /*!
             *
             * @param s - A CGAL segment
             * @return  A wrapped CGAL segment
             */
            typename Kernel::IntersectionVariant operator()(const CGAL_Segment &s) const {
                typename Kernel::Wrapper_Segment wrapper_seg(s);
                return wrapper_seg;
            }

            /*!
             *
             * @param l - A CGAL line
             * @return A wrapped CGAL line
             */
            typename Kernel::IntersectionVariant operator()(const CGAL_Line &l) const {
                typename Kernel::Wrapper_Line line(l);
                return line;
            }
        };

        /*!
         * Check whether two lines intersect
         * @tparam Kernel - A traits class that defines CGAL geometry types
         * @param Line1 - A wrapped CGAL line
         * @param Line2 - A wrapped CGAL line
         * @return True / False
         */
        template<class Kernel>
        bool do_intersect(Wrapper_CGAL_Line <Kernel> Line1, Wrapper_CGAL_Line <Kernel> Line2) {
            return CGAL::do_intersect(Line1.get(), Line2.get());
        }

        /*!
         * Get the result of intersection of two lines
         * @tparam Kernel - A traits class that defines CGAL geometry types
         * @param Line1 - A wrapped CGAL line
         * @param Line2 - A wrapped CGAL line
         * @return A CGAL geometric object
         */
        template<class Kernel>
        auto intersect(Wrapper_CGAL_Line <Kernel> Line1, Wrapper_CGAL_Line <Kernel> Line2) {
            return intersection(Line1.get(), Line2.get()); //intersection() is part of <CGAL/intersections_d.h>
        }

        /*!
         * Check whether a point belongs to a polygon
         * @tparam Kernel - A traits class that defines CGAL geometry types
         * @param Point - A wrapped CGAL point
         * @param Polygon - A wrapped CGAL polygon
         * @return True / False
         */
        template<class Kernel>
        bool point_in_polygon(Wrapper_CGAL_Point <Kernel> Point,
                              Wrapper_CGAL_Polygon <Kernel> Polygon) {
            typedef typename Kernel::Wrapper_CGAL_Kernel_2::GeometryType_2 K;
            typedef typename K::Point_2 Point_2;
            auto CoordinateIter = Point.begin();
            return ((Polygon.get()).bounded_side(
                    Point_2(*CoordinateIter, *(CoordinateIter + 1)))
                    == CGAL::ON_BOUNDED_SIDE);
        }

        /*! @class Wrapper_Min_Sphere
         *  @brief A wrapper for Minimum Enclosing Ball construction of CGAL
         * @tparam Kernel - A traits class that defines CGAL geometry types
         */
        template<class Kernel>
        class Wrapper_Min_Sphere {
        private:
            typedef typename Kernel::CGAL_MinSphere_ CGAL_MinSphere;
            typedef typename Kernel::CGAL_Point_ CGAL_Point;
            typedef typename Kernel::NT NT;
            std::set<CGAL_Point> points; //TODO: Remove the use of this datastructure
            NT radius;

            /*!
             * @details Takes a set of points (not necessarily unique) and creates a
             * unique set of points since it is required by the CGAL MEB
             * algorithm to converge
             * @tparam PointIterator - A random access iterator over a set of points
             * where each point is of type Wrapper_CGAL_Point<Kernel>
             * @param first - Iterator to the first point in a set of points
             * where each point is of type Wrapper_CGAL_Point<Kernel>
             * @param beyond - Iterator to the first point in a set of points
             * where each point is of type Wrapper_CGAL_Point<Kernel>
             */
            template<typename PointIterator,
                    typename = movetk::utils::requires_random_access_iterator<PointIterator>,
                    typename  = movetk::utils::requires_wrapper_point<Kernel,
                            typename PointIterator::value_type >>
            void to_CGAL_points(PointIterator first, PointIterator beyond) {
                //ASSERT_RANDOM_ACCESS_ITERATOR(PointIterator);
                // ASSERT_WRAPPER_POINT_TYPE(Kernel, first);
                PointIterator it = first;
                while (it != beyond) {
                    points.insert((*it).get());
                    it++;
                }
            }

        public:
            /*!
             * @brief constructs an MEB from a given set of points
             * @tparam PointIterator A random access iterator over a set of points
             * where each point is of type Wrapper_CGAL_Point<Kernel>
             * @tparam CenterIterator An output iterator that points to location
             * where a value has to be inserted
             * @param first - Iterator to the first point in a set of points
             * where each point is of type Wrapper_CGAL_Point<Kernel>
             * @param beyond - Iterator to the end of the last point in a set of points
             * where each point is of type Wrapper_CGAL_Point<Kernel>
             * @param iter - Iterator to the location where a value has to be inserted
             * @return Radius of the MEB
             */
            template<class PointIterator, class CenterIterator,
                    typename = movetk::utils::requires_random_access_iterator<PointIterator>,
                    typename = movetk::utils::requires_wrapper_point<Kernel,
                            typename PointIterator::value_type>,
                    typename = movetk::utils::requires_output_iterator<CenterIterator>,
                    typename = movetk::utils::requires_NT<Kernel,
                            typename CenterIterator::value_type> >
            NT operator()(PointIterator first, PointIterator beyond, CenterIterator iter) {
                //ASSERT_RANDOM_ACCESS_ITERATOR(PointIterator);
                //ASSERT_WRAPPER_POINT_TYPE(Kernel, first);
                //ASSERT_OUTPUT_ITERATOR(CenterIterator);
                this->to_CGAL_points(first, beyond);
                CGAL_MinSphere ms(begin(points), end(points));
                radius = ms.radius().first +
                         ms.radius().second * sqrt(ms.discriminant());
                for (typename CGAL_MinSphere::Cartesian_const_iterator cit =
                        ms.center_cartesian_begin(); cit != ms.center_cartesian_end();
                     cit++) {
                    *iter = cit->first + cit->second;
                }
                points.clear();
                return radius;
            }

            /*!
             * @brief constructs an MEB from a given set of points
             * @tparam PointIterator A random access iterator over a set of points
             * where each point is of type Wrapper_CGAL_Point<Kernel>
             * @param first - Iterator to the first point in a set of points
             * where each point is of type Wrapper_CGAL_Point<Kernel>
             * @param beyond - Iterator to the end of the last point in a set of points
             * where each point is of type Wrapper_CGAL_Point<Kernel>
             * @return Radius of the MEB
             */
            template<class PointIterator,
                    typename = movetk::utils::requires_random_access_iterator<PointIterator>,
                    typename = movetk::utils::requires_wrapper_point<Kernel,
                            typename PointIterator::value_type> >
            NT operator()(PointIterator first, PointIterator beyond) {
                //ASSERT_RANDOM_ACCESS_ITERATOR(PointIterator);
                //ASSERT_WRAPPER_POINT_TYPE(Kernel, first);
                this->to_CGAL_points(first, beyond);
                CGAL_MinSphere ms(begin(points), end(points));
                radius = ms.radius().first +
                         ms.radius().second * sqrt(ms.discriminant());
                points.clear();
                return radius;
            }
        };


        template<class Kernel>
        class Wrapper_Curve_Intersection {
        private:
            typedef typename Kernel::Wrapper_CGAL_Kernel_2::Point_2 Point_2;
            typedef typename Kernel::Wrapper_CGAL_Kernel_2::Curve_2 Curve;
            typedef Wrapper_CGAL_Point<Kernel> Wrapper_Point;
            typedef Wrapper_CGAL_Vector<Kernel> Wrapper_Vector;
            std::vector<Curve> Curves;
            std::list<Point_2> Points;
        public:
            template <class InputIterator>
            std::size_t operator()(InputIterator first, InputIterator beyond){
                InputIterator it = first + 1;
                Curves.clear();
                Points.clear();
                while (it != beyond){
                    Wrapper_Point p1 = *(it - 1);
                    Wrapper_Point p2 = *it;
                    Wrapper_Vector v = p2 - p1;
                    if ((v * v) < MOVETK_EPS) {
                        it++;
                        continue;
                    }
                    auto CoordinateIterP1 = p1.begin();
                    auto CoordinateIterP2 = p2.begin();
                    Curves.push_back(Curve(Point_2(*CoordinateIterP1, *(CoordinateIterP1 + 1)),
                                           Point_2(*CoordinateIterP2, *(CoordinateIterP2 + 1))
                                     )
                    );
                    it++;
                }
                CGAL::compute_intersection_points(std::begin(Curves),
                        std::end(Curves),std::back_inserter(Points));
                return Points.size();
            }
        };
    };

    /*!@struct Wrapper_CGAL_Kernel
     * @brief A traits class that defines CGAL geometry types and its wrappers
     * @tparam Kernel - The CGAL Kernel
     */
    template<class CGAL_Kernel>
    struct Wrapper_CGAL_Kernel {
        typedef typename CGAL_Kernel::NT NT;
        constexpr static size_t dim = CGAL_Kernel::dim;
        typedef typename CGAL_Kernel::SphereTraits SphereTraits;
        typedef typename CGAL::Min_sphere_of_spheres_d<SphereTraits> CGAL_MinSphere_;
        typedef typename CGAL_Kernel::GeometryType GeometryType;
        typedef typename GeometryType::Point_d CGAL_Point_;
        typedef typename GeometryType::Vector_d CGAL_Vector_;
        typedef typename GeometryType::Line_d CGAL_Line_;
        typedef typename GeometryType::Segment_d CGAL_Segment_;
        typedef typename GeometryType::Sphere_d CGAL_Sphere_;
        typedef Wrapper_CGAL_Point <Wrapper_CGAL_Kernel> Wrapper_Point;
        typedef Wrapper_CGAL_Line <Wrapper_CGAL_Kernel> Wrapper_Line;
        typedef Wrapper_CGAL_Segment <Wrapper_CGAL_Kernel> Wrapper_Segment;
        typedef Wrapper_CGAL_Vector <Wrapper_CGAL_Kernel> Wrapper_Vector;
        typedef Wrapper_CGAL_Polygon <Wrapper_CGAL_Kernel> Wrapper_Polygon;
        typedef Wrapper_CGAL_Sphere <Wrapper_CGAL_Kernel> Wrapper_Sphere;
        typedef CGAL_Algorithms::Wrapper_Min_Sphere<Wrapper_CGAL_Kernel> Wrapper_MinSphere;
        typedef boost::variant<Wrapper_Point, Wrapper_Line, Wrapper_Segment> IntersectionVariant;
        typedef CGAL_Algorithms::Intersection_visitor<Wrapper_CGAL_Kernel> Intersection_visitor;
        typedef CGAL_Algorithms::Wrapper_Curve_Intersection<Wrapper_CGAL_Kernel> Wrapper_Curve_Intersection;
        typedef void Wrapper_Squared_Distance;
        typedef void Wrapper_Discrete_Hausdorff_Distance;
        typedef void Wrapper_Discrete_Frechet_Distance;
        typedef void Wrapper_Minimum_Bounding_Rectangle;
        struct Wrapper_CGAL_Kernel_2 {
            typedef CGAL::Simple_cartesian<NT> GeometryType_2;
            typedef CGAL::Arr_segment_traits_2<GeometryType_2> Arrangement_Traits_2;
            typedef typename GeometryType_2::Point_2 Point_2;
            typedef typename GeometryType_2::Vector_2 Vector_2;
            typedef typename GeometryType_2::Line_2 Line_2;
            typedef typename GeometryType_2::Segment_2 Segment_2;
            typedef typename Arrangement_Traits_2::Curve_2 Curve_2;
        };
    };



} //namespace TSL

#endif /* GEOMETRY_H */

