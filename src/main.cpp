#include <cstdlib>
#include <cxxopts.hpp>
#include <exception>
#include <iostream>
#include <string>

#include "emulator.hpp"

std::string parsePath(const cxxopts::ParseResult& result);
int parseSpeed(const cxxopts::ParseResult& result);
int parseScale(const cxxopts::ParseResult& result);

int main(int argc, char* argv[]) {
    cxxopts::Options options{"chip8", "A CHIP-8 emulator written in C++"};
    options.add_options()
        ("h,help", "Print usage")
        ("r,rom", "ROM file path", cxxopts::value<std::string>())
        ("s,speed", "Emulation speed (in ticks per second)", 
            cxxopts::value<int>()->default_value("800"))
        ("x,scale", "Window scale factor", 
            cxxopts::value<int>()->default_value("20"));
    
    try {
        cxxopts::ParseResult result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help();
            return EXIT_SUCCESS;
        }

        int windowScale = parseScale(result);
        std::string romPath = parsePath(result);
        int emulationSpeed = parseSpeed(result);

        CHIP8::Emulator chip8{windowScale};
        chip8.loadRom(romPath);
        chip8.run(emulationSpeed);
    }
    catch (const cxxopts::exceptions::exception& e) {
        std::cerr << "Error parsing options: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

std::string parsePath(const cxxopts::ParseResult& result) {
    if (result.count("rom") == 0 || result["rom"].as<std::string>().empty()) {
        throw std::runtime_error("No ROM file path provided\n"
            "Usage: ./chip8 -r <PATH>");
    }
    return result["rom"].as<std::string>();
}

int parseSpeed(const cxxopts::ParseResult& result) {
    if (result["speed"].as<int>() <= 0) {
        throw std::invalid_argument(
            "Invalid argument: emulation speed must be greater than 0");
    }
    return result["speed"].as<int>();
}

int parseScale(const cxxopts::ParseResult& result) {
    if (result["scale"].as<int>() <= 0) {
        throw std::invalid_argument(
            "Invalid argument: window scale factor must be greater than 0");
    }
    return result["scale"].as<int>();
}