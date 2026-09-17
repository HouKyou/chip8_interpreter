// chip8.cpp : Defines the entry point for the application.
//

#include "Chip8.hpp"
#include <iostream>

int main()
{
	std::cout << "Chip 8 running..." << '\n';
	std::filesystem::path romPath = "D:/code/source/re/repo/pj_chip8/input/IBM Logo.ch8";
	Chip8 chip8;
	chip8.readRom(romPath);
	chip8.dumpMemory();
	return 0;
}
