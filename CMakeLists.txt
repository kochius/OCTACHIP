cmake_minimum_required(VERSION 3.22.1)
project(OCTACHIP)

set(PROJECT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PROJECT_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(PROJECT_WEB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/web)

if(NOT EMSCRIPTEN)
    include(FetchContent)

    FetchContent_Declare(
        cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts
        GIT_TAG v3.1.1
    )
    FetchContent_MakeAvailable(cxxopts)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG v1.13.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL/
        GIT_TAG release-2.26.5
    )
    FetchContent_MakeAvailable(SDL2)

    if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
        include(CTest)
        if(BUILD_TESTING)
            add_subdirectory(tests)
        endif()
    endif()
endif()

add_subdirectory(src)