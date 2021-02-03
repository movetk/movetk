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


#include <array>
#include "catch2/catch.hpp"

// Defines the geometry kernel
#include "test_includes.h"

#include <movetk/metric/Distances.h>
#include <movetk/metric/HausdorffDistance.h>
#include <movetk/metric/Norm.h>

// Add a message to a Catch2 require
#define REQUIRE_MESSAGE(cond, msg) do { INFO(msg); REQUIRE(cond); } while((void)0, 0)

// The norm to be used in weak Frechet distance computations.
typedef movetk_support::FiniteNorm<MovetkGeometryKernel, 2> Norm;

struct HausdorffTestcase
{
    // Two input lines, given as ipe paths, followed by the expected distance line.
    // Only the length of segment of expectedLine will be used.
    std::string polyA, polyB, expectedLine;
};
std::map<std::string, HausdorffTestcase> hausdorffTestCases
{
    {
        "Simple spike example",
        HausdorffTestcase
        {
            R"IPE(<path>
        96 448 m
        192 448 l
        256 448 l
        320 448 l
        384 448 l
        </path>
        )IPE",
            R"IPE(<path>
        96 448 m
        128 448 l
        144 448 l
        192 448 l
        208 512 l
        224 448 l
        256 448 l
        304 448 l
        384 448 l
        </path>
        )IPE",
        R"IPE(
        <path>
        208 448 m
        208 512 l
        </path>
        )IPE"
        }
    },
    {
        "Weaved grid example",
        HausdorffTestcase
        {
            R"IPE(<ipeselection pos="160 384">
        <path stroke="darkorange" pen="heavier" arrow="normal/normal">
        128 384 m
        320 384 l
        320 352 l
        128 352 l
        128 320 l
        320 320 l
        320 288 l
        128 288 l
        128 256 l
        320 256 l
        320 224 l
        128 224 l
        128 192 l
        320 192 l
        </path>
        </ipeselection>
        )IPE",
        R"IPE(<ipeselection pos="192 368">
        <path stroke="darkred" pen="heavier" arrow="normal/normal">
        128 384 m
        128 192 l
        160 192 l
        160 384 l
        192 384 l
        192 192 l
        224 192 l
        224 384 l
        256 384 l
        256 192 l
        288 192 l
        288 384 l
        320 384 l
        320 192 l
        </path>
        </ipeselection>
        )IPE",
        R"IPE(<ipeselection pos="64 416">
        <path stroke="black">
        64 416 m
        64 400 l
        </path>
        </ipeselection>
        )IPE"
        }
    },
     
    {
        "Single segment and spike example",
        HausdorffTestcase
        {
            R"IPE(
            <ipeselection pos="-80 352">
            <path stroke="black">
            128 320 m
            144 352 l
            160 368 l
            176 416 l
            192 368 l
            208 352 l
            224 320 l
            </path>
            </ipeselection>
            )IPE",
            R"IPE(
            <ipeselection pos="-32 368">
            <path stroke="black">
            128 320 m
            224 320 l
            </path>
            </ipeselection>
            )IPE",
            R"IPE(
            <ipeselection pos="176 384">
            <path matrix="1 0 0 1 32 32" stroke="black">
            176 320 m
            176 416 l
            </path>
            </ipeselection>s
            )IPE"
        }
    },
    {
        "Two segments example",
        HausdorffTestcase
        {
            R"IPE(
            <ipeselection pos="176 736">
            <path stroke="black" arrow="normal/normal">
            128 736 m
            192 736 l
            </path>
            </ipeselection>
            )IPE",
            R"IPE(
            <ipeselection pos="0 768">
            <path stroke="black" arrow="normal/normal">
            98.5874 755.395 m
            157.413 780.605 l
            </path>
            </ipeselection>
            )IPE",
            R"IPE(
            <ipeselection pos="174.706 758.303">
            <path stroke="black" arrow="normal/normal">
            157.413 780.605 m
            192 736 l
            </path>
            </ipeselection>
            )IPE"
        }
    },
    {
        "Degenerates segments example",
        HausdorffTestcase
        {
            R"IPE(
            <ipeselection pos="160 384">
<ipeselection pos="128 384">
<path stroke="darkred" pen="ultrafat">
128 384 m
320 384 l
</path>
</ipeselection>

            )IPE",
            R"IPE(
            <ipeselection pos="176 384">
<path stroke="darkred" pen="ultrafat" arrow="normal/normal">
160 384 m
192 384 l
</path>
</ipeselection>
            )IPE",
            R"IPE(
            <ipeselection pos="240 40s0">
<path stroke="darkred" pen="ultrafat" arrow="normal/normal">
192 400 m
320 400 l
</path>
</ipeselection>
            )IPE"
        }
    },
    {
        "Segment-point example",
        HausdorffTestcase
        {
            R"IPE(
            <ipeselection pos="176 736">
            <path stroke="black" arrow="normal/normal">
            128 736 m
            192 736 l
            </path>
            </ipeselection>
            )IPE",
            R"IPE(
            <ipeselection pos="0 768">
            <path stroke="black" arrow="normal/normal">
            98.5874 755.395 m
            </path>
            </ipeselection>
            )IPE",
            R"IPE(
            <ipeselection pos="139.422 746.917">
            <path stroke="black">
            98.5874 755.395 m
            192 736 l
            </path>
            </ipeselection>
            )IPE"
        }
    },
    {
        "Point-point example",
        HausdorffTestcase
        {
            R"IPE(
            <ipeselection pos="176 736">
            <path stroke="black" arrow="normal/normal">
            128 736 m
            </path>
            </ipeselection>
            )IPE",
            R"IPE(
            <ipeselection pos="0 768">
            <path stroke="black" arrow="normal/normal">
            98.5874 755.395 m
            </path>
            </ipeselection>
            )IPE",
            R"IPE(
            <ipeselection pos="96 736">
            <path stroke="black" arrow="normal/normal">
            98.5874 755.395 m
            128 736 l
            </path>
            </ipeselection>
            )IPE"
        }
    }
};

// Import test_helpers functionality
using namespace test_helpers;

TEST_CASE("Test Hausdorff halfplanes", "[continuous_hausdorff]")
{
    using CHd = movetk_support::ContinuousHausdorffBF<MovetkGeometryKernel, movetk_support::squared_distance_d<MovetkGeometryKernel, Norm>>;
    using Halfplane = typename CHd::HalfPlane;
    movetk_core::MakePoint<MovetkGeometryKernel> mkPoint;
    auto origin = mkPoint({ 0,0 });
    SECTION("Test halfplane distance")
    {
        SECTION("Trivial up normal with offset")
        {
            Halfplane hp{ mkPoint({0,1}) - origin, 5 };
            // Test with points
            REQUIRE(hp.distance(mkPoint({ 2,2 })) == Approx(-3.0));
            REQUIRE(hp.distance(mkPoint({ 2,6 })) == Approx(1.0));
            REQUIRE(hp.distance(mkPoint({ 0,5 })) == Approx(0.0));
            // Same with vector
            REQUIRE(hp.distance(mkPoint({ 2,2 }) - origin) == Approx(-3.0));
            REQUIRE(hp.distance(mkPoint({ 2,6 }) - origin) == Approx(1.0));
            REQUIRE(hp.distance(mkPoint({ 0,5 }) - origin) == Approx(0.0));
        }
        SECTION("Negative normal with offset")
        {
            Halfplane hp{ mkPoint({-1/std::sqrt(2.0),-1/std::sqrt(2.0)}) - origin, 5 };
            // Test vector
            REQUIRE(hp.distance(mkPoint({ 0,0 })) < 0 );
            REQUIRE(hp.distance(mkPoint({ -10,-15 })) > 0);
            REQUIRE(hp.distance(mkPoint({ 0,5 })) < 0);
            REQUIRE(hp.distance(mkPoint({ 0,0 }) - origin) < 0);
            REQUIRE(hp.distance(mkPoint({ -10,-15 }) - origin) > 0);
            REQUIRE(hp.distance(mkPoint({ 0,5 }) - origin) < 0);
        }
    }
    SECTION("Test halfplane intersection")
    {
        SECTION("Trivial up normal with offset")
        {
            Halfplane hp{ mkPoint({0,1}) - origin, 5 };
            // Degenerate case that lies in separating line
            {
                auto inters = hp.intersection(mkPoint({ 0,0 }), mkPoint({ 1,0 }) - origin);
                REQUIRE(!inters.has_value());
            }
            // The line degenerately lies on the separating boundary
            {
                auto inters = hp.intersection(mkPoint({ 0,5 }), mkPoint({ 1,0 }) - origin);
                REQUIRE(inters.has_value());
                // Value is set to 0, though entire line intersects.
                REQUIRE(inters.value() == Approx(0.0));
            }
        }
    }

    SECTION("Test halfplane from segment")
    {
        auto pnt = mkPoint({0,0});
        auto pnt2 = mkPoint({4,4});
        auto dir = pnt2 - pnt;
        auto dirLen = std::sqrt(dir*dir);
        Halfplane h0{ dir / dirLen, (pnt - origin) * dir / dirLen * -1.0 };
        Halfplane h1{ dir / dirLen * -1.0, (pnt2- origin) * dir / dirLen * -1.0 };

        REQUIRE(h0.normal.basis(0)*h0.normal == Approx(h0.normal.basis(1)*h0.normal));
        REQUIRE(h1.normal.basis(0)*h1.normal == Approx(h1.normal.basis(1)*h1.normal));
        auto checkPoint = mkPoint({ 2,3 });
        REQUIRE(h0.distance(checkPoint) > 0);
        REQUIRE(h1.distance(checkPoint) > 0);
    }
}


TEST_CASE("Test Hausdorff line intersection", "[continuous_hausdorff]")
{
    using CHd = movetk_support::ContinuousHausdorffBF<MovetkGeometryKernel, movetk_support::squared_distance_d<MovetkGeometryKernel, Norm>>;
    CHd chd;
    movetk_core::MakePoint<MovetkGeometryKernel> mkPoint;
    movetk_core::MakeSegment<MovetkGeometryKernel> mkSeg;
    auto origin = mkPoint({ 0,0 });
    using NT = typename MovetkGeometryKernel::NT;
    SECTION("Test parallel, non-overlapping lines")
    {
        auto p0 = mkPoint({ 4,0 });
        auto p1 = mkPoint({ 2,0 });
        auto dir0 = mkPoint({ 0,1 }) - origin;
        auto dir1 = mkPoint({ 0,1 }) - origin;
        std::tuple<bool,NT,NT> result = chd.computeIntersection(p0, dir0, p1, dir1);
        REQUIRE(!std::get<0>(result));
    }
    SECTION("Test parallel, overlapping lines")
    {
        auto p0 = mkPoint({ 4,0 });
        auto p1 = mkPoint({ 4,0 });
        auto dir0 = mkPoint({ 0,1 }) - origin;
        auto dir1 = mkPoint({ 0,1 }) - origin;
        bool intersects;
        NT t0, t1;
        std::tie(intersects, t0, t1) = chd.computeIntersection(p0, dir0, p1, dir1);
        REQUIRE(!intersects);
    }
    SECTION("Test intersecting lines")
    {
        {
            auto p0 = mkPoint({ 0,0 });
            auto p1 = mkPoint({ 0,2 });
            auto sq2 = std::sqrt(2.0);
            auto dir0 = mkPoint({ 1 / sq2,1 / sq2 }) - origin;
            auto dir1 = mkPoint({ 1.0 / sq2,-1.0 / sq2 }) - origin;
            bool intersects;
            NT t0, t1;
            std::tie(intersects, t0, t1) = chd.computeIntersection(p0, dir0, p1, dir1);
            REQUIRE(intersects);
            REQUIRE(t0 == Approx(sq2));
            REQUIRE(t1 == Approx(sq2));
        }
        
        {
            auto p0 = mkPoint({ 204,512 });
            auto dir0 = mkPoint({ 224,448 }) - p0;

            auto p1 = mkPoint({ 192,448 });
            auto dir1 = mkPoint({ 256,448 }) - p1;

            bool intersects;
            NT t0, t1;
            std::tie(intersects, t0, t1) = chd.computeIntersection(p0, dir0, p1, dir1);
            REQUIRE(intersects);
            REQUIRE(t0 == Approx(1.0));
            REQUIRE(t1 == Approx(0.5));
        }

    }
    SECTION("Test 45 degrees intersection")
    {
        // Rotations to check
        std::vector<NT> rotations = { 0, 0.1 * PI, 0.3 * PI, 0.5 * PI, 0.8 * PI, 1.3 * PI, 1.7*PI };
        for (const auto& rot : rotations)
        {
            auto target = mkSeg(rotatedPoint(-2, -2, rot), rotatedPoint(2, 2, rot));
            auto seg = mkSeg(rotatedPoint(0, 2, rot), rotatedPoint(0, -2, rot));

            bool intersects;
            NT t0, t1;
            std::tie(intersects, t0, t1) = chd.computeIntersection(target[0], target[1]-target[0], seg[0], seg[1]-seg[0]);
            REQUIRE(intersects);
            REQUIRE(t0 == Approx(0.5));
            REQUIRE(t1 == Approx(0.5));
        }
    }
}

TEST_CASE("Test Hausdorff square distance func computation", "[continuous_hausdorff]")
{
    using CHd = movetk_support::ContinuousHausdorffBF<MovetkGeometryKernel, movetk_support::squared_distance_d<MovetkGeometryKernel, Norm>>;
    using SegSquareDistFunc = CHd::SegmentSqDistFunc;
    CHd chd;
    movetk_core::MakePoint<MovetkGeometryKernel> mkPoint;
    movetk_core::MakeSegment<MovetkGeometryKernel> mkSeg;
    auto origin = mkPoint({ 0,0 });
    using NT = typename MovetkGeometryKernel::NT;
    using LowerEnvelope = typename CHd::SegmentDistanceLowerEnvelope;

    auto rotated = [mkPoint](const NT& x, const NT& y, const NT& phi)
    {
        auto len = std::hypot(x, y);
        return   mkPoint({ x*std::cos(phi) - y * std::sin(phi) , x*std::sin(phi) + y * std::cos(phi) });
    };
    auto rotatedAnOffset = [mkPoint](const NT& x, const NT& y, const NT& phi, const NT& xOff, const NT& yOff)
    {
        auto len = std::hypot(x, y);
        return   mkPoint({ x*std::cos(phi) - y * std::sin(phi) +yOff, x*std::sin(phi) + y * std::cos(phi) +yOff });
    };

    // Rotations to check
    std::vector<NT> rotations = { 0, 0.1 * PI, 0.3 * PI, 0.5 * PI, 0.8 * PI, 1.3 * PI, 1.7*PI };

    SECTION("Test point above segment")
    {
        for(const auto& rot: rotations)
        {
            LowerEnvelope envelope;
            auto p0 = rotated( 0,2, rot);
            auto seg = mkSeg(rotated(-1,0, rot), rotated(1,0 ,rot));
            chd.computeDistanceFunc(seg, p0, envelope);
            REQUIRE(envelope.distFuncs.size() == 1);
            auto func = envelope.distFuncs[0];
            // Applicable to entire segment
            REQUIRE(!func.isInvalid());
            REQUIRE(func.applicableSubsegment[0] == Approx(0));
            REQUIRE(func.applicableSubsegment[1] == Approx(1.0));
            // Squared distances below the point
            REQUIRE(func.value(0.5) == Approx(4.0));
            REQUIRE(func.value(0) == Approx(5.0));
            REQUIRE(func.value(1.0) == Approx(5.0));
            REQUIRE(func.value(1.0) == Approx(func.a + func.b + func.c));
            REQUIRE(func.c == Approx(5.0));

            REQUIRE(func.maxValue() == Approx(5.0));
        }
    }
    SECTION("Test point before segment")
    {
        for (const auto& rot : rotations)
        {
            LowerEnvelope envelope;
            auto p0 = rotated(-2, 2, rot);
            auto seg = mkSeg(rotated(-1, 0, rot), rotated(1, 0, rot));
            chd.computeDistanceFunc(seg, p0, envelope);
            REQUIRE(envelope.distFuncs.size() == 1);
            auto func = envelope.distFuncs[0];
            // Applicable to entire segment
            REQUIRE(!func.isInvalid());
            REQUIRE(func.applicableSubsegment[0] == Approx(0));
            REQUIRE(func.applicableSubsegment[1] == Approx(1.0));
            // Squared distances below the point
            REQUIRE(func.value(0) == Approx(5.0));
            REQUIRE(func.value(0.5) == Approx(8.0));
            REQUIRE(func.value(1.0) == Approx(13.0));
            REQUIRE(func.value(1.0) == Approx(func.a + func.b + func.c));
            REQUIRE(func.c == Approx(5.0));

            REQUIRE(func.maxValue() == Approx(13.0));
        }
    }
    SECTION("Test point after segment")
    {
        for (const auto& rot : rotations)
        {
            LowerEnvelope envelope;
            auto p0 = rotated(3.5, 2, rot);
            auto seg = mkSeg(rotated(-1, 0, rot), rotated(1, 0, rot));
            chd.computeDistanceFunc(seg, p0, envelope);
            REQUIRE(envelope.distFuncs.size() == 1);
            auto func = envelope.distFuncs[0];
            // Applicable to entire segment
            REQUIRE(!func.isInvalid());
            REQUIRE(func.applicableSubsegment[0] == Approx(0));
            REQUIRE(func.applicableSubsegment[1] == Approx(1.0));
            // Squared distances below the point
            REQUIRE(func.value(0) == Approx(2.0*2.0 + 4.5*4.5));
            REQUIRE(func.value(0.5) == Approx(2.0*2.0 + 3.5*3.5));
            REQUIRE(func.value(1.0) == Approx(2.0*2.0 + 2.5*2.5));
            REQUIRE(func.value(1.0) == Approx(func.a + func.b + func.c));
            REQUIRE(func.c == Approx(2.0*2.0 + 4.5*4.5));

            REQUIRE(func.maxValue() == Approx(2.0*2.0 + 4.5*4.5));
        }
    }
    SECTION("Test parallel, overlapping segments")
    {
        for (const auto& rot : rotations)
        {
            LowerEnvelope envelope;
            auto target = mkSeg(rotated(-2, 0, rot), rotated(0, 0, rot));
            auto seg = mkSeg(rotated(-1, 0, rot), rotated(1, 0, rot));
            chd.computeDistanceFunc(target, seg, envelope);
            REQUIRE(envelope.distFuncs.size() == 1);
            auto func = envelope.distFuncs[0];
            
            // Applicable to entire segment
            REQUIRE(!func.isInvalid());
            REQUIRE(func.applicableSubsegment[0] == Approx(0.5));
            REQUIRE(func.applicableSubsegment[1] == Approx(1.0));
            // Squared distances below the point
            REQUIRE(func.value(0.5) == Approx(0));
            REQUIRE(func.value(1.0) == Approx(0));
            REQUIRE(func.c == Approx(0));
            REQUIRE(func.b == Approx(0));
            REQUIRE(func.a == Approx(0));

            REQUIRE(func.maxValue() == Approx(0));
        }
    }
    SECTION("Test parallel, non-overlapping segments")
    {
        for (const auto& rot : rotations)
        {
            LowerEnvelope envelope;
            auto target = mkSeg(rotated(-2, 0, rot), rotated(0, 0, rot));
            auto seg = mkSeg(rotated(-1, 2, rot), rotated(1, 2, rot));
            chd.computeDistanceFunc(target, seg, envelope);
            REQUIRE(envelope.distFuncs.size() == 1);
            auto func = envelope.distFuncs[0];

            // Applicable to entire segment
            REQUIRE(!func.isInvalid());
            REQUIRE(func.applicableSubsegment[0] == Approx(0.5));
            REQUIRE(func.applicableSubsegment[1] == Approx(1.0));
            // Squared distances below the point
            REQUIRE(func.value(0.5) == Approx(4.0));
            REQUIRE(func.value(1.0) == Approx(4.0));
            REQUIRE(func.c == Approx(4.0));
            REQUIRE(func.b == Approx(0.0));
            REQUIRE(func.a == Approx(0.0));

            REQUIRE(func.maxValue() == Approx(4.0));
        }
    }
    SECTION("Test perpendicular, crossing segments")
    {
        SECTION("Non degenerate")
        {
            for (const auto& rot : rotations)
            {
                LowerEnvelope envelope;
                auto target = mkSeg(rotated(-2, 0, rot), rotated(2, 0, rot));
                auto seg = mkSeg(rotated(0, 2, rot), rotated(0, -2, rot));
                chd.computeDistanceFunc(target, seg, envelope);
                REQUIRE(envelope.distFuncs.size() == 1);
                auto dists = envelope.distFuncs;

                REQUIRE(dists.size() == 1);
                // Applicable to first half
                REQUIRE(!dists[0].isInvalid());
                REQUIRE(dists[0].applicableSubsegment[0] == Approx(0.));
                REQUIRE(dists[0].applicableSubsegment[1] == Approx(1.0));
                // 
                // Linear distance function: 2.0 - 2.0/0.5 x = 2.0 - 4.0 x
                // Squared distance function: 4.0 - 16.0 x + 16.0 x^2
                REQUIRE(dists[0].value(0.) == Approx(4.0));
                REQUIRE(dists[0].value(0.5) == Approx(0.0));
                REQUIRE(dists[0].value(1.0) == Approx(4.0));
                REQUIRE(dists[0].a == Approx(16.0));
                REQUIRE(dists[0].c == Approx(4.0));
                REQUIRE(dists[0].b == Approx(-16.0));
                REQUIRE(dists[0].maxValue() == Approx(4.0));
            }
        }

        SECTION("Degenerate intersection of endpoints")
        {
            // Test degenerate case where we are perpendicular to an endpoint
            for (const auto& rot : rotations)
            {
                LowerEnvelope envelope;
                auto target = mkSeg(rotated(-2, -2, rot), rotated(0, -2, rot));
                auto seg = mkSeg(rotated(0, 2, rot), rotated(0, -2, rot));
                chd.computeDistanceFunc(target, seg, envelope);
                REQUIRE(envelope.distFuncs.size() == 1);
                auto dists = envelope.distFuncs;

                REQUIRE(dists.size() == 1);
                // Applicable to first half
                REQUIRE(!dists[0].isInvalid());
                REQUIRE(dists[0].applicableSubsegment[0] == Approx(0.));
                REQUIRE(dists[0].applicableSubsegment[1] == Approx(1.0));
                // 
                // Linear distance function: 2.0 - 2.0 x
                // Squared distance function: 4.0 - 8.0 x + 4.0 x^2
                REQUIRE(dists[0].value(0.) == Approx(4.0));
                REQUIRE(dists[0].value(1.0) == Approx(0.0).margin(1e-10)); // Note that the default approximate margin fails here!
                REQUIRE(dists[0].a == Approx(4.0));
                REQUIRE(dists[0].b == Approx(-8.0));
                REQUIRE(dists[0].c == Approx(4.0));
                REQUIRE(dists[0].maxValue() == Approx(4.0));
            }
        }
        SECTION("Degenerate intersection of target segment with endpoint")
        {
            // Test degenerate case where we are perpendicular to an endpoint
            for (const auto& rot : rotations)
            {
                LowerEnvelope envelope;
                auto target = mkSeg(rotated(-2, -2, rot), rotated(2, -2, rot));
                auto seg = mkSeg(rotated(0, 2, rot), rotated(0, -2, rot));
                chd.computeDistanceFunc(target, seg, envelope);
                REQUIRE(envelope.distFuncs.size() == 1);
                auto dists = envelope.distFuncs;

                REQUIRE(dists.size() == 1);
                // Applicable to first half
                REQUIRE(!dists[0].isInvalid());
                REQUIRE(dists[0].applicableSubsegment[0] == Approx(0.));
                REQUIRE(dists[0].applicableSubsegment[1] == Approx(1.0));
                // 
                // Linear distance function: 2.0 - 2.0/0.5 x = 2.0 - 4.0 x
                // Squared distance function: 4.0 - 16.0 x + 16.0 x^2
                REQUIRE(dists[0].value(0.) == Approx(4.0));
                REQUIRE(dists[0].value(0.5) == Approx(0.0));
                REQUIRE(dists[0].value(1.0) == Approx(4.0));
                REQUIRE(dists[0].a == Approx(16.0));
                REQUIRE(dists[0].c == Approx(4.0));
                REQUIRE(dists[0].b == Approx(-16.0));
                REQUIRE(dists[0].maxValue() == Approx(4.0));
            }
        }
        
    }
    SECTION("Test perpendicular, non-crossing segments")
    {
        for (const auto& rot : rotations)
        {
            LowerEnvelope envelope;
            auto target = mkSeg(rotated(-2, 0, rot), rotated(2, 0, rot));
            auto seg = mkSeg(rotated(3, 2, rot), rotated(3, -2, rot));
            chd.computeDistanceFunc(target, seg, envelope);
            REQUIRE(envelope.distFuncs.size() == 1);
            auto dists = envelope.distFuncs;

            REQUIRE(dists.size() == 1);
            // Applicable to first half
            REQUIRE(!dists[0].isInvalid());
            REQUIRE(dists[0].applicableSubsegment[0] == Approx(0.));
            REQUIRE(dists[0].applicableSubsegment[1] == Approx(1.0));
            // 
            // Linear distance function: 5.0 - 4.0 x
            // Squared distance function: 25.0 - 40.0 x + 16.0 x
            REQUIRE(dists[0].value(0.) == Approx(25.0));
            REQUIRE(dists[0].value(1.0) == Approx(1.0));
            REQUIRE(dists[0].a == Approx(16.0));
            REQUIRE(dists[0].b == Approx(-40.0));
            REQUIRE(dists[0].c == Approx(25.0));
            REQUIRE(dists[0].maxValue() == Approx(25.0));
        }
    }
    SECTION("Test non-perpendicular/parallel, crossing segments")
    {
        SECTION("Segment fully inside slab")
        {
            for (const auto& rot : rotations)
            {
                LowerEnvelope envelope;
                auto target = mkSeg(rotated(-2, -2, rot), rotated(2, 2, rot));
                auto seg = mkSeg(rotated(0, 2, rot), rotated(0, -2, rot));
                chd.computeDistanceFunc(target, seg, envelope);
                REQUIRE(envelope.distFuncs.size() == 1);
                auto dists = envelope.distFuncs;

                REQUIRE(dists.size() == 1);
                // Applicable to first half
                REQUIRE(!dists[0].isInvalid());
                REQUIRE(dists[0].applicableSubsegment[0] == Approx(0.));
                REQUIRE(dists[0].applicableSubsegment[1] == Approx(1.0));
                // 
                // Linear distance function: 2.0 - 2.0/0.5 x = 2.0 - 4.0 x
                // Squared distance function: 4.0 - 16.0 x + 16.0 x^2
                REQUIRE(dists[0].value(0.) == Approx(4.0));
                REQUIRE(dists[0].value(0.5) == Approx(0.0).margin(1e-14)); // Precision error, 1e-16 in local test
                REQUIRE(dists[0].value(1.0) == Approx(4.0));
                REQUIRE(dists[0].a == Approx(16.0));
                REQUIRE(dists[0].c == Approx(4.0));
                REQUIRE(dists[0].b == Approx(-16.0));
                REQUIRE(dists[0].maxValue() == Approx(4.0));
            }
        }
        SECTION("Segment partially inside slab")
        {
            for (const auto& rot : rotations)
            {
                LowerEnvelope envelope;
                auto target = mkSeg(rotated(-3, -3, rot), rotated(3, 3, rot));
                auto seg = mkSeg(rotated(0, 2, rot), rotated(0, -2, rot));
                chd.computeDistanceFunc(target, seg, envelope);
                REQUIRE(envelope.distFuncs.size() == 1);
                auto dists = envelope.distFuncs;

                REQUIRE(dists.size() == 1);
                // Applicable to first half
                REQUIRE(!dists[0].isInvalid());
                REQUIRE(dists[0].applicableSubsegment[0] == Approx(1.0/6.0));
                REQUIRE(dists[0].applicableSubsegment[1] == Approx(5.0/6.0));
                // 
                // Linear distance function: 2.0 at t=1.0/6.0, 0 at t=0.5
                // Squared distance function: 9.0 - 36.0 x + 36.0 x^2
                REQUIRE(dists[0].value(1.0 / 6.0) == Approx(4.0));
                REQUIRE(dists[0].value(0.5) == Approx(0.0).margin(1e-14)); // Precision error, 1e-16 in local test
                REQUIRE(dists[0].value(5.0 / 6.0) == Approx(4.0));
                REQUIRE(dists[0].a == Approx(36.0));
                REQUIRE(dists[0].b == Approx(-36.0));
                REQUIRE(dists[0].c == Approx(9.0));
                REQUIRE(dists[0].maxValue() == Approx(4.0));
            }
        }
    }
    SECTION("Test non-perpendicular/parallel, non-crossing segments")
    {
        for (const auto& rot : rotations)
        {
            LowerEnvelope envelope;
            auto target = mkSeg(rotated(0, -3, rot), rotated(6, 3, rot));
            auto seg = mkSeg(rotated(0, 2, rot), rotated(0, -2, rot));
            chd.computeDistanceFunc(target, seg, envelope);
            REQUIRE(envelope.distFuncs.size() == 1);
            auto dists = envelope.distFuncs;

            REQUIRE(dists.size() == 1);
            // Applicable to subsegment
            REQUIRE(!dists[0].isInvalid());
            REQUIRE(dists[0].applicableSubsegment[0] == Approx(1.0 / 6.0));
            REQUIRE(dists[0].applicableSubsegment[1] == Approx(5.0 / 6.0));
            // Linear distance function: 1.0 at t=1.0/6.0, 5 at t=5.0/6.0
            // so 0 + 6.0 x
            // Squared distance function: 36.0 x^2
            REQUIRE(dists[0].value(1.0 / 6.0) == Approx(1.0));
            REQUIRE(dists[0].value(5.0 / 6.0) == Approx(25.0).margin(1e-14)); // Precision error, 1e-16 in local test
            REQUIRE(dists[0].a == Approx(36.0));
            REQUIRE(dists[0].b == Approx(0.0).margin(1e-14));
            REQUIRE(dists[0].c == Approx(0.0).margin(1e-14));
            REQUIRE(dists[0].maxValue() == Approx(25.0));
        }
    }
}
TEST_CASE("Test Hausdorff square distance func intersection", "[continuous_hausdorff]")
{
    using CHd = movetk_support::ContinuousHausdorffBF<MovetkGeometryKernel, movetk_support::squared_distance_d<MovetkGeometryKernel, Norm>>;
    using SegSquareDistFunc = CHd::SegmentSqDistFunc;

    auto setCenteredParabola = [](SegSquareDistFunc& target, NT scale, NT xCenter, NT offset)
    {
        // scale * (x - xCenter)^2 + offset
        // scale x^2 - 2 scale xCenter x + scale xCenter^2 + offset
        target.a = scale;
        target.b = -2.0 * scale * xCenter;
        target.c = scale * xCenter * xCenter + offset;
    };
    SECTION("Non intersecting functions")
    {
        SegSquareDistFunc dist0, dist1;
        dist0.applicableSubsegment = { 0,1 };
        dist1.applicableSubsegment = { 0,1 };
        setCenteredParabola(dist0, 5,1,10);
        setCenteredParabola(dist1, 2,0,4);

        auto intersections = dist0.computeIntersectionsUnbounded(dist1);
        REQUIRE(intersections.count == 0);
    }
    SECTION("Intersecting functions single point")
    {
        SegSquareDistFunc dist0, dist1;
        dist0.applicableSubsegment = { 0,1 };
        dist1.applicableSubsegment = { 0,1 };
        setCenteredParabola(dist0, 2, 1, 3);
        setCenteredParabola(dist1, 2, 0, 4);

        auto intersections = dist0.computeIntersectionsUnbounded(dist1);
        REQUIRE(intersections.count == 1);
        REQUIRE(intersections.intersectionPar[0] == Approx(0.25));
    }
    SECTION("Intersecting functions two points")
    {
        SegSquareDistFunc dist0, dist1;
        dist0.applicableSubsegment = { 0,1 };
        dist1.applicableSubsegment = { 0,1 };
        setCenteredParabola(dist0, 5, 1, 10);
        setCenteredParabola(dist1, 2, 2, 20);

        auto intersections = dist0.computeIntersectionsUnbounded(dist1);
        REQUIRE(intersections.count == 2);
        NT fact = 1.0 / 3.0;
        NT offset = 2.0 * std::sqrt(10);
        std::cout << intersections.intersectionPar[0] << std::endl;
        std::cout << intersections.intersectionPar[1] << std::endl;
        REQUIRE(intersections.intersectionPar[0] == Approx(fact*(1.0 - offset)) );
        REQUIRE(intersections.intersectionPar[1] == Approx(fact*(1.0 + offset)) );
    }
}

TEST_CASE("Test Hausdorff lower envelope computation", "[continuous_hausdorff]")
{
    using CHd = movetk_support::ContinuousHausdorffBF<MovetkGeometryKernel, movetk_support::squared_distance_d<MovetkGeometryKernel, Norm>>;
    using SegSquareDistFunc = CHd::SegmentSqDistFunc;
    CHd chd;
    movetk_core::MakePoint<MovetkGeometryKernel> mkPoint;
    movetk_core::MakeSegment<MovetkGeometryKernel> mkSeg;
    auto origin = mkPoint({ 0,0 });
    using NT = typename MovetkGeometryKernel::NT;
    using LowerEnvelope = typename CHd::SegmentDistanceLowerEnvelope;

    auto rotated = [mkPoint](const NT& x, const NT& y, const NT& phi)
    {
        auto len = std::hypot(x, y);
        return   mkPoint({ x*std::cos(phi) - y * std::sin(phi) , x*std::sin(phi) + y * std::cos(phi) });
    };
    auto rotatedAnOffset = [mkPoint](const NT& x, const NT& y, const NT& phi, const NT& xOff, const NT& yOff)
    {
        auto len = std::hypot(x, y);
        return   mkPoint({ x*std::cos(phi) - y * std::sin(phi) + yOff, x*std::sin(phi) + y * std::cos(phi) + yOff });
    };

    auto setCenteredParabola = [](SegSquareDistFunc& target, NT scale, NT xCenter, NT offset)
    {
        // scale * (x - xCenter)^2 + offset
        // scale x^2 - 2 scale xCenter x + scale xCenter^2 + offset
        target.a = scale;
        target.b = -2.0 * scale * xCenter;
        target.c = scale * xCenter * xCenter + offset;
    };
    auto createCenterdParabola = [](std::initializer_list<NT> els)
    {
        SegSquareDistFunc target;
        // scale * (x - xCenter)^2 + offset
        // scale x^2 - 2 scale xCenter x + scale xCenter^2 + offset
        NT scale = *els.begin();
        NT xCenter = *std::next(els.begin(),1);
        NT offset = *std::next(els.begin(), 2);
        target.a = scale;
        target.b = -2.0 * scale * xCenter;
        target.c = scale * xCenter * xCenter + offset;
        target.applicableSubsegment = { 0,1 };
        return target;
    };

    auto approxSameFunction = [](const SegSquareDistFunc& f0, const SegSquareDistFunc& f1, std::optional<NT> margin={})
    {
        if(margin.has_value())
        {
            REQUIRE(f0.a == Approx(f1.a).margin(margin.value()));
            REQUIRE(f0.b == Approx(f1.b).margin(margin.value()));
            REQUIRE(f0.c == Approx(f1.c).margin(margin.value()));
        }
        else
        {
            REQUIRE(f0.a == Approx(f1.a));
            REQUIRE(f0.b == Approx(f1.b));
            REQUIRE(f0.c == Approx(f1.c));
        }
    };

    SECTION("Compute lower envelope for two distance functions that intersect")
    {
        SegSquareDistFunc dist0;
        setCenteredParabola(dist0, 2.0, 1.0, 3.0);
        dist0.applicableSubsegment = { 0,1 };

        SegSquareDistFunc dist1;
        setCenteredParabola(dist1, 2.0, 0.0, 4.0);
        dist1.applicableSubsegment = { 0,1 };

        LowerEnvelope output;
        output.distFuncs = { dist1,dist0 };
        output.compute();
        REQUIRE(output.size() == 2);
        auto first = output.begin();
        auto second = std::next(first);
        // Check ranges
        REQUIRE(first->applicableSubsegment[0] == Approx(0));
        REQUIRE(first->applicableSubsegment[1] == Approx(0.25));
        REQUIRE(second->applicableSubsegment[0] == Approx(0.25));
        REQUIRE(second->applicableSubsegment[1] == Approx(1.0));
        // Check funcs
        approxSameFunction(*first, dist1);
        approxSameFunction(*second, dist0);
    }
    SECTION("Compute lower envelope for two distance functions that intersect twice")
    {
        SegSquareDistFunc dist0 = createCenterdParabola({ 30.0, 0.5, 10 });
        SegSquareDistFunc dist1 = createCenterdParabola({ 15, 0.4, 11 });

        LowerEnvelope output;
        output.distFuncs = { dist1,dist0 };
        output.compute();
        REQUIRE(output.size() == 3);
        auto first = output.begin();
        auto second = std::next(first);
        auto third = std::next(first,2);
        // Check ranges
        REQUIRE(first->applicableSubsegment[0] == Approx(0));
        REQUIRE(first->applicableSubsegment[1] == Approx(0.305608));
        REQUIRE(second->applicableSubsegment[0] == Approx(0.305608));
        REQUIRE(second->applicableSubsegment[1] == Approx(0.894392));
        REQUIRE(third->applicableSubsegment[0] == Approx(0.894392));
        REQUIRE(third->applicableSubsegment[1] == Approx(1.0));
        // Check funcs
        approxSameFunction(*first, dist1);
        approxSameFunction(*second, dist0);
        approxSameFunction(*third, dist1);
    }
    SECTION("Compute lower envelope for two distance functions that do not intersect")
    {
        SegSquareDistFunc dist0;
        setCenteredParabola(dist0, 5.0, 1.0, 10.0);
        dist0.applicableSubsegment = { 0,1 };

        SegSquareDistFunc dist1;
        setCenteredParabola(dist1, 2.0, 2.0, 0.0);
        dist1.applicableSubsegment = { 0,1 };

        LowerEnvelope output;
        output.distFuncs = { dist1,dist0 };
        output.compute();
        REQUIRE(output.size() == 1);
        auto first = output.begin();
        // Check ranges
        REQUIRE(first->applicableSubsegment[0] == Approx(0));
        REQUIRE(first->applicableSubsegment[1] == Approx(1.0));
        // Check funcs
        approxSameFunction(*first, dist1);
    }
    SECTION("Compute lower envelope for three distance functions that intersect")
    {
        SegSquareDistFunc dist0 = createCenterdParabola({ 5.0, 1.0, 2.4 });
        SegSquareDistFunc dist1 = createCenterdParabola({ 2.0, 2.0, 0 });
        SegSquareDistFunc dist2 = createCenterdParabola({ 3.0, 0.2, 5.0 });

        LowerEnvelope output;
        output.distFuncs = { dist0,dist1,dist2 };
        output.compute();
        REQUIRE(output.size() == 3);
        auto first = output.begin();
        auto second = std::next(output.begin());
        auto third = std::next(output.begin(),2);
        // Check ranges
        REQUIRE(first->applicableSubsegment[0] == Approx(0));
        REQUIRE(first->applicableSubsegment[1] == Approx(0.276462));
        REQUIRE(second->applicableSubsegment[0] == Approx(0.276462));
        REQUIRE(second->applicableSubsegment[1] == Approx(0.891107));
        REQUIRE(third->applicableSubsegment[0] == Approx(0.891107));
        REQUIRE(third->applicableSubsegment[1] == Approx(1.0));

        // Check funcs
        approxSameFunction(*first, dist2);
        approxSameFunction(*second, dist0);
        approxSameFunction(*third, dist1);
    }
}

std::map<std::string, HausdorffTestcase> singleSidedHausdorffTestCases
{
    {
"Weaved grid single edge example",
HausdorffTestcase
{
        R"IPE(<ipeselection pos="192 368">
        <path stroke="darkred" pen="heavier" arrow="normal/normal">
        128 384 m
        128 192 l
        </path>
        </ipeselection>
        )IPE",
    R"IPE(<ipeselection pos="160 384">
        <path stroke="darkorange" pen="heavier" arrow="normal/normal">
        128 384 m
        320 384 l
        320 352 l
        128 352 l
        128 320 l
        320 320 l
        320 288 l
        128 288 l
        128 256 l
        320 256 l
        320 224 l
        128 224 l
        128 192 l
        320 192 l
        </path>
        </ipeselection>
        )IPE",
        R"IPE(
        <ipeselection pos="352 288">
        <path stroke="darkred" pen="ultrafat" arrow="normal/normal">
        192 400 m
        208 400 l
        </path>
        </ipeselection>
        )IPE"
}
    },
     {
"Weaved grid single short edge example",
HausdorffTestcase
{
        R"IPE(<ipeselection pos="144 192">
<path stroke="darkred" pen="ultrafat" arrow="normal/normal">
128 192 m
160 192 l
</path>
</ipeselection>
        )IPE",
    R"IPE(<ipeselection pos="160 384">
        <path stroke="darkorange" pen="heavier" arrow="normal/normal">
        128 384 m
        320 384 l
        320 352 l
        128 352 l
        128 320 l
        320 320 l
        320 288 l
        128 288 l
        128 256 l
        320 256 l
        320 224 l
        128 224 l
        128 192 l
        320 192 l
        </path>
        </ipeselection>
        )IPE",
        R"IPE(
        <ipeselection pos="352 288">
        <path stroke="darkred" pen="ultrafat" arrow="normal/normal">
        192 400 m
        192 400 l
        </path>
        </ipeselection>
        )IPE"
}
    }
};

TEST_CASE("Check single-sided Hausdorff distance", "[continuous_hausdorff]")
{
    using CHd = movetk_support::ContinuousHausdorffBF<MovetkGeometryKernel, movetk_support::squared_distance_d<MovetkGeometryKernel, Norm>>;
    using SqDistance = movetk_support::squared_distance_d<MovetkGeometryKernel, Norm>;
    // Initialize algorithm.
    CHd continuousHausdorff;

    // Distance computer for expected distance
    SqDistance sqDist;

    for (const auto& pair : singleSidedHausdorffTestCases)
    {
        HausdorffTestcase tc = pair.second;
        SECTION(pair.first)
        {

            INFO(std::string("Test start : ") + pair.first);
            std::cout << std::string("Test start : ") + pair.first << std::endl;
            // Read input
            std::vector<MovetkGeometryKernel::MovetkPoint> polyA;
            test_helpers::parseIpePath(tc.polyA, polyA);
            std::vector<MovetkGeometryKernel::MovetkPoint> polyB;
            test_helpers::parseIpePath(tc.polyB, polyB);
            // Expected distance element
            std::vector<MovetkGeometryKernel::MovetkPoint> expectedDistLine;
            test_helpers::parseIpePath(tc.expectedLine, expectedDistLine);
            // Compute expected distance
            auto expectedDist = std::sqrt(sqDist(expectedDistLine[0], expectedDistLine[1]));

            auto dist = continuousHausdorff.oneSidedHausdorff(polyA.begin(), polyA.end(), polyB.begin(), polyB.end());
            REQUIRE(dist == Approx(expectedDist).margin(MOVETK_EPS));
        }
    }
}


TEST_CASE( "Check if polyline Hausdorff distance is correct", "[continuous_hausdorff]" ) 
{
    using CHd = movetk_support::ContinuousHausdorffBF<MovetkGeometryKernel, movetk_support::squared_distance_d<MovetkGeometryKernel, Norm>>;
    using SqDistance = movetk_support::squared_distance_d<MovetkGeometryKernel, Norm>;
    // Initialize algorithm.
    CHd continuousHausdorff;
    
    // Distance computer for expected distance
    SqDistance sqDist;

    for(const auto& pair: hausdorffTestCases)
    {
        HausdorffTestcase tc = pair.second;
        SECTION(pair.first)
        {

            INFO(std::string("Test start : ") + pair.first);
            std::cout << std::string("Test start : ") + pair.first << std::endl;
            // Read input
            std::vector<MovetkGeometryKernel::MovetkPoint> polyA;
            test_helpers::parseIpePath(tc.polyA, polyA);
            std::vector<MovetkGeometryKernel::MovetkPoint> polyB;
            test_helpers::parseIpePath(tc.polyB, polyB);
            // Expected distance element
            std::vector<MovetkGeometryKernel::MovetkPoint> expectedDistLine;
            test_helpers::parseIpePath(tc.expectedLine, expectedDistLine);
            // Compute expected distance
            auto expectedDist = std::sqrt(sqDist(expectedDistLine[0], expectedDistLine[1]));

            auto dist = continuousHausdorff(polyA.begin(), polyA.end(), polyB.begin(), polyB.end());
            REQUIRE(dist == Approx(expectedDist).margin(MOVETK_EPS));
        }
    }
}