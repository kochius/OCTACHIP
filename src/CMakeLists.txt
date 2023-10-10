set(MAIN_EXECUTABLE octachip)

add_executable(${MAIN_EXECUTABLE})

set_target_properties(${MAIN_EXECUTABLE}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/dist"
)

target_compile_features(${MAIN_EXECUTABLE} PRIVATE cxx_std_17)

target_compile_options(${MAIN_EXECUTABLE}
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:
            /W4
            /w14640
            /WX
            $<$<CONFIG:Debug>:/Zi>
        >
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
            -Wall
            -Wextra
            -Wshadow
            -Wnon-virtual-dtor
            -pedantic
            -Werror
            $<$<CONFIG:Debug>:-g>
        >
)

target_include_directories(${MAIN_EXECUTABLE} PRIVATE ${PROJECT_SRC_DIR})

target_sources(${MAIN_EXECUTABLE}
    PRIVATE
        core/instructions.cpp
        core/instructions.hpp
        core/interpreter.cpp
        core/interpreter.hpp
        core/opcode.cpp
        core/opcode.hpp
        core/random.cpp
        core/random.hpp
        core/types.hpp
        io/input.cpp
        io/input.hpp
        io/renderer.cpp
        io/renderer.hpp
)

if(EMSCRIPTEN)
    set_target_properties(${MAIN_EXECUTABLE}
        PROPERTIES
            SUFFIX ".js"
    )
    
    target_compile_options(${MAIN_EXECUTABLE}
        PRIVATE
            "SHELL:-s USE_SDL=2"
    )

    target_link_options(${MAIN_EXECUTABLE}
        PRIVATE
            "SHELL:-s USE_SDL=2"
            "SHELL:-s EXPORTED_FUNCTIONS='_main,\
                                          _loadRom,\
                                          _setSpeed,\
                                          _stop,_pause,\
                                          _resume,\
                                          _getRegisterValue,\
                                          _getProgramCounterValue,\
                                          _getIndexRegisterValue,\
                                          _getStackPointerValue,\
                                          _getDelayTimerValue,\
                                          _getSoundTimerValue,\
                                          _getStackValue,\
                                          _pushKeyDownEvent,\
                                          _pushKeyUpEvent'"
            "SHELL:-s EXPORTED_RUNTIME_METHODS=ccall"
            "SHELL:--preload-file ../../roms"
    )

    target_sources(${MAIN_EXECUTABLE}
        PRIVATE
            wasm_emulator.cpp
            wasm_emulator.hpp
            wasm_main.cpp
    )
else()
    target_link_libraries(${MAIN_EXECUTABLE}
        PRIVATE
            cxxopts
            SDL2main
            SDL2-static
    )

    target_sources(${MAIN_EXECUTABLE}
        PRIVATE
            emulator.cpp
            emulator.hpp
            main.cpp
    )
endif()