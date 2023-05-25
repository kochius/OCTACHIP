set(TESTS_EXECUTABLE chip8_tests)

enable_testing()

add_executable(${TESTS_EXECUTABLE})

set_target_properties(${TESTS_EXECUTABLE}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests_bin"
)

target_compile_features(${TESTS_EXECUTABLE} PRIVATE cxx_std_17)

# target_compile_options(${TESTS_EXECUTABLE}
#     PRIVATE
#         $<$<CXX_COMPILER_ID:MSVC>:
#             /W4
#             /w14640
#             /WX
#         >
#         $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
#             -Wall
#             -Wextra
#             -Wshadow
#             -Wnon-virtual-dtor
#             -pedantic
#             -Werror
#         >
# )

target_include_directories(${TESTS_EXECUTABLE} 
    PRIVATE
        ${PROJECT_SRC_DIR} 
        ${PROJECT_TESTS_DIR}
)

target_link_libraries(${TESTS_EXECUTABLE} PRIVATE gtest_main)

target_sources(${TESTS_EXECUTABLE}
    PRIVATE
        fixtures/instruction_test.hpp
        instructions/arithmetic_instructions.cpp
        instructions/flow_instructions.cpp
        instructions/io_instructions.cpp
        instructions/load_instructions.cpp
        instructions/misc_instructions.cpp
        mocks/mock_random.hpp
        ${PROJECT_SRC_DIR}/core/frame.hpp
        ${PROJECT_SRC_DIR}/core/frame.cpp
        ${PROJECT_SRC_DIR}/core/instructions.hpp
        ${PROJECT_SRC_DIR}/core/instructions.cpp
        ${PROJECT_SRC_DIR}/core/opcode.hpp
        ${PROJECT_SRC_DIR}/core/opcode.cpp
        ${PROJECT_SRC_DIR}/core/random.hpp
        ${PROJECT_SRC_DIR}/core/random.cpp
        ${PROJECT_SRC_DIR}/core/types.hpp
)