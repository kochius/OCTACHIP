#include "emulator.hpp"
#include "core/types.hpp"

using namespace OCTACHIP;

Emulator::Emulator(const std::filesystem::path& romPath, 
    const int instructionsPerSecond, const int windowScale) : 
    instructionsPerUpdate{static_cast<int>(instructionsPerSecond / 
        UPDATES_PER_SECOND)},
    interpreter{},
    input{},
    renderer{FRAME_WIDTH, FRAME_HEIGHT, windowScale, "OCTACHIP"} {
    interpreter.loadRom(romPath);
}

void Emulator::run() {
    bool running = true;
    auto lastUpdateTime = std::chrono::high_resolution_clock::now();
    double accumulator = 0.0;

    while (running) {
        double deltaTime = getDeltaTime(lastUpdateTime);

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

        running = input.processInput([&](const int key, const bool isPressed) {
            interpreter.setKey(key, isPressed);
        });
    }
}

double Emulator::getDeltaTime(
    std::chrono::high_resolution_clock::time_point& lastUpdateTime) {
    const auto now = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> deltaTime = now - lastUpdateTime;
    lastUpdateTime = now;

    return deltaTime.count();
}