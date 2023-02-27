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
/**
 * @namespace movetk::ds
 * @brief Contains general purpose and reusable datastructures for use in trajectory processing algorithms
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

/**
 * @brief Convenience traits class for a FreeSpaceCell. Defines the required types
 * based on the provided intersection traits.
 * @tparam _IntersectionTraits Traits of the intersections to compute on the boundary
 * of the FreeSpaceCell.
 */
template <class _IntersectionTraits>
struct FreeSpaceCellTraits {
	enum edge_orientation { Left, Top, Right, Bottom };
	enum vertex_orientation { BottomLeft, TopLeft, TopRight, BottomRight };
	using IntersectionTraits = _IntersectionTraits;
	using GeometryTraits = typename IntersectionTraits::GeometryTraits;
	using Norm = typename IntersectionTraits::Norm;
	using Intersections = std::vector<typename IntersectionTraits::value_type>;
	using OutputIterator = std::back_insert_iterator<Intersections>;
	using Vertices = std::set<std::size_t>;
	using vertex_iterator = typename Vertices::const_iterator;
	using const_iterator = typename Intersections::const_iterator;
};
namespace concepts {
/**
 * @concept FreeSpaceCellTraits
 * @brief Defines the required types to be defined for the traits for a freespace cell.
 */
template <typename TRAITS>
concept FreeSpaceCellTraits = requires(TRAITS t) {
	typename TRAITS::GeometryTraits;
	requires movetk::geom::concepts::BaseKernel<TRAITS>;
	typename TRAITS::OutputIterator;
	typename TRAITS::edge_orientation;
	typename TRAITS::Intersections;
	typename TRAITS::Vertices;
};
}  // namespace concepts

/**
 * @brief Class representing a freespace cell in a freespace diagram.
 * Currently, we only support L_2 based freespace cells.
 * @tparam CellTraits Traits of the cell
 */
template <class CellTraits>
class FreeSpaceCell {
public:
	using FreeSpaceCellTraits = CellTraits;
	using GeometryTraits = typename CellTraits::GeometryTraits;
	using Segment = typename GeometryTraits::MovetkSegment;
	using NT = typename CellTraits::GeometryTraits::NT;
	using Attributes = typename CellTraits::IntersectionTraits::Attributes;

	/**
	 * @brief Constructs a freespace cell from two segments P,Q and a radius.
	 * P and Q form the sides of the freespace rectangle. The radius defines
	 * the maximum distance for which we consider some point \f$p \in P\f$ and \f$q \in Q\f$
	 * to be free.
	 * @param P The first segment
	 * @param Q The second segment
	 * @param radius The radius
	 */
	FreeSpaceCell(const Segment& P, const Segment& Q, typename GeometryTraits::NT radius) {
		auto sphere = make_sphere(Q[0], radius);
		auto init_length = std::distance(std::begin(intersections), std::end(intersections));
		compute_intersections(sphere, P, typename CellTraits::OutputIterator(intersections));
		auto new_length = std::distance(std::begin(intersections), std::end(intersections));
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
		init_length = new_length;

		sphere = make_sphere(P[0], radius);
		compute_intersections(sphere, Q, typename CellTraits::OutputIterator(intersections));
		new_length = std::distance(std::begin(intersections), std::end(intersections));
		update_edge_id(std::begin(intersections) + init_length,
		               std::end(intersections),
		               CellTraits::edge_orientation::Bottom);
		init_length = new_length;
	}

	/**
	 * @brief Returns the start iterator of the vertices that define the freespace region
	 * @return Start iterator
	 */
	typename CellTraits::vertex_iterator vertices_begin() { return std::begin(vertices); }

	/**
	 * @brief Returns the end iterator of the vertices that define the freespace region
	 * @return End iterator
	 */
	typename CellTraits::vertex_iterator vertices_end() { return std::end(vertices); }

	/**
	 * @brief Returns the start iterator of the vertices that define the freespace region
	 * @return Start iterator
	 */
	typename CellTraits::const_iterator begin() { return std::begin(intersections); }

	/**
	 * @brief Returns the end iterator of the vertices that define the freespace region
	 * @return End iterator
	 */
	typename CellTraits::const_iterator end() { return std::end(intersections); }

private:
	geom::MakeSphere<GeometryTraits> make_sphere;
	geom::ComputeIntersections<typename CellTraits::IntersectionTraits> compute_intersections;
	typename CellTraits::Intersections intersections;
	typename CellTraits::Vertices vertices;
	using IntersectionsIterator = std::remove_cvref_t<decltype(intersections.begin())>;

	/**
	 * @brief Assigns the given edge ID to all intersections
	 * @param first Start of the intersections range to assign the edge ID to
	 * @param beyond End of the intersections range to assign the edge ID to
	 * @param edge_id The edge ID to assign
	 */
	void update_edge_id(IntersectionsIterator first, IntersectionsIterator beyond, std::size_t edge_id) {
		std::for_each(first, beyond, [edge_id](auto& val) { std::get<Attributes::ID>(val) = edge_id; });
	}

	// todo(bram): continue here
	void identify_free_vertices(IntersectionsIterator first,
	                            IntersectionsIterator beyond,
	                            const Segment& S,
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

			auto ip_1 = std::get<Attributes::POINT>(*first);
			auto ip_2 = std::get<Attributes::POINT>(*(first + 1));
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
};

/**
 * @brief Convenience class for the traits for the FreeSpaceDiagram
 * @tparam FreeSpaceCell The freespace cell traits to base these traits on
 */
template <class FreeSpaceCell>
struct FreeSpaceDiagramTraits {
	using FreeSpaceCellTraits = typename FreeSpaceCell::FreeSpaceCellTraits;
	using GeometryTraits = typename FreeSpaceCellTraits::GeometryTraits;
	using Cell = FreeSpaceCell;
	using Cells = std::vector<Cell>;
	using Rows = std::vector<Cells>;
	using CellsOutputIterator = std::back_insert_iterator<typename Rows::value_type>;
	using RowsOutputIterator = std::back_insert_iterator<Rows>;
	using iterator = movetk::utils::movetk_grid_iterator<Rows>;
};

/**
 * @brief A freespace diagram for two polylines.
 * @tparam _FreeSpaceDiagramTraits
 */
template <class _FreeSpaceDiagramTraits>
class FreeSpaceDiagram {
private:
	geom::MakeSegment<typename _FreeSpaceDiagramTraits::GeometryTraits> make_segment;
	typename _FreeSpaceDiagramTraits::Rows rows;

public:
	using FreeSpaceDiagramTraits = _FreeSpaceDiagramTraits;
	/**
	 * @brief Construct the freespace diagram for the given polylines
	 * @tparam InputIterator Input iterator type for the polylines
	 * @param polyline_p_first Start of the first polyline range
	 * @param polyline_p_beyond End of the first polyline range
	 * @param polyline_q_first Start of the second polyline range
	 * @param polyline_q_beyond End of the second polyline range
	 * @param radius The maximum distance to consider free
	 */
	template <class InputIterator>
	FreeSpaceDiagram(InputIterator polyline_p_first,
	                 InputIterator polyline_p_beyond,
	                 InputIterator polyline_q_first,
	                 InputIterator polyline_q_beyond,
	                 typename FreeSpaceDiagramTraits::GeometryTraits::NT radius) {
		auto rit = typename FreeSpaceDiagramTraits::RowsOutputIterator(rows);
		auto pit = polyline_p_first;
		auto num_cols = std::distance(polyline_q_first, polyline_q_beyond);
		auto num_rows = std::distance(polyline_p_first, polyline_p_beyond);
		std::size_t row_idx = 0;
		for (; pit != (polyline_p_beyond - 1); ++pit, ++row_idx) {
			auto qit = polyline_q_first;
			typename FreeSpaceDiagramTraits::Rows::value_type cells;
			auto cell_insert_iterator = typename FreeSpaceDiagramTraits::CellsOutputIterator(cells);
			std::size_t col_idx = 0;
			for (; qit != (polyline_q_beyond - 1); ++qit, ++col_idx) {
				// Construct a freespace cell for the two segments of the two polylines
				const auto P = make_segment(*pit, *(pit + 1));
				const auto Q = make_segment(*qit, *(qit + 1));
				typename FreeSpaceDiagramTraits::Cell fsc(P, Q, radius);
				// Add it to the row
				*cell_insert_iterator = fsc;
			}
			rit = cells;
		}
	}
	/**
	 * @brief Returns the begin iterator over the rows of the diagram
	 * @return The begin iterator
	*/
	typename FreeSpaceDiagramTraits::iterator begin() { return typename FreeSpaceDiagramTraits::iterator(rows); }
	/**
	 * @brief Returns the end iterator over the rows of the diagram
	 * @return The end iterator
	 */
	typename FreeSpaceDiagramTraits::iterator end() { return typename FreeSpaceDiagramTraits::iterator(rows, true); }
};


}  // namespace movetk::ds

#endif  // MOVETK_FREESPACEDIAGRAM_H
