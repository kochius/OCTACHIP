#include "core/types.hpp"
#include "wasm_emulator.hpp"

using namespace CHIP8;

Emulator::Emulator(const int windowScale, const int instructionsPerSecond) : 
    lastUpdateTime{},
    accumulator{},
    instructionsPerUpdate{static_cast<int>(instructionsPerSecond / 
        UPDATES_PER_SECOND)},
    interpreter{},
    input{},
    renderer{FRAME_WIDTH, FRAME_HEIGHT, windowScale, "CHIP-8 Emulator"} {}

void Emulator::reset() {
    accumulator = 0.0;
    interpreter.reset();
    renderer.drawFrame(interpreter.getFrame());
}

void Emulator::refreshUpdateTimer() {
    lastUpdateTime = std::chrono::high_resolution_clock::now();
}

void Emulator::loadRom(const std::filesystem::path& romPath) {
    interpreter.loadRom(romPath);
}

void Emulator::setSpeed(const int instructionsPerSecond) {
    instructionsPerUpdate = static_cast<int>(instructionsPerSecond / 
        UPDATES_PER_SECOND);
}

void Emulator::update() {
    input.processInput([&](const int key, const bool isPressed) {
        interpreter.setKey(key, isPressed);
    });

    double deltaTime = getDeltaTime();

    if (deltaTime > 0.25) {
        deltaTime = 0.25;
    }

    accumulator += deltaTime;

    while (accumulator >= UPDATE_INTERVAL) {
        accumulator -= UPDATE_INTERVAL;


        for (int i = 0; i < instructionsPerUpdate; i++) {
            interpreter.tick();
        }

        interpreter.updateTimers();
    }

    renderer.drawFrame(interpreter.getFrame());
}

double Emulator::getDeltaTime() {
    const auto now = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> deltaTime = now - lastUpdateTime;
    lastUpdateTime = now;

    return deltaTime.count();
}