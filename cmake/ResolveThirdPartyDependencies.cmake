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
            
            execute_process(
                COMMAND ${CMAKE_COMMAND} -S ${rapidjson_SOURCE_DIR} -B ${rapidjson_BINARY_DIR} -G${CMAKE_GENERATOR} 
                    -DRAPIDJSON_BUILD_TESTS=OFF
                    -DRAPIDJSON_BUILD_DOC=OFF
                    -DRAPIDJSON_BUILD_EXAMPLES=OFF
                    -DCMAKE_INSTALL_PREFIX=${rapidjson_BINARY_DIR}/installed
            )
            execute_process(
                COMMAND ${CMAKE_COMMAND} --build ${rapidjson_BINARY_DIR} --target install
            )
        endif()
        list(APPEND CMAKE_PREFIX_PATH ${rapidjson_BINARY_DIR}/installed)
        #TODO: remove when not necessary anymore
        set(RapidJSON_INCLUDE_DIR ${rapidjson_SOURCE_DIR}/include)
        find_package(RapidJSON CONFIG REQUIRED)
        CreateImportTarget(RapidJSON)
    endif()

    find_package(Catch2 QUIET)
    if(NOT Catch2_FOUND)
        message(STATUS "Adding external Catch2")
        # Add catch
        FetchContent_Declare(
            Catch2
            GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
            GIT_TAG v2.13.5
        )
        # Check if population has already been performed
        FetchContent_GetProperties(Catch2)
        if(NOT catch2_POPULATED)
            # Fetch the content using previously declared details
            FetchContent_Populate(Catch2)
            execute_process(
                COMMAND ${CMAKE_COMMAND} -S ${catch2_SOURCE_DIR} -B ${catch2_BINARY_DIR} -G${CMAKE_GENERATOR} -DCATCH_INSTALL_DOCS=OFF -DCATCH_BUILD_TESTING=OFF
                    -DCMAKE_INSTALL_PREFIX=${catch2_BINARY_DIR}/installed
            )
            execute_process(
                COMMAND ${CMAKE_COMMAND} --build ${catch2_BINARY_DIR} --target install
            )
            list(PREPEND CMAKE_PREFIX_PATH ${catch2_BINARY_DIR}/installed)
            list(PREPEND CMAKE_MODULE_PATH ${catch2_BINARY_DIR}/installed)
            list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
            find_package(Catch2 CONFIG REQUIRED)
            CreateImportTarget(Catch2)
        endif()
    endif()
endif()