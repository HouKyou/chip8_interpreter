#include "Chip8.hpp"
#include "SDLApp.hpp"
#include "Debugger.hpp"
#include "Disassembler.hpp"
#include <iostream>

constexpr int cyclePerFrame{ 10 };
constexpr auto frameDuration = std::chrono::microseconds{ 16667 };

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: chip8 <rom_file>\n";
		return 1;
	}
	const auto romPath = std::filesystem::path{ argv[1] };
	//const auto romPath = std::filesystem::path{ "input/pong.rom" };
	if (!std::filesystem::exists(romPath)) {
		std::cerr << "Rom not found: " << romPath << '\n';
		return 1;
	}
	Chip8 chip8;
	if (!chip8.readRom(romPath)) {
		return 1;
	}

	SDLApp app;
	Debugger debugger;
	bool running{ true };

	while (running) {
		auto frameStart = std::chrono::steady_clock::now();

		running = app.processEvents(chip8.keypad(), debugger);
		for (int i = 0; i < cyclePerFrame; ++i) {
			if (debugger.isPaused()) {
				if (debugger.takeStepRequest()) {
					chip8.cpuCycle();
					debugger.printState(chip8);
					Disassembler::printDisassembly(chip8);
				}
				break;
			}
			chip8.cpuCycle();
		}

		if (!debugger.isPaused()) {
			chip8.updateTimers();
		}
		
		if (chip8.drawFlag()) {
			app.renderDisplay(chip8.display());
			chip8.resetDrawFlag();
		}
		auto frameTime = std::chrono::steady_clock::now() - frameStart;
		if (frameTime < frameDuration) {
			auto sleepDuration = frameDuration - frameTime;
			SDL_Delay(
				static_cast<Uint32>(
					std::chrono::duration_cast<
						std::chrono::milliseconds>(
							sleepDuration
					).count()
				)
			);

		}
	}
	
	return 0;
}
