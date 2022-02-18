cmake_minimum_required(VERSION 3.11)
##from https://vicrucann.github.io/tutorials/quick-cmake-doxygen/

message(STATUS "doxygen ${CMAKE_SOURCE_DIR}")
message (STATUS "doxygen ${DOXYGEN_EXECUTABLE}")

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${PROJECT_SOURCE_DIR}/docs/movetk-doxy-config.cfg)
    set(DOXYGEN_OUT ${PROJECT_SOURCE_DIR}/docs)

    # request to configure the file
    #configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( movetk_generate_documentation ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/docs
            COMMENT "Generating MoveTk API documentation with Doxygen"
            VERBATIM )
else ()
    message(FATAL_ERROR "Doxygen need to be installed to generate the documentation")
endif ()
