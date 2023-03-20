cmake_minimum_required(VERSION 3.11)
project(movetk_examples)

# add dependencies
if (NOT TARGET c2d::movetk)
    find_package(movetk CONFIG REQUIRED)
endif()

if (MOVETK_WITH_CGAL_BACKEND)
  set(CGAL_BACKEND_ENABLED 1)
else ()
  set(CGAL_BACKEND_ENABLED 0)
endif ()

find_package(cxxopts REQUIRED GLOBAL)
if(NOT TARGET cxxopts::cxxopts)
    message(FATAL_ERROR "cxxopts::cxxopts not a target")
endif()

# Enable folders
if(CMAKE_GENERATOR MATCHES ".*Visual Studio.*")
    set_property( GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# Optional second argument is the source name. Otherwise, same as the example name is assume
function(CreateExample name)
        set(SRC_NAME ${name}.cpp)
        if(${ARGC} GREATER 1)
                set(SRC_NAME ${ARGV1})
        endif()
        file(GLOB _HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/include/*.h)
        MOVETK_LOG(VERBOSE "Created example ${name}")
        add_executable(${name} ${SRC_NAME} ${_HEADERS})
        target_compile_definitions(${name} PRIVATE -DCGAL_BACKEND_ENABLED=${CGAL_BACKEND_ENABLED})
        target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
        target_link_libraries(${name}
                PRIVATE
                c2d::movetk
                PUBLIC 
                cxxopts::cxxopts
                )
        if(CMAKE_GENERATOR MATCHES ".*Visual Studio.*")
                set_target_properties(${name} PROPERTIES FOLDER "Examples")
        endif()
        
        if(NOT "${name}" MATCHES "rawprobe2trajectories")
            target_precompile_headers(${name}
                REUSE_FROM rawprobe2trajectories
            )
        else()
            target_precompile_headers(${name}
                PRIVATE include/ExampleSetup.h
                include/GeolifeTrajectoryTraits.h
                include/HereTrajectoryTraits.h
                include/test_data.h
            )
        endif()
endfunction()

#IO examples
CreateExample(rawprobe2trajectories)
CreateExample(probe2trajectories)
CreateExample(geolife_count_probes)
CreateExample(geolife_distance_heading)
CreateExample(read_trajectories)
CreateExample(write_geojson)

# Segmentation examples
CreateExample(trajectory_segmentation)
CreateExample(MonotoneSegmentation monotone_segmentation.cpp)

# Creation examples
CreateExample(creating_geometry)

# Computations
CreateExample(compute_squared_distance)

# Simplification
CreateExample(douglas_peucker_simplification)
CreateExample(simplification)

CreateExample(model_based_segmentation)
CreateExample(outlier_detection)
CreateExample(trajectory_sampler)

# Interpolation examples
CreateExample(kinematic_interpolation)
CreateExample(linear_interpolation)
CreateExample(interpolation)

# Statistics example
CreateExample(compute_statistics)


if (MOVETK_WITH_CGAL_BACKEND)
    CreateExample(curves_intersections)
endif()