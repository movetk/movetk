cmake_minimum_required(VERSION 3.11)
project(movepy
        LANGUAGES CXX
        VERSION 0.1
        )

# set path to custom cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
# Add OS specific tools.
if(UNIX)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/unix")
elseif(MSVC)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/msvc")
endif()

# include common tools and workarounds
include(tools)

# run cmake with -DCMAKE_PREFIX_PATH=path-to-installed-cmake-dir-of-movetk
#find_package(movetk CONFIG REQUIRED)  ##enable only if movepy is being built as a separate project

find_package(Boost REQUIRED COMPONENTS iostreams log)
find_package(Threads REQUIRED)
find_package(GDAL 2.2 REQUIRED)
find_package(GeographicLib REQUIRED)
find_package(pybind11 REQUIRED)

#find_package(movetk REQUIRED) ##enable only if movepy is being built as a separate project

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/")
