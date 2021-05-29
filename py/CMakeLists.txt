cmake_minimum_required(VERSION 3.12)
project(PyMoveTk
        LANGUAGES CXX
        VERSION 0.1
        )

set(PYBIND11_PYTHON_VERSION 3.7 CACHE STRING "")

# run cmake with -DCMAKE_PREFIX_PATH=path-to-installed-cmake-dir-of-movetk
#find_package(movetk CONFIG REQUIRED)  ##enable only if movepy is being built as a separate project

# add dependencies
if (NOT TARGET c2d::movetk)
    find_package(movetk CONFIG REQUIRED)
endif()

# TODO: check if this is needed. pybind11 has logic for finding the right Python as well.
#find_package (Python COMPONENTS Interpreter Development REQUIRED)

# Try to find pybind11 on the system. Otherwise, resort to subdirectory

find_package(pybind11 CONFIG)

if(NOT TARGET pybind11::module)
# TODO: Add pybind11 via ExternalProject_Add
	add_subdirectory(pybind11)
endif()

# Set the cpp standard to use
set(PYBIND11_CPP_STANDARD -std=c++1z)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        PyMoveTk/*.h
        PyMoveTk/*.cpp
)

# Collect sources

pybind11_add_module(PyMoveTk ${SOURCES})
target_link_libraries(PyMoveTk PUBLIC movetk)

if(CMAKE_GENERATOR MATCHES ".*Visual Studio.*")
        source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${SOURCES})
endif()

# Build tests
if(MOVETK_BUILD_TESTS)
        add_subdirectory(test)
endif()


#install(TARGETS movetk_geometry EXPORT ${CMAKE_PROJECT_NAME}Targets
#        LIBRARY DESTINATION lib
#        ARCHIVE DESTINATION lib
#        RUNTIME DESTINATION bin
#        INCLUDES DESTINATION include
#        )