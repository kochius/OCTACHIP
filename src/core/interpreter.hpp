#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>

#include "core/random.hpp"
#include "core/types.hpp"

namespace OCTACHIP {

class Interpreter {
public:
    static constexpr uint16_t PROG_START_ADDRESS = 0x200;
    static constexpr uint16_t FONT_START_ADDRESS = 0x050;
    static constexpr int FONT_CHAR_SIZE = 5;
    static constexpr int FONT_CHAR_COUNT = 16;
    static constexpr int FONT_SET_SIZE = FONT_CHAR_COUNT * FONT_CHAR_SIZE;

    Interpreter();

    void reset();
    void loadRom(const std::filesystem::path& romPath);
    void updateTimers();
    void setKey(const int key, const bool isPressed);
    void setLoadStoreQuirk(const bool isEnabled);
    void setShiftQuirk(const bool isEnabled);
    void setWrapQuirk(const bool isEnabled);
    void tick();

    std::string getDisassembledInstructions() const;
    uint8_t getRegisterValue(const int index) const;
    uint16_t getProgramCounterValue() const;
    uint16_t getIndexRegisterValue() const;
    uint8_t getStackPointerValue() const;
    uint8_t getDelayTimerValue() const;
    uint8_t getSoundTimerValue() const;
    uint16_t getStackValue(const int index) const;
    const Frame& getFrame() const;
private:
    std::string disassembleOpcode(const int address) const;
    Memory memory;
    Registers registers;
    Stack stack;
    Frame frame;
    Keypad keypad;
    Keypad prevKeypadState;
    Random random;
    bool loadStoreQuirk;
    bool shiftQuirk;
    bool wrapQuirk;
};

}