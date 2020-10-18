cmake_minimum_required(VERSION 3.11)
project(movetk_test)

find_package(Catch2 REQUIRED PATHS ../third_party/Catch2/lib64/cmake/Catch2)

if (NOT TARGET c2d::movetk)
    find_package(movetk CONFIG REQUIRED)
endif()

if (WITH_CGAL_BACKEND)
    set(CGAL_BACKEND_ENABLED 1)
else ()
    set(CGAL_BACKEND_ENABLED 0)
endif ()

message(STATUS "CGAL_BACKEND_ENABLED ${CGAL_BACKEND_ENABLED}")

set(RAPIDJSON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/rapidjson/include/")
message(STATUS "RAPIDJSON_INCLUDE_DIR ${RAPIDJSON_INCLUDE_DIR}")

message(STATUS "CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(${RAPIDJSON_INCLUDE_DIR})


add_executable(movetk_tests
        test_includes.h
        tests_main.cpp
        test_csv.cpp
        test_categorical_field.cpp
        test_tuple_subsetting.cpp
        test_probe_point.cpp
        test_splitter.cpp
        test_geo.cpp
        test_rows2cols.cpp
        test_trajectory.cpp
        test_trajectory_gdal.cpp
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
        )

target_compile_definitions(movetk_tests PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})

target_link_libraries(movetk_tests
        PRIVATE
        c2d::movetk
        Catch2::Catch2
        )

include(CTest)
include(Catch)
catch_discover_tests(movetk_tests)


if (WITH_CGAL_BACKEND)
    add_executable(movetk_tests_CGAL
            tests_main.cpp
            )
    target_link_libraries(movetk_tests_CGAL
            PRIVATE
            c2d::movetk
            Catch2::Catch2
            )

    catch_discover_tests(movetk_tests_CGAL)


endif()



