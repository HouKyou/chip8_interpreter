#include "Chip8.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <iomanip>

Chip8::Chip8() {
	initialize();
	setupDispatchTable();
}

void Chip8::cpuCycle() {
	assert(programCounter_ + 1 < MemorySize);
	cpuFetch();
	tableDispatch();
	
}

void Chip8::cpuFetch() {
	opcode_ = memory_[programCounter_] << 8 | memory_[programCounter_ + 1];
	programCounter_ += 2;
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

void Chip8::updateTimers() {
	if (delayTimer_ > 0) --delayTimer_;
	if (soundTimer_ > 0) --soundTimer_;
}

void Chip8::resetDrawFlag() {
	drawFlag_ = false;
}

void Chip8::initialize() {
	registers_.fill(0);
	memory_.fill(0);
	stack_.fill(0);
	keypad_.fill(0);
	display_.fill(0);

	indexRegister_ = 0;
	programCounter_ = StartAddress;
	opcode_ = 0;
	stackPointer_ = 0;
	delayTimer_ = 0;
	soundTimer_ = 0;

	setFontsTable();
}

void Chip8::setFontsTable() {
	std::copy(fontTable.begin(),
		fontTable.end(),
		memory_.begin() + FontsStartAddress);
}

void Chip8::setupDispatchTable() {
	table.fill(&Chip8::opcodeNull);
	table0.fill(&Chip8::opcodeNull);
	table8.fill(&Chip8::opcodeNull);
	tableE.fill(&Chip8::opcodeNull);
	tableF.fill(&Chip8::opcodeNull);

	table[0x0] = &Chip8::table0Dispatch;
	table[0x1] = &Chip8::opcode1nnn;
	table[0x2] = &Chip8::opcode2nnn;
	table[0x3] = &Chip8::opcode3xkk;
	table[0x4] = &Chip8::opcode4xkk;
	table[0x5] = &Chip8::opcode5xy0;
	table[0x6] = &Chip8::opcode6xkk;
	table[0x7] = &Chip8::opcode7xkk;
	table[0x8] = &Chip8::table8Dispatch;
	table[0x9] = &Chip8::opcode9xy0;
	table[0xA] = &Chip8::opcodeAnnn;
	table[0xB] = &Chip8::opcodeBnnn;
	table[0xC] = &Chip8::opcodeCxkk;
	table[0xD] = &Chip8::opcodeDxyn;
	table[0xE] = &Chip8::tableEDispatch;
	table[0xF] = &Chip8::tableFDispatch;

	table0[0xE0] = &Chip8::opcode00E0;
	table0[0xEE] = &Chip8::opcode00EE;

	table8[0x0] = &Chip8::opcode8xy0;
	table8[0x1] = &Chip8::opcode8xy1;
	table8[0x2] = &Chip8::opcode8xy2;
	table8[0x3] = &Chip8::opcode8xy3;
	table8[0x4] = &Chip8::opcode8xy4;
	table8[0x5] = &Chip8::opcode8xy5;
	table8[0x6] = &Chip8::opcode8xy6;
	table8[0x7] = &Chip8::opcode8xy7;
	table8[0xE] = &Chip8::opcode8xyE;

	tableE[0xA1] = &Chip8::opcodeExA1;
	tableE[0x9E] = &Chip8::opcodeEx9E;

	tableF[0x07] = &Chip8::opcodeFx07;
	tableF[0x0A] = &Chip8::opcodeFx0A;
	tableF[0x15] = &Chip8::opcodeFx15;
	tableF[0x18] = &Chip8::opcodeFx18;
	tableF[0x1E] = &Chip8::opcodeFx1E;
	tableF[0x29] = &Chip8::opcodeFx29;
	tableF[0x33] = &Chip8::opcodeFx33;
	tableF[0x55] = &Chip8::opcodeFx55;
	tableF[0x65] = &Chip8::opcodeFx65;
}

void Chip8::tableDispatch() {
	(this->*table[(opcode_ & 0xF000) >> 12])();
}

void Chip8::table0Dispatch() {
	(this->*table0[opcode_ & 0x00FF])();
}

void Chip8::table8Dispatch() {
	(this->*table8[opcode_ & 0x000F])();
}

void Chip8::tableEDispatch() {
	(this->*tableE[opcode_ & 0x00FF])();
}

void Chip8::tableFDispatch() {
	(this->*tableF[opcode_ & 0x00FF])();
}

uint16_t Chip8::decodeNNN() const {
	return opcode_ & 0x0FFF;
}

uint8_t Chip8::decodeN() const {
	return opcode_ & 0x000F;
}

uint8_t Chip8::decodeX() const {
	return (opcode_ & 0x0F00) >> 8;
}

uint8_t Chip8::decodeY() const {
	return (opcode_ & 0x00F0) >> 4;
}

uint8_t Chip8::decodeKK() const {
	return opcode_ & 0x00FF;
}

void Chip8::jump(uint16_t addr) {
	programCounter_ = addr;
}

uint8_t Chip8::getRandByte() {
	return static_cast<uint8_t>(randByte(rng));
}

void Chip8::opcodeNull() {
	std::cerr << "Unknown opcode: 0x"
		<< std::hex
		<< opcode_
		<< '\n';
}

void Chip8::opcode00E0() {
	display_.fill(0);
	drawFlag_ = true;
}

void Chip8::opcode00EE() {
	assert(stackPointer_ > 0);
	programCounter_ = stack_[--stackPointer_];
}

void Chip8::opcode1nnn() {
	const uint16_t addr = decodeNNN();
	jump(addr);
}

void Chip8::opcode2nnn() {
	assert(stackPointer_ < stack_.size());
	stack_[stackPointer_++] = programCounter_;
	const uint16_t addr = decodeNNN();
	jump(addr);
}

void Chip8::opcode3xkk() {
	const uint8_t x = decodeX();
	const uint8_t kk = decodeKK();
	if (registers_[x] == kk)
		programCounter_ += 2;
}

void Chip8::opcode4xkk() {
	const uint8_t x = decodeX();
	const uint8_t kk = decodeKK();
	if (registers_[x] != kk)
		programCounter_ += 2;
}

void Chip8::opcode5xy0() {
	const uint8_t x = decodeX();
	const uint8_t y = decodeY();
	if (registers_[x] == registers_[y])
		programCounter_ += 2;
}

void Chip8::opcode6xkk() {
	const uint8_t x = decodeX();
	const uint8_t kk = decodeKK();
	registers_[x] = kk;
}

void Chip8::opcode7xkk() {
	const uint8_t x = decodeX();
	const uint8_t kk = decodeKK();
	registers_[x] += kk;
}

void Chip8::opcode8xy0() {
	const uint8_t x = decodeX();
	const uint8_t y = decodeY();
	registers_[x] = registers_[y];
}

void Chip8::opcode8xy1() {
	const uint8_t x = decodeX();
	const uint8_t y = decodeY();
	registers_[x] |= registers_[y];
}

void Chip8::opcode8xy2() {
	const uint8_t x = decodeX();
	const uint8_t y = decodeY();
	registers_[x] &= registers_[y];
}

void Chip8::opcode8xy3() {
	const uint8_t x = decodeX();
	const uint8_t y = decodeY();
	registers_[x] ^= registers_[y];
}

void Chip8::opcode8xy4() {
	const uint8_t x = decodeX();
	const uint8_t y = decodeY();
	const uint16_t sum = registers_[x] + registers_[y];
	registers_[0xF] = (sum > 0xFF);
	registers_[x] = sum & 0xFF;
}

void Chip8::opcode8xy5() {
	const uint8_t x = decodeX();
	const uint8_t y = decodeY();
	registers_[0xF] = (registers_[x] >= registers_[y]);
	registers_[x] -= registers_[y];
}

void Chip8::opcode8xy6() {
	const uint8_t x = decodeX();
	//const uint8_t y = decodeY();
	registers_[0xF] = (registers_[x] & 0x1);
	registers_[x] >>= 1;
}

void Chip8::opcode8xy7() {
	const uint8_t x = decodeX();
	const uint8_t y = decodeY();
	registers_[0xF] = (registers_[y] >= registers_[x]);
	registers_[x] = registers_[y] - registers_[x];
}

void Chip8::opcode8xyE() {
	const uint8_t x = decodeX();
	//const uint8_t y = decodeY();
	registers_[0xF] = registers_[x] >> 7;
	registers_[x] <<= 1;
}

void Chip8::opcode9xy0() {
	const uint8_t x = decodeX();
	const uint8_t y = decodeY();
	if (registers_[x] != registers_[y]) {
		programCounter_ += 2;
	}
}

void Chip8::opcodeAnnn() {
	const uint16_t addr = decodeNNN();
	indexRegister_ = addr;
}

void Chip8::opcodeBnnn() {
	const uint16_t addr = decodeNNN();
	jump(addr + registers_[0]);
}

void Chip8::opcodeCxkk() {
	const uint8_t x = decodeX();
	const uint8_t kk = decodeKK();
	registers_[x] = getRandByte() & kk;
}

void Chip8::opcodeDxyn() {
	const uint8_t x = decodeX();
	const uint8_t y = decodeY();
	const uint8_t n = decodeN();

	const uint8_t vx = registers_[x];
	const uint8_t vy = registers_[y];
	registers_[0xF] = 0;

	for (uint8_t row = 0; row < n; ++row) {
		assert(indexRegister_ + row < MemorySize);
		const uint8_t spriteByte = memory_[indexRegister_ + row];
		for (uint8_t col = 0; col < 8; ++col) {
			const uint8_t spriteBit = (spriteByte >> (7 - col)) & 0x1;
			if (!spriteBit) continue;
			
			const size_t px = (vx + col) % DisplayWidth;
			const size_t py = (vy + row) % DisplayHeight;
 			const size_t index = py * DisplayWidth + px;
			if (display_[index]) 
				registers_[0xF] = 1;
			display_[index] ^= 1;
		}
	}
	drawFlag_ = true;
}

void Chip8::opcodeEx9E() {
	const uint8_t x = decodeX();
	if (keypad_[registers_[x] & 0xF])
		programCounter_ += 2;
}

void Chip8::opcodeExA1() {
	const uint8_t x = decodeX();
	if (!keypad_[registers_[x] & 0xF])
		programCounter_ += 2;
}

void Chip8::opcodeFx07() {
	const uint8_t x = decodeX();
	registers_[x] = delayTimer_;
}

void Chip8::opcodeFx0A() {
	const uint8_t x = decodeX();
	for (size_t i = 0; i < 16; ++i) {
		if (keypad_[i]) {
			registers_[x] = i;
			return;
		}
	}
	programCounter_ -= 2;
}

void Chip8::opcodeFx15() {
	const uint8_t x = decodeX();
	delayTimer_ = registers_[x];
}

void Chip8::opcodeFx18() {
	const uint8_t x = decodeX();
	soundTimer_ = registers_[x];
}

void Chip8::opcodeFx1E() {
	const uint8_t x = decodeX();
	indexRegister_ += registers_[x];
}

void Chip8::opcodeFx29() {
	const uint8_t x = decodeX();
	indexRegister_ = registers_[x] * 5 + FontsStartAddress;
}

void Chip8::opcodeFx33() {
	assert(indexRegister_ + 2 < MemorySize);
	const uint8_t x = decodeX();
	const uint8_t vx =  registers_[x];
	memory_[indexRegister_] = vx / 100;
	memory_[indexRegister_ + 1] = vx / 10 % 10;
	memory_[indexRegister_ + 2] = vx % 10;
}

void Chip8::opcodeFx55() {
	const uint8_t x = decodeX();
	assert(indexRegister_ + x < MemorySize);
	for (uint8_t i = 0; i <= x; ++i) {
		memory_[indexRegister_ + i] = registers_[i];
	}
}

void Chip8::opcodeFx65() {
	const uint8_t x = decodeX();
	assert(indexRegister_ + x < MemorySize);
	for (uint8_t i = 0; i <= x; ++i) {
		registers_[i] = memory_[indexRegister_ + i];
	}
}
