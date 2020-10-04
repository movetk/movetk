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

find_package(Boost REQUIRED COMPONENTS iostreams log)
find_package(Threads REQUIRED)
find_package(GDAL 2.2 REQUIRED)
find_package(GeographicLib REQUIRED)
find_package(GSL 2.4 REQUIRED)
find_package(MPFR REQUIRED)


#find_package(GsTL REQUIRED /../third_party/GsTL/cmake)

# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
if(_IMPORT_PREFIX STREQUAL "/")
    set(_IMPORT_PREFIX "")
endif()

install(DIRECTORY  ${_IMPORT_PREFIX}/third_party/GsTL/include/
        DESTINATION include
        )
message(STATUS "import_prefix ${_IMPORT_PREFIX}/third_party/GsTL/include/")

set(GsTL_INCLUDE_DIR "${_IMPORT_PREFIX}/third_party/GsTL/include/")


message(STATUS "GsTL_include ${GsTL_INCLUDE_DIR} ")

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


if (WITH_CGAL_BACKEND)
    message(STATUS "CGAL_BACKEND ${WITH_CGAL_BACKEND}")
    add_library(movetk
            include/movetk/utils/GeolifeProbeTraits.h
            include/movetk/utils/GeolifeTrajectoryTraits.h
            include/movetk/utils/HereProbeTraits.h
            include/movetk/utils/HereTrajectoryTraits.h
            include/movetk/utils/GeometryBackendTraits.h
            include/movetk/logging.h
            include/movetk/ProbeTraits.h
            include/movetk/SortByField.h
            include/movetk/SplitByField.h
            include/movetk/SplitByDifferenceThreshold.h
            include/movetk/SplitByDistanceThreshold.h
            include/movetk/Splitter.h
            include/movetk/test_data.h
            include/movetk/ColumnarTrajectory.h
            include/movetk/TabularTrajectory.h
            include/movetk/TrajectoryReader.h
            include/movetk/TrajectoryTraits.h
            include/movetk/SortedProbeReader.h
            include/movetk/HighFrequencyTrajectorySplitter.h
            include/movetk/HighFrequencyTrajectoryReader.h
            include/movetk/HighFrequencyTrajectoryTraits.h
            include/movetk/TupleIterPrinter.h
            include/movetk/TuplePrinter.h
            include/movetk/io/gdal_PolygonReader.h
            include/movetk/io/ProbeReader.h
            include/movetk/io/csv/csv.h
            include/movetk/io/csv/CategoricalField.h
            include/movetk/io/csv/ParseDate.h
            include/movetk/io/GeoJSONUtils.h
            include/movetk/io/ColouredGeoJSONUtils.h
            include/movetk/geom/CGALTraits.h
            include/movetk/geom/trajectory_gdal.h
            include/movetk/geom/CGALWrapper.h
            include/movetk/geom/GeometryInterface.h
            include/movetk/geom/trajectory_to_interface.h
            include/movetk/metric/Norm.h
            include/movetk/metric/Distances.h
            include/movetk/utils/unituple.h
            include/movetk/utils/TrajectoryUtils.h
            include/movetk/utils/StringUtils.h
            include/movetk/utils/Asserts.h
            include/movetk/utils/Requirements.h
            include/movetk/utils/Iterators.h
            include/movetk/utils/text.h
            include/movetk/utils/Transpose.h
            include/movetk/utils/TypeChecks.h
            include/movetk/algo/AlgorithmTraits.h
            include/movetk/algo/SegmentationTraits.h
            include/movetk/algo/SegmentationPredicates.h
            include/movetk/algo/Segmentation.h
            include/movetk/algo/PolylineUtils.h
            include/movetk/algo/Interpolation.h
            include/movetk/algo/Search.h
            include/movetk/algo/Simplification.h
            include/movetk/algo/Similarity.h
            include/movetk/algo/Statistics.h
            include/movetk/algo/BrownianBridge.h
            include/movetk/algo/OutlierDetection.h
            include/movetk/algo/OutlierDetectionPredicates.h
            include/movetk/algo/Clustering.h
            include/movetk/ds/StartStopMatrix.h
            include/movetk/ds/Tree.h
            include/movetk/ds/FreeSpaceDiagram.h
            include/movetk/Documentation.h
            include/movetk/CartesianProbeTraits.h
            logging.cpp
            io/gdal_PolygonReader.cpp
            utils/text.cpp
            include/movetk/geo/geo.h
            geo/geo.cpp
            )
    else()
    message(STATUS "CGAL_BACKEND ${WITH_CGAL_BACKEND}")
    add_library(movetk
            include/movetk/utils/GeolifeProbeTraits.h
            include/movetk/utils/GeolifeTrajectoryTraits.h
            include/movetk/utils/HereProbeTraits.h
            include/movetk/utils/HereTrajectoryTraits.h
            include/movetk/utils/GeometryBackendTraits.h
            include/movetk/logging.h
            include/movetk/ProbeTraits.h
            include/movetk/SortByField.h
            include/movetk/SplitByField.h
            include/movetk/SplitByDifferenceThreshold.h
            include/movetk/SplitByDistanceThreshold.h
            include/movetk/Splitter.h
            include/movetk/test_data.h
            include/movetk/ColumnarTrajectory.h
            include/movetk/TabularTrajectory.h
            include/movetk/TrajectoryReader.h
            include/movetk/TrajectoryTraits.h
            include/movetk/SortedProbeReader.h
            include/movetk/HighFrequencyTrajectorySplitter.h
            include/movetk/HighFrequencyTrajectoryReader.h
            include/movetk/HighFrequencyTrajectoryTraits.h
            include/movetk/TupleIterPrinter.h
            include/movetk/TuplePrinter.h
            include/movetk/io/gdal_PolygonReader.h
            include/movetk/io/ProbeReader.h
            include/movetk/io/csv/csv.h
            include/movetk/io/csv/CategoricalField.h
            include/movetk/io/csv/ParseDate.h
            include/movetk/io/GeoJSONUtils.h
            include/movetk/io/ColouredGeoJSONUtils.h
            include/movetk/geom/trajectory_gdal.h
            include/movetk/geom/BoostGeometryWrapper.h
            include/movetk/geom/BoostGeometryTraits.h
            include/movetk/geom/GeometryInterface.h
            include/movetk/geom/trajectory_to_interface.h
            include/movetk/metric/Norm.h
            include/movetk/metric/Distances.h
            include/movetk/utils/unituple.h
            include/movetk/utils/TrajectoryUtils.h
            include/movetk/utils/StringUtils.h
            include/movetk/utils/Asserts.h
            include/movetk/utils/Requirements.h
            include/movetk/utils/Iterators.h
            include/movetk/utils/text.h
            include/movetk/utils/Transpose.h
            include/movetk/utils/TypeChecks.h
            include/movetk/algo/AlgorithmTraits.h
            include/movetk/algo/SegmentationTraits.h
            include/movetk/algo/SegmentationPredicates.h
            include/movetk/algo/Segmentation.h
            include/movetk/algo/PolylineUtils.h
            include/movetk/algo/Interpolation.h
            include/movetk/algo/Simplification.h
            include/movetk/algo/Search.h
            include/movetk/algo/BrownianBridge.h
            include/movetk/algo/Similarity.h
			include/movetk/algo/Statistics.h
            include/movetk/algo/OutlierDetection.h
            include/movetk/algo/OutlierDetectionPredicates.h
            include/movetk/algo/Clustering.h
            include/movetk/ds/StartStopMatrix.h
            include/movetk/ds/Tree.h
            include/movetk/ds/FreeSpaceDiagram.h
            include/movetk/Documentation.h
            include/movetk/CartesianProbeTraits.h
            logging.cpp
            io/gdal_PolygonReader.cpp
            utils/text.cpp
            include/movetk/geo/geo.h
            geo/geo.cpp
            include/third_party/miniball/Seb.h
            include/third_party/miniball/Seb_configure.h
            include/third_party/miniball/Seb_point.h
            include/third_party/miniball/Subspan.h
            include/third_party/miniball/Subspan-inl.h
            include/third_party/miniball/Seb_debug.h
            include/third_party/boost_geometry/discrete_hausdorff_distance.hpp
            include/third_party/boost_geometry/discrete_frechet_distance.hpp
            #include/third_party/miniball/Seb_debug.C
            )
endif()

target_compile_features(movetk PUBLIC cxx_std_17)
target_compile_definitions(movetk PUBLIC -DBOOST_LOG_DYN_LINK=1)

target_include_directories(movetk PRIVATE ${GsTL_INCLUDE_DIR})


if (WITH_CGAL_BACKEND)
    target_include_directories(movetk
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:include>
            ${GDAL_INCLUDE_DIR}
            ${GeographicLib_INCLUDE_DIR}
            ${CGAL_INCLUDE_DIR}
            ${MPFR_INCLUDE_DIR}
            ${GSL_INCLUDE_DIR}
            )
else ()
    target_include_directories(movetk
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:include>
            ${GDAL_INCLUDE_DIR}
            ${GeographicLib_INCLUDE_DIR}
            ${GSL_INCLUDE_DIR}
            )
endif ()

if (WITH_CGAL_BACKEND)
    target_link_libraries(movetk
            PUBLIC
            Threads::Threads
            Boost::log
            Boost::iostreams
            gdal
            Geographic
            ${CGAL_LIBRARY}
            gsl
	    gslcblas
            GMP::GMP
            MPFR::MPFR
            CGAL::CGAL
            )
else ()
    target_link_libraries(movetk
            PUBLIC
            Threads::Threads
            Boost::log
            Boost::iostreams
            gdal
            Geographic
            gsl
	    gslcblas
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
