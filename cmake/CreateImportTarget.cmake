macro(CreateImportTarget NAME)
    if(NOT TARGET ${NAME}::${NAME})
        # Check if lower case targets exist
        string(TOLOWER ${NAME} _LOWER_NAME)
        if(TARGET ${_LOWER_NAME}::${_LOWER_NAME})
            add_library(${NAME}::${NAME} ALIAS ${_LOWER_NAME}::${_LOWER_NAME})
        elseif(TARGET ${NAME}::${_LOWER_NAME})
            add_library(${NAME}::${NAME} ALIAS ${NAME}::${_LOWER_NAME})
        else()
            message(STATUS "Creating new target ${NAME}::${NAME}")
            # Try recreating the target from old style package variables
            add_library(${NAME}_internal UNKNOWN IMPORTED)

            # Create temporary variables for the target data
            set(${NAME}_internal_INCLUDE_DIRS)
            set(${NAME}_internal_LIBS)
            set(${NAME}_CREATED FALSE)

            # Require at least include directories
            if(DEFINED ${NAME}_INCLUDE_DIR)
                list(APPEND ${NAME}_internal_INCLUDE_DIRS ${${NAME}_INCLUDE_DIR})
                set(${NAME}_CREATED TRUE)
            endif()
            if(DEFINED ${NAME}_INCLUDE_DIRS)
                list(APPEND ${NAME}_internal_INCLUDE_DIRS ${${NAME}_INCLUDE_DIRS})
                set(${NAME}_CREATED TRUE)
            endif()
            if(NOT ${NAME}_CREATED)
                message(FATAL_ERROR "Could not find data for target ${NAME}")
            endif()

            set_target_properties(${NAME}_internal PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES ${NAME}_internal_INCLUDE_DIRS
            )
            if(DEFINED ${NAME}_LIBRARY)
                list(APPEND ${NAME}_internal_LIBS ${${NAME}_LIBRARY})
            endif()
            if(DEFINED ${NAME}_LIBRARIES)
                list(APPEND ${NAME}_internal_LIBS ${${NAME}_LIBRARIES})
            endif()
            set_target_properties(${NAME}_internal PROPERTIES
                IMPORTED_LOCATION ${NAME}_internal_LIBS
            )

            add_library(${NAME}::${NAME} ALIAS ${NAME}_internal)
        endif()
    else()
        message(STATUS "Target ${NAME}::${NAME} exists")
    endif()
endmacro()
