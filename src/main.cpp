#include <cstdlib>
#include <cxxopts.hpp>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

#include "emulator.hpp"

std::filesystem::path parsePath(const cxxopts::ParseResult& result);
int parseSpeed(const cxxopts::ParseResult& result);

int main(int argc, char* argv[]) {
    cxxopts::Options options("chip8", "A CHIP-8 emulator written in C++");
    options.add_options()
        ("h,help", "Print usage")
        ("r,rom", "ROM file path (*.ch8)", cxxopts::value<std::string>())
        ("s,speed", "Emulation speed (in ticks per second)", 
            cxxopts::value<int>()->default_value("700"));
    
    try {
        cxxopts::ParseResult result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << "\n";
            return EXIT_SUCCESS;
        }

        std::filesystem::path romPath = parsePath(result);
        if (romPath.empty()) {
            std::cerr << "No ROM file path provided\n";
            std::cerr << "Usage: ./chip8 -r <PATH>\n";
            return EXIT_FAILURE;
        }

        int ticksPerSecond = parseSpeed(result);

        CHIP8::Emulator chip8{};
        chip8.loadRom(romPath);
        chip8.run(ticksPerSecond);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

std::filesystem::path parsePath(const cxxopts::ParseResult& result) {
    if (result.count("rom")) {
        return std::filesystem::path{result["rom"].as<std::string>()};
    }
    return std::filesystem::path{};
}

int parseSpeed(const cxxopts::ParseResult& result) {
    return result["speed"].as<int>();
}