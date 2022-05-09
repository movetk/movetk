# Automatically install dependencies using conan

# We download the conan cmake file if not present yet.
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    #set(MOVETK_CONAN_VERSION "v0.17")
    set(MOVETK_CONAN_VERSION "develop")
    set(CONAN_SRC "https://github.com/conan-io/cmake-conan/raw/${MOVETK_CONAN_VERSION}/conan.cmake")
    message(STATUS "Auto-downloading conan.cmake from ${CONAN_SRC}")
    file(DOWNLOAD "${CONAN_SRC}" "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

# Include the downloaded conan cmake file
include(${CMAKE_BINARY_DIR}/conan.cmake)

# Configure the movetk dependencies.
conan_cmake_configure(REQUIRES 
# Dependencies
cgal/5.3
gdal/3.4.1
gsl/2.7
gmp/6.2.1
mpfr/4.1.0
boost/1.77.0
catch2/2.13.1
rapidjson/1.1.0 
geographiclib/1.52
zlib/1.2.12 # To fix conflict between boost and gdal requirement...
IMPORTS "bin, *.dll -> ./bin" # For windows, copy dependent dlls to the binary directory
GENERATORS 
cmake_find_package_multi # Use the config package generator to be able to do find_package(package CONFIG)

OPTIONS #Specify options for some dependencies to avoid pulling in too much.
#GDAL
gdal:shared=True
# Disable a lot of boost libraries
#Required by "log": "atomic"
#Required by "thread": "chrono"
#Required by "log": "container"
boost:without_context=True
boost:without_contract=True
boost:without_coroutine=True
#Required by "log": "date_time"
#Required by "log": "exception"
boost:without_fiber=True
#Required by "log": "filesystem"
# Include "graph"
boost:without_graph_parallel=True
#Include "iostreams"
boost:without_json=True
#Required by "log": "locale"
#Include "log"
#Required by "graph": "math"
boost:without_mpi=True
boost:without_nowide=True
boost:without_program_options=True
boost:without_python=True
#Required by "graph": "random"
#Required by "graph": "regex"
#Required by "graph": "serialization"
boost:without_stacktrace=True
#Include "system"
boost:without_test=True
# Include "thread"
boost:without_timer=True
boost:without_type_erasure=True
boost:without_wave=True
)

# Auto detect compilation settings for running
# TODO: fix for single build type
set(_CONFIGURATION_TYPES Release Debug)
foreach(TYPE ${_CONFIGURATION_TYPES})
conan_cmake_autodetect(settings BUILD_TYPE ${TYPE})
message(STATUS ${settings})
conan_cmake_install(PATH_OR_REFERENCE . 
    BUILD missing 
    REMOTE conancenter 
    SETTINGS ${settings})
endforeach()

#conan_cmake_autodetect(settings BUILD_TYPE Release;Debug;RelWithDebInfo;MinSizeRel)
#conan_cmake_install(PATH_OR_REFERENCE . BUILD missing REMOTE conancenter SETTINGS ${settings})

# Include the conan paths which will add paths to correctly find packages via find_package
list(PREPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR})