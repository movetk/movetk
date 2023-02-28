cmake_minimum_required(VERSION 3.18)
project(movetk
        LANGUAGES CXX
        VERSION 0.2
        )

option(MOVETK_GLIBCXX_PARALLEL "Use parallel STL algorithms at designated places (supported only under GNU/Linux with GCC)")
# Backend selection
option(MOVETK_WITH_CGAL_BACKEND "Build MoveTk with CGAL backend" OFF)
option(MOVETK_WITH_BOOST_BACKEND "Build MoveTk with Boost backend" ON)

option(MOVETK_BUILD_DOC "Build documentation" OFF)
option(MOVETK_BUILD_TESTS "Build MoveTk tests" OFF)
option(MOVETK_BUILD_PY "Build MoveTk Python bindings" OFF)
option(MOVETK_BUILD_EXAMPLES "Build MoveTk examples" OFF)
option(MOVETK_DOWNLOAD_THIRDPARTY "Download third party libraries, if possible, when they are not found" ON)

macro(MOVETK_LOG msg_type msg)
    message(${msg_type} "[MoveTk CMake] ${msg}")
endmacro()

# Create a list of the backends. We use lowercase names
set(MOVETK_BACKENDS)
if(MOVETK_WITH_CGAL_BACKEND)
  list(APPEND MOVETK_BACKENDS cgal)
endif()
if(MOVETK_WITH_BOOST_BACKEND)
  list(APPEND MOVETK_BACKENDS boost)
endif()

# Check we select at least one backend
list(LENGTH MOVETK_BACKENDS _BACKEND_COUNT)
if(${_BACKEND_COUNT} LESS 1)
  MOVETK_LOG(FATAL_ERROR "Select at least one backend to build")
endif()

if(UNIX)
  if(CMAKE_BUILD_TYPE MATCHES Release)
    if(MOVETK_GLIBCXX_PARALLEL)
      message("Using parallel STL")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -march=native -Ofast -D_GLIBCXX_PARALLEL")  # catch2 doesn't work with these.
    else()
      message("Not using parallel STL")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Ofast")
    endif()
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -O3")
  endif()
  if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
      # set path to custom cmake modules
      list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
      include(CodeCoverage)
      APPEND_COVERAGE_COMPILER_FLAGS()
      set(COVERAGE_LCOV_EXCLUDES 'usr/*')
      SETUP_TARGET_FOR_COVERAGE_LCOV(
          NAME ctest_coverage                    # New target name
          EXECUTABLE ctest -j ${PROCESSOR_COUNT} # Executable in PROJECT_BINARY_DIR
              DEPENDENCIES movetk_tests               # Dependencies to build first
      )
  endif() #CMAKE_BUILD_TYPE STREQUAL "Coverage"
endif()

# Color compiler output for Gcc and Clang.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options (-fcolor-diagnostics)
endif ()

#
# Find dependencies of MoveTk
#
include(cmake/ResolveThirdPartyDependencies.cmake)
include(cmake/ResolveMovetkDependencies.cmake)
MOVETK_LOG(TRACE "Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}")	
MOVETK_LOG(TRACE "Boost_LIBRARIES ${Boost_LIBRARIES}")
MOVETK_LOG(STATUS "GeographicLib_INCLUDE_DIRS ${GeographicLib_INCLUDE_DIRS}")
MOVETK_LOG(STATUS "GeographicLib_LIBRARIES ${GeographicLib_LIBRARIES}")

# Add project code
add_subdirectory(src)

MOVETK_LOG(STATUS "doxygen ${MOVETK_BUILD_DOC}")
MOVETK_LOG(STATUS "WITH_CGAL_BACKEND ${MOVETK_WITH_CGAL_BACKEND}")

# Build project documentation
if (MOVETK_BUILD_DOC)
    add_subdirectory(documentation)
endif()

# Build unit tests
if(MOVETK_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# Build usage examples
if(MOVETK_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

# Build python bindings
if(MOVETK_BUILD_PY)
  add_subdirectory(py)
endif()
