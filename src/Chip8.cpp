#include "Chip8.hpp"
#include <fstream>
#include <iostream>
#include <vector>

Chip8::Chip8() {
	initialize();
}

void Chip8::setFontsTable() {
	std::copy(fontTable.begin(),
		fontTable.end(),
		memory_.begin() + FontsStartAddress);
}

void Chip8::initialize() {
	setFontsTable();
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
	return static_cast<uint8_t>(randByte(rng));
}

void Chip8::jump(uint16_t addr) {
	programCounter_ = addr;
}

void Chip8::opcode00E0() {
	display_.fill(0);
}

void Chip8::opcode00EE() {
	programCounter_ = stack_[--stackPointer_];
}

void Chip8::opcode1nnn(uint16_t addr) {
	jump(addr);
}

void Chip8::opcode2nnn(uint16_t addr) {
	stack_[stackPointer_++] = programCounter_;
	jump(addr);
}

void Chip8::opcode3xkk(uint8_t vx, uint8_t val) {
	if (registers_[vx] == val)
		programCounter_ += 2;
}

void Chip8::opcode4xkk(uint8_t vx, uint8_t val) {
	if (registers_[vx] != val)
		programCounter_ += 2;
}

void Chip8::opcode5xy0(uint8_t vx, uint8_t vy) {
	if (registers_[vx] == registers_[vy])
		programCounter_ += 2;
}

void Chip8::opcode6xkk(uint8_t vx, uint8_t val) {
	registers_[vx] = val;
}

void Chip8::opcode7xkk(uint8_t vx, uint8_t val) {
	registers_[vx] += val;
}

void Chip8::opcode8xy0(uint8_t vx, uint8_t vy) {
	registers_[vx] = registers_[vy];
}

void Chip8::opcode8xy1(uint8_t vx, uint8_t vy) {
	registers_[vx] |= registers_[vy];
}

void Chip8::opcode8xy2(uint8_t vx, uint8_t vy) {
	registers_[vx] &= registers_[vy];
}

void Chip8::opcode8xy3(uint8_t vx, uint8_t vy) {
	registers_[vx] ^= registers_[vy];
}

void Chip8::opcode8xy4(uint8_t vx, uint8_t vy) {
	uint16_t sum = registers_[vx] + registers_[vy];
	registers_[0xF] = (sum > 0xFF);
	registers_[vx] = sum & 0xFF;
}

void Chip8::opcode8xy5(uint8_t vx, uint8_t vy) {
	registers_[0xF] = (registers_[vx] >= registers_[vy]);
	registers_[vx] -= registers_[vy];
}

void Chip8::opcode8xy6(uint8_t vx, uint8_t vy) {
	registers_[0xF] = (registers_[vx] & 0x1);
	registers_[vx] >>= 1;
}

void Chip8::opcode8xy7(uint8_t vx, uint8_t vy) {
	registers_[0xF] = (registers_[vy] >= registers_[vx]);
	registers_[vx] = registers_[vy] - registers_[vx];
}

void Chip8::opcode8xyE(uint8_t vx, uint8_t vy) {
	registers_[0xF] = registers_[vx] >> 7;
	registers_[vx] <<= 1;
}

void Chip8::opcode9xy0(uint8_t vx, uint8_t vy) {
	if (registers_[vx] != registers_[vy]) {
		programCounter_ += 2;
	}
}

void Chip8::opcodeAnnn(uint16_t addr) {
	indexRegister_ = addr;
}

void Chip8::opcodeBnnn(uint16_t addr) {
	jump(addr + registers_[0]);
}

void Chip8::opcodeCxkk(uint8_t vx, uint8_t val) {
	registers_[vx] = getRandByte() & val;
}

void Chip8::opcodeDxyn(uint8_t vx, uint8_t vy, uint8_t n) {
	for (size_t i = vx; i < DisplayHeight; i++) {
		int bit = 1;
		for (size_t j = vy; j < vy + n; j++) {
			size_t displayIdx = i * DisplayHeight + j;
			display_[i] ^= memory_[indexRegister_] & (0x80);
		}
	}
}
