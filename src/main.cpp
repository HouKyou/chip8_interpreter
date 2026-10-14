#include "Chip8.hpp"
#include "SDLApp.hpp"
#include <iostream>

constexpr int cyclePerFrame{ 10 };
constexpr auto frameDuration = std::chrono::microseconds{ 16667 };
constexpr std::string_view inputFolder{ "input" };

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: chip8 <rom_file>\n";
		return 1;
	}
	const auto romPath = std::filesystem::path{ inputFolder } / argv[1];
	if (!std::filesystem::exists(romPath)) {
		std::cerr << "Rom not found: " << romPath << '\n';
		return 1;
	}
	Chip8 chip8;
	if (!chip8.readRom(romPath)) {
		return 1;
	}

	SDLApp app;
	bool running{ true };

	while (running) {
		auto frameStart = std::chrono::steady_clock::now();
		running = app.processKeypad(chip8.keypad());
		for (int i = 0; i < cyclePerFrame; ++i) {
			chip8.cpuCycle();
		}
		chip8.updateTimers();
		app.renderDisplay(chip8.display());
		auto frameTime = std::chrono::steady_clock::now() - frameStart;
		if (frameTime < frameDuration) {
			auto sleepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(frameDuration - frameTime);
			SDL_Delay(sleepDuration.count());
		}
	}
	
	return 0;
}
