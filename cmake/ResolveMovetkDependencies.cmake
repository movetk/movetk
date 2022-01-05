#
# Find dependencies of MoveTk
#
find_package(Boost REQUIRED COMPONENTS iostreams log thread system log_setup graph)
set(Boost_USE_MULTITHREADED 1)

find_package(Threads REQUIRED)
if(NOT TARGET GDAL)
    if(NOT DEFINED GDAL_INCLUDE_DIRS OR NOT DEFINED GDAL_LIBRARIES)
        find_package(GDAL 2.2 REQUIRED)
    endif()
    include(${CMAKE_CURRENT_LIST_DIR}/PatchGdalInclude.cmake)
    PatchGdalInclude()
endif()
find_package(GeographicLib REQUIRED COMPONENTS SHARED)

find_package(GSL 2.4 REQUIRED)
find_package(MPFR REQUIRED)