#include <emscripten.h>

#include "wasm_emulator.hpp"

static constexpr int defaultWindowScale = 15;
static constexpr int defaultEmulationSpeed = 700;

OCTACHIP::Emulator emulator{defaultWindowScale, defaultEmulationSpeed};

extern "C" void loadRom(const char* romPath) {
    emulator.reset();
    emulator.loadRom(romPath);
}

extern "C" void setSpeed(const int emulationSpeed) {
    emulator.setSpeed(emulationSpeed);
}

extern "C" void stop() {
    emscripten_cancel_main_loop();
    emulator.reset();
}

extern "C" void pause() {
    emscripten_pause_main_loop();
}

extern "C" void resume() {
    emulator.refreshUpdateTimer();
    emscripten_resume_main_loop();
}

extern "C" int getVRegCount() {
    return emulator.getVRegCount();
}

extern "C" int getStackSize() {
    return emulator.getStackSize();
}

extern "C" uint8_t getRegisterValue(const int index) {
    return emulator.getRegisterValue(index);
}

extern "C" uint16_t getProgramCounterValue() {
    return emulator.getProgramCounterValue();
}

extern "C" uint16_t getIndexRegisterValue() {
    return emulator.getIndexRegisterValue();
}

extern "C" uint8_t getStackPointerValue() {
    return emulator.getStackPointerValue();
}

extern "C" uint8_t getDelayTimerValue() {
    return emulator.getDelayTimerValue();
}

extern "C" uint8_t getSoundTimerValue() {
    return emulator.getSoundTimerValue();
}

extern "C" uint16_t getStackValue(const int index) {
    return emulator.getStackValue(index);
}

void mainLoop() {
    emulator.update();
}

extern "C" int main() {
    emulator.refreshUpdateTimer();
    emscripten_set_main_loop(mainLoop, 0, 0);

    return EXIT_SUCCESS;
}