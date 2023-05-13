#pragma once

#include "frame.hpp"

#include <filesystem>

namespace CHIP8 {

class Interpreter {
public:
    void loadRom(std::filesystem::path romPath);
    void tick();
    void updateTimers();
    bool soundTimerOn() const;
    void setKey(int key, bool isPressed);
    const Frame& getFrame() const;
private:

};

}