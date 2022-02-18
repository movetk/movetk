cmake_minimum_required(VERSION 3.12)
project(PyMoveTk
        LANGUAGES CXX
        VERSION 0.1
        )

set(PYBIND11_PYTHON_VERSION 3.7 CACHE STRING "")

# add dependencies
if (NOT TARGET c2d::movetk)
    find_package(movetk CONFIG REQUIRED)
endif()

# Try to find pybind11 on the system. Otherwise, resort to subdirectory

find_package(pybind11 CONFIG)

if(NOT TARGET pybind11::module)
# TODO: Add pybind11 via ExternalProject_Add
	add_subdirectory(pybind11)
endif()

# Set the cpp standard to use
set(PYBIND11_CPP_STANDARD -std=c++1z)

# Collect sources
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        PyMoveTk/*.h
        PyMoveTk/*.cpp
)

# Setup python module using pybind11
pybind11_add_module(PyMoveTk ${SOURCES})
target_include_directories(PyMoveTk PRIVATE ${CMAKE_CURRENT_SOURC_DIR}/include)
target_link_libraries(PyMoveTk PUBLIC movetk)

# Create source groups for Visual Studio
if(CMAKE_GENERATOR MATCHES ".*Visual Studio.*")
        source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${SOURCES})
endif()

# Build python module tests
if(MOVETK_BUILD_TESTS)
        add_subdirectory(test)
endif()

# Install the shared library
install(TARGETS PyMoveTk 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
)