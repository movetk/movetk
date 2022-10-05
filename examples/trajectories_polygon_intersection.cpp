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
// Created by Mitra, Aniket on 19/12/2018.
//

#include <cassert>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <GeographicLib/Geohash.hpp>
#include "movetk/geom/CGALTraits.h"
#include "movetk/ds/Tree.h"
#include "movetk/utils/Iterators.h"

/*
* Example to check whether a trajectory intersects a set of polygons
* A time sorted trajectory is read from an input stream which is checked for
* intersection with a given set of polygons.
* The output is the list of polygons with an additional flag indicating whether
* the trajectory intersects the polygon or not.
*/

using namespace std;
using namespace GeographicLib;

template <class NT, size_t dimensions>
struct MyTraits
{
    // type definitions required for algorithm
    typedef movetk_support::CGALTraits<NT, dimensions> CGAL_GeometryBackend;
    typedef movetk_core::MovetkGeometryKernel<typename CGAL_GeometryBackend::Wrapper_CGAL_Geometry> MovetkGeometryKernel; // the geometry traits
    typedef typename MovetkGeometryKernel::MovetkPoint Point;
    typedef typename MovetkGeometryKernel::MovetkPolygon Polygon;
};

template <class T>
class Values
{
private:
    string id;
    std::vector<T> values;

public:
    typedef typename std::vector<T>::const_iterator Iterator;

    Values(string &identifier) : id(identifier) {}

    void operator()(T &value)
    {
        values.push_back(value);
    }

    string &operator()()
    {
        return id;
    }

    Iterator begin()
    {
        return cbegin(values);
    }

    Iterator end()
    {
        return cend(values);
    }
};

template <class GeometryTraits, class OutputIterator>
void make_polygons(string &PolygonFile, OutputIterator iter)
{
    cerr << "Reading in Polygons from: " << PolygonFile << endl;
    ifstream infile;
    typedef typename GeometryTraits::MovetkGeometryKernel::NT NT;
    movetk_support::cast<NT> cast;
    std::array<NT, 2> point;
    movetk_core::MakePoint<typename GeometryTraits::MovetkGeometryKernel> make_point;
    movetk_core::MakePolygon<typename GeometryTraits::MovetkGeometryKernel> make_polygon;
    std::vector<typename GeometryTraits::Point> points;
    vector<string> tokens;
    string PreviousId = "", CurrentId, line;
    size_t linecount = 0;
    size_t id_idx = 0, X_idx = 1, Y_idx = 2;
    NT X, Y;
    infile.open(PolygonFile);

    while (getline(infile, line))
    {
        movetk_support::split(line, movetk_core::movetk_back_insert_iterator(tokens));
        assert(tokens.size() >= 3);
        CurrentId = tokens[id_idx];
        X = cast(tokens[X_idx]);
        Y = cast(tokens[Y_idx]);
        point[0] = X;
        point[1] = Y;
        typename GeometryTraits::Point P = make_point(std::cbegin(point), std::cend(point));
        if (PreviousId == "")
        {
            points.push_back(P);
        }
        else if (PreviousId == CurrentId)
        {
            points.push_back(P);
        }
        else
        {
            assert(points.size() > 0);
            typename GeometryTraits::Polygon poly = make_polygon(std::cbegin(points), std::cend(points));
            *iter = poly;
            iter++;
            points.clear();
            points.push_back(P);
        }
        PreviousId = CurrentId;
        tokens.clear();
        linecount++;
    }
    assert(points.size() > 0);
    typename GeometryTraits::Polygon poly = make_polygon(std::cbegin(points), std::cend(points));
    *iter = poly;
    infile.close();
}

template <class GeometryTraits, class TreeType>
void build_index(TreeType &tree, string &CentroidsFile, size_t resolution)
{
    cerr << "Building Indexes: " << endl;
    assert(resolution > 0);
    typedef typename GeometryTraits::MovetkGeometryKernel::NT NT;
    movetk_support::cast<NT> cast;
    size_t linecount = 0, id_idx = 0, lat_idx = 1, lon_idx = 2;
    vector<string> tokens;
    ifstream infile;
    string line;
    NT Lat, Lon;
    infile.open(CentroidsFile);
    while (getline(infile, line))
    {
        string geohash;
        movetk_support::split(line, movetk_core::movetk_back_insert_iterator(tokens));
        assert(tokens.size() >= 3);
        Lat = cast(tokens[lat_idx]);
        Lon = cast(tokens[lon_idx]);
        Geohash::Forward(Lat, Lon, resolution, geohash);
        cerr << "Lat: " << Lat << ", Lon: " << Lon << ", Resolution:" << resolution << ", geohash: " << geohash << "\n";
        string ID = tokens[id_idx];
        tree.insert(std::begin(geohash), std::end(geohash), ID);
        linecount++;
        tokens.clear();
    }
}

static void show_usage(std::string name)
{
    std::cerr << "Usage: cat Trajectories | " << name << " <option(s)>\n"
              << "Description: Check for intersection of trajectories with polygon\n"
              << "\tuses geohashes for  first level filtering and then a point in polygon test \n"
              << "Trajectory Input Format: <TrajectoryId,Lat,Lon,X,Y> \n"
              << "Polygon Input Format: <Id,Vertex_X,Vertex_Y>\n"
              << "Polygon Centroids Input Format: <Id,Lat,Lon,X,Y>\n"
              << "Output Format: <polygon_id,point_in_polygon(0/1),trajectory_intersects(0/1)> \n"
              << "Pre-Conditions:\n"
              << "\t - Input sorted by Timestamp\n"
              << "\t - Timestamp should be unix timestamp\n"
              << "\t - The resolution should be a discrete non-negative value in (1,13) \n"
              << "Options:\n"
              << "\t-h,--help\t\t\tShow this help message\n"
              << "\t-p,--polygons\t\tFile containing a list of polygons\n"
              << "\t-c,--centroids\t\tFile containing the centroids of the input polygons\n"
              << "\t-r,--resolution\t\tThe minimum resolution of polygon geohash"
              << std::endl;
}

bool parse_input(int argc, char **argv, string &PolygonsFile, string &CentroidsFile, size_t &resolution)
{

    if (argc == 7)
    {
        bool valid = false;
        if (strcmp(argv[1], "-p") == 0 || strcmp(argv[1], "--polygons") == 0 ||
            strcmp(argv[3], "-p") == 0 || strcmp(argv[3], "--polygons") == 0 ||
            strcmp(argv[5], "-p") == 0 || strcmp(argv[5], "--polygons") == 0)
        {
            valid = true;
        }
        if (valid)
        {
            if (strcmp(argv[1], "-r") == 0 || strcmp(argv[1], "--resolution") == 0 ||
                strcmp(argv[3], "-r") == 0 || strcmp(argv[3], "--resolution") == 0 ||
                strcmp(argv[5], "-r") == 0 || strcmp(argv[5], "--resolution") == 0)
            {
                valid = true;
            }
            else
                valid = false;
        }
        if (valid)
        {
            if (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--centroids") == 0 ||
                strcmp(argv[3], "-c") == 0 || strcmp(argv[3], "--centroids") == 0 ||
                strcmp(argv[5], "-c") == 0 || strcmp(argv[5], "--centroids") == 0)
            {
                valid = true;
            }
            else
                valid = false;
        }

        if (valid)
        {
            for (size_t idx = 1; idx <= 5;)
            {
                if (strcmp(argv[idx], "-c") == 0 || strcmp(argv[idx], "--centroids") == 0)
                {
                    CentroidsFile = argv[idx + 1];
                }
                if (strcmp(argv[idx], "-r") == 0 || strcmp(argv[idx], "--resolution") == 0)
                {
                    resolution = stoi(argv[idx + 1]);
                }
                if (strcmp(argv[idx], "-p") == 0 || strcmp(argv[idx], "--polygons") == 0)
                {
                    PolygonsFile = argv[idx + 1];
                }
                idx += 2;
            }
        }

        if (valid)
        {
            return 1;
        }
    }

    show_usage(argv[0]);
    return 0;
}

template <class GeometryTraits, class Node>
struct Probe
{
    typedef typename GeometryTraits::MovetkGeometryKernel::NT NT;
    movetk_support::cast<NT> cast;
    std::array<NT, 2> point;
    size_t lat_idx = 1, lon_idx = 2, x_idx = 3, y_idx = 4;
    NT Lat, Lon;
    string geohash, id;
    size_t PolygonId = 0;
    int MatchedPolygonId = -1;
    movetk_core::MakePoint<typename GeometryTraits::MovetkGeometryKernel> make_point;

    template <class InputIterator, class PolygonsIterator>
    bool operator()(movetk_support::Tree<Node> &tree, size_t &resolution, InputIterator first,
                    PolygonsIterator pfirst, PolygonsIterator pbeyond)
    {
        Lat = cast(*(first + lat_idx));
        Lon = cast(*(first + lon_idx));
        point[0] = cast(*(first + x_idx));
        point[1] = cast(*(first + y_idx));
        PolygonId = 0;
        MatchedPolygonId = -1;
        typename GeometryTraits::Point P = make_point(std::cbegin(point), std::cend(point));
        Geohash::Forward(Lat, Lon, resolution + 1, geohash);
        typename Node::reference MatchedChild = tree.find(begin(geohash), end(geohash));
        if (tree.get_match_size() == resolution)
        {
            PolygonsIterator it = pfirst;
            while (it != pbeyond)
            {
                if (movetk_support::CGAL_Algorithms::point_in_polygon(P, *it))
                {
                    MatchedPolygonId = PolygonId;
                    return true;
                }
                PolygonId++;
                it++;
            }
        }
        return false;
    }

    int operator()()
    {
        return MatchedPolygonId;
    }

    friend void operator<<(std::ostream &out, const Probe &probe)
    {
        out << probe.Lat << "," << probe.Lon << "," << probe.geohash;
    }
};

int main(int argc, char **argv)
{
    cout.precision(numeric_limits<double>::digits10);
    string PolygonFile, CentroidsFile;
    size_t resolution = 0, NumElements = 5;
    string line, PreviousId = "", CurrentId;

    if (parse_input(argc, argv, PolygonFile, CentroidsFile, resolution) == 0)
        return 0;

    typedef MyTraits<long double, 2> GeometryTraits;
    typedef movetk_support::TrieNode<const char, Values<string>> Node;

    movetk_support::Tree<Node> tree(std::make_unique<Node>('a'));
    std::vector<typename GeometryTraits::Polygon> polygons;
    std::vector<std::pair<std::string, size_t>> leaves;
    std::vector<string> tokens;
    std::vector<bool> intersection_flags;
    std::vector<int> polygon_ids;
    size_t id_idx = 0, LineCount = 0;

    make_polygons<GeometryTraits>(PolygonFile, movetk_core::movetk_back_insert_iterator(polygons));

    cerr << "Number of Polygons inserted: " << polygons.size() << "\n";
    for (auto &polygon : polygons)
    {
        cerr << polygon;
        cerr << "\n";
    }

    build_index<GeometryTraits>(tree, CentroidsFile, resolution);
    tree.find(movetk_core::movetk_back_insert_iterator(leaves));
    cerr << "Branch Id, Number of Elements" << endl;
    for (auto &leaf : leaves)
    {
        std::cerr << leaf.first << "," << leaf.second << std::endl;
    }

    while (getline(cin, line))
    {
        movetk_support::split(line, movetk_core::movetk_back_insert_iterator(tokens));
        assert(tokens.size() >= 5);
        Probe<GeometryTraits, Node> probe;
        CurrentId = tokens[id_idx];

        if (PreviousId == "" || (PreviousId == CurrentId))
        {
            intersection_flags.push_back(probe(tree, resolution, std::begin(tokens),
                                               std::begin(polygons), std::end(polygons)));
            polygon_ids.push_back(probe());
        }
        else
        {
            size_t sum = std::accumulate(std::cbegin(intersection_flags), std::cend(intersection_flags), 0);

            if (sum > 0)
            {

                auto PolyIt = std::cbegin(polygon_ids);
                for (const auto &flag : intersection_flags)
                {
                    cout << *PolyIt++ << "," << flag << "," << true << endl;
                }
            }
            else
            {
                for (const auto &flag : intersection_flags)
                {
                    cout << -1 << "," << flag << "," << false << endl;
                }
            }

            intersection_flags.clear();
            polygon_ids.clear();
            intersection_flags.push_back(probe(tree, resolution, std::begin(tokens),
                                               std::begin(polygons), std::end(polygons)));
            polygon_ids.push_back(probe());
        }
        PreviousId = CurrentId;
        tokens.clear();
    }
    size_t sum = std::accumulate(std::cbegin(intersection_flags), std::cend(intersection_flags), 0);
    if (sum > 0)
    {

        auto PolyIt = std::cbegin(polygon_ids);
        for (const auto &flag : intersection_flags)
        {
            cout << *PolyIt++ << "," << flag << "," << true << endl;
        }
    }
    else
    {
        for (const auto &flag : intersection_flags)
        {
            cout << -1 << "," << flag << "," << false << endl;
        }
    }
}
