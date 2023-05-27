#include <chrono>

#include "emulator.hpp"
#include "core/frame.hpp"

using namespace CHIP8;

Emulator::Emulator(const int windowScale) : 
    interpreter{},
    input{},
    renderer{Frame::WIDTH, Frame::HEIGHT, windowScale, "CHIP-8"} {}

void Emulator::loadRom(const std::filesystem::path& romPath) {
    interpreter.loadRom(romPath);
}

void Emulator::run(const int ticksPerSecond) {
    const int ticksPerFrame = ticksPerSecond / FRAMES_PER_SECOND;
    long lastFrameTime = getCurrentTime();
    bool running = true;

    while (running) {
        const long timeElapsed = getCurrentTime() - lastFrameTime;

        if (timeElapsed >= 1000 / FRAMES_PER_SECOND) {
            lastFrameTime = getCurrentTime();

            for (int i = 0; i < ticksPerFrame; i++) {
                interpreter.tick();
            }
            
            interpreter.updateTimers();
            renderer.drawFrame(interpreter.getFrame());
        }

        running = input.processInput([&](const int key, const bool isPressed) {
            interpreter.setKey(key, isPressed);
        });
    }
}

long Emulator::getCurrentTime() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::system_clock::now().time_since_epoch()).count();
}