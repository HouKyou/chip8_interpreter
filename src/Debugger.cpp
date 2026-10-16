#include "Debugger.hpp"
#include "Chip8.hpp"
#include "Disassembler.hpp"
#include <iostream>
#include <iomanip>

void Debugger::printState(const Chip8& chip8) {
    std::cout << "PC: 0x"
        << std::hex
        << std::uppercase
        << std::setw(3)
        << std::setfill('0')
        << chip8.programCounter()
        << "\nOP: 0x"
        << std::setw(4)
        << chip8.opcode()
        << "\nI: 0x"
        << std::setw(3)
        << chip8.indexRegister();


    const auto& stack = chip8.stack();
    const uint16_t stackPointer = chip8.stackPointer();
    std::cout << "\nSP: 0x"
        << std::setw(3)
        << stackPointer
        << " Top: ";
    if (stackPointer == 0) {
        std::cout << "Empty";
    }
    else {
        std::cout 
            << "0x"
            << std::hex 
            << std::setw(3)
            << stack[stackPointer - 1];
    }

    std::cout << "\n\n";

    const auto& registers = chip8.registers();
    std::cout << "Register state:" << '\n';
    for (size_t i = 0; i < registers.size() / 2; ++i) {
        std::cout
            << "V"
            << std::hex
            << std::uppercase
            << i
            << ": 0x"
            << std::setfill('0')
            << std::setw(2)
            << static_cast<int>(registers[i]);
        std::cout << "        ";
        std::cout
            << "V"
            << std::hex
            << std::uppercase
            << i + 8
            << ": 0x"
            << std::setfill('0')
            << std::setw(2)
            << static_cast<int>(registers[i + 8])
            << '\n';
        
    }
    std::cout << '\n';
    std::cout << std::dec
        << std::nouppercase
        << std::setfill(' ');
}
