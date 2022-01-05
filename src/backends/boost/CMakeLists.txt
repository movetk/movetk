include(${CMAKE_CURRENT_LIST_DIR}/cmake/BoostBackendDependencies.cmake)
target_include_directories(movetk PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
target_compile_definitions(movetk PUBLIC -DMOVETK_WITH_BOOST_BACKEND=1)
target_link_libraries(movetk PUBLIC Boost::headers)