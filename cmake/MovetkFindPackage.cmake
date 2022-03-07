cmake_minimum_required(VERSION 3.21)

define_property(TARGET
                 PROPERTY SOURCE_PACKAGE
                 BRIEF_DOCS "Property defining the name of the source package" FULL_DOCS "Property defining the name of the source package")

#See https://stackoverflow.com/questions/45479456/cmake-is-there-a-way-to-get-a-list-of-imported-targets-that-belong-to-a-package
# This needs to be a macro, otherwise the found package is not available in the calling scope.
macro(movetk_find_package PACKAGE_NAME)
    #message(STATUS "movetk_find_package args: ${PACKAGE_NAME} -- EXTRA: ${ARGN}")
    get_property(import_targets_before DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY IMPORTED_TARGETS)
    #message(STATUS "Targets before: ${import_targets_before}")
    find_package(${PACKAGE_NAME} ${ARGN})
    get_property(import_targets_after DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY IMPORTED_TARGETS)
    #message(STATUS "Targets after: ${import_targets_after}")
    list(REMOVE_ITEM import_targets_after ${import_targets_before})
    #message(STATUS "Targets clean: ${import_targets_after}")
    foreach(target IN LISTS import_targets_after)
        set_target_properties(${target} PROPERTIES SOURCE_PACKAGE ${PACKAGE_NAME})
    endforeach()
endmacro()