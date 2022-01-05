#
# Called when GDAL is not a proper imported target. We need to apply some heuristics to determine 
# what it is.
#
macro(PatchGdalInclude)
    # Properties to add to custom target
    set(_GDAL_PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${GDAL_INCLUDE_DIRS})
    set(_GDAL_IS_SHARED 0)
    list(LENGTH GDAL_LIBRARIES LIB_COUNT)
    if(${LIB_COUNT} GREATER  1)
        # Presumably, we have different configurations, so split them
        set(NEXT_IS_RELEASE 0)
        set(NEXT_IS_DEBUG 0)
        foreach(part ${GDAL_LIBRARIES})
            if(${part} STREQUAL "Debug" OR ${part} STREQUAL "debug")
                set(NEXT_IS_DEBUG 1)
            elseif(${part} STREQUAL "Release" OR ${part} STREQUAL "optimized")
                set(NEXT_IS_RELEASE 1)
            else()
                if(${NEXT_IS_RELEASE})
                    set(NEXT_IS_RELEASE 0)
                    if(MSVC)
                        if(${part} MATCHES ".*_i.lib")
                            set(_GDAL_IS_SHARED 1)
                            list(APPEND _GDAL_PROPERTIES IMPORTED_IMPLIB_RELEASE ${part})
                        else()
                            list(APPEND _GDAL_PROPERTIES IMPORTED_LOCATION_RELEASE ${part})
                        endif()
                    else()
                        list(APPEND _GDAL_PROPERTIES IMPORTED_LOCATION_RELEASE ${part})
                        if(${part} MATCHES "*.so")
                            set(_GDAL_IS_SHARED 1)
                        endif()
                    endif()
                elseif(${NEXT_IS_DEBUG})
                    set(NEXT_IS_DEBUG 0)
                    if(MSVC)
                        if(${part} MATCHES ".*_i_d.lib")
                            set(_GDAL_IS_SHARED 1)
                            list(APPEND _GDAL_PROPERTIES IMPORTED_IMPLIB_DEBUG ${part})
                        else()
                            list(APPEND _GDAL_PROPERTIES IMPORTED_LOCATION_DEBUG ${part})
                        endif()
                    else()
                        list(APPEND _GDAL_PROPERTIES IMPORTED_LOCATION_DEBUG ${part})
                        if(${part} MATCHES "*.so")
                            set(_GDAL_IS_SHARED 1)
                        endif()
                    endif()
                endif()
            endif()
        endforeach()
    else()
        message(STATUS "Only a single GDAL lib?")
    endif()
    if(${_GDAL_IS_SHARED})
        add_library(GDAL SHARED IMPORTED GLOBAL)
    else()
        add_library(GDAL STATIC IMPORTED GLOBAL)
    endif()
    set_target_properties(GDAL PROPERTIES ${_GDAL_PROPERTIES})
    message(STATUS "Props: ${_GDAL_PROPERTIES}")
    message(STATUS "Libs: ${GDAL_LIBRARIES}")
endmacro()