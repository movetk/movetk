cmake_minimum_required(VERSION 3.11)
find_package(Python3)
# set input and output files
set(DOC_GENERATOR ${PROJECT_SOURCE_DIR}/documentation/build-documentation.py)

message(STATUS "[MoveTK] Doxygen build starting")

# note the option ALL which allows to build the docs together with the application
add_custom_target( movetk_generate_documentation 
        COMMAND Python3::Interpreter ${DOC_GENERATOR} ${PROJECT_VERSION}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/documentation
        COMMENT "Generating MoveTk API documentation with Doxygen"
        VERBATIM )
