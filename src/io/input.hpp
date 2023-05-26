#pragma once

#include <functional>
#include <SDL.h>
#include <unordered_map>

namespace CHIP8 {

class Input {
public:
    Input();
    bool processInput(const std::function<void(int, bool)>& keyEventHandler);
private:
    std::unordered_map<SDL_Keycode, uint8_t> keyMap;
};

}