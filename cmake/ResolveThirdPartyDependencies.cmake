include(${CMAKE_CURRENT_LIST_DIR}/CreateImportTarget.cmake)
include(FetchContent)

if(MOVETK_DOWNLOAD_THIRDPARTY)
    find_package(RapidJSON QUIET)
    if(NOT RapidJSON_FOUND)
        message(STATUS "Adding external RapidJSON")
        # Add rapidjson 
        FetchContent_Declare(
            RapidJSON
            GIT_REPOSITORY "https://github.com/Tencent/rapidjson.git"
            GIT_TAG 06d58b9e848c650114556a23294d0b6440078c61
        )
        
        # Check if population has already been performed
        FetchContent_GetProperties(RapidJSON)
        if(NOT rapidjson_POPULATED)
            # Fetch the content using previously declared details
            FetchContent_Populate(RapidJSON)

            set(RAPIDJSON_BUILD_TESTS OFF)
            set(RAPIDJSON_BUILD_DOC OFF)
            set(RAPIDJSON_BUILD_EXAMPLES OFF)
            add_subdirectory(${rapidjson_SOURCE_DIR} ${rapidjson_BINARY_DIR} EXCLUDE_FROM_ALL)
        endif()

        list(APPEND CMAKE_PREFIX_PATH ${rapidjson_BINARY_DIR})
        #TODO: remove when not necessary anymore
        set(RapidJSON_INCLUDE_DIR ${rapidjson_SOURCE_DIR}/include)
        CreateImportTarget(RapidJSON)
    endif()

    find_package(Catch2 QUIET)
    if(NOT Catch2_FOUND)
        message(STATUS "Adding external Catch2")
        # Add catch
        set(CATCH_INSTALL_DOCS OFF)
        set(CATCH_BUILD_TESTING OFF)
        FetchContent_Declare(
            Catch2
            GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
            GIT_TAG v2.13.1
            CMAKE_ARGS
                -DCATCH_INSTALL_DOCS=OFF
                -DCATCH_BUILD_TESTING=OFF
            OVERRIDE_FIND_PACKAGE
        )
        FetchContent_MakeAvailable(Catch2)
        # Add folder with helper cmake files of catch2 to the module path
        list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/contrib)
        CreateImportTarget(Catch2)
    endif()
endif()