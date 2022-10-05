include(${CMAKE_CURRENT_LIST_DIR}/CreateImportTarget.cmake)
if(MOVETK_DOWNLOAD_THIRDPARTY)
    find_package(RapidJSON QUIET)
    if(NOT RapidJSON_FOUND)
        include(FetchContent)
        message(STATUS "Adding external RapidJSON")
        # Add rapidjson 
        FetchContent_Declare(
            RapidJSON
            GIT_REPOSITORY "https://github.com/Tencent/rapidjson.git"
            GIT_TAG 06d58b9e848c650114556a23294d0b6440078c61
        )
        
        set(RAPIDJSON_BUILD_TESTS OFF)
        set(RAPIDJSON_BUILD_DOC OFF)
        set(RAPIDJSON_BUILD_EXAMPLES OFF)
        FetchContent_MakeAvailable(RapidJSON)

        list(APPEND CMAKE_PREFIX_PATH ${RapidJSON_BINARY_DIR})
        #TODO: remove when not necessary anymore
        set(RapidJSON_INCLUDE_DIR ${RapidJSON_SOURCE_DIR}/include)
        CreateImportTarget(RapidJSON)
    endif()

    find_package(Catch2 QUIET)
    if(NOT Catch2_FOUND)
        include(FetchContent)
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