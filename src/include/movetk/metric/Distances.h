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
#include "movetk/utils/Asserts.h"
#include "movetk/geom/GeometryInterface.h"
#include <boost/iterator/transform_iterator.hpp>

namespace movetk_support
{

    // based on Eiter, T., & Mannila, H. (1994). Computing Discrete Fréchet Distance ∗.

    template<class Kernel, class _Norm>
    struct squared_distance_d {
        using Norm = _Norm;
        using MovetkPoint = typename Kernel::MovetkPoint;

        typename Kernel::NT operator()(const MovetkPoint&p,
            const typename Kernel::MovetkLine &l) const
        {
            Norm n;
            typename Kernel::MovetkVector v = l[1] - l[0];
            typename Kernel::MovetkVector u = p - l[0];
            typename Kernel::NT uv = u * v;
            n(v);
            typename Kernel::NT vv = n ^ 2;
            v *= (uv / vv);
            typename Kernel::MovetkPoint Pb = l[0] + v;
            typename Kernel::MovetkVector v2 = p - Pb;
            return n(v2);
        }

        typename Kernel::NT operator()(const MovetkPoint& p,
            const typename Kernel::MovetkSegment &s) const
        {
            Norm n;
            typename Kernel::MovetkVector v = s[1] - s[0];
            typename Kernel::MovetkVector u = p - s[0];
            typename Kernel::MovetkVector v1 = p - s[1];
            typename Kernel::NT uv = u * v;
            if (uv <= 0)
            {
                return n(u);
            }
            n(v);
            typename Kernel::NT vv = n ^ Norm::P;
            if (vv <= uv)
            {
                return n(v1);
            }
            v *= (uv / vv);
            typename Kernel::MovetkPoint Pb = s[0] + v;
            typename Kernel::MovetkVector v2 = p - Pb;
            return n(v2);
        }

        typename Kernel::NT operator()(const MovetkPoint&p1,
            const MovetkPoint& p2) const
        {
            Norm n;
            typename Kernel::MovetkVector v = p2 - p1;
            return n(v);
        }
    };

    template <class Kernel, class Norm>
    class Discrete_Hausdorff
    {
    private:
        template <class InputIterator,
                  typename = movetk_core::requires_random_access_iterator<InputIterator>,
                  typename = movetk_core::requires_movetk_point<Kernel,
                                                                typename InputIterator::value_type>>
        typename Kernel::NT discrete_husdorff(InputIterator a_first, InputIterator a_beyond,
                                              InputIterator b_first, InputIterator b_beyond)
        {

            Norm norm;
            InputIterator it_a = a_first;
            typename Kernel::NT max_dist = 0;
            while (it_a != a_beyond)
            {
                InputIterator it_b = b_first;
                typename Kernel::MovetkVector v = *it_b - *it_a;
                typename Kernel::NT distance = norm(v);
                typename Kernel::NT min_dist = distance;
                it_b++;
                while (it_b != b_beyond)
                {
                    v = *it_b - *it_a;
                    distance = norm(v);
                    if (distance < min_dist)
                    {
                        min_dist = distance;
                    }
                    it_b++;
                }
                if (min_dist > max_dist)
                {
                    max_dist = min_dist;
                }
                it_a++;
            }

            typename Kernel::NT n = 1 / static_cast<typename Kernel::NT>(Norm::P);
            return std::pow(max_dist, n);
        }

    public:
        template <class InputIterator,
                  typename = movetk_core::requires_random_access_iterator<InputIterator>,
                  typename = movetk_core::requires_movetk_point<Kernel,
                                                                typename InputIterator::value_type>>
        typename Kernel::NT operator()(InputIterator polyline_a_first, InputIterator polyline_a_beyond,
                                       InputIterator polyline_b_first, InputIterator polyline_b_beyond)
        {

            typename Kernel::NT hd_pq = discrete_husdorff(polyline_a_first, polyline_a_beyond,
                                                          polyline_b_first, polyline_b_beyond);

            typename Kernel::NT hd_qp = discrete_husdorff(polyline_b_first, polyline_b_beyond,
                                                          polyline_a_first, polyline_a_beyond);

            return std::max(hd_pq, hd_qp);
        }
    };

    template <class Kernel, class Norm>
    class Discrete_Frechet
    {
    private:
        typedef typename Kernel::NT NT;

        template <class InputIterator,
                  typename = movetk_core::requires_random_access_iterator<InputIterator>,
                  typename = movetk_core::requires_movetk_point<Kernel,
                                                                typename InputIterator::value_type>>
        NT distance(InputIterator iter_a, InputIterator iter_b)
        {
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
                      InputIterator polyline_b_first, InputIterator polyline_b_beyond)
        {

            std::size_t size_polyline_b = std::distance(polyline_b_first, polyline_b_beyond);
            std::vector<NT> dp_row(size_polyline_b);
            std::fill(std::begin(dp_row), std::begin(dp_row) + size_polyline_b, -1);
            InputIterator it_a = polyline_a_first;
            while (it_a != polyline_a_beyond)
            {
                InputIterator it_b = polyline_b_first;
                std::size_t j = 0;
                typename Kernel::NT previous = -1, current = -1;
                while (it_b != polyline_b_beyond)
                {
                    if ((it_a == polyline_a_first) && (it_b == polyline_b_first))
                    {
                        current = distance(it_a, it_b);
                        dp_row[0] = current;
                    }

                    else if ((it_a != polyline_a_first) && (it_b == polyline_b_first))
                    {
                        current = std::max(dp_row[0], distance(it_a, it_b));
                        dp_row[0] = current;
                    }
                    else if ((it_a == polyline_a_first) && (it_b != polyline_b_first))
                    {
                        current = std::max(previous, distance(it_a, it_b));
                        dp_row[j - 1] = previous;
                    }
                    else
                    {
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
            NT n = 1 / static_cast<NT>(Norm::P);
            return  std::pow(dfd,n);
        }

    };


    /**
     * \brief Strong Frechet distance between polylines
     * Implementation of Alt & Godau in combination with either a double-and-search approach or a 
     * bisection (binary search) approach. For the latter, a decent upperbound is needed to determine
     * the search range.
     * See: http://www.staff.science.uu.nl/~kreve101/asci/ag-cfdbt-95.pdf
     * \tparam Kernel The geometry kernel
     * \tparam SqDistance Square distance type for compting segment-point distance.
     */
    template<typename Kernel, typename SqDistance>
    class StrongFrechet
    {
        // Typedefs
        using NT = typename Kernel::NT;
        using Point = typename Kernel::MovetkPoint;

        // The norm to use
        SqDistance m_sqDistance;

        // Upperbound on the allowed Frechet distance
        NT m_upperBound = std::numeric_limits<NT>::max();

        // Precision on the output strong Frechet distance
        NT m_precision = 1e-5;

        /**
         * \brief Polynomial for the freespace cell boundary.
         */
        struct Polynomial
        {
            // Parallel-perpendicular decomposition of distance between point and segment
            NT parallelDistance;
            NT perpendicularDistance;
            // Smallest epsilon needed to make the ball centered at the point touch the segment
            NT minimumEpsilon;
            // Type of relation between point and segment: 
            // 'i' : parallel projection of the point lies on the segment
            // 'a' : parallel projection of the point lies above the segment (relative to the segment direction)
            // 'b' : parallel projection of the point lies below the segment (relative to the segment direction)
            char type;

            // Non-normalized reachable range
            std::pair<NT, NT> range(NT epsilon) const
            {
                auto sq = [](auto el) {return el * el; };
                if (epsilon < minimumEpsilon) return std::make_pair<NT, NT>(std::numeric_limits<NT>::max(), std::numeric_limits<NT>::lowest());
                return std::make_pair(
                    type == 'b' ? 0.0 : parallelDistance - std::sqrt(sq(epsilon) - sq(perpendicularDistance)),
                    type == 'a' ? parallelDistance : parallelDistance + std::sqrt(sq(epsilon) - sq(perpendicularDistance))
                );
            }

            void compute(const Point& point, const Point& seg0, const Point& seg1)
            {
                typename SqDistance::Norm norm;
                auto dir = seg1 - seg0;
                auto segLen = std::sqrt(norm(dir));
                auto pntLen = std::sqrt(norm(point - seg0));
                // Projected value on line through segment,
                // with origin at start of segment
                parallelDistance = (point - seg0)*dir / segLen;
                perpendicularDistance = std::sqrt(pntLen*pntLen - parallelDistance*parallelDistance);
                auto seg = movetk_core::MakeSegment<Kernel>()(seg0, seg1);
                minimumEpsilon = std::sqrt(SqDistance()(point, seg));
                type = 'i';
                if (parallelDistance > segLen) type = 'a';
                else if (parallelDistance < 0) type = 'b';
            }
        };

        struct CellPolynomials
        {
            // Cell boundary polynomials for left (0) and bottom (1) boundaries.
            Polynomial polys[2];
        };

        /**
         * \brief Precomputes the polynomials describing the freespace cell boundaries.
         * \tparam PointIt Type of the input point iterators for the polylines
         * \param polyA First polyline, specified as pair of start and end iterator of points
         * \param polyB Second polyline, specified as pair of start and end iterator of points
         * \param polynomials Output table of polynomials.
         */
        template<typename PointIt>
        void precomputePolynomials(const std::pair<PointIt, PointIt>& polyA, const std::pair<PointIt, PointIt>& polyB, std::vector<std::vector<CellPolynomials>>& polynomials) const
        {
            // We don't save the boundaries at the top/right of the freespacediagram, since they are not need:
            // by convexity, if a path uses the boundary, then the left/bottom boundaries should have atleast one 
            // point of free space.
            const auto polyASize = std::distance(polyA.first, polyA.second);
            const auto polyBSize = std::distance(polyB.first, polyB.second);
            polynomials.resize(polyASize - 1, {});
            for (auto& el : polynomials)
            {
                el.resize(polyBSize - 1, CellPolynomials{});
            }
            std::size_t i = 0;
            for (auto pointA = polyA.first; pointA != std::prev(polyA.second); ++pointA, ++i)
            {
                std::size_t j = 0;
                for (auto pointB = polyB.first; pointB != std::prev(polyB.second); ++pointB, ++j)
                {
                    // Compute bottom boundary polynomial
                    polynomials[i][j].polys[0].compute(*pointA, *pointB, *(std::next(pointB)));
                    // Compute left boundary polynomial
                    polynomials[i][j].polys[1].compute(*pointB, *pointA, *std::next(pointA));
                }
            }
        }

        /**
         * \brief Given the freespace diagram and an epsilon, decide if the strong Frechet distance
         * is at most epsilon.
         * It is assumed that the given epsilon is larger than or equal to the smallest distance
         * of the endpoints of the polylines.
         * \param polynomials Freespace diagram, given as a table of polynomials
         * \param epsilon The maximum allowed Frechet distance
         * \return Whether or not the polylines are within Frechet distance epsilon
         */
        bool decide(const std::vector<std::vector<CellPolynomials>>& polynomials, NT epsilon) const
        {
            const auto maxI = polynomials.size();
            const auto maxJ = polynomials[0].size();

            const std::size_t sizes[2] = { maxI, maxJ };

            struct Interval
            {
                NT min = std::numeric_limits<NT>::max();
                NT max = std::numeric_limits<NT>::lowest();
                Interval(){}
                Interval(NT min, NT max):min(min),max(max){}
                bool isEmpty() const
                {
                    return max < min;
                }
                void assignMaxToMin(const Interval& other)
                {
                    if (isEmpty()) return;
                    min = std::max(min, other.min);
                }
            };
            struct CellIntervals
            {
                Interval intervals[2] = { {},{} };
                bool isReachable() const
                {
                    // Reachable if one of the intervals is not empty
                    return !intervals[0].isEmpty() || !intervals[1].isEmpty();
                }
                Interval& operator[](int i)
                {
                    return intervals[i];
                }
                const Interval& operator[](int i) const
                {
                    return intervals[i];
                }
            };
            // Save two rows/columns while computing the decision problem
            std::vector<CellIntervals> progress[2];
            // Which of the two saved rows/columns to fill. Either 0 or 1
            int current = 0;

            // Dimension: 0 = row direction, 1 is column direction
            // Cell intervals in dimesion 0 are the bottom intervals, otherwise the left intervals.
            // Pick the smallest dimension for storing the intermediate state.
            // We then iterate over the other dimension
            const int dim = maxI > maxJ ? 1 : 0;
            // The other dimension, where we will iterate over
            const int secondaryDim = 1 - dim;

            // Get the freespace interval for a cell boundary at the given dimension.
            auto getFreeSpace = [dim,&polynomials,epsilon](int primaryDimIndex, int secondaryDimIndex, int targetDim)
            {
                const auto r = dim == 0 ? primaryDimIndex : secondaryDimIndex;
                const auto c = dim == 0 ? secondaryDimIndex : primaryDimIndex;
                auto res = polynomials[r][c].polys[targetDim].range(epsilon);
                return Interval{ res.first, res.second };
            };

            // Initialize first row(col), depending on chosen dimension to compute over.
            progress[current].assign(sizes[dim], {});
            progress[current][0].intervals[secondaryDim] = Interval(std::numeric_limits<NT>::lowest(), std::numeric_limits<NT>::max()); // Fully open interval
            progress[current][0].intervals[dim] = Interval(std::numeric_limits<NT>::lowest(), std::numeric_limits<NT>::max()); // Fully open interval
            for(auto i = 1; i < sizes[dim]; ++i)
            {
                if(!progress[current][i-1].intervals[dim].isEmpty())
                {
                    progress[current][i].intervals[dim] = getFreeSpace(i, 0, dim);
                    progress[current][i].intervals[dim].assignMaxToMin(progress[current][i-1].intervals[dim]);
                }
            }
            // Go over all other rows(columns).
            for(auto j = 1; j < sizes[secondaryDim]; ++j)
            {
                // Fill other
                const auto prev = current;
                current = 1 - current;

                // Reset intervals to empty
                progress[current].assign(sizes[dim], {});

                // Compute first cell
                auto& firstCell = progress[current][0];
                const auto& prevFirstCell = progress[prev][0];
                if(!prevFirstCell[secondaryDim].isEmpty())
                {
                    firstCell[secondaryDim] = getFreeSpace(0, j, secondaryDim);
                    // Assign maximum of the minimum coordinates to the minimum
                    firstCell[secondaryDim].assignMaxToMin(prevFirstCell[secondaryDim]);
                }

                // Compute the intervals for the other cells
                const auto& prevCells = progress[prev];
                const auto& currCells = progress[current];
                bool hasReachable = firstCell.isReachable();
                for(auto i = 1; i < sizes[dim]; ++i)
                {
                    auto& currCell = progress[current][i];
                    // Compute secondary dimension element
                    if(prevCells[i].isReachable())
                    {
                        currCell[secondaryDim] = getFreeSpace(i, j, secondaryDim);
                        if (prevCells[i][dim].isEmpty() && !prevCells[i][secondaryDim].isEmpty())
                        {
                            currCell[secondaryDim].assignMaxToMin(prevCells[i][secondaryDim]);
                        }
                    }
                    if(currCells[i-1].isReachable())
                    {
                        currCell[dim] = getFreeSpace(i, j, dim);
                        // Compute primary dimension element
                        if (currCells[i - 1][secondaryDim].isEmpty() && !currCells[i - 1][dim].isEmpty())
                        {
                            currCell[dim].assignMaxToMin(currCells[i - 1][dim]);
                        }
                    }
                    
                    hasReachable = currCell.isReachable() || hasReachable;
                }
                // Early out
                if (!hasReachable) {
                    return false;
                }
            }
            
            return progress[current].back().isReachable();
        }

        /**
         * \brief Search the epsilon such that the strong Frechet distance is epsilon, within the predefined tolerance. The given
         * lower and upper bound give the range between which it is known that the value of epsilon should lie.
         * \param polynomials The freespace diagram, described as a table of polynomials describing the cell boundary freespace
         * \param tolerance The tolerance to use for determining the epsilon
         * \param lower The lower bound on the range to search epsilon in
         * \param upper The upper bound on the range to search epsilon in
         * \param outDist The output epsilon value
         * \return Whether or not the epsilon was found in the given range.
         */
        bool bisectionSearchInInterval(const std::vector<std::vector<CellPolynomials>>& polynomials, NT tolerance, NT lower, NT upper, NT& outDist) const
        {
            // Upper should be valid, otherwise we are searching in an infeasible interval.
            if (!decide(polynomials, upper)) return false;

            NT lBound = lower, rBound = upper;
            NT currentBest = upper;
            while (true)
            {
                if (std::abs(lBound - rBound) < tolerance) break;
                // New value to test
                const NT curr = (lBound + rBound) * 0.5;
                if (decide(polynomials, curr))
                {
                    rBound = curr;
                    currentBest = curr;
                }
                else
                {
                    lBound = curr;
                }
            }
            outDist = currentBest;

            return true;
        }

        template<typename PointIt>
        bool bisectionSearchUpperBounded(const std::pair<PointIt, PointIt>& polyA, const std::pair<PointIt, PointIt>& polyB, NT& outDist) const
        {
            // Polyline sizes (number of points)
            const auto polyASize = std::distance(polyA.first, polyA.second);
            const auto polyBSize = std::distance(polyB.first, polyB.second);

            // Minimum required epsilon to make start and end match for polylines.
            const NT minEps = std::sqrt(std::max(m_sqDistance(*polyA.first, *polyB.first), m_sqDistance(*(polyA.first + polyASize - 1), *(polyB.first + polyBSize - 1))));

            if (minEps > m_upperBound)
            {
                return false;
            }

            std::vector<std::vector<CellPolynomials>> polynomials;
            precomputePolynomials(polyA, polyB, polynomials);


            // If the endpoint epsilon is the smallest, within some fraction, we are ok with selecting that
            if (decide(polynomials, minEps + m_precision))
            {
                outDist = minEps;
                return true;
            }
            return bisectionSearchInInterval(polynomials, m_precision, minEps, m_upperBound, outDist);
        }
        
        template<typename PointIt>
        bool doubleAndSearch(const std::pair<PointIt, PointIt>& polyA, const std::pair<PointIt, PointIt>& polyB, NT& outDist)
        {
            // Polyline sizes (number of points)
            const auto polyASize = std::distance(polyA.first, polyA.second);
            const auto polyBSize = std::distance(polyB.first, polyB.second);
            // Minimum required epsilon to make start and end match for polylines.
            NT minEps = std::sqrt(std::max(m_sqDistance(*polyA.first, *polyB.first), m_sqDistance(*std::prev(polyA.second), *std::prev(polyB.second))));

            if (minEps > m_upperBound)
            {
                return false;
            }

            std::vector<std::vector<CellPolynomials>> polynomials;
            precomputePolynomials(polyA, polyB, polynomials);

            // If the endpoint epsilon is the smallest, within some fraction, we are ok with selecting that
            if (decide(polynomials, minEps + m_precision))
            {
                outDist = minEps + m_precision;
                return true;
            }
            if (minEps < m_precision)
            {
                minEps = m_precision;
            }

            // Double the epsilon at each step
            NT currEps = minEps * 2.0;
            while (true)
            {
                // Should happen at some point unless the input is extremely malformed
                if (decide(polynomials, currEps))
                {
                    return bisectionSearchInInterval(polynomials, m_precision, currEps * 0.5, currEps, outDist);
                }
                currEps *= 2.0;
            }
        }

    public:
        enum class Mode
        {
            BisectionSearch,
            DoubleAndSearch
        };
    private:
        Mode m_mode;
    public:
        StrongFrechet(Mode mode = Mode::DoubleAndSearch) : m_mode(mode) {}

        /**
         * \brief Returns the computation mode for the strong Frechet computation
         * \return The computation mode
         */
        Mode mode() const
        {
            return m_mode;
        }

        /**
         * \brief Set the computation mode for the strong Frechet computation
         * \param mode The computation mode
         */
        void setMode(Mode mode)
        {
            m_mode = mode;
        }

        /**
         * \brief Set upperbound for upperbounded search approaches
         * \param upperBound The upperbound on the Frechet distance
         */
        void setUpperbound(NT upperBound)
        {
            m_upperBound = upperBound;
        }

        /**
         * \brief Given two polylines, decide if the strong Frechet distance is at most epsilon.
         * \param polynomials Freespace diagram, given as a table of polynomials
         * \param epsilon The maximum allowed Frechet distance
         * \return Whether or not the polylines are within Frechet distance epsilon
         */
        template <class InputIterator,
            typename = movetk_core::requires_random_access_iterator<InputIterator>,
            typename = movetk_core::requires_movetk_point<Kernel,
            typename InputIterator::value_type>>
        bool decide(InputIterator poly_a, InputIterator poly_a_beyond, 
            InputIterator poly_b, InputIterator poly_b_beyond, NT epsilon) const
        {
            // Polyline sizes (number of points)
            const auto polyASize = std::distance(poly_a, poly_a_beyond);
            const auto polyBSize = std::distance(poly_b, poly_b_beyond);

            // Or error
            if (polyASize == 0 || polyBSize == 0) return false;

            if(polyASize == 1)
            {
                std::function<NT(const typename Kernel::MovetkPoint&)> transform = [poly_a](const auto& polyBEl)
                {
                    SqDistance sqDist;
                    return std::sqrt(sqDist(*poly_a, polyBEl));
                };
                auto maxElIt = std::max_element(boost::make_transform_iterator(poly_b, transform), boost::make_transform_iterator(poly_b_beyond, transform));
                return *maxElIt <= epsilon + m_precision;
            }
            if (polyBSize == 1)
            {
                std::function<NT(const typename Kernel::MovetkPoint&)> transform = [poly_b](const auto& polyAEl)
                {
                    SqDistance sqDist;
                    return std::sqrt(sqDist(*poly_b, polyAEl));
                };
                auto maxElIt = std::max_element(boost::make_transform_iterator(poly_a, transform), boost::make_transform_iterator(poly_a_beyond, transform));
                return *maxElIt <= epsilon + m_precision;
            }
            
            // Minimum required epsilon to make start and end match for polylines.
            NT minEps = std::max(std::sqrt(m_sqDistance(*poly_a, *poly_b)), std::sqrt(m_sqDistance(*std::prev(poly_a_beyond), *std::prev(poly_b_beyond))));

            if(polyASize == 2 && polyBSize == 2)
            {
                return minEps <= epsilon + m_precision;
            }

            if (minEps > m_upperBound + m_precision || minEps > epsilon + m_precision)
            {
                return false;
            }

            std::vector<std::vector<CellPolynomials>> polynomials;
            precomputePolynomials(std::make_pair(poly_a, poly_a_beyond), std::make_pair(poly_b, poly_b_beyond), polynomials);

            // If the endpoint epsilon is the smallest, within some fraction, we are ok with selecting that
            return decide(polynomials, epsilon);
        }

        /**
         * \brief Set the error tolerance in Frechet distance for the inexact methods.
         * \param tolerance The tolerance
         */
        void setTolerance(NT tolerance)
        {
            m_precision = tolerance;
        }
        NT tolerance() const
        {
            return m_precision;
        }

        template <class InputIterator,
            typename = movetk_core::requires_random_access_iterator<InputIterator>,
            typename = movetk_core::requires_movetk_point<Kernel,
            typename InputIterator::value_type>>
            bool operator()(InputIterator poly_a, InputIterator poly_a_beyond,
                InputIterator poly_b, InputIterator poly_b_beyond, typename Kernel::NT& output)
        {
            auto polyA = std::make_pair(poly_a, poly_a_beyond);
            auto polyB = std::make_pair(poly_b, poly_b_beyond);

            // Polyline sizes (number of points)
            const auto polyASize = std::distance(poly_a, poly_a_beyond);
            const auto polyBSize = std::distance(poly_b, poly_b_beyond);

            // Or error
            if (polyASize == 0 || polyBSize == 0) return false;

            if (polyASize == 1)
            {
                auto sqDist = m_sqDistance;
                // Use std::function, otherwise boost will error when trying to copy the transform_iterator
                std::function<NT(const typename Kernel::MovetkPoint&)> transform = [poly_a](const auto& polyBEl)
                {
                    SqDistance sqDist;
                    return std::sqrt(sqDist(*poly_a, polyBEl));
                };
                auto maxElIt = std::max_element(boost::make_transform_iterator(poly_b, transform), boost::make_transform_iterator(poly_b_beyond, transform));
                output = *maxElIt;
                return output <= m_upperBound + m_precision;
            }
            if (polyBSize == 1)
            {
                // Use std::function, otherwise boost will error when trying to copy the transform_iterator
                std::function<NT(const typename Kernel::MovetkPoint&)> transform = [poly_b](const auto& polyAEl)
                {
                    SqDistance sqDist;
                    return std::sqrt(sqDist(*poly_b, polyAEl));
                };
                auto maxElIt = std::max_element(boost::make_transform_iterator(poly_a, transform), boost::make_transform_iterator(poly_a_beyond, transform));
                output = *maxElIt;
                return output <= m_upperBound + m_precision;
            }
            
            switch (m_mode)
            {
            case Mode::BisectionSearch: return bisectionSearchUpperBounded(polyA, polyB, output);
            case Mode::DoubleAndSearch: return doubleAndSearch(polyA, polyB, output);
            default:return false;
            }
        }
        template <class InputIterator,
            typename = movetk_core::requires_random_access_iterator<InputIterator>,
            typename = movetk_core::requires_movetk_point<Kernel,
            typename InputIterator::value_type>>
        typename Kernel::NT operator()(InputIterator poly_a, InputIterator poly_a_beyond,
                InputIterator poly_b, InputIterator poly_b_beyond)
        {
            typename Kernel::NT epsilon = -1;
            (void)this->operator()(poly_a, poly_a_beyond, poly_b, poly_b_beyond, epsilon);
            return epsilon;
        }
    };


    template<typename Kernel, typename Norm>
    class StrongFrechetDistance
    {
        // Typedefs
        using NT = typename Kernel::NT;
        using Point = typename Kernel::MovetkPoint;

        // The norm to use
        Norm m_norm;

        // Upperbound on the allowed Frechet distance
        NT m_upperBound = std::numeric_limits<NT>::max();

        // Precision on the output strong Frechet distance
        NT m_precision = 1.0;

        struct Polynomial
        {
            // Parallel-perpendicular decomposition of distance between point and segment
            NT parallelDistance;
            NT perpendicularDistance;
            // Smallest epsilon needed to make the ball centered at the point touch the segment
            NT minimumEpsilon;
            // Type of relation between point and segment: 
            // 'i' : parallel projection of the point lies on the segment
            // 'a' : parallel projection of the point lies above the segment (relative to the segment direction)
            // 'b' : parallel projection of the point lies below the segment (relative to the segment direction)
            char type;

            std::pair<NT, NT> range(NT epsilon) const
            {
                if (epsilon < minimumEpsilon) return std::make_pair<NT, NT>(std::numeric_limits<NT>::max(), std::numeric_limits<NT>::lowest());
                return std::make_pair(
                    type == 'b' ? parallelDistance : parallelDistance - std::sqrt(sq(epsilon) - sq(perpendicularDistance)),
                    type == 'a' ? parallelDistance : parallelDistance + std::sqrt(sq(epsilon) - sq(perpendicularDistance))
                );
            }

            void compute(const Point& point, const Point& seg0, const Point& seg1)
            {
                Norm norm;
                auto dir = seg1 - seg0;
                auto segLen = norm(dir) ^ 1;
                // Projected value on line through segment,
                // with origin at start of segment
                parallelDistance = (point - seg0)*dir / segLen;
                perpendicularDistance = norm((point - seg0) - parallelDistance) ^ 1;
                minimumEpsilon = smallestDist(seg0, seg1, point);
                type = 'i';
                if (parallelDistance > segLen) type = 'a';
                else if (parallelDistance < 0) type = 'b';
            }
            bool compareLowerToUpper(const Polynomial& other, NT epsilon) const
            {
                auto sq = [](auto el) {return el * el; };
                const NT ownLower = type == 'b' ? parallelDistance : parallelDistance - std::sqrt(sq(epsilon) - sq(perpendicularDistance));
                const NT otherUpper = other.type == 'a' ? other.parallelDistance : other.parallelDistance + std::sqrt(sq(epsilon) - sq(other.perpendicularDistance));
                return ownLower < otherUpper;
            }
            bool compareLowerToLower(const Polynomial& other, NT epsilon) const
            {
                auto sq = [](auto el) {return el * el; };
                const NT ownLower = type == 'b' ? parallelDistance : parallelDistance - std::sqrt(sq(epsilon) - sq(perpendicularDistance));
                const NT otherLower = other.type == 'b' ? other.parallelDistance : other.parallelDistance - std::sqrt(sq(epsilon) - sq(other.perpendicularDistance));
                return ownLower < otherLower;
            }
            // Needed for c) type critical values
            NT findPassageOpeningEpsilon(const Polynomial& other, NT seglength) const
            {
                // These cases are actually a) and b) critical values
                if (type == 'b' || other.type == 'a') return std::max(minimumEpsilon, other.minimumEpsilon);

                auto sq = [](auto el) {return el * el; };
                auto a = sq(perpendicularDistance);
                auto b = sq(other.perpendicularDistance);
                auto c = parallelDistance + (other.type == 'c' ? seglength : other.parallelDistance);

                NT solution = 0.0;
                {
                    const auto diff = a - b;
                    const auto sum = a + b;
                    const auto cSq = sq(c * c);
                    solution = std::abs(std::sqrt(diff*diff + 2.0 * sum * cSq + cSq * cSq) / (2.0 * c));
                }
                return solution;
                //return std::max({solution, minimumEpsilon, other.minimumEpsilon});
            }
        };

        struct CellPolynomials
        {
            Polynomial L;
            Polynomial B;
        };

        /**
         * \brief Precomputes the polynomials describing the freespace cell boundaries.
         * \tparam PointIt Type of the input point iterators for the polylines
         * \param polyA First polyline, specified as pair of start and end iterator of points
         * \param polyB Second polyline, specified as pair of start and end iterator of points
         * \param polynomials Output table of polynomials.
         */
        template<typename PointIt>
        void precomputePolynomials(const std::pair<PointIt, PointIt>& polyA, const std::pair<PointIt, PointIt>& polyB, std::vector<std::vector<CellPolynomials>>& polynomials)
        {
            // We don't save the boundaries at the top/right of the freespacediagram, since they are not need:
            // by convexity, if a path uses the boundary, then the left/bottom boundaries should have atleast one 
            // point of free space.
            const auto polyASize = std::distance(polyA.first, polyA.second);
            const auto polyBSize = std::distance(polyB.first, polyB.second);
            polynomials.resize(polyASize - 1, {});
            for (auto& el : polynomials)
            {
                el.resize(polyBSize - 1, CellPolynomials{});
            }
            std::size_t i = 0;
            for (auto pointA = polyA.first; pointA != std::prev(polyA.second); ++pointA, ++i)
            {
                std::size_t j = 0;
                for (auto pointB = polyB.first; pointB != std::prev(polyB.second); ++pointB, ++j)
                {
                    // Compute left boundary polynomial
                    polynomials[i][j].L.compute(*pointA, *pointB, *(std::next(pointB)));
                    // Compute bottom boundary polynomial
                    polynomials[i][j].B.compute(*pointB, *pointA, *std::next(pointA));
                }
            }
        }

        /**
         * \brief Given the freespace diagram and an epsilon, decide if the strong Frechet distance
         * is at most epsilon.
         * It is assumed that the given epsilon is larger than or equal to the smallest distance
         * of the endpoints of the polylines.
         * \param polynomials Freespace diagram, given as a table of polynomials
         * \param epsilon The maximum allowed Frechet distance
         * \return Whether or not the polylines are within Frechet distance epsilon
         */
        bool decide(const std::vector<std::vector<CellPolynomials>>& polynomials, NT eps) const
        {
            const auto maxI = polynomials.size();
            const auto maxJ = polynomials[0].size();

            using Interval = std::pair<NT, NT>;
            // Mark intervals empty when they have incorrect order.
            auto isEmpty = [](const Interval& inter) {return inter.second < inter.first; };

            const NT negInf = std::numeric_limits<NT>::lowest();
            const NT posInf = std::numeric_limits<NT>::max();
            auto emptyInterval = std::make_pair(posInf, negInf);
            std::vector<Interval> L_col[2], B_col[2], L_row[2], B_row[2];
            // Index of vector to fill
            int current = 0;
            L_col[current].resize(maxJ, emptyInterval);
            B_col[current].resize(maxJ, emptyInterval);
            L_row[current].resize(maxI, emptyInterval);
            B_row[current].resize(maxI, emptyInterval);

            // Incrementally construct per row/col 
            B_col[current][1] = polynomials[0][1].B.range(eps);
            for (auto i = 2; i < maxI; ++i) {
                B_col[current][i] = polynomials[0][i].B.range(eps);
                if (isEmpty(B_col[current][i])) break;
                B_col[current][i].first = std::max(B_col[current][i - 1].first, B_col[current][i - 1].first);
            }

            L_row[current][1] = polynomials[1][0].L.range(eps);
            for (auto i = 2; i < maxI; ++i) {
                L_row[current][i] = polynomials[i][0].L.range(eps);
                if (isEmpty(B_col[current][i])) break;
                L_row[current][i].first = std::max(L_row[current][i - 1].first, L_row[current][i - 1].first);
            }

            auto assignMaxOfMinimumOrEmpty = [isEmpty, emptyInterval](Interval& target, const Interval& compare)
            {
                if (isEmpty(compare)) target = emptyInterval;
                else target.first = std::max(target.first, compare.first);
            };

            auto minComplexity = std::min(maxI, maxJ);
            // Number of rows and columns left, to be modified in the loop
            int colNum = maxJ - 1;
            int rowNum = maxI - 1;

            for (; colNum >= 1 && rowNum >= 1; --colNum, --rowNum)
            {
                const auto prev = current;
                const auto currCol = maxJ - colNum;
                const auto currRow = maxI - rowNum;
                current = 1 - current;
                L_col[current].resize(colNum);
                B_col[current].resize(colNum);
                L_row[current].resize(rowNum);
                B_row[current].resize(rowNum);

                B_col[current][0] = polynomials[currRow][currCol].B.range();
                if (isEmpty(L_col[prev][1]))
                {
                    assignMaxOfMinimumOrEmpty(B_col[current][0], B_row[prev][1]);
                }
                B_row[current][0] = B_col[current][0];

                L_col[current][0] = polynomials[currRow][currCol].L.range();
                if (isEmpty(B_col[prev][1]))
                {
                    assignMaxOfMinimumOrEmpty(L_col[current][0], L_row[prev][1]);
                }
                L_row[current][0] = L_col[current][0];

                for (int i = 1; i < colNum; ++i)
                {
                    L_col[current][i] = polynomials[currRow][currCol + i].L.range(eps);
                    if (isEmpty(B_col[prev][i + 1]))
                    {
                        assignMaxOfMinimumOrEmpty(L_col[current][i], L_col[prev][i + 1]);
                    }
                    B_col[current][i] = polynomials[currRow][currCol + i].B.range(eps);
                    if (isEmpty(L_col[prev][i + 1]))
                    {
                        assignMaxOfMinimumOrEmpty(B_col[current][i], B_col[prev][i + 1]);
                    }
                }
                for (int i = 1; i < rowNum; ++i)
                {
                    L_row[current][i] = polynomials[currRow + i][currCol].L.range(eps);
                    if (isEmpty(B_row[prev][i + 1]))
                    {
                        assignMaxOfMinimumOrEmpty(L_row[current][i], L_row[prev][i + 1]);
                    }
                    B_row[current][i] = polynomials[currRow + i][currCol].B.range(eps);
                    if (isEmpty(L_row[prev][i + 1]))
                    {
                        assignMaxOfMinimumOrEmpty(B_row[current][i], B_row[prev][i + 1]);
                    }
                }
            }
            // Determine if we can reach the topright cell, in which case the strong Frechet distance
            // is at most epsilon.
            if (rowNum > colNum)
            {
                return !isEmpty(L_row[current].back()) || !isEmpty(B_row[current].back());
            }
            else
            {
                return !isEmpty(L_col[current].back()) || !isEmpty(B_col[current].back());
            }
        }

        /**
         * \brief O((p^2q + q^ p) \log(pq)) algorithm for computing the Frechet distance
         * Computes all critical values: types a), b) and c).
         * Performs binary search on the values, going left when the decision problem on
         * the current value evaluates to true, otherwise going right.
         * TODO robustness
         * \param polyA First polyline
         * \param polyB Second polyline
         * \return The Frechet distance
         */
        template<typename PointIt>
        NT fullSearch(const std::pair<PointIt, PointIt>& polyA, const std::pair<PointIt, PointIt>& polyB)
        {
            // Critical values of types a) and b) in the paper:
            // a) smallest distance between vertices of the polylines
            // b) smallest distance such that a connection between Freespace cells just opens
            // Sorted set
            const auto polyASize = std::distance(polyA.first, polyA.second);
            const auto polyBSize = std::distance(polyB.first, polyB.second);

            std::vector<std::vector<CellPolynomials>> polynomials;
            precomputePolynomials(polyA, polyB, polynomials);

            // Minimum required epsilon to make start and end mathc.
            const NT minEps = std::max(m_norm(*polyA.first, *polyB.first), m_norm(*(polyA.first + polyASize - 1), *(polyB.first + polyBSize - 1)));

            // If the endpoint epsilon is the smallest possible Frechet distance, within some precision factor, we are ok with selecting that
            if (decide(polynomials, minEps + m_precision))
            {
                return minEps;
            }

            std::set<NT> critValues;
            critValues.insert(minEps);
            for (auto i = 0; i < polyASize; ++i)
            {
                for (auto j = 0; j < polyBSize; ++j)
                {
                    const auto& pointA = *(polyA.first + i);
                    const auto& pointB = *(polyB.first + j);

                    // a) values (insert if above minimum epsilon 
                    auto dist = m_norm(pointA, pointB);
                    if (dist > minEps) critValues.insert(dist);

                    // b) values
                    // See later loop

                    // Critical values of type c):
                    // c) a horizontal or vertical passage between not necessarily adjacent cells is
                    // possibly opened.
                    if (i != polyASize - 1)
                    {
                        for (auto k = j + 1; k < polyBSize; ++k) //TODO should this be less equal?
                        {
                            // Check horizontal openings
                            auto cCritPair = polynomials[i][j].L.criticalValue(polynomials[i][k].L);
                            if (cCritPair.first && cCritPair.second > minEps) // Crit value is not redundant.
                            {
                                critValues.insert(cCritPair.second);
                            }
                        }
                    }
                    if (j != polyBSize - 1)
                    {
                        for (auto k = i + 1; k < polyASize; ++k) //TODO should this be less equal?
                        {
                            // Check horizontal openings
                            auto cCritPair = polynomials[i][j].B.criticalValue(polynomials[k][j].B);
                            if (cCritPair.first && cCritPair.second > minEps) // Crit value is not redundant.
                            {
                                critValues.insert(cCritPair.second);
                            }
                        }
                    }
                }
            }
            for (const auto& row : polynomials)
            {
                for (const auto& cell : row)
                {
                    // Add b) type critical values, if set.
                    if (cell.L.type != 'n' && cell.L.minEps > minEps) critValues.insert(cell.L.minEps);
                    if (cell.B.type != 'n' && cell.B.minEps > minEps) critValues.insert(cell.B.minEps);
                }
            }

            // TODO prevent copy if possible
            std::vector<NT> critValueList(critValues.begin(), critValues.end());

            // Perform binary search on critical values.
            // Go left on positive answer, right otherwise.
            std::size_t lower = 0, upper = critValueList.size() - 1;
            auto critValueListStart = critValueList.begin();
            bool lastFeasible = false;
            while (true)
            {
                if (lower == upper) break;
                auto pos = std::floor((lower + upper) / 2);
                auto curr = critValueListStart + pos;
                lastFeasible = decide(polynomials, *curr);
                if (lastFeasible)
                {
                    upper = pos;
                }
                else
                {
                    lower = pos + 1;
                }
            }
            if (!lastFeasible) {
                if (lower == critValueList.size() - 1)
                {
                    assert(decide(polynomials, critValueList.back() + m_precision));
                    return critValueList.back();
                }
                else if (critValueList.size() >= 2 && decide(polynomials, 0.5*(critValueList[lower] + critValueList[lower + 1])))
                {
                    return critValueList[lower];
                }
                return *(critValueListStart + (lower + 1));
            }
            return *(critValueListStart + lower);
        }

        template<typename PointIt>
        bool fullSearchUpperBounded(const std::pair<PointIt, PointIt>& polyA, const std::pair<PointIt, PointIt>& polyB, NT upperBound, NT& outDist)
        {
            // Critical values of types a) and b) in the paper:
            // a) smallest distance between vertices of the polylines
            // b) smallest distance such that a connection between Freespace cells just opens
            // Sorted set
            const auto polyASize = std::distance(polyA.first, polyA.second);
            const auto polyBSize = std::distance(polyB.first, polyB.second);

            std::vector<std::vector<CellPolynomials>> polynomials;
            precomputePolynomials(polyA, polyB, polynomials);

            // Minimum required epsilon to make start and end mathc.
            const NT minEps = std::max(m_norm(*polyA.first, *polyB.first), m_norm(*(polyA.first + polyASize - 1), *(polyB.first + polyBSize - 1)));

            if (minEps > upperBound)
            {
                return false;
            }
            // If the endpoint epsilon is the smallest, within some fraction, we are ok with selecting that
            if (decide(polynomials, minEps + m_precision))
            {
                outDist = minEps;
                return true;
            }
            std::set<NT> critValues;
            auto insertCrit = [&critValues, upperBound](NT val)
            {
                if (val <= upperBound) critValues.insert(val);
            };
            insertCrit(minEps);
            for (auto i = 0; i < polyASize; ++i)
            {
                for (auto j = 0; j < polyBSize; ++j)
                {
                    const auto& pointA = *(polyA.first + i);
                    const auto& pointB = *(polyB.first + j);

                    // a) values (insert if above minimum epsilon) (vertex-vertex distances)
                    auto dist = m_norm(pointA, pointB);
                    if (dist > minEps) insertCrit(dist);

                    // b) values (vertex-segment distances)
                    // See later loop

                    // Critical values of type c):
                    // c) a horizontal or vertical passage between not necessarily adjacent cells is
                    // possibly opened.
                    if (i != polyASize - 1)
                    {
                        for (auto k = j + 1; k < polyBSize; ++k) //TODO should this be less equal?
                        {
                            // Check horizontal openings
                            auto cCritPair = polynomials[i][j].L.criticalValue(polynomials[i][k].L);
                            if (cCritPair.first && cCritPair.second > minEps) // Crit value is not redundant.
                            {
                                insertCrit(cCritPair.second);
                            }
                        }
                    }
                    if (j != polyBSize - 1)
                    {
                        for (auto k = i + 1; k < polyASize; ++k) //TODO should this be less equal?
                        {
                            // Check horizontal openings
                            auto cCritPair = polynomials[i][j].B.criticalValue(polynomials[k][j].B);
                            if (cCritPair.first && cCritPair.second > minEps) // Crit value is not redundant.
                            {
                                insertCrit(cCritPair.second);
                            }
                        }
                    }
                }
            }
            for (const auto& row : polynomials)
            {
                for (const auto& cell : row)
                {
                    // Add b) type critical values, if set.
                    if (cell.L.type != 'n' && cell.L.minEps > minEps) insertCrit(cell.L.minEps);
                    if (cell.B.type != 'n' && cell.B.minEps > minEps) insertCrit(cell.B.minEps);
                }
            }

            // TODO prevent copy if possible
            std::vector<NT> critValueList(critValues.begin(), critValues.end());

            // Perform binary search on critical values.
            // Go left on positive answer, right otherwise.
            long long lower = 0, upper = critValueList.size() - 1;
            auto begin = critValueList.begin();
            bool lastFeasible = false;
            while (true)
            {
                if (lower == upper || lower > upper) break;
                auto pos = std::floor((lower + upper) / 2);
                auto curr = begin + pos;
                lastFeasible = decide(polynomials, *curr);
                if (lastFeasible)
                {
                    upper = pos;
                }
                else
                {
                    lower = pos + 1;
                }
            }
            if (!lastFeasible) {
                if (lower == critValueList.size() - 1)
                {
                    assert(decide(polynomials, critValueList.back() + 0.001));
                    outDist = critValueList.back();
                    return true;
                }
                else if (critValueList.size() >= 2 && decide(polynomials, 0.5*(critValueList[lower] + critValueList[lower + 1])))
                {
                    outDist = critValueList[lower];
                    return true;
                }
                outDist = *(begin + (lower + 1));
                return true;
            }
            outDist = *(begin + lower);
            return true;
        }

        bool bisectionSearchInInterval(const std::vector<std::vector<CellPolynomials>>& polynomials, NT tolerance, NT lower, NT upper, NT& outDist) const
        {
            // Upper should be valid, otherwise we are searching in an infeasible interval.
            if (!decide(polynomials, upper)) return false;

            NT lBound = lower, rBound = upper;
            NT currentBest = upper;
            while (true)
            {
                if (std::abs(lBound - rBound) < tolerance) break;
                // New value to test
                const NT curr = (lBound + rBound) * 0.5;
                if (decide(polynomials, curr))
                {
                    rBound = curr;
                    currentBest = curr;
                }
                else
                {
                    lBound = curr;
                }
            }
            outDist = currentBest;

            return true;
        }

        template<typename PointIt>
        bool bisectionSearchUpperBounded(const std::pair<PointIt, PointIt>& polyA, const std::pair<PointIt, PointIt>& polyB, NT& outDist)
        {
            // Polyline sizes (number of points)
            const auto polyASize = std::distance(polyA.first, polyA.second);
            const auto polyBSize = std::distance(polyB.first, polyB.second);

            // Minimum required epsilon to make start and end match for polylines.
            const NT minEps = std::max(m_norm(*polyA.first, *polyB.first), m_norm(*(polyA.first + polyASize - 1), *(polyB.first + polyBSize - 1)));

            if (minEps > m_upperBound)
            {
                return false;
            }

            std::vector<std::vector<CellPolynomials>> polynomials;
            precomputePolynomials(polyA, polyB, polynomials);


            // If the endpoint epsilon is the smallest, within some fraction, we are ok with selecting that
            if (decide(polynomials, minEps + m_precision))
            {
                outDist = minEps;
                return true;
            }
            return bisectionSearchInInterval(polynomials, m_precision, minEps, m_upperBound, outDist);
        }
        template<typename PointIt>
        bool doubleAndSearch(const std::pair<PointIt, PointIt>& polyA, const std::pair<PointIt, PointIt>& polyB, NT& outDist)
        {
            // Polyline sizes (number of points)
            const auto polyASize = std::distance(polyA.first, polyA.second);
            const auto polyBSize = std::distance(polyB.first, polyB.second);
            // Minimum required epsilon to make start and end match for polylines.
            NT minEps = std::max(m_norm(*polyA.first, *polyB.first), m_norm(*std::prev(polyA.second), *std::prev(polyB.second)));

            if (minEps > m_upperBound)
            {
                return false;
            }

            std::vector<std::vector<CellPolynomials>> polynomials;
            precomputePolynomials(polyA, polyB, polynomials);

            // If the endpoint epsilon is the smallest, within some fraction, we are ok with selecting that
            if (decide(polynomials, minEps + m_precision))
            {
                outDist = minEps + m_precision;
                return true;
            }
            if (minEps < m_precision)
            {
                minEps = m_precision;
            }

            // Double the epsilon at each step
            NT currEps = minEps * 2.0;
            while (true)
            {
                // Should happen at some point unless the input is extremely malformed
                if (decide(polynomials, currEps))
                {
                    return bisectionSearchInInterval(polynomials, m_precision, currEps * 0.5, currEps, outDist);
                }
                currEps *= 2.0;
            }
        }

    public:
        enum class Mode
        {
            AllCriticalValues,
            //ParametricSearch,
            BisectionSearch,
            DoubleAndSearch
        };
    private:
        Mode m_mode;
    public:
        StrongFrechetDistance(Mode mode = Mode::BisectionSearch) : m_mode(mode) {}

        Mode mode() const
        {
            return m_mode;
        }
        void setMode(Mode mode)
        {
            m_mode = mode;
        }

        /**
         * \brief Set upperbound for upperbounded search approaches
         * \param upperBound The upperbound on the Frechet distance
         */
        void setUpperbound(NT upperBound)
        {
            m_upperBound = upperBound;
        }

        /**
         * \brief Set the error tolerance in Frechet distance for the inexact methods.
         * \param tolerance The tolerance
         */
        void setTolerance(NT tolerance)
        {
            m_precision = tolerance;
        }

        // Implementation of Alt & Godau,
        // http://www.staff.science.uu.nl/~kreve101/asci/ag-cfdbt-95.pdf
        // TODO: maybe parametric search:
        // https://www.sciencedirect.com/science/article/pii/S0925772104000203
        template <class InputIterator,
            typename = movetk_core::requires_random_access_iterator<InputIterator>,
            typename = movetk_core::requires_movetk_point<Kernel,
            typename InputIterator::value_type>>
            bool operator()(InputIterator poly_a, InputIterator poly_a_beyond,
                InputIterator poly_b, InputIterator poly_b_beyond, typename Kernel::NT& output)
        {
            auto polyA = std::make_pair(poly_a, poly_a_beyond);
            auto polyB = std::make_pair(poly_b, poly_b_beyond);
            switch (m_mode)
            {
            case Mode::AllCriticalValues: output = fullSearch(polyA, polyB); return true;
            case Mode::BisectionSearch: return bisectionSearchUpperBounded(polyA, polyB, output);
            case Mode::DoubleAndSearch: return doubleAndSearch(polyA, polyB, output);
            default:return false;
            }

        }
        
        template <class InputIterator,
            typename = movetk_core::requires_random_access_iterator<InputIterator>,
            typename = movetk_core::requires_movetk_point<Kernel,
            typename InputIterator::value_type>>
            typename Kernel::NT operator()(InputIterator poly_a, InputIterator poly_a_beyond,
                InputIterator poly_b, InputIterator poly_b_beyond)
        {
            typename Kernel::NT output = 0.0;
            (void)this->operator()(poly_a, poly_a_beyond, poly_b, poly_b_beyond, output);
            return output;
        }

    };
}
#endif //MOVETK_DISTANCES_H
