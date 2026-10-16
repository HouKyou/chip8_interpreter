#include "Disassembler.hpp"
#include "Chip8.hpp"
#include<sstream>
#include<iomanip>
#include<iostream>

std::string Disassembler::decode(uint16_t opcode) {
    const uint8_t x = (opcode & 0x0F00) >> 8;
    const uint8_t y = (opcode & 0x00F0) >> 4;
    const uint8_t n = opcode & 0x000F;
    const uint8_t kk = opcode & 0x00FF;
    const uint16_t nnn = opcode & 0x0FFF;

    std::stringstream ss;

    ss << std::uppercase;
    switch (opcode & 0xF000) {
    case 0x0000:
        switch (opcode) {
        case 0x00E0: return "CLS";
        case 0x00EE: return "RET";
        default:
            ss << "SYS 0x" << std::hex << nnn;
            return ss.str();
        }

    case 0x1000:
        ss << "JP 0x" << std::hex << nnn;
        return ss.str();

    case 0x2000:
        ss << "CALL 0x" << std::hex << nnn;
        return ss.str();

    case 0x3000:
        ss << "SE V" << std::hex << static_cast<int>(x)
            << ", 0x" << static_cast<int>(kk);
        return ss.str();

    case 0x4000:
        ss << "SNE V" << std::hex << static_cast<int>(x)
            << ", 0x" << static_cast<int>(kk);
        return ss.str();

    case 0x5000:
        if ((opcode & 0x000F) == 0) {
            ss << "SE V" << std::hex << static_cast<int>(x)
                << ", V" << static_cast<int>(y);
            return ss.str();
        }
        break;

    case 0x6000:
        ss << "LD V" << std::hex << static_cast<int>(x)
            << ", 0x" << static_cast<int>(kk);
        return ss.str();

    case 0x7000:
        ss << "ADD V" << std::hex << static_cast<int>(x)
            << ", 0x" << static_cast<int>(kk);
        return ss.str();

    case 0x8000:
        switch (opcode & 0x000F) {
        case 0x0:
            ss << "LD V" << std::hex << static_cast<int>(x)
                << ", V" << static_cast<int>(y);
            return ss.str();

        case 0x1:
            ss << "OR V" << std::hex << static_cast<int>(x)
                << ", V" << static_cast<int>(y);
            return ss.str();

        case 0x2:
            ss << "AND V" << std::hex << static_cast<int>(x)
                << ", V" << static_cast<int>(y);
            return ss.str();

        case 0x3:
            ss << "XOR V" << std::hex << static_cast<int>(x)
                << ", V" << static_cast<int>(y);
            return ss.str();

        case 0x4:
            ss << "ADD V" << std::hex << static_cast<int>(x)
                << ", V" << static_cast<int>(y);
            return ss.str();

        case 0x5:
            ss << "SUB V" << std::hex << static_cast<int>(x)
                << ", V" << static_cast<int>(y);
            return ss.str();

        case 0x6:
            ss << "SHR V" << std::hex << static_cast<int>(x);
            return ss.str();

        case 0x7:
            ss << "SUBN V" << std::hex << static_cast<int>(x)
                << ", V" << static_cast<int>(y);
            return ss.str();

        case 0xE:
            ss << "SHL V" << std::hex << static_cast<int>(x);
            return ss.str();
        }
        break;

    case 0x9000:
        if ((opcode & 0x000F) == 0) {
            ss << "SNE V" << std::hex << static_cast<int>(x)
                << ", V" << static_cast<int>(y);
            return ss.str();
        }
        break;

    case 0xA000:
        ss << "LD I, 0x" << std::hex << nnn;
        return ss.str();

    case 0xB000:
        ss << "JP V0, 0x" << std::hex << nnn;
        return ss.str();

    case 0xC000:
        ss << "RND V" << std::hex << static_cast<int>(x)
            << ", 0x" << static_cast<int>(kk);
        return ss.str();

    case 0xD000:
        ss << "DRW V" << std::hex << static_cast<int>(x)
            << ", V" << static_cast<int>(y)
            << ", 0x" << static_cast<int>(n);
        return ss.str();

    case 0xE000:
        switch (opcode & 0x00FF) {
        case 0x9E:
            ss << "SKP V" << std::hex << static_cast<int>(x);
            return ss.str();

        case 0xA1:
            ss << "SKNP V" << std::hex << static_cast<int>(x);
            return ss.str();
        }
        break;

    case 0xF000:
        switch (opcode & 0x00FF) {
        case 0x07:
            ss << "LD V" << std::hex << static_cast<int>(x)
                << ", DT";
            return ss.str();

        case 0x0A:
            ss << "LD V" << std::hex << static_cast<int>(x)
                << ", K";
            return ss.str();

        case 0x15:
            ss << "LD DT, V" << std::hex << static_cast<int>(x);
            return ss.str();

        case 0x18:
            ss << "LD ST, V" << std::hex << static_cast<int>(x);
            return ss.str();

        case 0x1E:
            ss << "ADD I, V" << std::hex << static_cast<int>(x);
            return ss.str();

        case 0x29:
            ss << "LD F, V" << std::hex << static_cast<int>(x);
            return ss.str();

        case 0x33:
            ss << "LD B, V" << std::hex << static_cast<int>(x);
            return ss.str();

        case 0x55:
            ss << "LD [I], V0-V" << std::hex << static_cast<int>(x);
            return ss.str();

        case 0x65:
            ss << "LD V0-V" << std::hex << static_cast<int>(x)
                << ", [I]";
            return ss.str();
        }
        break;
    }

    ss << "UNKNOWN 0x"
        << std::hex
        << opcode;

    return ss.str();
}

void Disassembler::printDisassembly(Chip8& chip8) {
    const auto& memory = chip8.memory();
    const uint16_t programCounter = chip8.programCounter();
    uint16_t start = (programCounter >= 10) ? programCounter - 10 : 0;

    for (size_t pc = start; pc < start + 20; pc += 2) {
        if (pc + 1 >= memory.size()) 
            break;
        uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
        if (pc == programCounter) {
            std::cout << "->    ";
        }
        else {
            std::cout << "      ";
        }
        std::cout << "0x"
            << std::hex
            << std::uppercase
            << std::setw(3)
            << std::setfill('0')
            << pc
            << ": "
            << decode(opcode)
            << '\n';
    }
    std::cout << "-------------------------\n\n";
    std::cout << std::dec << std::setfill(' ') << std::nouppercase;
}
