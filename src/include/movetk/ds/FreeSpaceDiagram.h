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
// Created by Mitra, Aniket on 2019-09-08.
//

#ifndef MOVETK_FREESPACEDIAGRAM_H
#define MOVETK_FREESPACEDIAGRAM_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/property_map/property_map.hpp>

#include "movetk/geom/GeometryInterface.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/Requirements.h"

namespace movetk::ds {
// based on https://doi.org/10.1142/S0218195995000064

template <class _IntersectionTraits>
struct FreeSpaceCellTraits {
	enum edge_orientation { Left, Top, Right, Bottom };
	enum vertex_orientation { BottomLeft, TopLeft, TopRight, BottomRight };
	typedef _IntersectionTraits IntersectionTraits;
	typedef typename IntersectionTraits::GeometryTraits GeometryTraits;
	typedef typename IntersectionTraits::Norm Norm;
	typedef std::vector<typename IntersectionTraits::value_type> Intersections;
	typedef movetk::utils::movetk_back_insert_iterator<Intersections> OutputIterator;
	typedef std::set<std::size_t> Vertices;
	typedef typename Vertices::const_iterator vertex_iterator;
	typedef typename Intersections::const_iterator const_iterator;
};

/*!
 *
 * @tparam CellTraits
 *
 */
template <class CellTraits>
class FreeSpaceCell {
private:
	geom::MakeSphere<typename CellTraits::GeometryTraits> make_sphere;
	geom::ComputeIntersections<typename CellTraits::IntersectionTraits> compute_intersections;
	typename CellTraits::Intersections intersections;
	typename CellTraits::Vertices vertices;
	typedef typename CellTraits::GeometryTraits::NT NT;
	using Attributes = typename CellTraits::IntersectionTraits::Attributes;

	template <class InputIterator>
	void update_edge_id(InputIterator first, InputIterator beyond, std::size_t edge_id) {
		auto it = first;
		while (it != beyond) {
			std::get<Attributes::ID>(*it) = edge_id;
			it++;
		}
	}

	template <class InputIterator>
	void identify_free_vertices(InputIterator first,
	                            InputIterator beyond,
	                            typename CellTraits::GeometryTraits::MovetkSegment S,
	                            std::size_t edge_id) {
		if ((edge_id != CellTraits::edge_orientation::Left) && (edge_id != CellTraits::edge_orientation::Right))
			return;

		std::size_t num_intersections = std::distance(first, beyond);

		if (num_intersections == 1) {
			auto value = std::get<Attributes::SQUARED_RATIO>(*first);
			if ((value > 0) && (value < 1))
				return;
			if (value == 1) {
				if (edge_id == CellTraits::edge_orientation::Left)
					vertices.insert(CellTraits::vertex_orientation::TopLeft);
				if (edge_id == CellTraits::edge_orientation::Right)
					vertices.insert(CellTraits::vertex_orientation::TopRight);
			}
			if (value == 0) {
				if (edge_id == CellTraits::edge_orientation::Left)
					vertices.insert(CellTraits::vertex_orientation::BottomLeft);
				if (edge_id == CellTraits::edge_orientation::Right)
					vertices.insert(CellTraits::vertex_orientation::BottomRight);
			}
		}

		if (num_intersections == 2) {
			auto sign_ip_1 = std::get<Attributes::SIGN_DISCRIMINANT>(*first);
			auto sign_ip_2 = std::get<Attributes::SIGN_DISCRIMINANT>(*(first + 1));

			if ((sign_ip_1 == 1) && (sign_ip_2 == 1))
				return;

			typename CellTraits::GeometryTraits::MovetkPoint ip_1 = std::get<Attributes::POINT>(*first);
			typename CellTraits::GeometryTraits::MovetkPoint ip_2 = std::get<Attributes::POINT>(*(first + 1));
			std::size_t orientation_ip_1 = CellTraits::vertex_orientation::BottomLeft;
			std::size_t orientation_ip_2 = CellTraits::vertex_orientation::BottomRight;
			if (sign_ip_1 == -1) {
				typename CellTraits::GeometryTraits::MovetkVector v1 = S[0] - ip_1;
				typename CellTraits::GeometryTraits::MovetkVector v2 = S[1] - ip_1;
				if ((v1 * v1) < MOVETK_EPS) {
					if (edge_id == CellTraits::edge_orientation::Left)
						orientation_ip_1 = CellTraits::vertex_orientation::BottomLeft;
					if (edge_id == CellTraits::edge_orientation::Right)
						orientation_ip_1 = CellTraits::vertex_orientation::BottomRight;
				} else if ((v2 * v2) < MOVETK_EPS) {
					if (edge_id == CellTraits::edge_orientation::Left)
						orientation_ip_1 = CellTraits::vertex_orientation::TopLeft;
					if (edge_id == CellTraits::edge_orientation::Right)
						orientation_ip_1 = CellTraits::vertex_orientation::TopRight;
				}
			}
			if (sign_ip_2 == -1) {
				typename CellTraits::GeometryTraits::MovetkVector v1 = S[0] - ip_2;
				typename CellTraits::GeometryTraits::MovetkVector v2 = S[1] - ip_2;
				if ((v1 * v1) < MOVETK_EPS) {
					if (edge_id == CellTraits::edge_orientation::Left)
						orientation_ip_2 = CellTraits::vertex_orientation::BottomLeft;
					if (edge_id == CellTraits::edge_orientation::Right)
						orientation_ip_2 = CellTraits::vertex_orientation::BottomRight;
				} else if ((v2 * v2) < MOVETK_EPS) {
					if (edge_id == CellTraits::edge_orientation::Left)
						orientation_ip_2 = CellTraits::vertex_orientation::TopLeft;
					if (edge_id == CellTraits::edge_orientation::Right)
						orientation_ip_2 = CellTraits::vertex_orientation::TopRight;
				}
			}

			if ((sign_ip_1 == -1) && (sign_ip_2 == -1)) {
				if (orientation_ip_1 == orientation_ip_2) {
					return;
				} else {
					vertices.insert(orientation_ip_1);
					vertices.insert(orientation_ip_2);
				}
			} else if ((sign_ip_1 == -1) && (sign_ip_2 != -1)) {
				vertices.insert(orientation_ip_1);
			} else if ((sign_ip_1 != -1) && (sign_ip_2 == -1)) {
				vertices.insert(orientation_ip_2);
			}
		}
	}

public:
	typedef CellTraits FreeSpaceCellTraits;

	/*!
	 *
	 * @param P
	 * @param Q
	 * @param radius
	 */
	FreeSpaceCell(typename CellTraits::GeometryTraits::MovetkSegment P,
	              typename CellTraits::GeometryTraits::MovetkSegment Q,
	              typename CellTraits::GeometryTraits::NT radius) {
		typename CellTraits::GeometryTraits::MovetkSphere sphere = make_sphere(Q[0], radius);
		std::size_t init_length = std::distance(std::begin(intersections), std::end(intersections));
		compute_intersections(sphere, P, typename CellTraits::OutputIterator(intersections));
		std::size_t new_length = std::distance(std::begin(intersections), std::end(intersections));
		update_edge_id(std::begin(intersections) + init_length,
		               std::end(intersections),
		               CellTraits::edge_orientation::Left);
		identify_free_vertices(std::begin(intersections) + init_length,
		                       std::end(intersections),
		                       P,
		                       CellTraits::edge_orientation::Left);
		init_length = new_length;

		sphere = make_sphere(P[1], radius);
		compute_intersections(sphere, Q, typename CellTraits::OutputIterator(intersections));
		new_length = std::distance(std::begin(intersections), std::end(intersections));
		update_edge_id(std::begin(intersections) + init_length, std::end(intersections), CellTraits::edge_orientation::Top);
		init_length = new_length;

		sphere = make_sphere(Q[1], radius);
		compute_intersections(sphere, P, typename CellTraits::OutputIterator(intersections));
		new_length = std::distance(std::begin(intersections), std::end(intersections));
		update_edge_id(std::begin(intersections) + init_length,
		               std::end(intersections),
		               CellTraits::edge_orientation::Right);
		identify_free_vertices(std::begin(intersections) + init_length,
		                       std::end(intersections),
		                       P,
		                       CellTraits::edge_orientation::Right);
		;
		init_length = new_length;

		sphere = make_sphere(P[0], radius);
		compute_intersections(sphere, Q, typename CellTraits::OutputIterator(intersections));
		new_length = std::distance(std::begin(intersections), std::end(intersections));
		update_edge_id(std::begin(intersections) + init_length,
		               std::end(intersections),
		               CellTraits::edge_orientation::Bottom);
		init_length = new_length;
	}

	/*!
	 *
	 * @return
	 */
	typename CellTraits::vertex_iterator vertices_begin() { return std::begin(vertices); }

	/*!
	 *
	 * @return
	 */
	typename CellTraits::vertex_iterator vertices_end() { return std::end(vertices); }

	/*!
	 *
	 * @return
	 */
	typename CellTraits::const_iterator begin() { return std::begin(intersections); }

	/*!
	 *
	 * @return
	 */
	typename CellTraits::const_iterator end() { return std::end(intersections); }
};


template <class FreeSpaceCell>
struct FreeSpaceDiagramTraits {
	typedef typename FreeSpaceCell::FreeSpaceCellTraits FreeSpaceCellTraits;
	typedef typename FreeSpaceCellTraits::GeometryTraits GeometryTraits;
	typedef FreeSpaceCell Cell;
	typedef std::vector<Cell> Cells;
	typedef std::vector<Cells> Rows;
	typedef movet::utils::movetk_back_insert_iterator<typename Rows::value_type> CellsOutputIterator;
	typedef movet::utils::movetk_back_insert_iterator<Rows> RowsOutputIterator;
	typedef movet::utils::movetk_grid_iterator<Rows> iterator;
};

template <class _FreeSpaceDiagramTraits>
class FreeSpaceDiagram {
private:
	geom::MakeSegment<typename _FreeSpaceDiagramTraits::GeometryTraits> make_segment;
	typename _FreeSpaceDiagramTraits::Rows rows;

public:
	typedef _FreeSpaceDiagramTraits FreeSpaceDiagramTraits;
	template <class InputIterator>
	FreeSpaceDiagram(InputIterator polyline_p_first,
	                 InputIterator polyline_p_beyond,
	                 InputIterator polyline_q_first,
	                 InputIterator polyline_q_beyond,
	                 typename FreeSpaceDiagramTraits::GeometryTraits::NT radius) {
		auto rit = typename FreeSpaceDiagramTraits::RowsOutputIterator(rows);
		auto pit = polyline_p_first;
		std::size_t num_cols = std::distance(polyline_q_first, polyline_q_beyond);
		std::size_t num_rows = std::distance(polyline_p_first, polyline_p_beyond);
		std::size_t row_idx = 0;
		while (pit != (polyline_p_beyond - 1)) {
			auto qit = polyline_q_first;
			typename FreeSpaceDiagramTraits::Rows::value_type cells;
			auto cit = typename FreeSpaceDiagramTraits::CellsOutputIterator(cells);
			std::size_t col_idx = 0;
			while (qit != (polyline_q_beyond - 1)) {
				typename FreeSpaceDiagramTraits::GeometryTraits::MovetkSegment P = make_segment(*pit, *(pit + 1));
				typename FreeSpaceDiagramTraits::GeometryTraits::MovetkSegment Q = make_segment(*qit, *(qit + 1));
				typename FreeSpaceDiagramTraits::Cell fsc(P, Q, radius);
				cit = fsc;
				qit++;
				col_idx++;
			}
			rit = cells;
			row_idx++;
			pit++;
		}
	}
	typename FreeSpaceDiagramTraits::iterator begin() { return typename FreeSpaceDiagramTraits::iterator(rows); }

	typename FreeSpaceDiagramTraits::iterator end() { return typename FreeSpaceDiagramTraits::iterator(rows, true); }
};


}  // namespace movetk::ds

#endif  // MOVETK_FREESPACEDIAGRAM_H
