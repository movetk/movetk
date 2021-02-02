/*
 * Copyright (C) 2018-2020
 * HERE Europe B.V.
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
 // Created by Bram Custers (b.a.custers@tue.nl)

#ifndef MOVETK_HAUSDORFF_DISTANCES_H
#define MOVETK_HAUSDORFF_DISTANCES_H

#include <iostream>
#include "movetk/geom/GeometryInterface.h"
#include <boost/graph/adjacency_list.hpp>
#include <optional>

namespace movetk_support
{
    template<class Kernel, class SqDistance>
    struct ContinuousHausdorffBF
    {
        using MovetkSegment = typename Kernel::MovetkSegment;
        using MovetkPoint = typename Kernel::MovetkPoint;
        using NT = typename Kernel::NT;
        using MovetkVector = typename Kernel::MovetkVector;

        struct SegmentSqDistFunc
        {
            // Describes the squared distance function f(x) = a x^2 + b x + c, with x \in [0,1]
            // s.t. x=0 is at the start of the segment and x=1 at the end.
            typename Kernel::NT a = 0, b = 0, c = 0;

            // Where does the function apply on the segment.
            // TODO: can be modeled as an Interval, similar to the one in StrongFrechet.
            std::array<typename Kernel::NT, 2> applicableSubsegment {1,-1};

            /**
             * \brief Result struct for intersections between functions.
             * TODO: maybe model as IntersectionTraits of custom geometric object.
             */
            struct Intersections
            {
                int count = 0;
                std::array<NT,2> intersectionPar = {-1,-1};
            };

            NT middleOfSegment() const
            {
                return 0.5 * (applicableSubsegment[0] + applicableSubsegment[1]);
            }

            Intersections computeIntersectionsUnbounded(const SegmentSqDistFunc& other) const
            {
                Intersections inters;

                const auto a = this->a - other.a;
                const auto b = this->b - other.b;
                const auto c = this->c - other.c;

                // Catch degenerate cases
                if (std::abs(a) < MOVETK_EPS)
                {
                    if (std::abs(b) < MOVETK_EPS) return inters;
                    inters.count = 1;
                    inters.intersectionPar[0] = -c / b;
                    return inters;
                }

                auto det = b * b - 4.0 * a * c;
                // Case det ~ 0.
                if (std::abs(det) < MOVETK_EPS)
                {
                    inters.count = 1;
                    inters.intersectionPar[0] = -b / (2.0 * a);
                    return inters;
                }
                if (det < 0) return inters;
                inters.count = 2;
                auto sqrtDet = std::sqrt(det);
                inters.intersectionPar[0] = (-b - sqrtDet) / (2.0 * a);
                inters.intersectionPar[1] = (-b + sqrtDet) / (2.0 * a);
                
                return inters;
            }

            Intersections computeIntersections(const SegmentSqDistFunc& other, const std::array<NT,2>& interval) const
            {
                Intersections inters = computeIntersectionsUnbounded(other);

                // Prune out intersections outside of the interval
                // Only works for 2 intersections!
                std::size_t target = 0;
                for(int i = 0; i < inters.count; ++i)
                {
                    // Be a bit conservative here...
                    const bool isOutside = inters.intersectionPar[i] - interval[1] > -MOVETK_EPS || inters.intersectionPar[i] - interval[0] <MOVETK_EPS;
                    if(!isOutside)
                    {
                        inters.intersectionPar[target] = inters.intersectionPar[i];
                        ++target;
                    }
                }
                inters.count = target;

                return inters;
            }

            /**
             * \brief Copies the polynomial from another function
             * \param other The other function
             */
            void setFunctionFromOther(const SegmentSqDistFunc& other)
            {
                a = other.a;
                b = other.b;
                c = other.c;
            }

            /**
             * \brief Tests if the applicable subsegment of this function overlaps with that of another function
             * \param other The other function
             * \return Whether the subsegments overlap
             */
            bool overlapsWith(const SegmentSqDistFunc& other) const
            {
                auto minCoord = std::min(applicableSubsegment[0], other.applicableSubsegment[0]);
                auto maxCoord = std::max(applicableSubsegment[1], other.applicableSubsegment[1]);
                auto len = applicableSubsegment[1] - applicableSubsegment[0];
                auto otherLen = other.applicableSubsegment[1] - other.applicableSubsegment[0];
                return maxCoord - minCoord - len - otherLen < MOVETK_EPS;
            }

            /**
             * \brief Computes the overlapping subsegment between this function and another
             * \param other The other function
             * \return The ovelapping subsegment
             */
            std::array<NT,2> overlappingSegment(const SegmentSqDistFunc& other) const
            {
                return { std::max(applicableSubsegment[0],other.applicableSubsegment[0]), std::min(applicableSubsegment[1],other.applicableSubsegment[1]) };
            }

            bool isInvalid() const
            {
                return applicableSubsegment[1] < applicableSubsegment[0];
            }

            void makeInvalid()
            {
                applicableSubsegment = { 1,-1 };
            }

            NT value(NT x) const
            {
                return a * x * x + b * x + c;
            }

            NT maxValue() const
            {
                // We don't really need this, but okay
                if(a < 0)
                {
                    // Top of parabola
                    auto topLocation = -b / (2.0 * a);
                    if(topLocation < applicableSubsegment[0] || topLocation > applicableSubsegment[1])
                    {
                        return std::max(value(applicableSubsegment[0]), value(applicableSubsegment[1]));
                    }
                    else
                    {
                        return std::max(value(topLocation), std::max(value(applicableSubsegment[0]), value(applicableSubsegment[1])));
                    }
                }
                // Positive a.
                return std::max(value(applicableSubsegment[0]), value(applicableSubsegment[1]));
            }

            NT minValue() const
            {
                if(std::abs(a) < MOVETK_EPS)
                {
                    if(std::abs(b) < MOVETK_EPS)
                    {
                        return c;
                    }
                    return std::min(value(applicableSubsegment[0]), value(applicableSubsegment[1]));
                }
                // Lowest x
                auto lowestX = -b * 0.5 / a;
                if(lowestX >= 1.0 || lowestX <= 0)
                {
                    return std::min(value(applicableSubsegment[0]), value(applicableSubsegment[1]));
                }
                return value(lowestX);
            }

            bool operator<(const SegmentSqDistFunc& other) const
            {
                return maxValue() < other.maxValue();
            }

            // Compute local f(x) = a x^2 + bx + c, such that f(x) = g(x)^2 with g(x) = b x + c.
            void setSquaredFromLinearFunction(NT b, NT c)
            {
                // (bx + c)^2 =
                // (b^2) x^2 + (2bc)x + (c^2)
                this->a = b * b;
                this->b = 2.0 * b * c;
                this->c = c * c;
            }
            // Compute local f(x) = a x^2 + bx + c, such that f(x) = g(x) with g(x) = a (x-offset)^2 + b (x-offset) + c,
            // with a,b,c,offset of g(x) the input of this function
            void setFromSquaredFunction(NT a, NT b, NT c, NT offset)
            {
                // a(x - offset) ^ 2 + b(x - offset) + c =
                // ax^2 - 2aoffset + a offset^2 + bx - b offset + c =
                // (a) x^2 + (b- 2a offset)x + (- b offset + c + a offset^2)=
                this->a = a;
                this->b = b - 2 * a * offset;
                this->c = c - b * offset + a * offset * offset;
            }
            // Coefficients are b (x-offset) + c.
            void setSquaredFromLinearFunctionWithOffset(NT b, NT c,NT offset)
            {   
                // (b (x-offset) + c)^2 =
                // c^2 + 2 b c (x-offset) + b^2 (x-offset)^2 =
                // c^2 + 2 b c x - 2 b c offset + b^2 x^2 - 2 b^2 offset x + b^2 offset^2 =
                // (b ^ 2 ) x ^ 2  + (2 b c - 2 b ^ 2 offset) x + (c^2  - 2 b c offset + b ^ 2 offset ^ 2 ) =
                //(b ^ 2 ) x ^ 2  + 2 b( c - b offset) x + ((c - b offset)^2) =

                this->a = b * b;
                this->b = 2.0 * b * (c - b * offset);
                this->c = (c - b*offset) * (c - b*offset);
            }
            struct StartComparator
            {
                bool operator()(const SegmentSqDistFunc& f0, const SegmentSqDistFunc& f1)
                {
                    return f0.applicableSubsegment[0] < f1.applicableSubsegment[0];
                }
            };
        };

        /**
         * \brief Structure that records the lower envelope of the intersection of all (squared) distance functions
         * for a segment, i.e. the lower envelope of the function in x-distance space.
         */
        struct SegmentDistanceLowerEnvelope
        {
            // Distance functions for the current segment.
            std::vector<SegmentSqDistFunc> distFuncs;

            using LowerEnvelopeContainer = std::set<SegmentSqDistFunc, typename SegmentSqDistFunc::StartComparator>;
            LowerEnvelopeContainer lowerEnvelope;
            NT lowerEnvelopeMax = -1;

            void addDistanceFunction(const SegmentSqDistFunc& func)
            {
                assert(!std::isnan(func.applicableSubsegment[0]));
                assert(!std::isnan(func.applicableSubsegment[1]));
                assert(!std::isnan(func.a));
                assert(!std::isnan(func.b));
                assert(!std::isnan(func.c));
                assert(func.applicableSubsegment[1] - func.applicableSubsegment[0] > MOVETK_EPS);
                distFuncs.push_back(func);
            }

            void insert(const SegmentSqDistFunc& func)
            {
                assert(func.applicableSubsegment[1] - func.applicableSubsegment[0] > MOVETK_EPS);
                lowerEnvelope.insert(func);
            }

            /**
             * \brief Computes the lower envelope from the current distance functions.
             * Note that this is a somewhat naive implementation, a sweepline in the x direction would perhaps
             * be more performant
             */
            void compute()
            {
                lowerEnvelope.clear();
                lowerEnvelopeMax = -1;

                auto it = distFuncs.begin();
                auto end = distFuncs.end();
                if (it == end) { return; }

                struct Changes
                {
                    // Modifications to the lower envelope.
                    std::vector<typename LowerEnvelopeContainer::iterator> toDelete;
                    std::vector<SegmentSqDistFunc> toInsert;

                    void addDelete(typename LowerEnvelopeContainer::iterator it)
                    {
                        toDelete.push_back(it);
                    }
                    void addInsert(const SegmentSqDistFunc& func)
                    {
                        assert(func.applicableSubsegment[1] - func.applicableSubsegment[0] > MOVETK_EPS);
                        toInsert.push_back(func);
                    }

                    void apply(LowerEnvelopeContainer& lowerEnvelope)
                    {
                        // This should go well, since the iterators should be unique.
                        for (const auto& el : toDelete)
                        {
                            // Elements are not contiguous perse, so erase individually.
                            lowerEnvelope.erase(el);
                        }
                        // Insert new elements
                        for (const auto& el : toInsert)
                        {
                            lowerEnvelope.insert(el);
                        }
                    }
                    void clear()
                    {
                        toDelete.clear();
                        toInsert.clear();
                    }
                };

                lowerEnvelope = { *it };
                ++it;

                // Track changes
                Changes changes;

                for (; it != end; ++it)
                {
                    using It = decltype(lowerEnvelope.begin());

                    It mergeIt = std::lower_bound(lowerEnvelope.begin(), lowerEnvelope.end(), *it, typename SegmentSqDistFunc::StartComparator{});
                    // Check if we are at end, then just add the new function
                    if (mergeIt == lowerEnvelope.end())
                    {
                        changes.addInsert(*it);
                    }
                    // Check if our function is before the beginning
                    else if (mergeIt == lowerEnvelope.begin() && it->applicableSubsegment[0] < mergeIt->applicableSubsegment[0] - MOVETK_EPS)
                    {
                        auto maxVal = lowerEnvelope.begin()->applicableSubsegment[0];
                        SegmentSqDistFunc newFunc = *it;
                        // Crop the applicable subsegment
                        newFunc.applicableSubsegment[1] = std::min(newFunc.applicableSubsegment[1], maxVal);
                        // Mark the element to be inserted.
                        changes.addInsert(newFunc);
                    }
                    // Move one back, since that element should be first compared to our function
                    else if (mergeIt != lowerEnvelope.begin())
                    {
                        mergeIt = std::prev(mergeIt);
                    }

                    for (; mergeIt != lowerEnvelope.end(); ++mergeIt)
                    {
                        // Stop when the current function is below the mergeable function.
                        if (mergeIt->applicableSubsegment[0]-it->applicableSubsegment[1] > MOVETK_EPS)
                        {
                            break;
                        }

                        assert(it->overlapsWith(*mergeIt));

                        std::array<NT, 2> overlapSegment = it->overlappingSegment(*mergeIt);
                        // Overlap is close to a single point: we are going to ignore it
                        if(overlapSegment[1] - overlapSegment[0] < MOVETK_EPS)
                        {
                            break;
                        }

                        // Find intersection of the two parabola's (or lower order power series...)
                        typename SegmentSqDistFunc::Intersections intersections = it->computeIntersections(*mergeIt, overlapSegment);


                        // No intersection: we either keep the original, or fully replace it
                        if (intersections.count == 0)
                        {
                            // Checking at the boundaries of the overlap is rather unstable, we will use the midpoint,
                            // since the lower/higher relation should not change without intersections.
                            const auto mergerStartLoc = 0.5 * (overlapSegment[0] + overlapSegment[1]);
                            // Replace if new value is lower
                            if (it->value(mergerStartLoc) < mergeIt->value(mergerStartLoc))
                            {
                                changes.addDelete(mergeIt);
                                SegmentSqDistFunc newFunc = *it;
                                newFunc.applicableSubsegment = overlapSegment;
                                changes.addInsert(newFunc);
                            }
                            // Don't replace otherwise
                        }
                        // There are intersections
                        else
                        {
                            // Two intersections, so split current subsegment in 3 pieces.
                            // TODO: remerge subsegments belonging to the same function.
                            if (intersections.count == 2)
                            {
                                std::array<SegmentSqDistFunc, 3> segs{};
                                // New functions
                                segs[0].applicableSubsegment = { overlapSegment[0],intersections.intersectionPar[0] };
                                segs[1].applicableSubsegment = { intersections.intersectionPar[0],intersections.intersectionPar[1] };
                                segs[2].applicableSubsegment = { intersections.intersectionPar[1],overlapSegment[1] };

                                // Check which one is closest in the middle of the first subsegment
                                for(int i = 0; i <3; ++i)
                                {
                                    auto halfWay = segs[i].middleOfSegment();
                                    if (it->value(halfWay) < mergeIt->value(halfWay))
                                    {
                                        segs[i].setFunctionFromOther(*it);
                                    }
                                    else 
                                    {
                                        segs[i].setFunctionFromOther(*mergeIt);
                                    }
                                }
                                for (const auto& el : segs) changes.addInsert(el);
                            }
                            else
                            {
                                auto splitPoint = intersections.intersectionPar[0];
                                // New functions
                                std::array<SegmentSqDistFunc, 2> segs{};
                                segs[0].applicableSubsegment = { overlapSegment[0],splitPoint };
                                segs[1].applicableSubsegment = { splitPoint,overlapSegment[1] };

                                // Check which one is closest in the middle of the first subsegment
                                for (int i = 0; i < 2; ++i)
                                {
                                    auto halfWay = segs[i].middleOfSegment();
                                    if (it->value(halfWay) < mergeIt->value(halfWay))
                                    {
                                        segs[i].setFunctionFromOther(*it);
                                    }
                                    else
                                    {
                                        segs[i].setFunctionFromOther(*mergeIt);
                                    }
                                }
                                for (const auto& el : segs) changes.addInsert(el);
                            }
                            // Delete the old func later
                            changes.addDelete(mergeIt);
                        }
                    }

                    // Update the output
                    changes.apply(lowerEnvelope);
                    changes.clear();
                }
            }

            std::size_t size() const
            {
                return lowerEnvelope.size();
            }

            auto begin() const
            {
                return std::cbegin(lowerEnvelope);
            }
            auto end() const
            {
                return std::cend(lowerEnvelope);
            }
        };

        // TODO: something for the core geometry?

        /**
         * \brief Represents a halfplane with its separator defined by N * pos = offset. Its signed distance is given by d(vec) = N * vec - offset,
         * i.e. N points in the positive direction of halfplane relative to the separator.
         */
        struct HalfPlane
        {
            // Normal of the halfplane
            typename Kernel::MovetkVector normal;
            // Offset from origin in the direction of the normal.
            typename Kernel::NT offset;

            typename Kernel::NT distance(const typename Kernel::MovetkVector& point) const
            {
                return normal * point - offset;
            }
            typename Kernel::NT distance(const typename Kernel::MovetkPoint& point) const
            {
                return normal * (point - movetk_core::MakePoint<Kernel>()({ (NT)0,(NT)0 })) - offset;
            }
            // TODO: Move this to appropriate place

            /**
             * \brief Compute the intersection with a line, defined by a point that intersects the line and the
             * direction of the line
             * \param point The intersecting point on the line
             * \param dir The direction of the line
             * \return Potential intersection parameter t if an intersection exists. The intersection is at point + t * dir.
             */
            std::optional<typename Kernel::NT> intersection(const MovetkPoint& point, const MovetkVector& dir)
            {
                auto posVec = (point - movetk_core::MakePoint<Kernel>()({ (NT)0,(NT)0 }));
                // Parallel line to halfplane boundary
                if(std::abs(normal * dir) < MOVETK_EPS)
                {
                    if (std::abs(distance(point)) < MOVETK_EPS) return 0;
                    // No intersection
                    return {};
                }
                return (offset - normal * posVec) / (normal * dir);
            }
        };

        std::tuple<bool, NT, NT> computeIntersection(const MovetkPoint& p0, const MovetkVector& dir0, const MovetkPoint& p1, const MovetkVector& dir1) const
        {
            // TODO: assumed non-denegerate: lines do not lie on top of eachother and are not parallel.

            const auto dir0Sq = dir0 * dir0;
            const auto dir1Sq = dir1 * dir1;
            const auto dotDir = dir0 * dir1;
            auto dir0Len = std::sqrt(dir0Sq);
            auto dir1Len = std::sqrt(dir1Sq);

            auto pointDiff = p1 - p0;

            // Parallel or anti-parallel: we return false for now
            if(std::abs(std::abs(dotDir) - dir0Len*dir1Len) < MOVETK_EPS)
            {
                return std::make_tuple(false, -1, -1);
            }

            auto origin = movetk_core::MakePoint<Kernel>()({ (NT)0,(NT)0 });

            // Assumes the direction are not parallel and the lines are not degenerate.
            auto p0Vec = p0 - origin;
            auto p1Vec = p1 - origin;
            auto pDiff = p0 - p1;

            NT t0=-1, t1=-1;
            if(std::abs(dotDir) < MOVETK_EPS)
            {
                t1 = dir1 * pDiff / dir1Sq;
                t0 = dir0 * pDiff / dir0Sq * -1.0;
            }
            else
            {
                t1 = (dir0Sq * dir1 - dotDir*dir0) * (pDiff) / (dir0Sq * dir1Sq - dotDir * dotDir);
                t0 = (dotDir * t1 -dir0 * pDiff) / dir0Sq;
            }
            return std::make_tuple(true, t0, t1);
        }

        /**
         * \brief Compute the distance function at 'target' as given by the source segment, i.e. the function describes the distance from any point
         * on target to the closest point on source.
         * \param target The target segment
         * \param distanceSource The segment that is the source of the distance function
         * \param output Lower envelope to append the distance function to.
         */
        void computeDistanceFunc(const MovetkSegment& target, const MovetkSegment& distanceSource, SegmentDistanceLowerEnvelope& output) const
        {
            SqDistance sqDist;

            movetk_core::MakeLine<Kernel> makeLine;
            auto origin = movetk_core::MakePoint<Kernel>()({0,0});

            // The distance only applies to the subsegment that is in the x-y ''slab'', defined by extending
            // the vertices of distanceSource in the perpendicular directions
            // Halfplane 0
            auto srcDir = distanceSource[1] - distanceSource[0];
            auto srcDirLen = std::sqrt(srcDir*srcDir);
            
            HalfPlane h0{ srcDir / srcDirLen, (distanceSource[0]-origin) * srcDir / srcDirLen };
            HalfPlane h1{ srcDir / srcDirLen * -1.0, (distanceSource[1]-origin) * srcDir / srcDirLen * -1.0};

            auto q0 = target[0];
            auto q1 = target[1];
            auto targetDir = target[1] - target[0];
            auto targetDirLen = std::sqrt((q1 - q0)*(q1 - q0));

            // Check that the segment false into the induced slab of the source segment, if not then we don't compute 
            // the distance function (distance function is then determined by vertices, which are handled separately)
            if ((h0.distance(q0) < 0 && h0.distance(q1) < 0) || (h1.distance(q0) < 0 && h1.distance(q1) < 0))
            {
                // Don't return anything
                return;
            }

            // Compute intersection between infinite lines that go through the segments
            NT t0, t1;
            bool intersects;
            std::tie(intersects, t0, t1) = computeIntersection(q0, targetDir, distanceSource[0], srcDir);

            // We are parallel
            if(!intersects)
            {
                // Find closest (squared) distance to infinite line and the range that is influenced
                auto dist = sqDist(makeLine(distanceSource[0], distanceSource[1]), q0);
                SegmentSqDistFunc func;
                func.c = dist;
                func.a = 0;
                func.b = 0;

                // Get ranges that overlap on the target segment
                auto par0 = (distanceSource[0] - target[0]) * targetDir / (targetDirLen*targetDirLen);
                auto par1 = (distanceSource[1] - target[0]) * targetDir / (targetDirLen*targetDirLen);

                NT tMin, tMax;
                std::tie(tMin, tMax) = std::minmax(par0, par1);
                // Parallel, but not in the same 'slab', so nothing to add.
                if (tMin >= 1.0 || tMax <= 0) return;

                tMin = std::max<NT>(0, tMin);
                tMax = std::min<NT>(1, tMax);
                func.applicableSubsegment = { tMin, tMax };
                output.addDistanceFunction(func);
            }
            // Degenerate point intersection: this is handled when determining distance functions between points-segments
            else if((std::abs(t0) < MOVETK_EPS && std::abs(t1-1.0) < MOVETK_EPS) || (std::abs(t1) < MOVETK_EPS && std::abs(t0 - 1.0) < MOVETK_EPS))
            {
                return;
            }
            // The segments intersects
            else if(t0 >= 0 && t0 <= 1.0 && t1 >= 0 && t1 <= 1.0)
            {
                // We are perpendicular, so the distanceFunction is somewhat peculiar
                if(std::abs(srcDir * targetDir) < MOVETK_EPS)
                {
                    // Distances from endpoints to the source
                    const auto dist0 = std::sqrt(sqDist(q0, distanceSource));
                    const auto dist1 = std::sqrt(sqDist(q1, distanceSource));

                    if(std::abs(t0) > MOVETK_EPS)
                    {
                        // From dist0 at t=0 to zero at t=t0.
                        SegmentSqDistFunc func0;
                        func0.setSquaredFromLinearFunction(-dist0 / t0, dist0);
                        func0.applicableSubsegment = { 0, 1.0 };
                        output.addDistanceFunction(func0);

                    }
                    else if(std::abs(1.0-t0) > MOVETK_EPS)
                    {
                        SegmentSqDistFunc func1;
                        func1.setSquaredFromLinearFunctionWithOffset(dist1 / (1.0 - t0), 0, t0);
                        func1.applicableSubsegment = { 0, 1.0 };
                        output.addDistanceFunction(func1);
                    }
                    return;
                }
                std::stringstream debugLog;
                debugLog << "Target :" << target[0] << ";" << target[1] << " , src: " << distanceSource[0] << ";" << distanceSource[1] << std::endl;
                std::cerr << debugLog.str();

                // Get the range that is affected
                std::optional<NT> par0 = h0.intersection(q0, targetDir);
                std::optional<NT> par1 = h1.intersection(q0, targetDir);
                assert(par0.has_value() && par1.has_value());

                NT tMin, tMax;
                std::tie(tMin, tMax) = std::minmax(par0.value(), par1.value());
                tMin = std::max<NT>(0, tMin);
                tMax = std::min<NT>(1, tMax);
                assert(tMin - MOVETK_EPS <= t0 && t0 <= tMax + MOVETK_EPS);

                // Distances from reachablepoints to the source
                const auto dist0 = std::sqrt(sqDist(q0 + targetDir * tMin, distanceSource));
                const auto dist1 = std::sqrt(sqDist(q0 + targetDir * tMax, distanceSource));

                // The squared distance function is symmetric around t0! We only need a single function.
                // To determine the function, we need one of the two sides tMin/tMax to not be degenerately the 
                // same as t0. So, try both sides for constructing the function
                if(std::abs(t0-tMin) > MOVETK_EPS)
                {
                    SegmentSqDistFunc func0;
                    func0.setSquaredFromLinearFunctionWithOffset(-dist0 / (t0 - tMin), dist0, tMin);
                    func0.applicableSubsegment = { tMin, tMax };
                    output.addDistanceFunction(func0);
                }
                else if (std::abs(tMax - t0) > MOVETK_EPS)
                {
                    // From 0 at t=t0 to dist1 at t=1.0.
                    SegmentSqDistFunc func1;
                    func1.applicableSubsegment = { t0, tMax };
                    func1.setSquaredFromLinearFunctionWithOffset(dist1 / (tMax - t0), 0, t0);
                    output.addDistanceFunction(func1);
                }
                // Otherwise, this was a degenerate function?
            }
            // We are not parallel and we don't intersect
            else
            {
                // We are perpendicular, so the distanceFunction is somewhat peculiar
                if (std::abs(srcDir * targetDir) < MOVETK_EPS)
                {
                    // Distances from endpoints to the source
                    const auto dist0 = std::sqrt(sqDist(q0, distanceSource));
                    const auto dist1 = std::sqrt(sqDist(q1, distanceSource));

                    // Full segment has a distance that is a linear function.
                    SegmentSqDistFunc func;
                    func.applicableSubsegment = { 0, 1.0 };
                    func.setSquaredFromLinearFunction(dist1-dist0, dist0);
                    output.addDistanceFunction(func);
                    return;
                }
                // Get the range that is affected
                std::optional<NT> par0 = h0.intersection(q0, targetDir);
                std::optional<NT> par1 = h1.intersection(q0, targetDir);
                assert(par0.has_value() && par1.has_value());

                NT tMin, tMax;
                std::tie(tMin, tMax) = std::minmax(par0.value(), par1.value());
                tMin = std::max<NT>(0, tMin);
                tMax = std::min<NT>(1, tMax);

                // Degenerate point intersection: this is handled separately
                if(std::abs(tMin - tMax) < MOVETK_EPS)
                {
                    return;
                }
                assert(std::abs(tMin - tMax) > MOVETK_EPS);

                // Distances from reachablepoints to the source
                const auto dist0 = std::sqrt(sqDist(q0 + targetDir * tMin, distanceSource));
                const auto dist1 = std::sqrt(sqDist(q0 + targetDir * tMax, distanceSource));

                // From dist0 at t=0 to zero at t=t0.
                SegmentSqDistFunc func;
                func.applicableSubsegment = { tMin, tMax };
                func.setSquaredFromLinearFunctionWithOffset((dist1-dist0)/(tMax-tMin), dist0, tMin);
                output.addDistanceFunction(func);
            }
        }
        
        /**
         * \brief Compute the distance function at 'target' as given by the source point, i.e. the function describes the distance from any point
         * on target to source.
         * \param target The target segment
         * \param distanceSource The point that is the source of the distance function
         * \param output Lower envelope to append the distance function to.
         */
        void computeDistanceFunc(const MovetkSegment& target, const MovetkPoint& distanceSource, SegmentDistanceLowerEnvelope& output) const
        {
            SqDistance sqDist;
            movetk_core::MakeLine<MovetkGeometryKernel> mkLine;

            // Compute the distane function relative for the target segment

            SegmentSqDistFunc func;
            func.applicableSubsegment = { 0.0, 1.0 };

            const auto distVec = distanceSource - target[0];
            // Segment direction
            const auto dir = target[1] - target[0];
            // Segment squared length
            const auto dirSq = dir * dir;

            const auto projectionNorm = distVec * dir / dirSq; // Projection, normalized to [0,1] interval via segLen
            const auto dist = sqDist(distanceSource, mkLine(target[0], target[1]));

            // Squard distance is dist + segLen^2(x-projection)^2

            // Quadratic function for parameter x \in [0,1], denoting parameterization parameter along segment.
            func.setFromSquaredFunction(dirSq, 0.0, dist, projectionNorm);
            output.addDistanceFunction(func);
        }
        
        template<typename InputIteratorB>
        typename Kernel::NT oneSidedSegmentHausdorff(const MovetkSegment& seg, InputIteratorB poly_b, InputIteratorB poly_b_beyond) const
        {
            movetk_core::MakeSegment<Kernel> mkSeg;
            const auto polyBSize = std::distance(poly_b, poly_b_beyond);

            SegmentDistanceLowerEnvelope envelope;
            envelope.distFuncs.reserve(polyBSize + polyBSize - 1); //For vertices + segments. Can be more when we have splitted funcs.

            // Compute distance functions and compute lower envelope.

            // First compute for points
            for (auto itB = poly_b; itB != poly_b_beyond; ++itB)
            {
                computeDistanceFunc(seg, *itB, envelope);
            }

            // Then for segments
            for (auto itB = poly_b, itBNext = std::next(poly_b); itBNext != poly_b_beyond; ++itB, ++itBNext)
            {
                // Potentially multiple distance functions.
                computeDistanceFunc(seg, mkSeg(*itB, *itBNext), envelope);
            }
            // Compute lower envelope of distance functions
            envelope.compute();
            std::cout << "Lower envelope maxima: ";
            for(const auto& el: envelope)
            {
                std::cout << "[" << el.applicableSubsegment[0] << "," << el.applicableSubsegment[1] << " ] " << el.maxValue() << ",";
            }
            std::cout << std::endl;

            // Get maximum of lower envelope, this is the onesided Hausdorff of this segment to the polyline
            return std::max_element(envelope.begin(), envelope.end())->maxValue();
        }

        template<typename InputIteratorA, typename InputIteratorB>
        typename Kernel::NT oneSidedHausdorff(InputIteratorA poly_a, InputIteratorA poly_a_beyond, InputIteratorB poly_b, InputIteratorB poly_b_beyond) const
        {
            // TODO: degenerate input size.
            NT maxVal = -1;

            auto polyBSize = std::distance(poly_b, poly_b_beyond);

            movetk_core::MakeSegment<Kernel> mkSeg;
            SqDistance sqDist;

            // Compute for points on poly first
            for(auto it = poly_a; it != poly_a_beyond; ++it)
            {
                // Determine closest point
                NT minVal = sqDist(*it, *poly_b);
                for(auto itB= poly_b, itBNext=std::next(poly_b); itBNext !=poly_b_beyond;++itB,++itBNext)
                {
                    minVal = std::min(minVal, sqDist(*it, *itBNext));
                    minVal = std::min(minVal, sqDist(*it, mkSeg(*itB,*itBNext)));
                }
                maxVal = std::max(maxVal, minVal);
            }
            std::cout << "Max val after points: " << std::sqrt(maxVal) << std::endl;
            // Compute for segments on poly_a
            for (auto itA = poly_a, itANext = std::next(poly_a); itANext != poly_a_beyond; ++itA, ++itANext)
            {
                std::vector<SegmentSqDistFunc> distFuncs;
                distFuncs.reserve(polyBSize + polyBSize - 1); //For vertices + segments. Can be more when we have splitted funcs.
                auto seg = mkSeg(*itA, *itANext);

                // Get maximum of lower envelope, this is the onesided Hausdorff of this segment to the polyline
                auto maxMinDist = oneSidedSegmentHausdorff(seg, poly_b, poly_b_beyond);

                // Update the maximum min dist.
                maxVal = std::max(maxVal, maxMinDist);
                std::cout << "New max val after segment: " << std::sqrt(maxVal) << std::endl;
            }

            // We computed with all squared distances, so take the square root.
            return std::sqrt(maxVal);
        }

    public:
        template<typename InputIteratorA, typename InputIteratorB>
        typename Kernel::NT operator()(InputIteratorA poly_a, InputIteratorA poly_a_beyond, InputIteratorB poly_b, InputIteratorB poly_b_beyond) const
        {
            return std::max(
                oneSidedHausdorff(poly_a, poly_a_beyond, poly_b, poly_b_beyond),
                oneSidedHausdorff(poly_b, poly_b_beyond, poly_a, poly_a_beyond)
            );
        }
    };
}
#endif //MOVETK_DISTANCES_H
