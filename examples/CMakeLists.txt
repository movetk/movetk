cmake_minimum_required(VERSION 3.11)
project(movetk_examples)

# add dependencies
if (NOT TARGET c2d::movetk)
    find_package(movetk CONFIG REQUIRED)
endif()

if (WITH_CGAL_BACKEND)
  set(CGAL_BACKEND_ENABLED 1)
else ()
  set(CGAL_BACKEND_ENABLED 0)
endif ()

message(STATUS "CGAL_BACKEND_ENABLED ${CGAL_BACKEND_ENABLED}")

include_directories(include/)

add_executable(rawprobe2trajectories rawprobe2trajectories.cpp)
target_compile_definitions(rawprobe2trajectories PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(rawprobe2trajectories
        PRIVATE
        c2d::movetk
        )

add_executable(probe2trajectories probe2trajectories.cpp)
target_compile_definitions(probe2trajectories PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(probe2trajectories
        PRIVATE
        c2d::movetk
        )

add_executable(geolife_count_probes geolife_count_probes.cpp)
target_compile_definitions(geolife_count_probes PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(geolife_count_probes
        PRIVATE
        c2d::movetk
        )
add_executable(geolife_distance_heading geolife_distance_heading.cpp)
target_compile_definitions(geolife_distance_heading PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(geolife_distance_heading
        PRIVATE
        c2d::movetk
        )

add_executable(read_trajectories read_trajectories.cpp)
target_compile_definitions(read_trajectories PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(read_trajectories
        PRIVATE
        c2d::movetk
        )

add_executable(probe2wkt probe2wkt.cpp)
target_compile_definitions(probe2wkt PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(probe2wkt
        PRIVATE
        c2d::movetk
        )

add_executable(TrajectorySegmentationLocation segment_by_meb.cpp)
target_compile_definitions(TrajectorySegmentationLocation PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(TrajectorySegmentationLocation
        PRIVATE
        c2d::movetk
        )

add_executable(TrajectorySegmentationSpeed segment_by_speed.cpp)
target_compile_definitions(TrajectorySegmentationSpeed PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(TrajectorySegmentationSpeed
        PRIVATE
        c2d::movetk
        )

add_executable(TrajectorySegmentationHeading segment_by_heading.cpp)
target_compile_definitions(TrajectorySegmentationHeading PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(TrajectorySegmentationHeading
        PRIVATE
        c2d::movetk
        )


add_executable(TrajectorySegmentationVelocity segment_by_velocity.cpp)
target_compile_definitions(TrajectorySegmentationVelocity PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(TrajectorySegmentationVelocity
        PRIVATE
        c2d::movetk
        )

add_executable(MonotoneSegmentation monotone_segmentation.cpp)
target_compile_definitions(MonotoneSegmentation PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(MonotoneSegmentation
        PRIVATE
        c2d::movetk
        )

add_executable(make_point make_point.cpp)
target_compile_definitions(make_point PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(make_point
        PRIVATE
        c2d::movetk
        )


add_executable(make_min_sphere make_min_sphere.cpp)
target_compile_definitions(make_min_sphere PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(make_min_sphere
        PRIVATE
        c2d::movetk
        )

add_executable(make_segment make_segment.cpp)
target_compile_definitions(make_segment PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(make_segment
        PRIVATE
        c2d::movetk
        )


add_executable(make_line make_line.cpp)
target_compile_definitions(make_line PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(make_line
        PRIVATE
        c2d::movetk
        )

add_executable(make_polygon make_polygon.cpp)
target_compile_definitions(make_polygon PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(make_polygon
        PRIVATE
        c2d::movetk
        )

add_executable(make_sphere make_sphere.cpp)
target_compile_definitions(make_sphere PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(make_sphere
        PRIVATE
        c2d::movetk
        )


add_executable(compute_squared_distance compute_squared_distance.cpp)
target_compile_definitions(compute_squared_distance PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(compute_squared_distance
        PRIVATE
        c2d::movetk
        )

add_executable(douglas_peucker_simplification douglas_peucker_simplification.cpp)
target_compile_definitions(douglas_peucker_simplification PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(douglas_peucker_simplification
        PRIVATE
        c2d::movetk
        )

add_executable(simplification simplification.cpp)
target_compile_definitions(simplification PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(simplification
        PRIVATE
        c2d::movetk
        )

add_executable(directed_graph directed_graph.cpp)
target_compile_definitions(directed_graph PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(directed_graph
        PRIVATE
        c2d::movetk
        )

add_executable(model_based_segmentation model_based_segmentation.cpp)
target_compile_definitions(model_based_segmentation PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(model_based_segmentation
        PRIVATE
        c2d::movetk
        )

add_executable(outlier_detection outlier_detection.cpp)
target_compile_definitions(outlier_detection PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(outlier_detection
        PRIVATE
        c2d::movetk
        )

add_executable(trajectory_sampler trajectory_sampler.cpp)
target_compile_definitions(trajectory_sampler PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(trajectory_sampler
        PRIVATE
        c2d::movetk
        )


add_executable(kinematic_interpolation kinematic_interpolation.cpp)
target_compile_definitions(kinematic_interpolation PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(kinematic_interpolation
        PRIVATE
        c2d::movetk
        )

add_executable(linear_interpolation linear_interpolation.cpp)
target_compile_definitions(linear_interpolation PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(linear_interpolation
        PRIVATE
        c2d::movetk
        )


add_executable(interpolation interpolation.cpp)
target_compile_definitions(interpolation PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
target_link_libraries(interpolation
        PRIVATE
        c2d::movetk
        )

# Add statistics example
add_executable(example_statistics compute_statistics.cpp)
        target_compile_definitions(example_statistics PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
        target_link_libraries(example_statistics
                PRIVATE
                c2d::movetk
                )

if (WITH_CGAL_BACKEND)

    add_executable(TrajectoriesPolygonIntersection trajectories_polygon_intersection.cpp)
    target_compile_definitions(TrajectoriesPolygonIntersection PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
    target_link_libraries(TrajectoriesPolygonIntersection
            PRIVATE
            c2d::movetk
            )


    add_executable(curves_intersections curves_intersections.cpp)
    target_compile_definitions(curves_intersections PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
    target_link_libraries(curves_intersections
            PRIVATE
            c2d::movetk
            )
endif()




if (WITH_CGAL_BACKEND)
    # installation info
    install(TARGETS MonotoneSegmentation TrajectoriesPolygonIntersection model_based_segmentation outlier_detection  interpolation EXPORT ${CMAKE_PROJECT_NAME}Targets
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            RUNTIME DESTINATION bin
            INCLUDES DESTINATION include
            )
    else()
        # installation info
        install(TARGETS MonotoneSegmentation model_based_segmentation outlier_detection interpolation EXPORT ${CMAKE_PROJECT_NAME}Targets
                LIBRARY DESTINATION lib
                ARCHIVE DESTINATION lib
                RUNTIME DESTINATION bin
                INCLUDES DESTINATION include
                )
endif ()

# generate configuration files and install the package
#configure_and_install(../cmake/simple_package-config.cmake.in SameMajorVersion)
