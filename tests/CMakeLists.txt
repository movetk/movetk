cmake_minimum_required(VERSION 3.11)

find_package(Catch2 REQUIRED)
find_package(Boost REQUIRED)

if(NOT TARGET Catch2::Catch2)
    message(FATAL_ERROR "No Catch2 target")
endif()

add_executable(movetk_tests
        test_includes.h
        helpers/CustomCatchTemplate.h
        helpers/TestJsonReader.h
        tests_main.cpp
        test_csv.cpp
        test_categorical_field.cpp
        test_tuple_subsetting.cpp
        test_probe_point.cpp
        test_splitter.cpp
        test_geo.cpp
        test_rows2cols.cpp
        test_trajectory.cpp
        test_polyline_utils.cpp
        test_interpolation.cpp
        test_tree.cpp
        test_trajectory_utils.cpp
        test_trajectory_statistics.cpp
        test_seb.cpp
        test_norm.cpp
        test_squared_distance.cpp
        test_douglas_peucker.cpp
        test_wedge.cpp
        test_chan_chin.cpp
        test_imai_iri.cpp
        test_LCS.cpp
        test_discrete_hausdorff.cpp
        test_discrete_frechet.cpp
        test_brownian_bridge.cpp
        test_model_based_segmentation.cpp
        test_outlier_detection.cpp
        test_intersection.cpp
        test_free_space_diagram.cpp
        test_clustering.cpp
        test_mbr.cpp

        test_geojson.cpp
        test_strong_frechet.cpp
        test_weak_frechet.cpp
        test_agarwal.cpp
        )


set_property(TARGET movetk_tests PROPERTY CXX_STANDARD 20)
set_property(TARGET movetk_tests PROPERTY CXX_STANDARD_REQUIRED TRUE)
# Copy test data
add_custom_command(TARGET movetk_tests PRE_BUILD 
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:movetk_tests>/data
    COMMAND ${CMAKE_COMMAND} -Dsrc_dir="${CMAKE_CURRENT_LIST_DIR}/data"
        -Ddst_dir=$<TARGET_FILE_DIR:movetk_tests>/data
        -P ${PROJECT_SOURCE_DIR}/cmake/update_directory.cmake
)

# Copy binary dependencies gotten from Conan
add_custom_command(TARGET movetk_tests PRE_BUILD 
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:movetk_tests>/data
    COMMAND ${CMAKE_COMMAND} -Dsrc_dir="${PROJECT_BINARY_DIR}/bin"
        -Ddst_dir=$<TARGET_FILE_DIR:movetk_tests>
        -P ${PROJECT_SOURCE_DIR}/cmake/update_directory.cmake
)

if(MSVC)
    target_precompile_headers(movetk_tests
      PRIVATE test_includes.h
    )
endif()
if(CMAKE_GENERATOR MATCHES ".*Visual Studio.*")
    set_target_properties(movetk_tests PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:movetk_tests>)
endif()

include(CTest)

target_link_libraries(movetk_tests
        PUBLIC
        c2d::movetk
        Catch2::Catch2
        Boost::headers
        )
# Include CMake helpers of catch2
include(Catch)
catch_discover_tests(movetk_tests)