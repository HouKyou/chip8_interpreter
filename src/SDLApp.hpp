#pragma once

#include<SDL3/SDL.h>
#include<array>

class SDLApp {
public:
	SDLApp();
	~SDLApp();

	static constexpr std::size_t KeyCount{ 16 };
	static constexpr std::size_t DisplayWidth{ 64 };
	static constexpr std::size_t DisplayHeight{ 32 };
	static constexpr std::size_t FontTableSize{ 80 };

	bool processKeypad(std::array<uint8_t, KeyCount>& keypad);
	void renderDisplay(const std::array<uint8_t, DisplayWidth * DisplayHeight>& display);

private:
	SDL_Window* window_{ nullptr };
	SDL_Renderer* renderer_{ nullptr };

	static constexpr int Scale{ 15 };
};
