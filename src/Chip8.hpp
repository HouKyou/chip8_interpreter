#pragma once

#include <cstdint>
#include <array>
#include <filesystem>
#include <random>

class Chip8 {
private:

	static constexpr std::size_t RegisterCount = 16;
	static constexpr std::size_t MemorySize = 4096;
	static constexpr std::size_t StackSize = 16;
	static constexpr uint16_t StartAddress = 0x200;
	static constexpr std::size_t KeyCount = 16;
	static constexpr std::size_t DisplayWidth = 64;
	static constexpr std::size_t DisplayHeight = 32;
	static constexpr std::size_t FontTableSize = 80;
	static constexpr std::size_t FontsStartAddress = 0x50;

	static constexpr std::array<uint8_t, FontTableSize> fontTable = {
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
	uint16_t indexRegister_{};
	uint16_t programCounter_{ StartAddress };
	std::array<uint16_t, StackSize> stack_{};
	uint8_t stackPointer_{};
	uint8_t delayTimer_{};
	uint8_t soundTimer_{};

	std::array<uint8_t, KeyCount> keypad_{};
	std::array<uint32_t, DisplayWidth * DisplayHeight> display_{};
	std::mt19937 rng{ std::random_device{}() };
	std::uniform_int_distribution<int> randByte{ 0, 255U };

public:
	Chip8();
	void initialize();
	void setFontsTable();
	bool readRom(const std::filesystem::path& romPath);
	void dumpMemory() const;
	uint8_t getRandByte();

	void jump(uint16_t addr);

	//00E0 - CLS
	//Clear the display.
	void opcode00E0();
	
	// 00EE - RET
	// Return from a subroutine.
	void opcode00EE();

	//1nnn - JP addr
	//Jump to location nnn.
	void opcode1nnn(uint16_t addr);

	//2nnn - CALL addr
	//Call subroutine at nnn.
	void opcode2nnn(uint16_t addr);

	//3xkk - SE Vx, byte
	//Skip next instruction if Vx = kk.
	void opcode3xkk(uint8_t vx, uint8_t val);

	//4xkk - SNE Vx, byte
	//Skip next instruction if Vx != kk.
	void opcode4xkk(uint8_t vx, uint8_t val);

	//5xy0 - SE Vx, Vy
	//Skip next instruction if Vx = Vy.
	void opcode5xy0(uint8_t vx, uint8_t vy);

	//6xkk - LD Vx, byte
	//Set Vx = kk.
	void opcode6xkk(uint8_t vx, uint8_t val);

	//7xkk - ADD Vx, byte
	//Set Vx = Vx + kk.
	void opcode7xkk(uint8_t vx, uint8_t val);
	
	//8xy0 - LD Vx, Vy
	//Set Vx = Vy.
	void opcode8xy0(uint8_t vx, uint8_t vy);

	//8xy1 - OR Vx, Vy
	//Set Vx = Vx OR Vy.
	void opcode8xy1(uint8_t vx, uint8_t vy);

	//8xy2 - AND Vx, Vy
	//Set Vx = Vx AND Vy.
	void opcode8xy2(uint8_t vx, uint8_t vy);

	//8xy3 - XOR Vx, Vy
	//Set Vx = Vx XOR Vy.
	void opcode8xy3(uint8_t vx, uint8_t vy);

    //8xy4 - ADD Vx, Vy
    //Set Vx = Vx + Vy, set VF = carry.
	void opcode8xy4(uint8_t vx, uint8_t vy);
    
    //8xy5 - SUB Vx, Vy
    //Set Vx = Vx - Vy, set VF = NOT borrow.
	void opcode8xy5(uint8_t vx, uint8_t vy);

    //8xy6 - SHR Vx {, Vy}
    //8Set Vx = Vx SHR 1.
	void opcode8xy6(uint8_t vx, uint8_t vy);

    //8xy7 - SUBN Vx, Vy
    //Set Vx = Vy - Vx, set VF = NOT borrow.
	void opcode8xy7(uint8_t vx, uint8_t vy);

    //8xyE - SHL Vx {, Vy}
    //Set Vx = Vx SHL 1.
	void opcode8xyE(uint8_t vx, uint8_t vy);

	//9xy0 - SNE Vx, Vy
	//Skip next instruction if Vx != Vy.
	void opcode9xy0(uint8_t vx, uint8_t vy);

	//Annn - LD I, addr
	//Set I = nnn.
	void opcodeAnnn(uint16_t addr);

	//Bnnn - JP V0, addr
	//Jump to location nnn + V0.
	void opcodeBnnn(uint16_t addr);

	//Cxkk - RND Vx, byte
	//Set Vx = random byte AND kk.
	void opcodeCxkk(uint8_t vx, uint8_t val);

	//Dxyn - DRW Vx, Vy, nibble
	//Display n - byte sprite starting at memory location I at(Vx, Vy), 
	// set VF = collision.
	void opcodeDxyn(uint8_t vx, uint8_t vy, uint8_t n);
};
