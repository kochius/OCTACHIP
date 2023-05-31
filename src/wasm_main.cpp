#include <chrono>
#include <emscripten.h>
#include <string>

#include "wasm_emulator.hpp"

CHIP8::Emulator chip8{};

void loadRom(const std::string& romPath) {
    chip8.interpreter.loadRom(romPath);
}

void mainLoop() {
    static double accumulator = 0.0;
    static auto previousTime = std::chrono::high_resolution_clock::now();

    const auto currentTime = std::chrono::high_resolution_clock::now();
    const double deltaTime = std::chrono::duration<double>(currentTime - 
        previousTime).count();
    previousTime = currentTime;

    accumulator += deltaTime;

    while (accumulator >= CHIP8::Emulator::DELTA_TIME) {
        accumulator -= CHIP8::Emulator::DELTA_TIME;

        for (int i = 0; i < CHIP8::Emulator::INTSTRUCTIONS_PER_UPDATE; i++) {
            chip8.interpreter.tick();
        }

        chip8.interpreter.updateTimers();
    }

    chip8.renderer.drawFrame(chip8.interpreter.getFrame());

    chip8.input.processInput([&](const int key, const bool isPressed) {
        chip8.interpreter.setKey(key, isPressed);
    });
}

int main() {
    loadRom("roms/tetris.ch8");
    emscripten_set_main_loop(mainLoop, 0, 1);
    return EXIT_SUCCESS;
}