cmake_minimum_required(VERSION 3.11)

# Set path to custom cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
# Add OS specific tools.
if(UNIX)
        list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/unix")
elseif(MSVC)
        list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/msvc")
endif()

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS include/*.h src/*.cpp src/*.h)

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

foreach(_BACKEND ${MOVETK_BACKENDS})
	# Include the backend code. These should register necessary properties on the
	# movetk target.
	include(backends/${_BACKEND}/CMakeLists.txt)
endforeach()

# 
# Setup compilation settings
#
#target_compile_features(movetk PUBLIC cxx_std_17)
set(CXX_STANDARD 20)
set_property(TARGET movetk PROPERTY CXX_STANDARD 20)
set_property(TARGET movetk PROPERTY CXX_STANDARD_REQUIRED TRUE)
target_compile_features(movetk PUBLIC cxx_std_20)
target_compile_definitions(movetk PUBLIC -DBOOST_LOG_DYN_LINK=1)
if(MSVC)
	target_compile_options(movetk PUBLIC /experimental:external /Zc:preprocessor /external:anglebrackets /external:W0)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(movetk PUBLIC -fconcepts-diagnostics-depth=3)
endif()

#
# Setup included directories
#
target_include_directories(movetk
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
)

#
# Link MoveTk to dependencies
#
target_link_libraries(movetk
            PUBLIC
            Threads::Threads
            Boost::headers Boost::log Boost::iostreams Boost::thread Boost::system Boost::log_setup Boost::graph
            RapidJSON::RapidJSON
            GeographicLib::GeographicLib
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

# Install backend data
foreach(_BACKEND ${MOVETK_BACKENDS})
	# Include the backend code. These should register necessary properties on the
	# movetk target.
        if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/backends/${_BACKEND}/cmake)
                install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/backends/${_BACKEND}/cmake/ DESTINATION ${_CFG_CMAKE_DIR}/backends/${_BACKEND}/cmake)
        endif()
        # Assume atleast an include directory exists
        install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/backends/${_BACKEND}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
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
SET (CPACK_DEBIAN_PACKAGE_DEPENDS "gcc, libboost-dev, libcgal-dev, libgeographic-dev, libgsl-dev")
SET (CPACK_RPM_PACKAGE_REQUIRES "gcc, boost-devel, CGAL-devel, GeographicLib-devel, gsl-devel")
SET (CPACK_DEBIAN_PACKAGE_MAINTAINER "HERE Technologies")

INCLUDE(CPack)
