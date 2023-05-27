#pragma once

#include <array>
#include <cstdint>
#include <filesystem>

#include "core/frame.hpp"
#include "core/random.hpp"
#include "core/types.hpp"

namespace CHIP8 {

class Interpreter {
public:
    Interpreter();
    void loadRom(const std::filesystem::path& romPath);
    void updateTimers();
    bool soundTimerOn() const;
    const Frame& getFrame() const;
    void setKey(const int key, const bool isPressed);
    void tick();
private:
    static constexpr uint16_t PROG_START_ADDRESS = 0x200;
    static constexpr uint16_t FONT_START_ADDRESS = 0x050;
    static constexpr int FONT_CHAR_SIZE = 5;
    static constexpr int FONT_CHAR_COUNT = 16;
    static constexpr int FONT_SET_SIZE = FONT_CHAR_COUNT * FONT_CHAR_SIZE;

    Memory memory;
    Registers registers;
    Stack stack;
    Frame frame;
    Keypad keypad;
    Random<uint8_t> random;
};

}