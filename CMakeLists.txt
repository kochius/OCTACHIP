cmake_minimum_required(VERSION 3.22.1)
project(chip8)

set(CMAKE_CXX_STANDARD 17)
set(PROJECT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

enable_testing()

add_subdirectory(tests)

include(FetchContent)

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL/
    GIT_TAG release-2.0.20
)
FetchContent_MakeAvailable(SDL2)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.13.0
)
FetchContent_MakeAvailable(googletest)

set(HEADERS
    src/emulator.hpp
    src/frame.hpp
    src/instructions.hpp
    src/interpreter.hpp
    src/opcode.hpp
    src/random.hpp
    src/types.hpp
    src/io/input.hpp
    src/io/renderer.hpp)

set(SOURCES
    main.cpp
    src/emulator.cpp
    src/frame.cpp
    src/interpreter.cpp
    src/opcode.cpp
    src/random.cpp
    src/io/input.cpp
    src/io/renderer.cpp)

add_executable(${CMAKE_PROJECT_NAME} ${HEADERS} ${SOURCES})

target_link_libraries(chip8 SDL2-static)