#pragma once

#include "frame.hpp"
#include "random.hpp"
#include "types.hpp"

#include <array>
#include <cstdint>
#include <filesystem>

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

    Memory memory{};
    Stack stack{};
    Registers registers;
    Frame frame;
    Random random;
};

}