#ifndef MOVETK_UTILS_META
#define MOVETK_UTILS_META
#include <tuple>
#include <type_traits>

namespace movetk::utils {
namespace detail {
template <typename TUPLE>
struct index_sequence_for_tuple {
	using type = std::make_index_sequence<std::tuple_size_v<TUPLE>>;
};
template <typename TUPLE>
using index_sequence_for_tuple_t = typename index_sequence_for_tuple<TUPLE>::type;

template <typename TUPLE, typename TYPE, typename INDEX_SEQ>
struct tuple_contains_type_impl;
template <typename TYPE, typename... TS>
struct tuple_contains_type_impl<std::tuple<TS...>, TYPE, std::index_sequence<>> {
	static constexpr bool value = false;
};
template <typename TUPLE, typename TYPE, size_t I, size_t... IS>
struct tuple_contains_type_impl<TUPLE, TYPE, std::index_sequence<I, IS...>> {
	static constexpr bool value = std::is_same_v<TYPE, std::tuple_element_t<I, TUPLE>> ||
	                              tuple_contains_type_impl<TUPLE, TYPE, std::index_sequence<IS...>>::value;
};
template <typename TUPLE_LIKE_TYPE, template <typename...> typename TARGET_CONTAINER, typename... FRONT_TYPES>
struct transfer_types_impl;

template <template <typename...> typename SOURCE_CONTAINER,
          template <typename...>
          typename TARGET_CONTAINER,
          typename... FRONT_TYPES,
          typename... CONTAINER_TYPES>
struct transfer_types_impl<SOURCE_CONTAINER<CONTAINER_TYPES...>, TARGET_CONTAINER, FRONT_TYPES...> {
	using type = TARGET_CONTAINER<FRONT_TYPES..., CONTAINER_TYPES...>;
};
}  // namespace detail


template <typename TUPLE, typename TYPE>
struct tuple_contains_type {
	using tuple_index_sequence = detail::index_sequence_for_tuple_t<TUPLE>;
	static constexpr bool value = detail::tuple_contains_type_impl<TUPLE, TYPE, tuple_index_sequence>::value;
};
template <typename TUPLE_LIKE_TYPE, template <typename...> typename TARGET_CONTAINER, typename... FRONT_TYPES>
using transfer_types = typename detail::transfer_types_impl<TUPLE_LIKE_TYPE, TARGET_CONTAINER, FRONT_TYPES...>::type;
}  // namespace movetk::utils
#endif