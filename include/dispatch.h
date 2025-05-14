#ifndef DISPATCH_H
#define DISPATCH_H

#include "chip8.h"

// Function pointer type for opcode handlers
typedef void (*OpcodeHandler)(Chip8 *chip8, uint16_t opcode);

// Initialize main dispatch table and subdispatch tables
void opcode_dispatch_init(void);

// Decode and dispatch an opcode to the appropriate handler
bool dispatch_opcode(Chip8 *chip8, uint16_t opcode);

// Special-case dispatchers that need further decoding based on lower bits
void op_0xxx(Chip8 *chip8, uint16_t opcode); // For opcodes starting with 0x0***
void op_8xxx(Chip8 *chip8, uint16_t opcode); // For opcodes starting with 0x8***
void op_Exxx(Chip8 *chip8, uint16_t opcode); // For opcodes starting with 0xE***
void op_Fxxx(Chip8 *chip8, uint16_t opcode); // For opcodes starting with 0xF***

#endif
