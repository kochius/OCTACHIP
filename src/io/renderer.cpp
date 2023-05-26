#include <stdexcept>

#include "io/renderer.hpp"

using namespace CHIP8;

Renderer::Renderer(const int width, const int height, const int scalar,
    const std::string& title) :
        window{nullptr},
        renderer{nullptr},
        baseWidth{width},
        baseHeight{height},
        windowScale{scalar} {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("Failed to initialize SDL video subsystem: " + 
            std::string(SDL_GetError()));
    }

    SDL_CreateWindowAndRenderer(width * windowScale, height * windowScale, 0, 
        &window, &renderer);
    
    if (window == nullptr || renderer == nullptr) {
        throw std::runtime_error("Failed to create SDL window and renderer: " + 
            std::string(SDL_GetError()));
    }
    
    SDL_SetWindowTitle(window, title.data());
}

Renderer::~Renderer() { 
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Renderer::drawFrame(const Frame& frame) {
    clearRenderer();
    for (int row = 0; row < baseHeight; row++) {
        for (int col = 0; col < baseWidth; col++) {
            if (frame[col + (row * baseWidth)]) {
                drawPixel(col, row);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void Renderer::drawPixel(const int col, const int row) {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_Rect pixelBlock{col * windowScale, row * windowScale, windowScale, 
        windowScale};
    SDL_RenderDrawRect(renderer, &pixelBlock);
    SDL_RenderFillRect(renderer, &pixelBlock);
}

void Renderer::clearRenderer() {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
}