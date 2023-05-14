#include "interpreter.hpp"

#include <fstream>
#include <iterator>
#include <random>
#include <stdexcept>

using namespace CHIP8;

Interpreter::Interpreter() :
    pc{PROG_START_ADDRESS},
    random{std::random_device{}(), 0, 255} {
    constexpr unsigned int FONT_SET_SIZE = 80;
    constexpr unsigned int FONT_START_ADDRESS = 0x50;
    constexpr std::array<uint8_t, FONT_SET_SIZE> fontSet = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    std::copy(std::begin(fontSet), std::end(fontSet), 
        std::begin(this->memory) + FONT_START_ADDRESS);
}

void Interpreter::loadRom(const std::filesystem::path& romPath) {
    using namespace std::string_literals;

    if (!std::filesystem::exists(romPath)) {
        throw std::runtime_error("File not found: "s + romPath.string());
    }
    
    std::ifstream romFile{romPath, std::ios_base::in | std::ios_base::binary};
    if (!romFile) {
        throw std::runtime_error("Unable to open file: "s + romPath.string());
    }

    const uintmax_t romSize = std::filesystem::file_size(romPath);
    constexpr unsigned int MAX_ROM_SIZE = MEMORY_SIZE - PROG_START_ADDRESS;
    if (romSize > MAX_ROM_SIZE) {
        const uintmax_t overflow = romSize - MAX_ROM_SIZE;
        throw std::length_error("File exceeds maximum ROM size by "s + 
            std::to_string(overflow) + " bytes: "s + romPath.string() + 
            " (actual size: " + std::to_string(romSize) + ")");
    }

    romFile.seekg(0, std::ios_base::beg);
    if (!romFile.read(reinterpret_cast<char*>(this->memory.data() + 
        PROG_START_ADDRESS), static_cast<std::streamsize>(romSize))) {
        throw std::runtime_error("Unable to read file: "s + romPath.string());
    }
}