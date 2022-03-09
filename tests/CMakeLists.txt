cmake_minimum_required(VERSION 3.11)

message(STATUS "Prefix and module path at tests: ${CMAKE_MODULE_PATH} ${CMAKE_PREFIX_PATH}")
find_package(Catch2 REQUIRED)#PATHS ../third_party/Catch2/lib64/cmake/Catch2)

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
        test_strong_frechet.cpp
        test_weak_frechet.cpp
        test_agarwal.cpp
        )
file(GLOB test_data data/*.json)

add_custom_command(TARGET movetk_tests PRE_BUILD 
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:movetk_tests>/data
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${test_data} $<TARGET_FILE_DIR:movetk_tests>/data
)

if(MSVC)
    target_precompile_headers(movetk_tests
      PRIVATE test_includes.h
    )
endif()

include(Catch)
target_link_libraries(movetk_tests
        PRIVATE
        c2d::movetk
        Catch2::Catch2
        )
include(CTest)

if(MSVC)
add_custom_command( TARGET movetk_tests POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E echo "Copying dependencies:" $<JOIN:$<TARGET_RUNTIME_DLLS:movetk_tests>,",">
                    COMMAND ${CMAKE_COMMAND} -E copy
                        $<TARGET_RUNTIME_DLLS:movetk_tests>
                        $<TARGET_FILE_DIR:movetk_tests>
                    COMMAND_EXPAND_LISTS)
endif()
catch_discover_tests(movetk_tests)


