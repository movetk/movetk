cmake_minimum_required(VERSION 3.11)
project(movetk_documentation)

##from https://vicrucann.github.io/tutorials/quick-cmake-doxygen/

message(STATUS "doxygen ${CMAKE_SOURCE_DIR}")
message (STATUS "doxygen ${DOXYGEN_EXECUTABLE}")

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/docs/movetk-doxy-config.cfg)
    set(DOXYGEN_OUT ${CMAKE_SOURCE_DIR}/docs)

    # request to configure the file
    #configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the documentation")
endif (DOXYGEN_FOUND)
