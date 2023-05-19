#pragma once

#include <array>
#include <cstdint>
#include <filesystem>

#include "frame.hpp"
#include "random.hpp"
#include "types.hpp"

namespace CHIP8 {

class Interpreter {
public:
    Interpreter();
    void loadRom(const std::filesystem::path& romPath);
    void updateTimers();
    bool soundTimerOn() const;
    void setKey(int key, bool isPressed);
    const Frame& getFrame() const;
    void tick();
private:
    static constexpr int PROG_START_ADDRESS = 0x200;
    static constexpr int FONT_START_ADDRESS = 0x50;
    static constexpr int FONT_CHAR_SIZE = 0x05;
    static constexpr int FONT_SET_SIZE = 80;

    Memory memory{};
    Stack stack{};
    Registers registers;
    Frame frame;
    Keypad keypad{};
    Random random;
};

}