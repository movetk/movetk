#
# Find dependencies of MoveTk core library
#
include(${CMAKE_CURRENT_LIST_DIR}/CreateImportTarget.cmake)

set(Boost_USE_MULTITHREADED 1)
find_package(Boost COMPONENTS iostreams log thread system log_setup graph)
CreateImportTarget(Boost)

find_package(Threads REQUIRED)

# Find GDAL and fix the target if it is not provided.
find_package(GDAL 3.0 REQUIRED)
CreateImportTarget(GDAL)

find_package(GeographicLib REQUIRED COMPONENTS SHARED)
CreateImportTarget(GeographicLib)

find_package(GSL 2.5 REQUIRED)
CreateImportTarget(GSL)

find_package(RapidJSON REQUIRED)
CreateImportTarget(RapidJSON)