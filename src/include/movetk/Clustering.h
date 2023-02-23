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
// Created by Mitra, Aniket on 03/10/2019.
//

#ifndef MOVETK_CLUSTERING_H
#define MOVETK_CLUSTERING_H

#include "movetk/ds/FreeSpaceDiagram.h"
#include "movetk/utils/Iterators.h"

namespace movetk::clustering {

/**
 * @brief Functor for subtrajectory clustering
 * @details Based on https://doi.org/10.1007/978-3-540-92182-0_57
 * @tparam ClusteringTraits The traits for the clustering algorithm
*/
template <class ClusteringTraits>
class SubTrajectoryClustering {
	
private:
	typename ClusteringTraits::Graph graph;
	typename ClusteringTraits::vertex_iterator vi, vi_end;
	typename ClusteringTraits::edge_iterator ei, ei_end;
	typename ClusteringTraits::out_edge_iterator oei, oei_end;
	using vertex_orientation = typename ClusteringTraits::vertex_orientation;
	std::size_t num_rows = 0, num_cols = 0, row_idx = 0, col_idx = 0;
	std::size_t max_length = 0, start_idx = 0, end_idx = 0, _cluster_size = 0;

	template <class VertexIterator>
	bool is_free(VertexIterator first, VertexIterator beyond, std::size_t vertex_id) {
		return std::find_if(first, beyond, [&vertex_id](const auto& vert_id) { return vertex_id == vert_id; }) != beyond;
	}

	template <class T>
	void add_edges(T& cell) {
		std::size_t bottom_left_idx = (num_cols * row_idx) + col_idx;
		std::size_t bottom_right_idx = (num_cols * row_idx) + (col_idx + 1);
		std::size_t top_left_idx = (num_cols * (row_idx + 1)) + col_idx;
		std::size_t top_right_idx = (num_cols * (row_idx + 1)) + (col_idx + 1);

		bool flag_a = is_free(cell.vertices_begin(), cell.vertices_end(), vertex_orientation::TopLeft);

		bool flag_b = is_free(cell.vertices_begin(), cell.vertices_end(), vertex_orientation::BottomLeft);

		if (flag_a && flag_b)
			boost::add_edge(top_left_idx, bottom_left_idx, 0, graph);

		flag_a = is_free(cell.vertices_begin(), cell.vertices_end(), vertex_orientation::TopRight);

		flag_b = is_free(cell.vertices_begin(), cell.vertices_end(), vertex_orientation::TopLeft);


		if (flag_a && flag_b)
			boost::add_edge(top_right_idx, top_left_idx, 0, graph);


		flag_a = is_free(cell.vertices_begin(), cell.vertices_end(), vertex_orientation::TopRight);

		flag_b = is_free(cell.vertices_begin(), cell.vertices_end(), vertex_orientation::BottomRight);

		if (flag_a && flag_b)
			boost::add_edge(top_right_idx, bottom_right_idx, 0, graph);


		flag_a = is_free(cell.vertices_begin(), cell.vertices_end(), vertex_orientation::BottomRight);

		flag_b = is_free(cell.vertices_begin(), cell.vertices_end(), vertex_orientation::BottomLeft);


		if (flag_a && flag_b)
			boost::add_edge(bottom_right_idx, bottom_left_idx, 0, graph);

		flag_a = is_free(cell.vertices_begin(), cell.vertices_end(), vertex_orientation::TopRight);

		flag_b = is_free(cell.vertices_begin(), cell.vertices_end(), vertex_orientation::BottomLeft);


		if (flag_a && flag_b)
			boost::add_edge(top_right_idx, bottom_left_idx, 0, graph);

		if (col_idx == (num_cols - 2)) {
			col_idx = 0;
			row_idx++;
		} else {
			col_idx++;
		}
	}

	void update_edge_label() {
		for (auto it = vi; it != vi_end; ++it) {
			std::tie(oei, oei_end) = boost::out_edges(*it, graph);
			for (; oei != oei_end; ++oei) {
				typename ClusteringTraits::out_edge_iterator target_oei, target_oei_end;
				auto current_label = static_cast<std::size_t>(boost::target(*oei, graph));
				std::tie(target_oei, target_oei_end) = boost::out_edges(boost::target(*oei, graph), graph);
				std::size_t num_elements = std::distance(target_oei, target_oei_end);
				if (num_elements > 0) {
					for (; target_oei != target_oei_end; ++target_oei) {
						if (label[*target_oei] < current_label) {
							current_label = label[*target_oei];
						}
					}
				}
				boost::put(label, *oei, current_label);
			}
		}
	}

	typename ClusteringTraits::vertex_iterator traverse_left(std::size_t x_start,
	                                                         typename ClusteringTraits::vertex_iterator source) {
		if (boost::edge(*source, *(source - 1), graph).second) {
			source = source - 1;
			if ((*source % num_cols) == x_start)
				return source;
			else
				return traverse_left(x_start, source);
		} else if (boost::edge(*source, *(source - 1 - num_cols), graph).second) {
			source = source - 1 - num_cols;
			if ((*source % num_cols) == x_start)
				return source;
			else
				return traverse_left(x_start, source);
		} else
			return source;
	}

	std::size_t sweep_line(std::size_t x_start, std::size_t x_end) {
		std::size_t num_curves = 0;
		auto top_y = vi_end - (num_cols - x_end);
		while (*top_y != x_end) {
			auto left = traverse_left(x_start, top_y);
			bool flag_a = ((*left % num_cols) == x_start);
			bool flag_b = (*top_y == *(left + (x_end - x_start)));
			if (flag_a && !flag_b) {
				top_y = left + (x_end - x_start);
				num_curves++;
			} else if (flag_a && flag_b) {
				top_y = top_y - num_cols;
				num_curves++;
			} else {
				top_y = top_y - num_cols;
			}
		}
		if (*top_y == x_end) {
			auto left = traverse_left(x_start, top_y);
			bool flag_a = ((*left % num_cols) == x_start);
			bool flag_b = (*top_y == *(left + (x_end - x_start)));
			if (flag_a && !flag_b) {
				num_curves++;
			} else if (flag_a && flag_b) {
				num_curves++;
			}
		}
		return num_curves;
	}

public:
	/**
	 * @brief Constructs the clusters for the provided polyline
	 * @tparam InputIterator The iterator type of the polyline coordinate range
	 * @param polyline_first Start of the polyline coordinate range
	 * @param polyline_beyond End of the polyline coordinate range 
	 * @param num_cluster_threshold  
	 * @param radius 
	*/
	template <class InputIterator>
	SubTrajectoryClustering(InputIterator polyline_first,
	                        InputIterator polyline_beyond,
	                        std::size_t num_cluster_threshold,
	                        typename ClusteringTraits::NT radius) {
		typename ClusteringTraits::FreeSpaceDiagram fsd(polyline_first,
		                                                polyline_beyond,
		                                                polyline_first,
		                                                polyline_beyond,
		                                                radius);
		num_cols = std::distance(polyline_first, polyline_beyond);
		num_rows = num_cols;
		for (auto cell : fsd) add_edges(cell);

		std::tie(vi, vi_end) = boost::vertices(graph);
		std::tie(ei, ei_end) = boost::edges(graph);
		update_edge_label();

		std::size_t ls = 0;

		while (ls < (num_cols - 1)) {
			std::size_t lt = ls + 1;
			std::size_t num_clusters = sweep_line(ls, lt);
			std::size_t max_cluster_size = num_clusters;
			if ((num_clusters >= num_cluster_threshold) && (lt < num_cols)) {
				lt++;
				num_clusters = sweep_line(ls, lt);
				max_cluster_size = num_clusters;
				while ((num_clusters >= num_cluster_threshold) && (++lt < num_cols)) {
					max_cluster_size = num_clusters;
					num_clusters = sweep_line(ls, lt);
				}
			}
			if ((lt - ls) > max_length) {
				max_length = lt - ls;
				start_idx = ls;
				end_idx = lt;
				_cluster_size = max_cluster_size;
			}
			ls = lt;
		}
	}


	typename ClusteringTraits::edge_iterator edges_begin() { return ei; }

	typename ClusteringTraits::edge_iterator edges_end() { return ei_end; }

	std::size_t get_subtrajectory_cluster_length() { return max_length; }

	std::pair<std::size_t, std::size_t> get_subtrajectory_indices() { return std::make_pair(start_idx, end_idx); }

	std::size_t get_cluster_size() { return _cluster_size; }

	typename boost::property_map<typename ClusteringTraits::Graph, boost::edge_name_t>::type label =
	    boost::get(boost::edge_name_t(), graph);
};


/*!
 * @struct ClusteringTraits
 * @brief This traits class serves as a collection of types
 * required for clustering with Discrete Frechet Distance
 * @tparam _FreeSpaceDiagram - A model of the Free Space Diagram.
 * For example @refitem movetk_support::FreeSpaceDiagram
 */
template <class _FreeSpaceDiagram>
struct ClusteringTraits {
	typedef _FreeSpaceDiagram FreeSpaceDiagram;
	typedef typename FreeSpaceDiagram::FreeSpaceDiagramTraits FreeSpaceDiagramTraits;
	typedef typename FreeSpaceDiagramTraits::FreeSpaceCellTraits FreeSpaceCellTraits;
	typedef typename FreeSpaceCellTraits::IntersectionTraits IntersectionTraits;
	typedef typename FreeSpaceDiagramTraits::GeometryTraits GeometryTraits;
	typedef typename GeometryTraits::NT NT;
	typedef typename FreeSpaceCellTraits::vertex_orientation vertex_orientation;
	typedef typename boost::property<boost::vertex_index_t, std::size_t> VertexProperty;
	typedef typename boost::property<boost::edge_name_t, std::size_t> EdgeProperty;
	typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS, VertexProperty, EdgeProperty> Graph;
	typedef typename boost::graph_traits<Graph>::vertex_iterator vertex_iterator;
	typedef typename boost::graph_traits<Graph>::edge_iterator edge_iterator;
	typedef typename boost::graph_traits<Graph>::out_edge_iterator out_edge_iterator;
	typedef typename boost::graph_traits<Graph>::edge_descriptor edge_descriptor;
};

}  // namespace movetk::clustering

#endif  // MOVETK_CLUSTERING_H
