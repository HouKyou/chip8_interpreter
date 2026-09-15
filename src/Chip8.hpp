#pragma once

#include <cstdint>
#include <array>

inline constexpr std::size_t RegisterNum = 16;
inline constexpr std::size_t MemorySize = 4096;
inline constexpr std::size_t StackSize = 16;
inline constexpr uint16_t StartAddress = 0x200;
inline constexpr std::size_t KeyCount = 16;
inline constexpr std::size_t DisplayWidth = 64;
inline constexpr std::size_t DisplayHeight = 32;

class Chip8 {
private:
	std::array<uint8_t, RegisterNum> registers_{};
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
	Chip8() = default;
};
