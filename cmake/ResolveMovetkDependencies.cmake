#
# Find dependencies of MoveTk core library
#
find_package(Boost REQUIRED COMPONENTS iostreams log thread system log_setup graph)
set(Boost_USE_MULTITHREADED 1)

find_package(Threads REQUIRED)
if(NOT TARGET GDAL)
    if(NOT DEFINED GDAL_INCLUDE_DIRS OR NOT DEFINED GDAL_LIBRARIES)
        message(STATUS "Searching for GDAL")
        find_package(GDAL 2.2 REQUIRED)
    endif()
    include(${CMAKE_CURRENT_LIST_DIR}/PatchGdalInclude.cmake)
    PatchGdalInclude()
    if(NOT TARGET GDAL)
        message(FATAL_ERROR "Could not find and reconstruct GDAL")
    endif()
endif()
find_package(GeographicLib REQUIRED COMPONENTS SHARED)

find_package(GSL 2.4 REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
find_package(MPFR REQUIRED)
list(POP_BACK CMAKE_MODULE_PATH)