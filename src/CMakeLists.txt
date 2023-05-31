set(CHIP8_EXECUTABLE chip8)

add_executable(${CHIP8_EXECUTABLE})

if(NOT EMSCRIPTEN)
    target_link_libraries(${CHIP8_EXECUTABLE}
        PRIVATE
            SDL2-static
            cxxopts
    )

    target_sources(${CHIP8_EXECUTABLE}
        PRIVATE
            main.cpp
            emulator.cpp
            emulator.hpp
    )
else()
    set_target_properties(${CHIP8_EXECUTABLE}
        PROPERTIES
            SUFFIX ".html"
    )
    
    target_compile_options(${CHIP8_EXECUTABLE}
        PRIVATE
            -s USE_SDL=2
    )

    target_link_options(${CHIP8_EXECUTABLE}
        PRIVATE
            -s USE_SDL=2
            --preload-file ../../roms
            --shell-file ${PROJECT_WEB_DIR}/shell.html
    )

    target_sources(${CHIP8_EXECUTABLE}
        PRIVATE
            wasm_main.cpp
            wasm_emulator.hpp
    )
endif()

set_target_properties(${CHIP8_EXECUTABLE}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_compile_features(${CHIP8_EXECUTABLE} PRIVATE cxx_std_17)

target_compile_options(${CHIP8_EXECUTABLE}
    PRIVATE
        $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:
            /W4
            /w14640
            /WX
            /Zi
        >
        $<$<AND:$<CONFIG:Debug>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:
            -Wall
            -Wextra
            -Wshadow
            -Wnon-virtual-dtor
            -pedantic
            -Werror
            -g
        >
)

target_include_directories(${CHIP8_EXECUTABLE} PRIVATE ${PROJECT_SRC_DIR})

target_sources(${CHIP8_EXECUTABLE}
    PRIVATE
        core/frame.cpp
        core/frame.hpp
        core/instructions.cpp
        core/instructions.hpp
        core/interpreter.cpp
        core/interpreter.hpp
        core/opcode.cpp
        core/opcode.hpp
        core/random.hpp
        core/types.hpp
        io/input.cpp
        io/input.hpp
        io/renderer.cpp
        io/renderer.hpp
)