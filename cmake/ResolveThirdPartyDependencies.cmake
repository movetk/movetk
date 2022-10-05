
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
            CMAKE_ARGS
                -DRAPIDJSON_BUILD_TESTS=OFF
                -DRAPIDJSON_BUILD_DOC=OFF
                -DRAPIDJSON_BUILD_EXAMPLES=OFF
            OVERRIDE_FIND_PACKAGE
        )
        FetchContent_MakeAvailable(RapidJSON)
    endif()

    find_package(Catch2 QUIET)
    if(NOT Catch2_FOUND)
        include(FetchContent)
        message(STATUS "Adding external RapidJSON")
        # Add rapidjson 
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
    endif()
endif()