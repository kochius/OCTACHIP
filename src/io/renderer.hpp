#pragma once

#include <SDL.h>
#include <string_view>

#include "core/frame.hpp"

namespace CHIP8 {

class Renderer {
public:
    Renderer(const int width, const int height, const std::string_view& title);
    ~Renderer();
    void drawFrame(const Frame& frame);
    void drawPixel(int row, int col);
    void clearRenderer();
private:
    static constexpr int renderScale = 20;

    SDL_Window* window;
    SDL_Renderer* renderer;
    int baseWidth;
    int baseHeight;
};

}