#ifndef MOVETK_EXAMPLES_EXAMPLESETUP_H
#define MOVETK_EXAMPLES_EXAMPLESETUP_H
/*
 * Copyright (C) 2018-2023
 * HERE Europe B.V.
 * Utrecht University (The Netherlands).
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
// Created by Bram Custers(bacusters@gmail.com)
//
#include <concepts>
#include <cxxopts.hpp>
#include <string>
#include <tuple>
#include <vector>

#include "movetk/io/ProbeReader.h"
#include "movetk/utils/GeometryBackendTraits.h"
#include "movetk/utils/Meta.h"

namespace movetk::examples {

inline bool is_backend(const std::string& backend) {
	constexpr auto BACKEND_COUNT = std::tuple_size_v<movetk::backends::AvailableBackends>;
	bool is_known = false;
	movetk::utils::for_each_c<0, BACKEND_COUNT>([&backend, &is_known](auto index) {
		using Backend = std::tuple_element_t<index.value, movetk::backends::AvailableBackends>;
		if (Backend::name == backend) {
			is_known = true;
		}
	});
	return is_known;
}
inline int get_backend_index_for_name(const std::string& backend) {
	constexpr auto BACKEND_COUNT = std::tuple_size_v<movetk::backends::AvailableBackends>;
	int backend_index = -1;
	movetk::utils::for_each_c<0, BACKEND_COUNT>([&backend, &backend_index](auto index) {
		using Backend = std::tuple_element_t<index.value, movetk::backends::AvailableBackends>;
		if (Backend::name == backend) {
			backend_index = index.value;
		}
	});
	return backend_index;
}
inline std::vector<std::string> get_backend_names() {
	constexpr auto BACKEND_COUNT = std::tuple_size_v<movetk::backends::AvailableBackends>;
	std::vector<std::string> names;
	movetk::utils::for_each_c<0, BACKEND_COUNT>([&names](auto index) {
		using Backend = std::tuple_element_t<index.value, movetk::backends::AvailableBackends>;
		names.push_back(Backend::name);
	});
	return names;
}

namespace concepts {
using first_backend = std::tuple_element_t<0, movetk::backends::AvailableBackends>;
template <typename T>
concept Example = requires(T& t, cxxopts::ParseResult& arguments) {
	requires std::is_default_constructible_v<T>;
	{ T::NAME } -> std::convertible_to<std::string>;
	{ T::DESCRIPTION } -> std::convertible_to<std::string>;
	t.template run<first_backend>(arguments);
};
template <typename T>
concept ExampleWithOptions = Example<T> && requires(T& t, cxxopts::OptionAdder& option_adder) {
	t.add_options(option_adder);
};
template <typename T>
concept ExampleWithOptionsAndPositionals = Example<T> &&
    requires(T& t, cxxopts::OptionAdder& option_adder, std::vector<std::string>& positionals) {
	t.add_options(option_adder, positionals);
};
}  // namespace concepts

/**
 * @brief Base class with some commonly used functionality
 */
struct BaseExample {
	template <typename T, typename... VALUES>
	static inline void assign_values(const std::vector<T>& data, VALUES&... values) {
		assign_values_impl(data, std::make_index_sequence<sizeof...(VALUES)>{}, values...);
	}

	template <typename ProbeTraits>
	static std::unique_ptr<movetk::io::ProbeReader<ProbeTraits>> get_probe_reader(cxxopts::ParseResult& arguments,
	                                                                              const std::string& file_key,
	                                                                              const char* alternative) {
		if (!arguments.count(file_key)) {
			// Use built-in test data if a file is not specified
			return movetk::io::ProbeReaderFactory::create_from_string<ProbeTraits>(alternative);
		} else {
			// Example: Process trajectories from a (zipped) CSV file (e.g., probe_data_lametro.20180918.wayne.csv.gz)
			return movetk::io::ProbeReaderFactory::create<ProbeTraits>(arguments[file_key].as<std::string>());
		}
	}

private:
	template <typename T, typename... VALUES, size_t... IS>
	static inline void assign_values_impl(const std::vector<T>& data, std::index_sequence<IS...>, VALUES&... values) {
		((values = data[IS]), ...);
	}
};

/**
 * @brief Runner object capable of running examples that follow the concepts::Example concept.
 *
 * The runner runs the example with the backend, specified by the "backend" command line argument.
 * Can be one of the backends defined in AvailableBackends, using their ``name`` to identify them.
 */
struct ExampleRunner {
	/**
	 * @brief Runs the give example
	 * @tparam EXAMPLE The example type
	 * @param example The example
	 * @param argc Number of command line arguments
	 * @param argv Command line arguments
	 */
	template <concepts::Example EXAMPLE>
	int run_example(int argc, char** argv) {
		EXAMPLE example;
		cxxopts::Options options(EXAMPLE::NAME, EXAMPLE::DESCRIPTION);
		const auto default_backend = std::tuple_element_t<0, movetk::backends::AvailableBackends>::name;
		[[maybe_unused]] auto option_adder =
		    options.add_options()("b,backend",
		                          "Backend to run",
		                          cxxopts::value<std::string>()->default_value(default_backend));
		option_adder("h,help", "Show help");
		if constexpr (concepts::ExampleWithOptions<EXAMPLE>) {
			example.add_options(option_adder);
		} else if constexpr (concepts::ExampleWithOptionsAndPositionals<EXAMPLE>) {
			std::vector<std::string> positionals;
			example.add_options(option_adder, positionals);
			options.parse_positional(positionals.begin(), positionals.end());
		}
		auto result = options.parse(argc, argv);

		const auto backend_string = result["backend"].as<std::string>();
		if (!is_backend(backend_string)) {
			std::string message = "Invalid backend string '" + backend_string + "'. Available options: ";
			bool first = true;
			for (const auto& backend : get_backend_names()) {
				if (!first) {
					message += ",";
				} else {
					first = false;
				}
				message += backend;
			}
			throw std::invalid_argument(message.c_str());
		}
		const auto backend_index = get_backend_index_for_name(backend_string);

		std::cout << "Running " << EXAMPLE::NAME << " with backend " << backend_string << '\n';

		if (result.count("help") == 1) {
			std::cout << options.help() << std::endl;
			return 0;
		}
		try {
			run_example_with_backend(example, result, backend_index);
		} catch (std::exception& e) {
			std::cout << "Caught exception: " << e.what() << '\n';
			std::cout << "Usage:" << std::endl;
			std::cout << options.help() << std::endl;
			return 1;
		}
		return 0;
	}
	/**
	 * @brief Runs the example with the given arguments and specific backend index
	 * @param example The example
	 * @param arguments The parsed arguments
	 * @param backend_index The backend index
	 */
	template <concepts::Example EXAMPLE>
	void run_example_with_backend(EXAMPLE& example, cxxopts::ParseResult& arguments, int backend_index) {
		constexpr auto BACKEND_COUNT = std::tuple_size_v<movetk::backends::AvailableBackends>;
		movetk::utils::for_each_c<0, BACKEND_COUNT>([&example, backend_index, &arguments](auto index) {
			if (index.value != backend_index) {
				return;
			}
			using Backend = std::tuple_element_t<index.value, movetk::backends::AvailableBackends>;
			example.run<typename Backend::MovetkGeometryKernel>(arguments);
		});
	}
};
}  // namespace movetk::examples
#endif