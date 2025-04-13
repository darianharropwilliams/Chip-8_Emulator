#ifndef DISPATCH_H
#define DISPATCH_H

#include "chip8.h"

// Type alias for an opcode handler function
typedef void (*OpcodeHandler)(Chip8 *chip8, uint16_t opcode);

// Initialize all dispatch tables (main and subtables)
void opcode_dispatch_init(void);

// Given an opcode, decode and execute it using dispatch tables
bool dispatch_opcode(Chip8 *chip8, uint16_t opcode);


void op_0xxx(Chip8 *chip8, uint16_t opcode); // Requires subdispatching
void op_8xxx(Chip8 *chip8, uint16_t opcode); // Requires subdipatching
void op_Exxx(Chip8 *chip8, uint16_t opcode); // Requires subdispatching
void op_Fxxx(Chip8 *chip8, uint16_t opcode); // Requires subdispatching



#endif
