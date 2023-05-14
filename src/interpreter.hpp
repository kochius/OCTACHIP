#pragma once

#include "frame.hpp"
#include "random.hpp"

#include <array>
#include <cstdint>
#include <filesystem>

namespace CHIP8 {

class Interpreter {
public:
    Interpreter(); // done
    void loadRom(const std::filesystem::path& romPath); // next
    void tick();
    void updateTimers();
    bool soundTimerOn() const;
    void setKey(int key, bool isPressed);
    const Frame& getFrame() const;
private:
    static constexpr int MEMORY_SIZE = 4096;
    static constexpr int V_REG_COUNT = 16;
    static constexpr int STACK_SIZE = 16;
    static constexpr int PROG_START_ADDRESS = 0x200;

    std::array<uint8_t, MEMORY_SIZE> memory{};
    std::array<uint8_t, V_REG_COUNT> v{};
    std::array<uint16_t, STACK_SIZE> stack{};
    uint16_t pc{};
    uint16_t i{};
    uint8_t delayTimer{};
    uint8_t soundTimer{};
    Frame frame;
    Random random;
};

}