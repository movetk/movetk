cmake_minimum_required(VERSION 3.11)

# Set path to custom cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
# Add OS specific tools.
if(UNIX)
        list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/unix")
elseif(MSVC)
        list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/msvc")
endif()

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
        include/movetk/io/CategoricalField.h
        include/movetk/io/csv/csv.h
        include/movetk/io/ParseDate.h
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
        include/movetk/algo/simplification/Agarwal.h
        include/movetk/algo/simplification/ChanChin.h
        include/movetk/algo/simplification/DouglasPeucker.h
        include/movetk/algo/simplification/ImaiIri.h
        include/movetk/algo/similarity/LongestCommonSubSequence.h
        include/movetk/algo/similarity/DynamicTimeWarping.h
        include/movetk/algo/segmentation/MonotoneSegmentation.h
        include/movetk/algo/segmentation/ModelBasedSegmentation.h
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
        include/movetk/utils/GeometryBackendTraits.h
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
)

#Potentially more options than Boost and CGAL
if(FALSE)
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

#
# Setup the library
#
MOVETK_LOG(TRACE "Using CGAL backend ${MOVETK_WITH_CGAL_BACKEND}")
add_library(movetk ${SOURCES} )


#
# Find dependencies of MoveTk
#
include(../cmake/ResolveMovetkDependencies.cmake)
MOVETK_LOG(TRACE "Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}")	
MOVETK_LOG(TRACE "Boost_LIBRARIES ${Boost_LIBRARIES}")
MOVETK_LOG(TRACE "GeographicLib_INCLUDE_DIRS ${GeographicLib_INCLUDE_DIRS}")
MOVETK_LOG(TRACE "GeographicLib_LIBRARIES ${GeographicLib_LIBRARIES}")

#
# Custom dependencies included in the repository
#
set(GsTL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/GsTL/include/")
MOVETK_LOG(TRACE "GsTL_include ${GsTL_INCLUDE_DIR} ")
set(RAPIDJSON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/rapidjson/include/")
MOVETK_LOG(TRACE "RAPIDJSON_INCLUDE_DIR ${RAPIDJSON_INCLUDE_DIR}")

#
# Setup installation for thirdparty code
#
install(DIRECTORY  ${GsTL_INCLUDE_DIR}
        ${RAPIDJSON_INCLUDE_DIR}
        DESTINATION include
        )
include_directories(${GsTL_INCLUDE_DIR}  ${RAPIDJSON_INCLUDE_DIR})

foreach(_BACKEND ${MOVETK_BACKENDS})
	# Include the backend code. These should register necessary properties on the
	# movetk target.
	include(backends/${_BACKEND}/CMakeLists.txt)
endforeach()

# 
# Setup compilation settings
#
target_compile_features(movetk PUBLIC cxx_std_17)
target_compile_definitions(movetk PUBLIC -DBOOST_LOG_DYN_LINK=1)
if(MSVC)
	target_compile_options(movetk PUBLIC /experimental:external /Zc:preprocessor /external:anglebrackets /external:W0)
endif()

#
# Setup included directories
#
target_include_directories(movetk
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
            PRIVATE
            $<BUILD_INTERFACE:${GsTL_INCLUDE_DIR}> $<BUILD_INTERFACE:${RAPIDJSON_INCLUDE_DIR}>
)

#
# Link MoveTk to dependencies
#
target_link_libraries(movetk
            PUBLIC
            Threads::Threads
            Boost::headers Boost::log Boost::iostreams Boost::thread Boost::system Boost::log_setup Boost::graph
            GDAL
            GeographicLib
            GSL::gsl GSL::gslcblas #${GSL_LIBRARIES}
            )

#
# Add alias for movetk
#
add_library(c2d::movetk ALIAS movetk)

#
# Setup movetk installation
#
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(_CFG_FILE movetkConfig)
set(_CFG_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/movetk/cmake)

install(TARGETS movetk EXPORT ${CMAKE_PROJECT_NAME}Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )
install(DIRECTORY include/movetk include/third_party ${GsTL_INCLUDE_DIR} 
        ${RAPIDJSON_INCLUDE_DIR}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT Devel
        )
# Install our dependencies resolution script
set(_MOVETK_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR}/../cmake)
install(FILES ${_MOVETK_CMAKE_DIR}/ResolveMovetkDependencies.cmake DESTINATION ${_CFG_CMAKE_DIR}/helpers)
install(FILES ${_MOVETK_CMAKE_DIR}/PatchGdalInclude.cmake DESTINATION ${_CFG_CMAKE_DIR}/helpers)
install(FILES ${_MOVETK_CMAKE_DIR}/FindMPFR.cmake DESTINATION ${_CFG_CMAKE_DIR}/helpers)
install(FILES ${_MOVETK_CMAKE_DIR}/FindGMP.cmake DESTINATION ${_CFG_CMAKE_DIR}/helpers)

# Install backend data
foreach(_BACKEND ${MOVETK_BACKENDS})
	# Include the backend code. These should register necessary properties on the
	# movetk target.
        if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/backends/${_BACKEND}/cmake)
                install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/backends/${_BACKEND}/cmake/ DESTINATION ${_CFG_CMAKE_DIR}/backends/${_BACKEND}/cmake)
        endif()
endforeach()

# Make sure we export the exportset containing movetk and the associated targets file.
install(EXPORT ${CMAKE_PROJECT_NAME}Targets
    DESTINATION ${_CFG_CMAKE_DIR}
)
#
# Generate configuration files and install the package
#
configure_package_config_file(../cmake/${_CFG_FILE}.cmake.in 
        ${CMAKE_CURRENT_BINARY_DIR}/${_CFG_FILE}.cmake
        INSTALL_DESTINATION ${_CFG_CMAKE_DIR}
)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${_CFG_FILE}Version.cmake
  COMPATIBILITY SameMajorVersion
)
# Setup install of config files
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${_CFG_FILE}.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${_CFG_FILE}Version.cmake
        DESTINATION ${_CFG_CMAKE_DIR} )
#
# Setup package
#

# Include CPack to get target for package.
SET (CPACK_GENERATOR "DEB;RPM")
SET (CPACK_PACKAGE_FILE_LICENSE "../LICENSE")
SET (CPACK_PACKAGE_VERSION "0.0.1")
SET (CPACK_DEBIAN_PACKAGE_DEPENDS "gcc, libboost-dev, libgdal-dev, libcgal-dev, libgeographic-dev, libgsl-dev")
SET (CPACK_RPM_PACKAGE_REQUIRES "gcc, boost-devel, gdal-devel, CGAL-devel, GeographicLib-devel, gsl-devel")
SET (CPACK_DEBIAN_PACKAGE_MAINTAINER "HERE Technologies")

INCLUDE(CPack)
