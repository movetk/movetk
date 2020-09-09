cmake_minimum_required(VERSION 3.11)

project(GsTL CXX)


add_library(GsTL
        include/GsTL/cdf/categ_non_param_cdf.h)

target_include_directories(GsTL
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
        )

target_link_libraries(GsTL PUBLIC)