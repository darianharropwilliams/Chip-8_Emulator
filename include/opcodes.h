#ifndef OPCODES_H
#define OPCODES_H

#include "chip8.h"

// Top-level opcode handlers, mapped in the main dispatch table [0x0 - 0xF]
void op_1nnn(Chip8 *chip8, uint16_t opcode); // Jump to address NNN
void op_2nnn(Chip8 *chip8, uint16_t opcode); // Call subroutine at NNN
void op_3xkk(Chip8 *chip8, uint16_t opcode); // Skip next instruction if Vx == KK
void op_4xkk(Chip8 *chip8, uint16_t opcode); // Skip next instruction if Vx != KK
void op_5xy0(Chip8 *chip8, uint16_t opcode); // Skip next instruction if Vx == Vy
void op_6xkk(Chip8 *chip8, uint16_t opcode); // Set Vx = KK
void op_7xkk(Chip8 *chip8, uint16_t opcode); // Add KK to Vx (no carry)
void op_9xy0(Chip8 *chip8, uint16_t opcode); // Skip next instruction if Vx != Vy
void op_Annn(Chip8 *chip8, uint16_t opcode); // Set I = NNN
void op_Bnnn(Chip8 *chip8, uint16_t opcode); // Jump to address NNN + V0
void op_Cxkk(Chip8 *chip8, uint16_t opcode); // Set Vx = random byte AND KK
void op_Dxyn(Chip8 *chip8, uint16_t opcode); // Draw sprite at (Vx, Vy)

// Opcode subtable handlers (based on lower bits of certain opcodes)
void op_00E0(Chip8 *chip8, uint16_t opcode); // Clear the display
void op_00EE(Chip8 *chip8, uint16_t opcode); // Return from subroutine

void op_8xy0(Chip8 *chip8, uint16_t opcode); // Set Vx = Vy
void op_8xy1(Chip8 *chip8, uint16_t opcode); // Set Vx = Vx OR Vy
void op_8xy2(Chip8 *chip8, uint16_t opcode); // Set Vx = Vx AND Vy
void op_8xy3(Chip8 *chip8, uint16_t opcode); // Set Vx = Vx XOR Vy
void op_8xy4(Chip8 *chip8, uint16_t opcode); // Add Vy to Vx with carry
void op_8xy5(Chip8 *chip8, uint16_t opcode); // Subtract Vy from Vx with borrow
void op_8xy6(Chip8 *chip8, uint16_t opcode); // Shift Vx right by 1 (Vy may be used on some interpreters)
void op_8xy7(Chip8 *chip8, uint16_t opcode); // Set Vx = Vy - Vx with borrow
void op_8xyE(Chip8 *chip8, uint16_t opcode); // Shift Vx left by 1 (Vy may be used on some interpreters)

void op_Ex9E(Chip8 *chip8, uint16_t opcode); // Skip if key in Vx is pressed
void op_ExA1(Chip8 *chip8, uint16_t opcode); // Skip if key in Vx is not pressed

void op_Fx07(Chip8 *chip8, uint16_t opcode); // Set Vx = delay timer
void op_Fx0A(Chip8 *chip8, uint16_t opcode); // Wait for key press, store in Vx
void op_Fx15(Chip8 *chip8, uint16_t opcode); // Set delay timer = Vx
void op_Fx18(Chip8 *chip8, uint16_t opcode); // Set sound timer = Vx
void op_Fx1E(Chip8 *chip8, uint16_t opcode); // Add Vx to I
void op_Fx29(Chip8 *chip8, uint16_t opcode); // Set I to sprite location for digit Vx
void op_Fx33(Chip8 *chip8, uint16_t opcode); // Store BCD of Vx at I, I+1, I+2
void op_Fx55(Chip8 *chip8, uint16_t opcode); // Store registers V0 through Vx in memory starting at I
void op_Fx65(Chip8 *chip8, uint16_t opcode); // Load registers V0 through Vx from memory starting at I

#endif
