function(OneOfTargetsExists SUCCESS_VAR TARGET_VAR)
    foreach(_ARG IN LISTS ARGN)
        if(TARGET ${_ARG})
            set(${SUCCESS_VAR} TRUE PARENT_SCOPE)
            set(${TARGET_VAR} ${_ARG} PARENT_SCOPE)
            message(VERBOSE "Target ${_ARG} exists")
            return()
        else()
            message(VERBOSE "Target ${_ARG} does not exist")
        endif()
    endforeach()
    set(${SUCCESS_VAR} FALSE PARENT_SCOPE)
endfunction()

macro(CreateImportTarget NAME)
    if(NOT TARGET ${NAME}::${NAME})
        # Check if lower case targets exist
        string(TOLOWER ${NAME} _LOWER_NAME)
        string(TOUPPER ${NAME} _UPPER_NAME)

        # Try variations of lower case and regular case target names
        # Pick one of those if it exists
        OneOfTargetsExists(_TARGET_FOUND _TARGET ${_LOWER_NAME}::${_LOWER_NAME}
            ${NAME} ${_LOWER_NAME} ${NAME}::${_LOWER_NAME} ${_LOWER_NAME}::${NAME})
        message(VERBOSE "Target: ${_TARGET}")
        if(NOT ${_TARGET_FOUND})
            message(STATUS "Creating new target ${NAME}::${NAME}")
            # Create temporary variables for the target data
            set(${NAME}_internal_INCLUDE_DIRS)
            set(${NAME}_internal_LIBS)
            set(${NAME}_CREATED FALSE)

            # Require at least include directories
            if(DEFINED ${NAME}_INCLUDE_DIR)
                list(APPEND ${NAME}_internal_INCLUDE_DIRS ${${NAME}_INCLUDE_DIR})
                set(${NAME}_CREATED TRUE)
            endif()
            if(DEFINED ${_LOWER_NAME}_INCLUDE_DIR)
                list(APPEND ${NAME}_internal_INCLUDE_DIRS ${${_LOWER_NAME}_INCLUDE_DIR})
                set(${NAME}_CREATED TRUE)
            endif()
            if(DEFINED ${_UPPER_NAME}_INCLUDE_DIR)
                list(APPEND ${NAME}_internal_INCLUDE_DIRS ${${_UPPER_NAME}_INCLUDE_DIR})
                set(${NAME}_CREATED TRUE)
            endif()

            if(DEFINED ${NAME}_INCLUDE_DIRS)
                list(APPEND ${NAME}_internal_INCLUDE_DIRS ${${NAME}_INCLUDE_DIRS})
                set(${NAME}_CREATED TRUE)
            endif()
            if(DEFINED ${_LOWER_NAME}_INCLUDE_DIRS)
                list(APPEND ${NAME}_internal_INCLUDE_DIRS ${${_LOWER_NAME}_INCLUDE_DIRS})
                set(${NAME}_CREATED TRUE)
            endif()
            if(DEFINED ${_UPPER_NAME}_INCLUDE_DIRS)
                list(APPEND ${NAME}_internal_INCLUDE_DIRS ${${_UPPER_NAME}_INCLUDE_DIRS})
                set(${NAME}_CREATED TRUE)
            endif()


            if(NOT ${NAME}_CREATED)
                message(FATAL_ERROR "Could not find data for target ${NAME}")
            endif()

            message(VERBOSE "Include dirs: ${${NAME}_internal_INCLUDE_DIRS}")
            message(VERBOSE "Libs: ${${NAME}_internal_LIBS}")

            if(DEFINED ${NAME}_LIBRARY)
                list(APPEND ${NAME}_internal_LIBS ${${NAME}_LIBRARY})
            endif()
            if(DEFINED ${NAME}_LIBRARIES)
                list(APPEND ${NAME}_internal_LIBS ${${NAME}_LIBRARIES})
            endif()
            list(LENGTH ${NAME}_internal_LIBS _LIBCOUNT)
            if(_LIBCOUNT GREATER 0)
                # Try recreating the target from old style package variables
                add_library(${NAME}_internal UNKNOWN IMPORTED)
                set_target_properties(${NAME}_internal PROPERTIES
                    IMPORTED_LOCATION "${${NAME}_internal_LIBS}"
                )
            else()
                # Try recreating the target from old style package variables
                add_library(${NAME}_internal INTERFACE IMPORTED)
            endif()
            set_target_properties(${NAME}_internal PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${${NAME}_internal_INCLUDE_DIRS}"
            )

            add_library(${NAME}::${NAME} ALIAS ${NAME}_internal)
        else()
            # Alias the found target
            add_library(${NAME}::${NAME} ALIAS ${_TARGET})
        endif()
    else()
        message(DEBUG "Target ${NAME}::${NAME} exists")
    endif()
endmacro()


