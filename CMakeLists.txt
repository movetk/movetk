cmake_minimum_required(VERSION 3.11)
project(movetk)

option(MOVETK_GLIBCXX_PARALLEL "Use parallel STL algorithms at designated places (supported only under GNU/Linux with GCC)")
option(MOVETK_BUILD_DOC "Build documentation" OFF)
option(MOVETK_WITH_CGAL_BACKEND "Use CGAL backend in MoveTk" OFF)
option(MOVETK_BUILD_TESTS "Build MoveTk tests" OFF)
option(MOVETK_BUILD_PY "Build MoveTk Python bindings" OFF)
option(MOVETK_BUILD_EXAMPLES "Build MoveTk examples" OFF)

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
endif()

if(UNIX)
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


# add project code
add_subdirectory(src)

message(STATUS "doxygen ${BUILD_DOC}")
message(STATUS "WITH_CGAL_BACKEND ${WITH_CGAL_BACKEND}")

if (MOVETK_BUILD_DOC)
    #add project documentation
    add_subdirectory(docs)
endif()

# add unit tests
if(MOVETK_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# add usage example
if(MOVETK_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

# add python bindings
if(MOVETK_BUILD_PY)
  add_subdirectory(py)
endif()
