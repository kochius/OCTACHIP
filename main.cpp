#include "src/emulator.hpp"

#include <iostream>
#include <random>

int main() {
    CHIP8::Emulator chip8{};
    std::cout << "Loading ROM..." << "\n";
    chip8.loadRom("./roms/IBM-Logo.ch8");
    std::cout << "Running emulator..." << "\n";
    chip8.run(700);
    return EXIT_SUCCESS;
}