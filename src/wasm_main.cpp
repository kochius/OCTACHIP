#include <emscripten.h>
#include <emscripten/bind.h>
#include <string>
#include <SDL.h>

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

extern "C" void setLoadStoreQuirk(bool isEnabled) {
    emulator.setLoadStoreQuirk(isEnabled);
}

extern "C" void setShiftQuirk(bool isEnabled) {
    emulator.setShiftQuirk(isEnabled);
}

extern "C" void setWrapQuirk(bool isEnabled) {
    emulator.setWrapQuirk(isEnabled);
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

extern "C" void pushKeyDownEvent(const int key) {
    SDL_Event event;
    event.type = SDL_KEYDOWN;
    event.key.keysym.sym = key;
    SDL_PushEvent(&event);
}

extern "C" void pushKeyUpEvent(const int key) {
    SDL_Event event;
    event.type = SDL_KEYUP;
    event.key.keysym.sym = key;
    SDL_PushEvent(&event);
}

std::string getDisassembledInstructions() {
    return emulator.getDisassembledInstructions();
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("getDisassembledInstructions", 
        &getDisassembledInstructions);
}

void mainLoop() {
    emulator.update();
}

extern "C" int main() {
    emulator.refreshUpdateTimer();
    emscripten_set_main_loop(mainLoop, 0, 0);

    return EXIT_SUCCESS;
}