#include "SDLApp.hpp"

SDLApp::SDLApp() {
    SDL_Init(SDL_INIT_VIDEO);
    window_ = SDL_CreateWindow(
        "CHIP-8",
        DisplayWidth * Scale,
        DisplayHeight * Scale,
        0
    );
    renderer_ = SDL_CreateRenderer(
        window_,
        nullptr
    );
}

SDLApp::~SDLApp() {
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

bool SDLApp::processEvents(std::array<uint8_t, KeyCount>& keypad) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_EVENT_QUIT) 
            return false;
        if(event.type != SDL_EVENT_KEY_DOWN &&
           event.type != SDL_EVENT_KEY_UP) 
            continue;
        bool pressed = event.type == SDL_EVENT_KEY_DOWN;
        switch(event.key.key) {
            case SDLK_1: keypad[0x1] = pressed; break;
            case SDLK_2: keypad[0x2] = pressed; break;
            case SDLK_3: keypad[0x3] = pressed; break;
            case SDLK_4: keypad[0xC] = pressed; break;
            
            case SDLK_Q: keypad[0x4] = pressed; break;
            case SDLK_W: keypad[0x5] = pressed; break;
            case SDLK_E: keypad[0x6] = pressed; break;
            case SDLK_R: keypad[0xD] = pressed; break;

            case SDLK_A: keypad[0x7] = pressed; break;
            case SDLK_S: keypad[0x8] = pressed; break;
            case SDLK_D: keypad[0x9] = pressed; break;
            case SDLK_F: keypad[0xE] = pressed; break;

            case SDLK_Z: keypad[0xA] = pressed; break;
            case SDLK_X: keypad[0x0] = pressed; break;
            case SDLK_C: keypad[0xB] = pressed; break;
            case SDLK_V: keypad[0xF] = pressed; break;
        }
    }
	return true;
}

void SDLApp::render(const std::array<uint8_t, DisplayWidth* DisplayHeight>& display) {
    SDL_SetRenderDrawColor(
        renderer_,
        0, 0, 0, 255
    );
    SDL_RenderClear(renderer_);

    SDL_SetRenderDrawColor(
        renderer_,
        255, 255, 255, 255
    );

    for (uint8_t y = 0; y < DisplayHeight; ++y) {
        for (uint8_t x = 0; x < DisplayWidth; ++x) {
            if (!display[y * 64 + x]) 
                continue;
            SDL_FRect rect{
                static_cast<float>(x * Scale),
                static_cast<float>(y * Scale),
                static_cast<float>(Scale),
                static_cast<float>(Scale)
            };
            SDL_RenderFillRect(
                renderer_,
                &rect
            );
        }
    }
    SDL_RenderPresent(renderer_);
}
