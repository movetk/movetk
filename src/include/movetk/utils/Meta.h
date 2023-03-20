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

namespace detail {
template <typename CALLABLE, int... INDICES>
constexpr void for_each_c_impl(CALLABLE&& function, std::integer_sequence<int, INDICES...>) {
	(function(std::integral_constant<int, INDICES>{}), ...);
}
template <int offset, typename T>
struct offset_index_sequence {};
template <int offset, int... INDICES>
struct offset_index_sequence<offset, std::integer_sequence<int, INDICES...>> {
	using type = std::integer_sequence<int, (offset + INDICES)...>;
};
}  // namespace detail
/**
 * @brief Execute a callable for each compile time index
 * @tparam start Start of the range
 * @tparam end End of the integer range
 * @tparam CALLABLE The callable type. Should accept a std::integral_constant<int,value> argument, with
 * value the index.
 * @param function The function  to call
 */
template <int start, int end, typename CALLABLE>
constexpr void for_each_c(CALLABLE&& function) {
	using index_sequence = std::make_integer_sequence<int, end - start>;
	using offset_sequence = typename detail::offset_index_sequence<start, index_sequence>::type;
	detail::for_each_c_impl(std::forward<CALLABLE>(function), offset_sequence{});
}

namespace detail {
template <size_t... INDICES>
struct subtuple_constructor {
	template <typename TUPLE>
	constexpr auto operator()(TUPLE&& tuple) const {
		return std::make_tuple(std::get<INDICES>(std::forward<TUPLE>(tuple))...);
	}
};
}  // namespace detail

template <size_t... INDICES>
constexpr auto sub_tuple = detail::subtuple_constructor<INDICES...>{};

}  // namespace movetk::utils
#endif