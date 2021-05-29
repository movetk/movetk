cmake_minimum_required(VERSION 3.11)
project(movetk
        LANGUAGES CXX
        VERSION 0.2
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

find_package(Boost REQUIRED COMPONENTS iostreams log thread system log_setup graph)
set(Boost_USE_MULTITHREADED 1)
message(STATUS "Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}")	
message(STATUS "Boost_LIBRARIES ${Boost_LIBRARIES}")

find_package(Threads REQUIRED)
find_package(GDAL 2.2 REQUIRED)
find_package(GeographicLib REQUIRED COMPONENTS SHARED)
message(STATUS "GeographicLib_INCLUDE_DIRS ${GeographicLib_INCLUDE_DIRS}")
message(STATUS "GeographicLib_LIBRARIES ${GeographicLib_LIBRARIES}")

find_package(GSL 2.4 REQUIRED)
find_package(MPFR REQUIRED)


#find_package(GsTL REQUIRED /../third_party/GsTL/cmake)

# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
if(_IMPORT_PREFIX STREQUAL "/")
    set(_IMPORT_PREFIX "")
endif()

set(GsTL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/GsTL/include/")

message(STATUS "GsTL_include ${GsTL_INCLUDE_DIR} ")

set(RAPIDJSON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/rapidjson/include/")

message(STATUS "RAPIDJSON_INCLUDE_DIR ${RAPIDJSON_INCLUDE_DIR}")

install(DIRECTORY  ${GsTL_INCLUDE_DIR}
        ${RAPIDJSON_INCLUDE_DIR}
        DESTINATION include
        )


include_directories(${GsTL_INCLUDE_DIR}  ${RAPIDJSON_INCLUDE_DIR})

if (WITH_CGAL_BACKEND)
        find_package(CGAL REQUIRED)

        #Use different multiple precision frameworks between Unix and Windows
        if(UNIX)
                find_package(GMP REQUIRED)
        elseif(MSVC)
                find_package(MPIR REQUIRED)  # exports GMP_INCLUDE_DIR and GMP::GMP
        endif()
        find_package(MPFR REQUIRED)

        message(STATUS "gmp_include ${GMP_INCLUDE_DIR}")
        message(STATUS "gmp_lib ${GMP_LIBRARIES}")
        message(STATUS "mpfr_include ${MPFR_INCLUDE_DIR}")
        message(STATUS "mpfr_lib ${MPFR_LIBRARIES}")
        message(STATUS "cgal_include ${CGAL_INCLUDE_DIR}")
        message(STATUS "cgal_lib ${CGAL_LIBRARIES}")
        message(STATUS "gsl_include ${GSL_INCLUDE_DIR}")
        message(STATUS "gsl_lib ${GSL_LIBRARY}")
else()
        message(STATUS "gsl_include ${GSL_INCLUDE_DIR}")
        message(STATUS "gsl_lib ${GSL_LIBRARY}")
        message(STATUS "gdal_include ${GDAL_INCLUDE_DIR}")
        message(STATUS "gdal_lib ${GDAL_LIBRARY}")
endif()

set(_USE_BOOST NOT MOVETK_WITH_CGAL_BACKEND)
set(SOURCES
        geo/geo.cpp
        include/movetk/algo/AlgorithmTraits.h
        include/movetk/algo/BrownianBridge.h
        include/movetk/algo/Clustering.h
        include/movetk/algo/Interpolation.h
        include/movetk/algo/OutlierDetection.h
        include/movetk/algo/OutlierDetectionPredicates.h
        include/movetk/algo/PolylineUtils.h
        include/movetk/algo/Search.h
        include/movetk/algo/Segmentation.h
        include/movetk/algo/SegmentationPredicates.h
        include/movetk/algo/SegmentationTraits.h
        include/movetk/algo/Similarity.h
        include/movetk/algo/Simplification.h
        include/movetk/algo/Statistics.h
        include/movetk/Documentation.h
        include/movetk/ds/ColumnarTrajectory.h
        include/movetk/ds/FreeSpaceDiagram.h
        include/movetk/ds/StartStopMatrix.h
        include/movetk/ds/TabularTrajectory.h
        include/movetk/ds/Tree.h
        include/movetk/geo/geo.h
        include/movetk/geom/GeometryInterface.h
        include/movetk/geom/trajectory_gdal.h
        include/movetk/geom/trajectory_to_interface.h
        include/movetk/io/CartesianProbeTraits.h
        include/movetk/io/csv/CategoricalField.h
        include/movetk/io/csv/csv.h
        include/movetk/io/csv/ParseDate.h
        include/movetk/io/gdal_PolygonReader.h
        include/movetk/io/GeoJSON.h
        include/movetk/io/HighFrequencyTrajectoryReader.h
        include/movetk/io/HighFrequencyTrajectorySplitter.h
        include/movetk/io/HighFrequencyTrajectoryTraits.h
        include/movetk/io/ProbeReader.h
        include/movetk/io/ProbeTraits.h
        include/movetk/io/SortByField.h
        include/movetk/io/SortedProbeReader.h
        include/movetk/io/SplitByDifferenceThreshold.h
        include/movetk/io/SplitByDistanceThreshold.h
        include/movetk/io/SplitByField.h
        include/movetk/io/Splitter.h
        include/movetk/io/TrajectoryReader.h
        include/movetk/io/TrajectoryTraits.h
        include/movetk/io/TupleIterPrinter.h
        include/movetk/io/TuplePrinter.h
        include/movetk/logging.h
        include/movetk/metric/DistanceInterface.h
        include/movetk/metric/Distances.h
        include/movetk/metric/DistanceTraits.h
        include/movetk/metric/Norm.h
        include/movetk/test_data.h
        include/movetk/utils/Asserts.h
        include/movetk/utils/GeolifeProbeTraits.h
        include/movetk/utils/GeolifeTrajectoryTraits.h
        include/movetk/utils/GeometryBackendTraits.h
        include/movetk/utils/HereProbeTraits.h
        include/movetk/utils/HereTrajectoryTraits.h
        include/movetk/utils/Iterators.h
        include/movetk/utils/Requirements.h
        include/movetk/utils/StringUtils.h
        include/movetk/utils/text.h
        include/movetk/utils/TrajectoryUtils.h
        include/movetk/utils/Transpose.h
        include/movetk/utils/TypeChecks.h
        include/movetk/utils/unituple.h
        io/gdal_PolygonReader.cpp
        logging.cpp
        utils/text.cpp
)
#Potentially more options than Boost and CGAL
if (MOVETK_WITH_CGAL_BACKEND)
	list(APPEND SOURCES 
		include/movetk/geom/CGALTraits.h
		include/movetk/geom/CGALWrapper.h
	)
elseif(_USE_BOOST)
	list(APPEND SOURCES 
		include/movetk/geom/BoostGeometryWrapper.h
		include/movetk/geom/BoostGeometryTraits.h
		include/third_party/miniball/Seb.h
		include/third_party/miniball/Seb_configure.h
		include/third_party/miniball/Seb_point.h
		include/third_party/miniball/Subspan.h
		include/third_party/miniball/Subspan-inl.h
		include/third_party/miniball/Seb_debug.h
		include/third_party/boost_geometry/discrete_hausdorff_distance.hpp
		include/third_party/boost_geometry/discrete_frechet_distance.hpp
	)
endif()

# Add source groups for visual studio
if(CMAKE_GENERATOR MATCHES ".*Visual Studio.*")
	source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${SOURCES})
endif()

message(TRACE "[MoveTk] Using CGAL backend ${MOVETK_WITH_CGAL_BACKEND}")
add_library(movetk ${SOURCES} )

target_compile_features(movetk PUBLIC cxx_std_17)
target_compile_definitions(movetk PUBLIC -DBOOST_LOG_DYN_LINK=1)

target_include_directories(movetk PRIVATE ${GsTL_INCLUDE_DIR} ${RAPIDJSON_INCLUDE_DIR})

target_include_directories(movetk
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:include>
			${Boost_INCLUDE_DIRS}
            ${GDAL_INCLUDE_DIR}
            ${GeographicLib_INCLUDE_DIRS}
            ${GSL_INCLUDE_DIR}
)
if (MOVETK_WITH_CGAL_BACKEND)
    target_include_directories(movetk
            PUBLIC
            ${CGAL_INCLUDE_DIR}
            ${MPFR_INCLUDE_DIR}
            )
endif ()
target_link_libraries(movetk
            PUBLIC
            Threads::Threads
            ${Boost_LIBRARIES}
            ${GDAL_LIBRARY}
            ${GeographicLib_LIBRARIES}
            ${GSL_LIBRARIES}
            )
if (MOVETK_WITH_CGAL_BACKEND)
    target_link_libraries(movetk
            PUBLIC
            ${CGAL_LIBRARY}
            GMP::GMP
            MPFR::MPFR
            CGAL::CGAL
            )
endif ()

add_library(c2d::movetk ALIAS movetk)

# installation info
install(TARGETS movetk EXPORT ${CMAKE_PROJECT_NAME}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )
install(DIRECTORY include/movetk include/third_party ${GsTL_INCLUDE_DIR} 
        ${RAPIDJSON_INCLUDE_DIR}
        DESTINATION include
        COMPONENT Devel
        )

# generate configuration files and install the package
configure_and_install(../cmake/simple_package-config.cmake.in SameMajorVersion)

# Include CPack to get target for package.
SET (CPACK_GENERATOR "DEB;RPM")
SET (CPACK_PACKAGE_FILE_LICENSE "../LICENSE")
SET (CPACK_PACKAGE_VERSION "0.0.1")
SET (CPACK_DEBIAN_PACKAGE_DEPENDS "gcc, libboost-dev, libgdal-dev, libcgal-dev, libgeographic-dev, libgsl-dev")
SET (CPACK_RPM_PACKAGE_REQUIRES "gcc, boost-devel, gdal-devel, CGAL-devel, GeographicLib-devel, gsl-devel")
SET (CPACK_DEBIAN_PACKAGE_MAINTAINER "HERE Technologies")

INCLUDE(CPack)
