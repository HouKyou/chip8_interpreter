#include "Chip8.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>

Chip8::Chip8() {
	initialize();
}

void Chip8::setFontsTable() {
	std::copy(fontTable.begin(),
		fontTable.end(),
		memory_.begin() + FontsStartAddress);
}

void Chip8::initialize() {
	registers_.fill(0);
	memory_.fill(0);
	stack_.fill(0);
	keypad_.fill(0);
	display_.fill(0);

	indexRegister_ = 0;
	programCounter_ = StartAddress;
	stackPointer_ = 0;
	delayTimer_ = 0;
	soundTimer_ = 0;

	setFontsTable();
}

bool Chip8::readRom(const std::filesystem::path& romPath) {
	std::ifstream file(romPath, std::ios::binary | std::ios::ate);
	if (!file) {
		std::cerr << "Failed to open ROM" << '\n';
		return false;
	}
	const std::size_t romSize = static_cast<std::size_t>(file.tellg());
	if (romSize > MemorySize - StartAddress) {
		std::cerr << "Not enough memory for " << romPath.filename() << '\n';
		return false;
	}
	std::cout << "Loading rom: " << romPath.filename().string() << '\n';
	std::cout << "Size: " << romSize << '\n';

	file.seekg(0, std::ios::beg);
	file.read(reinterpret_cast<char*>(memory_.data() + StartAddress),
		static_cast<std::streamsize>(romSize));
	if (!file) {
		std::cerr << "Failed to read ROM" << '\n';
		return false;
	}
	return true;
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
	assert(stackPointer_ > 0);
	programCounter_ = stack_[--stackPointer_];
}

void Chip8::opcode1nnn(uint16_t addr) {
	jump(addr);
}

void Chip8::opcode2nnn(uint16_t addr) {
	assert(stackPointer_ < stack_.size());
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
	const uint16_t sum = registers_[vx] + registers_[vy];
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
	registers_[0xF] = 0;
	const uint8_t x = registers_[vx];
	const uint8_t y = registers_[vy];

	for (uint8_t row = 0; row < n; ++row) {
		const uint8_t spriteByte = memory_[indexRegister_ + row];
		for (uint8_t col = 0; col < 8; ++col) {
			const uint8_t spriteBit = (spriteByte >> (7 - col)) & 0x1;
			if (!spriteBit) continue;
			
			const size_t px = (x + col) % DisplayWidth;
			const size_t py = (y + row) % DisplayHeight;
 			const size_t index = py * DisplayWidth + px;
			if (display_[index]) 
				registers_[0xF] = 1;
			display_[index] ^= 1;
		}
	}
}

void Chip8::opcodeEx9E(uint8_t vx) {
	if (keypad_[registers_[vx] & 0xF])
		programCounter_ += 2;
}

void Chip8::opcodeExA1(uint8_t vx) {
	if (!keypad_[registers_[vx] & 0xF])
		programCounter_ += 2;
}

void Chip8::opcodeFx07(uint8_t vx) {
	registers_[vx] = delayTimer_;
}

void Chip8::opcodeFx0A(uint8_t vx) {
	for (size_t i = 0; i < 16; ++i) {
		if (keypad_[i]) {
			registers_[vx] = i;
			return;
		}
	}
	programCounter_ -= 2;
}

void Chip8::opcodeFx15(uint8_t vx) {
	delayTimer_ = registers_[vx];
}

void Chip8::opcodeFx18(uint8_t vx) {
	soundTimer_ = registers_[vx];
}

void Chip8::opcodeFx1E(uint8_t vx) {
	indexRegister_ += registers_[vx];
}

void Chip8::opcodeFx29(uint8_t vx) {
	indexRegister_ = registers_[vx] * 5 + FontsStartAddress;
}

void Chip8::opcodeFx33(uint8_t vx) {
	const uint8_t val =  registers_[vx];
	memory_[indexRegister_] = val / 100;
	memory_[indexRegister_ + 1] = val / 10 % 10;
	memory_[indexRegister_ + 2] = val % 10;
}

void Chip8::opcodeFx55(uint8_t vx) {
	for (uint8_t i = 0; i <= vx; ++i) {
		memory_[indexRegister_ + i] = registers_[i];
	}
}

void Chip8::opcodeFx65(uint8_t vx) {
	for (uint8_t i = 0; i <= vx; ++i) {
		registers_[i] = memory_[indexRegister_ + i];
	}
}
