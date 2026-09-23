#include "Chip8.hpp"
#include <fstream>
#include <iostream>
#include <vector>

Chip8::Chip8() {
	initialize();
}

void Chip8::setFonts() {
	std::copy(fontTable.begin(),
		fontTable.end(),
		memory_.begin() + FontsStartAddress);
}

void Chip8::initialize() {
	setFonts();
}

bool Chip8::readRom(const std::filesystem::path& romPath) {
	std::ifstream file(romPath, std::ios::binary | std::ios::ate);
	if (!file) {
		std::cerr << "Failed to open ROM" << '\n';
		return 1;
	}
	const std::size_t romSize = static_cast<std::size_t>(file.tellg());
	if (romSize > MemorySize - StartAddress) {
		std::cerr << "Not enough memory for " << romPath.filename() << '\n';
		return 1;
	}
	std::cout << "Loading rom: " << romPath.filename().string() << '\n';
	std::cout << "Size: " << romSize << '\n';

	file.seekg(0, std::ios::beg);
	file.read(reinterpret_cast<char*>(memory_.data() + StartAddress),
		static_cast<std::streamsize>(romSize));
	if (!file) {
		std::cerr << "Failed to read ROM" << '\n';
		return 1;
	}
	return 0;
}

void Chip8::dumpMemory() const {
	std::cout << "Memory dump:" << std::endl;
	for (std::size_t i = 0; i < MemorySize; ++i) {
		if (i % 16 == 0) {
			std::cout << '\n'
				<< std::hex
				<< std::setw(4)
				<< std::setfill('0')
				<< i
				<< ': ';
		}
		std::cout << std::hex
			<< std::setw(2)
			<< std::setfill('0')
			<< static_cast<int>(memory_[i])
			<< ' ';
	}
	std::cout << std::dec << '\n';
}

uint8_t Chip8::getRandByte() {
	std::cout << "random byte: " << randByte(rng) << '\n';
	return static_cast<uint8_t>(randByte(rng));
}

void Chip8::opcode00E0() {
	display_.fill(0);
}

void Chip8::opcode00EE() {
	programCounter_ = stack_[--stackPointer_];
}

void Chip8::opcode1NNN(uint16_t address) {
	programCounter_ = address;
}

void Chip8::opcode2NNN(uint16_t address) {
	stack_[stackPointer_++] = programCounter_;
	opcode1NNN(address);
}

