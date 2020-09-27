cmake_minimum_required(VERSION 3.11)
project(movepy
        LANGUAGES CXX
        VERSION 0.1
        )


# run cmake with -DCMAKE_PREFIX_PATH=path-to-installed-cmake-dir-of-movetk
#find_package(movetk CONFIG REQUIRED)  ##enable only if movepy is being built as a separate project

# add dependencies
if (NOT TARGET c2d::movetk)
    find_package(movetk CONFIG REQUIRED)
endif()


set(PYBIND11_CPP_STANDARD -std=c++1z)

find_package(pybind11 REQUIRED)

#find_package(movetk REQUIRED) ##enable only if movepy is being built as a separate project

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/")

add_library(movetk_geometry MODULE movetk_geometry.cpp)

target_include_directories(movetk_geometry
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
        )


target_link_libraries(movetk_geometry
        PUBLIC
        ${GDAL_LIBRARY}
        ${GeographicLib_LIBRARIES}
        c2d::movetk
        pybind11::module
        )

set_target_properties(movetk_geometry PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
        SUFFIX "${PYTHON_MODULE_EXTENSION}")
