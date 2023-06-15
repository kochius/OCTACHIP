#pragma once

#include <SDL.h>
#include <string>

#include "core/types.hpp"

namespace OCTACHIP {

class Renderer {
public:
    Renderer(const int width, const int height, const int scalar, 
        const std::string& title);
    ~Renderer();
    void drawFrame(const Frame& frame);
    void drawPixel(const int row, const int col);
    void clearRenderer();
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    int baseWidth;
    int baseHeight;
    int windowScale;
};

}