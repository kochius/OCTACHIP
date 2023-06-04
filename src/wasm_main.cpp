#include <emscripten.h>

#include "wasm_emulator.hpp"

static constexpr int defaultWindowScale = 15;
static constexpr int defaultEmulationSpeed = 700;

CHIP8::Emulator chip8{defaultWindowScale, defaultEmulationSpeed};

extern "C" void loadRom(const char* romPath) {
    chip8.reset();
    chip8.loadRom(romPath);
}

extern "C" void setSpeed(const int emulationSpeed) {
    chip8.setSpeed(emulationSpeed);
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

extern "C" uint8_t getRegisterValue(const int index) {
    return chip8.getRegisterValue(index);
}

extern "C" uint16_t getProgramCounterValue() {
    return chip8.getProgramCounterValue();
}

extern "C" uint16_t getIndexRegisterValue() {
    return chip8.getIndexRegisterValue();
}

extern "C" uint8_t getStackPointerValue() {
    return chip8.getStackPointerValue();
}

extern "C" uint8_t getDelayTimerValue() {
    return chip8.getDelayTimerValue();
}

extern "C" uint8_t getSoundTimerValue() {
    return chip8.getSoundTimerValue();
}

void mainLoop() {
    chip8.update();
}

extern "C" int main() {
    chip8.refreshUpdateTimer();
    emscripten_set_main_loop(mainLoop, 0, 0);

    return EXIT_SUCCESS;
}