#include <emscripten.h>

#include "wasm_emulator.hpp"

static constexpr int defaultWindowScale = 15;
static constexpr int defaultEmulationSpeed = 700;

CHIP8::Emulator chip8{defaultWindowScale, defaultEmulationSpeed};

void loadRom(const char* romPath) {
    chip8.loadRom(romPath);
}

extern "C" void stop() {
    emscripten_cancel_main_loop();
    chip8.reset();
}

extern "C" void pause() {
    emscripten_pause_main_loop();
}

extern "C" void resume() {
    chip8.refreshUpdateTimer();
    emscripten_resume_main_loop();
}

void mainLoop() {
    chip8.update();
}

extern "C" int main() {
    chip8.refreshUpdateTimer();

    const char* romPath = "roms/tetris.ch8";
    chip8.loadRom(romPath);
    emscripten_set_main_loop(mainLoop, 0, 0);

    return EXIT_SUCCESS;
}