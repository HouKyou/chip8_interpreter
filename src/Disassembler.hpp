#pragma once

#include<cstdint>
#include<string>

class Chip8;

namespace Disassembler {
	std::string decode(uint16_t opcode);
	void printDisassembly(Chip8& chip8);
};
