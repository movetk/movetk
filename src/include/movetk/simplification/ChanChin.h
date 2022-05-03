#ifndef MOVETK_ALGO_SIMPLIFICATION_CHANCHIN_H
#define MOVETK_ALGO_SIMPLIFICATION_CHANCHIN_H
#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/DistanceInterface.h"
#include "movetk/utils/Iterators.h"
#include "movetk/utils/Requirements.h"
namespace movetk::algo::simplification {

template <class GeometryTraits, class Wedge, typename = movetk::utils::requires_planar_geometry<GeometryTraits>>
class ChanChin {
private:
	using NT = typename GeometryTraits::NT;
	NT eps;
	template <class InputIterator, class OutputIterator>
	void algorithm(InputIterator first, InputIterator beyond, OutputIterator result) {
		if (first == beyond) {
			return;
		}
		InputIterator it = first;
		InputIterator jit;
		std::size_t vertex1, vertex2;
		std::size_t current_index = 0;
		for (; it != (beyond - 1); ++it, ++current_index) {
			Wedge Wi(*it, *(it + 1), eps);
			vertex1 = current_index;
			vertex2 = current_index + 1;
			*result = std::make_pair(vertex1, vertex2);
			if (it == (beyond - 2)) {
				break;
			}
			jit = it + 2;
			vertex2 = vertex1 + 2;
			for (; jit != beyond; ++jit, ++vertex2) {
				Wedge Wj(*it, *jit, eps);
				Wi = Wi * Wj;
				if (Wi.is_empty()) {
					break;
				}
				if (Wi * (*jit)) {
					*result = std::make_pair(vertex1, vertex2);
				}
			}
		}
	}

public:
	ChanChin() = default;

	ChanChin(NT epsilon) : eps(epsilon) {}

	template <class InputIterator,
	          class OutputIterator,
	          typename = movetk::utils::requires_random_access_iterator<InputIterator>,
	          typename = movetk::utils::requires_output_iterator<OutputIterator>,
	          typename = movetk::utils::requires_movetk_point<GeometryTraits, typename InputIterator::value_type>,
	          typename = movetk::utils::requires_pair<typename OutputIterator::value_type>,
	          typename = movetk::utils::requires_size_t<typename OutputIterator::value_type::first_type>,
	          typename = movetk::utils::requires_size_t<typename OutputIterator::value_type::second_type>>
	void operator()(InputIterator first, InputIterator beyond, OutputIterator result) {
		std::vector<typename OutputIterator::value_type> edges1, edges2;
		std::size_t NumElements = std::distance(first, beyond);
		algorithm(first, beyond, movetk::utils::movetk_back_insert_iterator(edges1));

		algorithm(std::make_reverse_iterator(beyond),
		          std::make_reverse_iterator(first),
		          movetk::utils::movetk_back_insert_iterator(edges2));

		for (auto& edge : edges2) {
			edge = std::make_pair(NumElements - 1 - edge.second, NumElements - 1 - edge.first);
		}

		std::sort(std::begin(edges1), std::end(edges1), [](auto& i, auto& j) { return (i.first < j.first); });

		std::sort(std::begin(edges1), std::end(edges1), [](auto& i, auto& j) { return (i.second < j.second); });

		std::sort(std::begin(edges2), std::end(edges2), [](auto& i, auto& j) { return (i.first < j.first); });

		std::sort(std::begin(edges2), std::end(edges2), [](auto& i, auto& j) { return (i.second < j.second); });

		std::set_intersection(std::begin(edges1),
		                      std::end(edges1),
		                      std::begin(edges2),
		                      std::end(edges2),
		                      result,
		                      [](auto& i, auto& j) { return (i.first < j.first) || (i.second < j.second); });
	}
};

}  // namespace movetk::algo::simplification
#endif