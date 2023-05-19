cmake_minimum_required(VERSION 3.22.1)

set(TESTS ${CMAKE_PROJECT_NAME}-tests)

set(SOURCES
    instructions-tests.cpp
    ${PROJECT_SRC_DIR}/src/frame.cpp
    ${PROJECT_SRC_DIR}/src/opcode.cpp
    ${PROJECT_SRC_DIR}/src/random.cpp
)

add_executable(${TESTS} ${HEADERS} ${SOURCES})

target_link_libraries(${TESTS} gtest_main)

target_include_directories(${TESTS} PUBLIC ${PROJECT_SRC_DIR})