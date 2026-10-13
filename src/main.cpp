// chip8.cpp : Defines the entry point for the application.
//

#include "Chip8.hpp"
#include "SDLApp.hpp"
#include <iostream>

int main() {
	static const std::filesystem::path romPath = "D:/code/source/re/repo/pj_chip8/input/IBM Logo.ch8";
	Chip8 chip8;
	chip8.readRom(romPath);
	SDLApp app;
	bool running = true;
	auto lastTimer = std::chrono::steady_clock::now();

	while (running) {
		running = app.processEvents(chip8.keypad());
		for (int i = 0; i < 10; ++i) {
			chip8.cpuCycle();
		}
		auto now = std::chrono::steady_clock::now();
		if (now - lastTimer >= std::chrono::milliseconds(16)) {
			chip8.updateTimers();
			lastTimer = now;
		}
		app.render(chip8.display());
		SDL_Delay(1);
	}
	
	return 0;
}

