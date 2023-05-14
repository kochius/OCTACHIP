#pragma once

#include <array>

namespace CHIP8 {

class Frame {
public:
    static constexpr int WIDTH = 64;
    static constexpr int HEIGHT = 32;

    const bool& operator[](const int index) const {
        return pixels.at(index);
    }
private:
    std::array<bool, WIDTH * HEIGHT> pixels{};
};

}