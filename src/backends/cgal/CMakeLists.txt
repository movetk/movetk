include(${CMAKE_CURRENT_LIST_DIR}/cmake/CgalBackendDependencies.cmake)

target_include_directories(movetk PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
target_compile_definitions(movetk PUBLIC -DMOVETK_WITH_CGAL_BACKEND=1)
target_link_libraries(movetk PUBLIC 
MPFR::MPFR
CGAL::CGAL)

# Generate a stub library such that the backend files show up in Visual Studio
if(${CMAKE_GENERATOR} MATCHES ".*Visual Studio.*")
    file(GLOB_RECURSE MOVETK_CGAL_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/include/*.h ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp ${CMAKE_CURRENT_LIST_DIR}/src/*.h)
    target_sources(movetk PRIVATE ${MOVETK_CGAL_SOURCES})

    file(TOUCH ${CMAKE_CURRENT_BINARY_DIR}/stub.cpp)

    add_library(movetk_cgal_backend STATIC EXCLUDE_FROM_ALL ${MOVETK_CGAL_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/stub.cpp )
    target_link_libraries(movetk_cgal_backend PUBLIC movetk)
    set_property(TARGET movetk_cgal_backend PROPERTY CXX_STANDARD 20)
    set_property(TARGET movetk_cgal_backend PROPERTY CXX_STANDARD_REQUIRED TRUE)
endif()

