#include <chrono>

#include "emulator.hpp"
#include "core/frame.hpp"

using namespace CHIP8;

Emulator::Emulator() : 
    interpreter{},
    renderer{Frame::WIDTH, Frame::HEIGHT, "CHIP-8"} {
}

void Emulator::loadRom(const std::filesystem::path& romPath) {
    interpreter.loadRom(romPath);
}

void Emulator::run(const int ticksPerSecond) {
    const int ticksPerFrame = ticksPerSecond / framesPerSecond;
    long lastFrameTime = getCurrentTime();
    bool running = true;
    while (running) {
        running = input.processInput([&](int key, bool isPressed) {
            interpreter.setKey(key, isPressed);
        });
        long timeElapsed = getCurrentTime() - lastFrameTime;
        if (timeElapsed >= 1000 / framesPerSecond) {
            lastFrameTime = getCurrentTime();
            for (int i = 0; i < ticksPerFrame; i++) {
                interpreter.tick();
            }
            interpreter.updateTimers();
            renderer.drawFrame(interpreter.getFrame());
        }
    }
}

long Emulator::getCurrentTime() const {
    using namespace std::chrono;
    return duration_cast<milliseconds>
        (system_clock::now().time_since_epoch()).count();
}