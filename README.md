# CHIP-8 Interpreter

A CHIP-8 emulator written in C++20 with SDL3 featuring opcode dispatch tables, interactive debugging tools, and instruction disassembly.

## Features

- Complete CHIP-8 CPU emulation
- SDL3-based graphics rendering
- Keyboard input handling
- Opcode dispatch-table architecture
- Interactive debugger
- Instruction disassembler

## Screenshots

Add screenshots or GIFs here once available.

## Architecture

The emulator is organized around a CHIP-8 virtual machine implementation.

### CPU

Responsible for:

- Opcode fetch
- Opcode decode
- Opcode execution
- Timer updates
- Register management
- Stack operations

### Display

SDL3 is used to render the CHIP-8 display buffer.

CHIP-8 resolution:

```text
64 x 32
```

### Input

Maps keyboard input to CHIP-8 hexadecimal keys:

```text
1 2 3 C
4 5 6 D
7 8 9 E
A 0 B F
```

### Disassembler

The disassembler converts raw opcodes into human-readable instructions.

```text
6AFF -> LD VA, 0xFF
7A01 -> ADD VA, 0x01
DAB5 -> DRW VA, VB, 0x5
```

### Debugger

Runtime debugging tools include:

- Program Counter inspection
- Opcode inspection
- Index Register inspection
- Stack inspection
- Register dumps
- Instruction disassembly surrounding the current PC

### Opcode Dispatch Tables

Rather than using a single large switch statement, opcode execution is implemented using dispatch tables.

```cpp
table[0x1] = &Chip8::opcode1nnn;
table[0x2] = &Chip8::opcode2nnn;
table[0x3] = &Chip8::opcode3xkk;
```

Execution:

```cpp
(this->*table[(opcode_ & 0xF000) >* 12])();
```

## Building

### Requirements

- C++20 compatible compi*er
- CMake 3.8+
- SDL3

### Build
```bash
cmake -B build
cmake --build build
```

This project is provided for educational purposes.
