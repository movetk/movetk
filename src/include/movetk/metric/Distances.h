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
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

namespace movetk::metric
{

    // based on Eiter, T., & Mannila, H. (1994). Computing Discrete Fréchet Distance ∗.

    template <class Kernel, class _Norm>
    struct squared_distance_d
    {
        using Norm = _Norm;
        using MovetkPoint = typename Kernel::MovetkPoint;

        typename Kernel::NT operator()(const MovetkPoint &p,
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

        typename Kernel::NT operator()(typename Kernel::MovetkLine &l, const MovetkPoint &p)
        {
            return this->operator()(p, l);
        }

        typename Kernel::NT operator()(const MovetkPoint &p,
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
        typename Kernel::NT operator()(typename Kernel::MovetkSegment &s, const MovetkPoint &p) const
        {
            return this->operator()(p, s);
        }

        typename Kernel::NT operator()(const MovetkPoint &p1,
                                       const MovetkPoint &p2) const
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
                  typename = movetk::utils::requires_random_access_iterator<InputIterator>,
                  typename = movetk::utils::requires_movetk_point<Kernel,
                                                                typename InputIterator::value_type>>
        typename Kernel::NT singlesided_discrete_hausdorff(InputIterator a_first, InputIterator a_beyond,
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
                  typename = movetk::utils::requires_random_access_iterator<InputIterator>,
                  typename = movetk::utils::requires_movetk_point<Kernel,
                                                                typename InputIterator::value_type>>
        typename Kernel::NT operator()(InputIterator polyline_a_first, InputIterator polyline_a_beyond,
                                       InputIterator polyline_b_first, InputIterator polyline_b_beyond)
        {

            typename Kernel::NT hd_pq = singlesided_discrete_hausdorff(polyline_a_first, polyline_a_beyond,
                                                          polyline_b_first, polyline_b_beyond);

            typename Kernel::NT hd_qp = singlesided_discrete_hausdorff(polyline_b_first, polyline_b_beyond,
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
                  typename = movetk::utils::requires_random_access_iterator<InputIterator>,
                  typename = movetk::utils::requires_movetk_point<Kernel,
                                                                typename InputIterator::value_type>>
        NT distance(InputIterator iter_a, InputIterator iter_b)
        {
            Norm norm;
            typename Kernel::MovetkVector v = *iter_b - *iter_a;
            return norm(v);
        }

    public:
        template <class InputIterator,
                  typename = movetk::utils::requires_random_access_iterator<InputIterator>,
                  typename = movetk::utils::requires_movetk_point<Kernel,
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
            return std::pow(dfd, n);
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
    template <typename Kernel, typename SqDistance>
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
                auto sq = [](auto el) { return el * el; };
                if (epsilon < minimumEpsilon)
                    return std::make_pair<NT, NT>(std::numeric_limits<NT>::max(), std::numeric_limits<NT>::lowest());
                return std::make_pair(
                    type == 'b' ? 0.0 : parallelDistance - std::sqrt(sq(epsilon) - sq(perpendicularDistance)),
                    type == 'a' ? parallelDistance : parallelDistance + std::sqrt(sq(epsilon) - sq(perpendicularDistance)));
            }

            void compute(const Point &point, const Point &seg0, const Point &seg1)
            {
                typename SqDistance::Norm norm;
                auto dir = seg1 - seg0;
                auto segLen = std::sqrt(norm(dir));
                auto pntLen = std::sqrt(norm(point - seg0));
                // Projected value on line through segment,
                // with origin at start of segment
                parallelDistance = (point - seg0) * dir / segLen;
                perpendicularDistance = std::sqrt(pntLen * pntLen - parallelDistance * parallelDistance);
                auto seg = movetk::geom::MakeSegment<Kernel>()(seg0, seg1);
                minimumEpsilon = std::sqrt(SqDistance()(point, seg));
                type = 'i';
                if (parallelDistance > segLen)
                    type = 'a';
                else if (parallelDistance < 0)
                    type = 'b';
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
        template <typename PointItA, typename PointItB>
        void precomputePolynomials(const std::pair<PointItA, PointItA> &polyA, const std::pair<PointItB, PointItB> &polyB, std::vector<std::vector<CellPolynomials>> &polynomials) const
        {
            // We don't save the boundaries at the top/right of the freespacediagram, since they are not need:
            // by convexity, if a path uses the boundary, then the left/bottom boundaries should have atleast one
            // point of free space.
            const auto polyASize = std::distance(polyA.first, polyA.second);
            const auto polyBSize = std::distance(polyB.first, polyB.second);
            polynomials.resize(polyASize - 1, {});
            for (auto &el : polynomials)
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
        bool decide(const std::vector<std::vector<CellPolynomials>> &polynomials, NT epsilon) const
        {
            const auto maxI = polynomials.size();
            const auto maxJ = polynomials[0].size();

            const std::size_t sizes[2] = {maxI, maxJ};

            struct Interval
            {
                NT min = std::numeric_limits<NT>::max();
                NT max = std::numeric_limits<NT>::lowest();
                Interval() {}
                Interval(NT min, NT max) : min(min), max(max) {}
                bool isEmpty() const
                {
                    return max < min;
                }
                void assignMaxToMin(const Interval &other)
                {
                    if (isEmpty())
                        return;
                    min = std::max(min, other.min);
                }
            };
            struct CellIntervals
            {
                Interval intervals[2] = {{}, {}};
                bool isReachable() const
                {
                    // Reachable if one of the intervals is not empty
                    return !intervals[0].isEmpty() || !intervals[1].isEmpty();
                }
                Interval &operator[](int i)
                {
                    return intervals[i];
                }
                const Interval &operator[](int i) const
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
            auto getFreeSpace = [dim, &polynomials, epsilon](int primaryDimIndex, int secondaryDimIndex, int targetDim) {
                const auto r = dim == 0 ? primaryDimIndex : secondaryDimIndex;
                const auto c = dim == 0 ? secondaryDimIndex : primaryDimIndex;
                auto res = polynomials[r][c].polys[targetDim].range(epsilon);
                return Interval{res.first, res.second};
            };

            // Initialize first row(col), depending on chosen dimension to compute over.
            progress[current].assign(sizes[dim], {});
            progress[current][0].intervals[secondaryDim] = Interval(std::numeric_limits<NT>::lowest(), std::numeric_limits<NT>::max()); // Fully open interval
            progress[current][0].intervals[dim] = Interval(std::numeric_limits<NT>::lowest(), std::numeric_limits<NT>::max());          // Fully open interval
            for (auto i = 1; i < sizes[dim]; ++i)
            {
                if (!progress[current][i - 1].intervals[dim].isEmpty())
                {
                    progress[current][i].intervals[dim] = getFreeSpace(i, 0, dim);
                    progress[current][i].intervals[dim].assignMaxToMin(progress[current][i - 1].intervals[dim]);
                }
            }
            // Go over all other rows(columns).
            for (auto j = 1; j < sizes[secondaryDim]; ++j)
            {
                // Fill other
                const auto prev = current;
                current = 1 - current;

                // Reset intervals to empty
                progress[current].assign(sizes[dim], {});

                // Compute first cell
                auto &firstCell = progress[current][0];
                const auto &prevFirstCell = progress[prev][0];
                if (!prevFirstCell[secondaryDim].isEmpty())
                {
                    firstCell[secondaryDim] = getFreeSpace(0, j, secondaryDim);
                    // Assign maximum of the minimum coordinates to the minimum
                    firstCell[secondaryDim].assignMaxToMin(prevFirstCell[secondaryDim]);
                }

                // Compute the intervals for the other cells
                const auto &prevCells = progress[prev];
                const auto &currCells = progress[current];
                bool hasReachable = firstCell.isReachable();
                for (auto i = 1; i < sizes[dim]; ++i)
                {
                    auto &currCell = progress[current][i];
                    // Compute secondary dimension element
                    if (prevCells[i].isReachable())
                    {
                        currCell[secondaryDim] = getFreeSpace(i, j, secondaryDim);
                        if (prevCells[i][dim].isEmpty() && !prevCells[i][secondaryDim].isEmpty())
                        {
                            currCell[secondaryDim].assignMaxToMin(prevCells[i][secondaryDim]);
                        }
                    }
                    if (currCells[i - 1].isReachable())
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
                if (!hasReachable)
                {
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
        bool bisectionSearchInInterval(const std::vector<std::vector<CellPolynomials>> &polynomials, NT tolerance, NT lower, NT upper, NT &outDist) const
        {
            // Upper should be valid, otherwise we are searching in an infeasible interval.
            if (!decide(polynomials, upper))
                return false;

            NT lBound = lower, rBound = upper;
            NT currentBest = upper;
            while (true)
            {
                if (std::abs(lBound - rBound) < tolerance)
                    break;
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

        template <typename PointItA, typename PointItB>
        bool bisectionSearchUpperBounded(const std::pair<PointItA, PointItA> &polyA, const std::pair<PointItB, PointItB> &polyB, NT &outDist) const
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

        template <typename PointItA, typename PointItB>
        bool doubleAndSearch(const std::pair<PointItA, PointItA> &polyA, const std::pair<PointItB, PointItB> &polyB, NT &outDist)
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
        template <class InputIteratorA, class InputIteratorB,
                  typename = movetk::utils::requires_random_access_iterator<InputIteratorA>,
                  typename = movetk::utils::requires_movetk_point_iterator<Kernel, InputIteratorA>,
                  typename = movetk::utils::requires_random_access_iterator<InputIteratorB>,
                  typename = movetk::utils::requires_movetk_point_iterator<Kernel, InputIteratorB>>
        bool decide(InputIteratorA poly_a, InputIteratorA poly_a_beyond,
                    InputIteratorB poly_b, InputIteratorB poly_b_beyond, NT epsilon) const
        {
            // Polyline sizes (number of points)
            const auto polyASize = std::distance(poly_a, poly_a_beyond);
            const auto polyBSize = std::distance(poly_b, poly_b_beyond);

            // Or error
            if (polyASize == 0 || polyBSize == 0)
                return false;

            if (polyASize == 1)
            {
                // std::function<const NT &(const typename Kernel::MovetkPoint &)> transform = [poly_a](const auto &polyBEl) -> const NT & {
                //     SqDistance sqDist;
                //     return std::sqrt(sqDist(*poly_a, polyBEl));
                // };
                // auto first = boost::make_transform_iterator(poly_b, transform);
                // auto beyond = boost::make_transform_iterator(poly_b_beyond, transform);
                std::vector<NT> distances;
                std::transform(poly_b, poly_b_beyond, std::back_insert_iterator(distances),
                               [poly_a](const auto &polyBEl) {
                                   SqDistance sqDist;
                                   return std::sqrt(sqDist(*poly_a, polyBEl));
                               });
                auto maxElIt = std::max_element(std::begin(distances), std::end(distances));
                return *maxElIt <= epsilon + m_precision;
            }
            if (polyBSize == 1)
            {
                // std::function<const NT &(const typename Kernel::MovetkPoint &)> transform = [poly_b](const auto &polyAEl) -> const NT & {
                //     SqDistance sqDist;
                //     return std::sqrt(sqDist(*poly_b, polyAEl));
                // };
                // auto first = boost::make_transform_iterator(poly_a, transform);
                // auto beyond = boost::make_transform_iterator(poly_a_beyond, transform);
                // auto maxElIt = std::max_element(first, beyond);

                std::vector<NT> distances;
                std::transform(poly_a, poly_a_beyond, std::back_insert_iterator(distances),
                               [poly_b](const auto &polyAEl) {
                                   SqDistance sqDist;
                                   return std::sqrt(sqDist(*poly_b, polyAEl));
                               });
                auto maxElIt = std::max_element(std::begin(distances), std::end(distances));

                return *maxElIt <= epsilon + m_precision;
            }

            // Minimum required epsilon to make start and end match for polylines.
            NT minEps = std::max(std::sqrt(m_sqDistance(*poly_a, *poly_b)), std::sqrt(m_sqDistance(*std::prev(poly_a_beyond), *std::prev(poly_b_beyond))));

            if (polyASize == 2 && polyBSize == 2)
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

        template <class InputIteratorA, class InputIteratorB,
                  typename = movetk::utils::requires_random_access_iterator<InputIteratorA>,
                  typename = movetk::utils::requires_movetk_point_iterator<Kernel, InputIteratorA>,
                  typename = movetk::utils::requires_random_access_iterator<InputIteratorB>,
                  typename = movetk::utils::requires_movetk_point_iterator<Kernel, InputIteratorB>>
        bool operator()(InputIteratorA poly_a, InputIteratorA poly_a_beyond,
                        InputIteratorB poly_b, InputIteratorB poly_b_beyond, typename Kernel::NT &output)
        {
            auto polyA = std::make_pair(poly_a, poly_a_beyond);
            auto polyB = std::make_pair(poly_b, poly_b_beyond);

            // Polyline sizes (number of points)
            const auto polyASize = std::distance(poly_a, poly_a_beyond);
            const auto polyBSize = std::distance(poly_b, poly_b_beyond);

            // Or error
            if (polyASize == 0 || polyBSize == 0)
                return false;

            if (polyASize == 1)
            {
                auto sqDist = m_sqDistance;
                // Use std::function, otherwise boost will error when trying to copy the transform_iterator
                // std::function<NT(const typename Kernel::MovetkPoint &)> transform = [poly_a](const auto &polyBEl) {
                //     SqDistance sqDist;
                //     return std::sqrt(sqDist(*poly_a, polyBEl));
                // };

                // auto maxElIt = std::max_element(boost::make_transform_iterator(poly_b, transform), boost::make_transform_iterator(poly_b_beyond, transform));

                std::vector<NT> distances;
                std::transform(poly_b, poly_b_beyond, std::back_insert_iterator(distances),
                               [poly_a](const auto &polyBEl) {
                                   SqDistance sqDist;
                                   return std::sqrt(sqDist(*poly_a, polyBEl));
                               });
                auto maxElIt = std::max_element(std::begin(distances), std::end(distances));

                output = *maxElIt;
                return output <= m_upperBound + m_precision;
            }
            if (polyBSize == 1)
            {
                // // Use std::function, otherwise boost will error when trying to copy the transform_iterator
                // std::function<NT(const typename Kernel::MovetkPoint &)> transform = [poly_b](const auto &polyAEl) {
                //     SqDistance sqDist;
                //     return std::sqrt(sqDist(*poly_b, polyAEl));
                // };
                // auto maxElIt = std::max_element(boost::make_transform_iterator(poly_a, transform), boost::make_transform_iterator(poly_a_beyond, transform));

                std::vector<NT> distances;
                std::transform(poly_a, poly_a_beyond, std::back_insert_iterator(distances),
                               [poly_b](const auto &polyAEl) {
                                   SqDistance sqDist;
                                   return std::sqrt(sqDist(*poly_b, polyAEl));
                               });
                auto maxElIt = std::max_element(std::begin(distances), std::end(distances));

                output = *maxElIt;
                return output <= m_upperBound + m_precision;
            }

            switch (m_mode)
            {
            case Mode::BisectionSearch:
                return bisectionSearchUpperBounded(polyA, polyB, output);
            case Mode::DoubleAndSearch:
                return doubleAndSearch(polyA, polyB, output);
            default:
                return false;
            }
        }

        template <class InputIteratorA, class InputIteratorB,
                  typename = movetk::utils::requires_random_access_iterator<InputIteratorA>,
                  typename = movetk::utils::requires_movetk_point_iterator<Kernel, InputIteratorA>,
                  typename = movetk::utils::requires_random_access_iterator<InputIteratorB>,
                  typename = movetk::utils::requires_movetk_point_iterator<Kernel, InputIteratorB>>
        typename Kernel::NT operator()(InputIteratorA poly_a, InputIteratorA poly_a_beyond,
                                       InputIteratorB poly_b, InputIteratorB poly_b_beyond)
        {
            typename Kernel::NT epsilon = -1;
            (void)this->operator()(poly_a, poly_a_beyond, poly_b, poly_b_beyond, epsilon);
            return epsilon;
        }
    };

    /**
     * \brief Implementation of the weak Frechet distance between two polylines
     *  Based on 'Computing the Fréchet distance between two polygonal curves', Alt & Godau, 1995
     *  http://www.staff.science.uu.nl/~kreve101/asci/ag-cfdbt-95.pdf
     * \tparam GeometryTraits The geometry kernel
     * \tparam SquaredDistance Squared difference computer that supports computing the distance between a point and a segment
     */
    template <class GeometryTraits, class SquaredDistance>
    class WeakFrechet
    {
        using NT = typename GeometryTraits::NT;
        using FreeSpaceGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_weight_t, NT>>;
        using FreeSpaceGraphTraits = boost::graph_traits<FreeSpaceGraph>;
        using FreeSpaceGraphWeightMap = typename boost::property_map<FreeSpaceGraph, boost::edge_weight_t>::type;

        // Squared norm to use
        SquaredDistance m_sqDist;
        geom::MakeSegment<GeometryTraits> make_segment;

        // Combination function for searching minimax paths via boost dijkstra.
        struct MiniMaxCombineDistance
        {
            NT operator()(NT dist, NT weight) const
            {
                return std::max(dist, weight);
            }
        };

        /**
         * \brief Setup the freespace grid-graph for computing the weak Frechet distance
         * \tparam InIt Input iterator for points of polylines
         * \param polyA First polyline, specified as a pair of input iterators (start and beyond)
         * \param polyB Second polyline, specified as a pair of input iterators (start and beyond)
         * \param freeSpaceGraph Output freespace graph
         * \param weightMap Output weightmap for the edges of the freespace graph.
         */
        template <typename InIt>
        void setup_freespace_graph(std::pair<InIt, InIt> polyA, std::pair<InIt, InIt> polyB, FreeSpaceGraph &freeSpaceGraph, FreeSpaceGraphWeightMap &weightMap)
        {
            const auto polyASegCount = std::distance(polyA.first, polyA.second) - 1;
            const auto polyBSegCount = std::distance(polyB.first, polyB.second) - 1;

            // Convert grid indices to single vector index
            auto vertIndex = [polyASegCount](int i, int j) {
                return i + j * polyASegCount;
            };

            const auto vertexCount = polyASegCount * polyBSegCount;
            freeSpaceGraph = FreeSpaceGraph(vertexCount);
            // Get the property map representing edge weights on the graph.
            weightMap = boost::get(boost::edge_weight, freeSpaceGraph);
            {
                auto pointAIt = polyA.first;
                for (int i = 0; i < polyASegCount; ++i, ++pointAIt)
                {
                    // Create the segment
                    auto seg = make_segment(*pointAIt, *std::next(pointAIt));
                    // Determine closest distances and store as weights
                    auto pointBIt = polyB.first;
                    for (int j = 0; j < polyBSegCount - 1; ++j, ++pointBIt)
                    {
                        auto startVert = boost::vertex(vertIndex(i, j), freeSpaceGraph);
                        auto endVert = boost::vertex(vertIndex(i, j + 1), freeSpaceGraph);
                        auto edgeDescriptor = boost::add_edge(startVert, endVert, freeSpaceGraph);
                        weightMap[edgeDescriptor.first] = m_sqDist(seg, *pointBIt);
                    }
                }
            }
            {
                // Create horizontal edges in the freespace graph
                auto pointBIt = polyB.first;
                for (int j = 0; j < polyBSegCount; ++j, ++pointBIt)
                {
                    // Create the segment
                    auto seg = make_segment(*pointBIt, *std::next(pointBIt));
                    // Determine closest distances and store as weights
                    auto pointAIt = polyA.first;
                    for (int i = 0; i < polyASegCount - 1; ++i, ++pointAIt)
                    {
                        auto startVert = boost::vertex(vertIndex(i, j), freeSpaceGraph);
                        auto endVert = boost::vertex(vertIndex(i + 1, j), freeSpaceGraph);
                        auto edgeDescriptor = boost::add_edge(startVert, endVert, freeSpaceGraph);
                        weightMap[edgeDescriptor.first] = m_sqDist(seg, *pointAIt);
                    }
                }
            }
        }

    public:
        WeakFrechet() {}
        /**
         * @brief Computes the weak Frechet distance between two polylines
         * @tparam InputIterator Iterator type for polyline iterators
         * @param polyline_a_first Start of first polyline
         * @param polyline_a_beyond End of first polyline
         * @param polyline_b_first Start of second polyline
         * @param polyline_b_beyond End of second polyline
         * @return The weak Frechet distance.
         */
        template <class InputIterator,
                  typename = movetk::utils::requires_random_access_iterator<InputIterator>,
                  typename = movetk::utils::requires_movetk_point<GeometryTraits,
                                                                typename InputIterator::value_type>>
        typename GeometryTraits::NT operator()(InputIterator polyline_a_first, InputIterator polyline_a_beyond,
                                               InputIterator polyline_b_first, InputIterator polyline_b_beyond)
        {
            // Point counts of the polylines
            const auto polyASize = std::distance(polyline_a_first, polyline_a_beyond);
            const auto polyBSize = std::distance(polyline_b_first, polyline_b_beyond);

            // Distances between starts resp. ends of the polylines (squared)
            const auto startMatchDist = std::sqrt(m_sqDist(*polyline_a_first, *polyline_b_first));
            const auto endMatchDist = std::sqrt(m_sqDist(*std::prev(polyline_a_beyond), *std::prev(polyline_b_beyond)));

            // Special case input
            if (polyASize <= 2 && polyBSize <= 2)
            {
                return std::max(startMatchDist, endMatchDist);
            }

            // Setup free space grid graph. The weights on the edges are the minimum distances for the cell border, represented by that edge, to open
            FreeSpaceGraph freeSpaceGraph;
            FreeSpaceGraphWeightMap weightMap;
            setup_freespace_graph(std::make_pair(polyline_a_first, polyline_a_beyond), std::make_pair(polyline_b_first, polyline_b_beyond),
                                freeSpaceGraph, weightMap);

            std::vector<NT> distances(boost::num_vertices(freeSpaceGraph));
            auto distanceMap = boost::make_iterator_property_map(distances.begin(), get(boost::vertex_index, freeSpaceGraph));

            // Find min max path, uses default less comparison for weights and custom combine fucntion
            boost::dijkstra_shortest_paths(freeSpaceGraph, boost::vertex(0, freeSpaceGraph), boost::distance_zero(0).distance_map(distanceMap).distance_combine(MiniMaxCombineDistance()));

            const auto freeSpaceMatchDist = distances[boost::num_vertices(freeSpaceGraph) - 1];

            return std::sqrt(std::max({startMatchDist, endMatchDist, freeSpaceMatchDist}));
        }

        /**
         * \brief Computes the weak Frechet distance and returns a matching between the polylines that witnesses the returned distance.
         * \tparam InputIterator Type of the input iterators, specifying the input polylines as points
         * \tparam OutputIterator Type of the output iterator that accepts the matching in the form of std::pair<std::pair<int,int>, NT> elements
         * \param polyline_a_first Start of the first polyline
         * \param polyline_a_beyond End of the first polyline
         * \param polyline_b_first Start of the second polyline
         * \param polyline_b_beyond End of the second polyline
         * \param matching_output Output iterator that will be supplied with the matching. The matching is described as a pair of indices giving the
         * segment indices on polyline_a resp. polyline_b and a minimum distance to reach the next segment pair in the returned list. Note
         * that the matching start with an entry of {{-1,-1}, startMatchDist} where startMatchDist is the distance to match the starting points of the
         * two polylines. The minimum distance of the last entry is the distance to match the endpoints of the polylines.
         * \return The weak Frechet distance between the given polylines.
         */
        template <class InputIterator, class OutputIterator,
                  typename = movetk::utils::requires_random_access_iterator<InputIterator>,
                  typename = movetk::utils::requires_movetk_point<GeometryTraits, typename InputIterator::value_type>,
                  typename = movetk::utils::requires_equality<std::pair<std::pair<int, int>, NT>, typename OutputIterator::value_type>>
        typename GeometryTraits::NT operator()(InputIterator polyline_a_first, InputIterator polyline_a_beyond,
                                               InputIterator polyline_b_first, InputIterator polyline_b_beyond, OutputIterator matching_output)
        {
            // Point counts of the polylines
            const auto polyASize = std::distance(polyline_a_first, polyline_a_beyond);
            const auto polyBSize = std::distance(polyline_b_first, polyline_b_beyond);

            // Distances between starts resp. ends of the polylines (squared)
            const auto startMatchDist = m_sqDist(*polyline_a_first, *polyline_b_first);
            const auto endMatchDist = m_sqDist(*std::prev(polyline_a_beyond), *std::prev(polyline_b_beyond));

            // Special case input
            if (polyASize <= 2 && polyBSize <= 2)
            {
                auto res = std::sqrt(std::max(startMatchDist, endMatchDist));
                // Return the segment matching path and the associated distance for the segment
                *matching_output = std::make_pair(std::make_pair(0, 0), res);
                return res;
            }

            // Setup free space grid graph. The weights on the edges are the minimum distances for the cell border, represented by that edge, to open
            FreeSpaceGraph freeSpaceGraph;
            FreeSpaceGraphWeightMap weightMap;
            setup_freespace_graph(std::make_pair(polyline_a_first, polyline_a_beyond), std::make_pair(polyline_b_first, polyline_b_beyond),
                                freeSpaceGraph, weightMap);

            std::vector<NT> distances(boost::num_vertices(freeSpaceGraph));
            auto distanceMap = boost::make_iterator_property_map(distances.begin(), get(boost::vertex_index, freeSpaceGraph));

            // Create the predecessors map that will track how we got to the minimax path value
            std::vector<typename FreeSpaceGraphTraits::vertex_descriptor> predecessors(boost::num_vertices(freeSpaceGraph));
            auto predecessorsMap = boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index, freeSpaceGraph));

            // Find min max path, uses default less comparison for weights and custom combine fucntion
            boost::dijkstra_shortest_paths(freeSpaceGraph, boost::vertex(0, freeSpaceGraph),
                                           // Named arguments
                                           boost::distance_zero(0)
                                               .distance_map(distanceMap)
                                               .distance_combine(MiniMaxCombineDistance())
                                               .predecessor_map(predecessorsMap));

            const auto freeSpaceMatchDist = distances[boost::num_vertices(freeSpaceGraph) - 1];

            auto vertDescToIndices = [polyASize](typename FreeSpaceGraph::vertex_descriptor desc) {
                int j = (int)(desc / (polyASize - 1));
                int i = desc - j * (polyASize - 1);
                return std::make_pair(i, j);
            };
            auto val = std::sqrt(std::max({startMatchDist, endMatchDist, freeSpaceMatchDist}));

            // Reconstruct the path in the freespace graph
            std::vector<std::pair<std::pair<int, int>, NT>> matching;
            auto matchingInserter = std::back_inserter(matching);
            auto curr = boost::vertex(boost::num_vertices(freeSpaceGraph) - 1, freeSpaceGraph);
            *matchingInserter = std::make_pair(vertDescToIndices(curr), std::sqrt(endMatchDist));
            while (true)
            {
                auto prev = curr;
                curr = predecessors[curr];
                auto edge = boost::edge(curr, prev, freeSpaceGraph);
                auto weight = weightMap[edge.first];
                *matchingInserter = std::make_pair(vertDescToIndices(curr), std::sqrt(weight));
                if (curr == boost::vertex(0, freeSpaceGraph))
                    break;
            }
            // Add the start matching
            *matchingInserter = std::make_pair(std::make_pair(-1, -1), startMatchDist);

            // Reverse to get forward order matching
            std::reverse(matching.begin(), matching.end());
            // Copy to output
            std::copy(matching.begin(), matching.end(), matching_output);

            return std::sqrt(std::max({startMatchDist, endMatchDist, freeSpaceMatchDist}));
        }
    };
} // namespace movetk_support
#endif //MOVETK_DISTANCES_H
