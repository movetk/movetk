#
# Find dependencies of MoveTk core library
#

# Prefer config first, then try old module style.
# TODO: CMake should do this by itself, check version of CMake required for this.
find_package(Boost CONFIG COMPONENTS iostreams log thread system log_setup graph)
if(NOT Boost_FOUND)
    find_package(Boost REQUIRED COMPONENTS iostreams log thread system log_setup graph)
endif()
set(Boost_USE_MULTITHREADED 1)

find_package(Threads REQUIRED)

# Find GDAL and fix the target if it is not provided.
find_package(GDAL 3.0 CONFIG)
if(NOT TARGET GDAL::GDAL)
    if(NOT DEFINED GDAL_INCLUDE_DIRS OR NOT DEFINED GDAL_LIBRARIES)
        message(STATUS "Searching for GDAL")
        find_package(GDAL 3.0 CONFIG)
        find_package(GDAL 3.0 REQUIRED)
    endif()
    if(NOT TARGET GDAL::GDAL)
        include(${CMAKE_CURRENT_LIST_DIR}/PatchGdalInclude.cmake)
        PatchGdalInclude()
        if(NOT TARGET GDAL::GDAL)
            message(FATAL_ERROR "Could not find and reconstruct GDAL")
        endif()
    endif()
endif()

find_package(GeographicLib REQUIRED COMPONENTS SHARED)

find_package(GSL 2.7 CONFIG REQUIRED NAMES gsl GSL)

find_package(MPFR CONFIG REQUIRED NAMES mpfr MPFR)
if(NOT TARGET MPFR::MPFR AND TARGET mpfr::mpfr)
    add_library(MPFR::MPFR ALIAS mpfr::mpfr)
endif()

find_package(RapidJSON REQUIRED)