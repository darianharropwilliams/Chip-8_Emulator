#ifndef OPCODES_H
#define OPCODES_H

#include "chip8.h"

// Main opcode handlers (mapped in main_table[0x0 - 0xF])
void op_0xxx(Chip8 *chip8, uint16_t opcode);
void op_1nnn(Chip8 *chip8, uint16_t opcode);
void op_2nnn(Chip8 *chip8, uint16_t opcode);
void op_3xkk(Chip8 *chip8, uint16_t opcode);
void op_4xkk(Chip8 *chip8, uint16_t opcode);
void op_5xy0(Chip8 *chip8, uint16_t opcode);
void op_6xkk(Chip8 *chip8, uint16_t opcode);
void op_7xkk(Chip8 *chip8, uint16_t opcode);
void op_8xxx(Chip8 *chip8, uint16_t opcode);
void op_9xy0(Chip8 *chip8, uint16_t opcode);
void op_Annn(Chip8 *chip8, uint16_t opcode);
void op_Bnnn(Chip8 *chip8, uint16_t opcode);
void op_Cxkk(Chip8 *chip8, uint16_t opcode);
void op_Dxyn(Chip8 *chip8, uint16_t opcode);
void op_Exxx(Chip8 *chip8, uint16_t opcode);
void op_Fxxx(Chip8 *chip8, uint16_t opcode);

// Subtable handlers (called by op_0xxx, op_8xxx, etc.)
void op_00E0(Chip8 *chip8, uint16_t opcode);
void op_00EE(Chip8 *chip8, uint16_t opcode);

void op_8xy0(Chip8 *chip8, uint16_t opcode);
void op_8xy1(Chip8 *chip8, uint16_t opcode);
void op_8xy2(Chip8 *chip8, uint16_t opcode);
void op_8xy3(Chip8 *chip8, uint16_t opcode);
void op_8xy4(Chip8 *chip8, uint16_t opcode);
void op_8xy5(Chip8 *chip8, uint16_t opcode);
void op_8xy6(Chip8 *chip8, uint16_t opcode);
void op_8xy7(Chip8 *chip8, uint16_t opcode);
void op_8xyE(Chip8 *chip8, uint16_t opcode);

void op_Ex9E(Chip8 *chip8, uint16_t opcode);
void op_ExA1(Chip8 *chip8, uint16_t opcode);

void op_Fx07(Chip8 *chip8, uint16_t opcode);
void op_Fx0A(Chip8 *chip8, uint16_t opcode);
void op_Fx15(Chip8 *chip8, uint16_t opcode);
void op_Fx18(Chip8 *chip8, uint16_t opcode);
void op_Fx1E(Chip8 *chip8, uint16_t opcode);
void op_Fx29(Chip8 *chip8, uint16_t opcode);
void op_Fx33(Chip8 *chip8, uint16_t opcode);
void op_Fx55(Chip8 *chip8, uint16_t opcode);
void op_Fx65(Chip8 *chip8, uint16_t opcode);

#endif
