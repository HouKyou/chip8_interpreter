#pragma once

#include <cstdint>
#include <array>
#include <filesystem>

class Chip8 {
private:

	static constexpr std::size_t RegisterCount = 16;
	static constexpr std::size_t MemorySize = 4096;
	static constexpr std::size_t StackSize = 16;
	static constexpr uint16_t StartAddress = 0x200;
	static constexpr std::size_t KeyCount = 16;
	static constexpr std::size_t DisplayWidth = 64;
	static constexpr std::size_t DisplayHeight = 32;
	static constexpr std::size_t FontsSize = 80;
	static constexpr std::size_t FontsStartAddress = 0x50;

	static constexpr std::array<uint8_t, FontsSize> fontTable = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	std::array<uint8_t, RegisterCount> registers_{};
	std::array<uint8_t, MemorySize> memory_{};
	uint16_t opcode_{};
	uint16_t indexRegister_{};
	uint16_t programCounter_{ StartAddress };
	std::array<uint16_t, StackSize> stack_{};
	uint8_t stackPointer_{};
	uint8_t delayTimer_{};
	uint8_t soundTimer_{};

	std::array<uint8_t, KeyCount> keypad_{};
	std::array<uint32_t, DisplayWidth * DisplayHeight> display_{};
public:
	Chip8();
	void initialize();
	void setFonts();
	bool readRom(const std::filesystem::path& romPath);
	void dumpMemory() const;
};
