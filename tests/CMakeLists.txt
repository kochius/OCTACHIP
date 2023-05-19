cmake_minimum_required(VERSION 3.22.1)

set(TESTS ${CMAKE_PROJECT_NAME}-tests)

set(HEADERS
    fixtures/VxVyInstructionTest.hpp
    ${PROJECT_SRC_DIR}/src/core/frame.hpp
    ${PROJECT_SRC_DIR}/src/core/instructions.hpp
    ${PROJECT_SRC_DIR}/src/core/opcode.hpp
    ${PROJECT_SRC_DIR}/src/core/random.hpp
    ${PROJECT_SRC_DIR}/src/core/types.hpp
)

set(SOURCES
    instructions-tests.cpp
    ${PROJECT_SRC_DIR}/src/core/frame.cpp
    ${PROJECT_SRC_DIR}/src/core/instructions.cpp
    ${PROJECT_SRC_DIR}/src/core/opcode.cpp
    ${PROJECT_SRC_DIR}/src/core/random.cpp
)

add_executable(${TESTS} ${HEADERS} ${SOURCES})

target_link_libraries(${TESTS} gtest_main)

target_include_directories(${TESTS} PUBLIC ${PROJECT_SRC_DIR})