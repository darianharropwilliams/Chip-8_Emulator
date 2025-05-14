#include "opcodes.h"
#include "display.h"
#include "input.h"
#include "timer.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Macros for extracting components of an opcode
#define OPCODE_NNN(op) ((op) & 0x0FFF)       // Lowest 12 bits
#define OPCODE_X(op)   (((op) >> 8) & 0x0F)  // 2nd nibble
#define OPCODE_Y(op)   (((op) >> 4) & 0x0F)  // 3rd nibble
#define OPCODE_N(op)   ((op) & 0x000F)       // Lowest nibble
#define OPCODE_KK(op)  ((op) & 0x00FF)       // Lowest byte

/**
 * 00E0 - CLS
 * Clear the display.
 */
void op_00E0(Chip8 *chip8, uint16_t opcode) {
    memset(chip8->display, 0, sizeof(chip8->display));
    chip8->draw_flag = true;
}

#include "utils.h" // Ensures access to test_halt

/**
 * 00EE - RET
 * Return from subroutine.
 * Sets the PC to the address at the top of the stack and decrements SP.
 */
void op_00EE(Chip8 *chip8, uint16_t opcode) {
    if (chip8->sp == 0) {
        if (chip8->test_mode) {
            test_halt(chip8, chip8->rom_path);  // Exit cleanly in test mode
        }
        DEBUG_PRINT(chip8, "Stack underflow on RET\n");
        return;
    }

    DEBUG_PRINT(chip8, "[DEBUG] RET with SP=%u, target=0x%04X\n", chip8->sp, chip8->stack[chip8->sp - 1]);
    chip8->sp--;
    chip8->pc = chip8->stack[chip8->sp];
}

/**
 * 1nnn - JP addr
 * Jump to address NNN.
 */
void op_1nnn(Chip8 *chip8, uint16_t opcode) {
    chip8->pc = OPCODE_NNN(opcode);
}

/**
 * 2nnn - CALL addr
 * Call subroutine at address NNN.
 */
void op_2nnn(Chip8 *chip8, uint16_t opcode) {
    if (chip8->sp >= STACK_SIZE) {
        DEBUG_PRINT(chip8, "Stack overflow on CALL\n");
        return;
    }
    uint16_t address = OPCODE_NNN(opcode);
    DEBUG_PRINT(chip8, "[DEBUG] CALL 0x%03X from 0x%04X (SP=%u)\n", address, chip8->pc, chip8->sp);
    chip8->stack[chip8->sp++] = chip8->pc;
    chip8->pc = address;
}

/**
 * 3xkk - SE Vx, kk
 * Skip the next instruction if Vx == kk.
 */
void op_3xkk(Chip8 *chip8, uint16_t opcode) {
    if (chip8->V[OPCODE_X(opcode)] == OPCODE_KK(opcode))
        chip8->pc += 2;
}

/**
 * 4xkk - SNE Vx, kk
 * Skip the next instruction if Vx != kk.
 */
void op_4xkk(Chip8 *chip8, uint16_t opcode) {
    if (chip8->V[OPCODE_X(opcode)] != OPCODE_KK(opcode))
        chip8->pc += 2;
}

/**
 * 5xy0 - SE Vx, Vy
 * Skip the next instruction if Vx == Vy.
 */
void op_5xy0(Chip8 *chip8, uint16_t opcode) {
    if (chip8->V[OPCODE_X(opcode)] == chip8->V[OPCODE_Y(opcode)])
        chip8->pc += 2;
}

/**
 * 6xkk - LD Vx, kk
 * Set Vx = kk.
 */
void op_6xkk(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = OPCODE_X(opcode);
    chip8->V[Vx] = OPCODE_KK(opcode);
    DEBUG_PRINT(chip8, "[DEBUG] LD V%X, 0x%02X\n", Vx, chip8->V[Vx]);
}

/**
 * 7xkk - ADD Vx, kk
 * Add kk to Vx (no carry).
 */
void op_7xkk(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = OPCODE_X(opcode);
    uint8_t result = chip8->V[Vx] + OPCODE_KK(opcode);
    chip8->V[Vx] = result;
    DEBUG_PRINT(chip8, "[DEBUG] ADD V%X += 0x%02X → 0x%02X\n", Vx, OPCODE_KK(opcode), result);
}

/**
 * 8xy0 - LD Vx, Vy
 * Set Vx = Vy.
 */
void op_8xy0(Chip8 *chip8, uint16_t opcode) {
    chip8->V[OPCODE_X(opcode)] = chip8->V[OPCODE_Y(opcode)];
}

/**
 * 8xy1 - OR Vx, Vy
 * Set Vx = Vx OR Vy.
 */
void op_8xy1(Chip8 *chip8, uint16_t opcode) {
    chip8->V[OPCODE_X(opcode)] |= chip8->V[OPCODE_Y(opcode)];
}

/**
 * 8xy2 - AND Vx, Vy
 * Set Vx = Vx AND Vy.
 */
void op_8xy2(Chip8 *chip8, uint16_t opcode) {
    chip8->V[OPCODE_X(opcode)] &= chip8->V[OPCODE_Y(opcode)];
}

/**
 * 8xy3 - XOR Vx, Vy
 * Set Vx = Vx XOR Vy.
 */
void op_8xy3(Chip8 *chip8, uint16_t opcode) {
    chip8->V[OPCODE_X(opcode)] ^= chip8->V[OPCODE_Y(opcode)];
}

/**
 * 8xy4 - ADD Vx, Vy
 * Add Vy to Vx. VF is set to 1 if there’s a carry, 0 otherwise.
 */
void op_8xy4(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = OPCODE_X(opcode);
    uint8_t Vy = OPCODE_Y(opcode);
    uint16_t sum = chip8->V[Vx] + chip8->V[Vy];
    chip8->V[0xF] = sum > 0xFF;
    chip8->V[Vx] = sum;
}

/**
 * 8xy5 - SUB Vx, Vy
 * Set Vx = Vx - Vy. VF is set to 0 when there is a borrow, 1 otherwise.
 */
void op_8xy5(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = OPCODE_X(opcode);
    uint8_t Vy = OPCODE_Y(opcode);
    chip8->V[0xF] = chip8->V[Vx] > chip8->V[Vy];
    chip8->V[Vx] -= chip8->V[Vy];
}

/**
 * 8xy6 - SHR Vx
 * Shift Vx right by 1. VF is set to the least significant bit prior to shift.
 */
void op_8xy6(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = OPCODE_X(opcode);
    chip8->V[0xF] = chip8->V[Vx] & 0x1;
    chip8->V[Vx] >>= 1;
}

/**
 * 8xy7 - SUBN Vx, Vy
 * Set Vx = Vy - Vx. VF is set to 0 when there is a borrow, 1 otherwise.
 */
void op_8xy7(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = OPCODE_X(opcode);
    uint8_t Vy = OPCODE_Y(opcode);
    chip8->V[0xF] = chip8->V[Vy] > chip8->V[Vx];
    chip8->V[Vx] = chip8->V[Vy] - chip8->V[Vx];
}

/**
 * 8xyE - SHL Vx
 * Shift Vx left by 1. VF is set to the most significant bit prior to shift.
 */
void op_8xyE(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = OPCODE_X(opcode);
    chip8->V[0xF] = (chip8->V[Vx] & 0x80) >> 7;
    chip8->V[Vx] <<= 1;
}

/**
 * 9xy0 - SNE Vx, Vy
 * Skip next instruction if Vx != Vy.
 */
void op_9xy0(Chip8 *chip8, uint16_t opcode) {
    if (chip8->V[OPCODE_X(opcode)] != chip8->V[OPCODE_Y(opcode)])
        chip8->pc += 2;
}

/**
 * Annn - LD I, addr
 * Set I = NNN.
 */
void op_Annn(Chip8 *chip8, uint16_t opcode) {
    chip8->I = OPCODE_NNN(opcode);
    DEBUG_PRINT(chip8, "[DEBUG] Set I = 0x%03X\n", chip8->I);
}

/**
 * Bnnn - JP V0, addr
 * Jump to location NNN + V0.
 */
void op_Bnnn(Chip8 *chip8, uint16_t opcode) {
    chip8->pc = OPCODE_NNN(opcode) + chip8->V[0];
}

/**
 * Cxkk - RND Vx, kk
 * Set Vx = random byte AND kk.
 */
void op_Cxkk(Chip8 *chip8, uint16_t opcode) {
    uint8_t rnd = rand() % 256;
    chip8->V[OPCODE_X(opcode)] = rnd & OPCODE_KK(opcode);
}

/**
 * Dxyn - DRW Vx, Vy, N
 * Draw N-byte sprite at (Vx, Vy). Set VF = collision.
 */
void op_Dxyn(Chip8 *chip8, uint16_t opcode) {
    uint8_t x = chip8->V[OPCODE_X(opcode)];
    uint8_t y = chip8->V[OPCODE_Y(opcode)];
    uint8_t n = OPCODE_N(opcode);
    const uint8_t *sprite = &chip8->memory[chip8->I];
    chip8->V[0xF] = draw_sprite(chip8, x, y, n, sprite);
    chip8->draw_flag = true;
}

/**
 * Ex9E - SKP Vx
 * Skip next instruction if key in Vx is pressed.
 */
void op_Ex9E(Chip8 *chip8, uint16_t opcode) {
    if (is_key_pressed(chip8, chip8->V[OPCODE_X(opcode)]))
        chip8->pc += 2;
}

/**
 * ExA1 - SKNP Vx
 * Skip next instruction if key in Vx is not pressed.
 */
void op_ExA1(Chip8 *chip8, uint16_t opcode) {
    if (!is_key_pressed(chip8, chip8->V[OPCODE_X(opcode)]))
        chip8->pc += 2;
}

/**
 * Fx07 - LD Vx, DT
 * Set Vx = delay timer.
 */
void op_Fx07(Chip8 *chip8, uint16_t opcode) {
    chip8->V[OPCODE_X(opcode)] = chip8->delay_timer;
}

/**
 * Fx0A - LD Vx, K
 * Wait for key press, store it in Vx.
 */
void op_Fx0A(Chip8 *chip8, uint16_t opcode) {
    for (uint8_t key = 0; key < KEYPAD_SIZE; ++key) {
        if (chip8->keypad[key]) {
            chip8->V[OPCODE_X(opcode)] = key;
            return;
        }
    }
    chip8->pc -= 2;  // Repeat instruction if no key pressed
}

/**
 * Fx15 - LD DT, Vx
 * Set delay timer = Vx.
 */
void op_Fx15(Chip8 *chip8, uint16_t opcode) {
    chip8->delay_timer = chip8->V[OPCODE_X(opcode)];
}

/**
 * Fx18 - LD ST, Vx
 * Set sound timer = Vx.
 */
void op_Fx18(Chip8 *chip8, uint16_t opcode) {
    chip8->sound_timer = chip8->V[OPCODE_X(opcode)];
}

/**
 * Fx1E - ADD I, Vx
 * Add Vx to I.
 */
void op_Fx1E(Chip8 *chip8, uint16_t opcode) {
    chip8->I += chip8->V[OPCODE_X(opcode)];
}

/**
 * Fx29 - LD F, Vx
 * Set I = location of sprite for digit in Vx.
 */
void op_Fx29(Chip8 *chip8, uint16_t opcode) {
    chip8->I = chip8->V[OPCODE_X(opcode)] * 5;
}

/**
 * Fx33 - LD B, Vx
 * Store BCD of Vx at I, I+1, I+2.
 */
void op_Fx33(Chip8 *chip8, uint16_t opcode) {
    uint8_t value = chip8->V[OPCODE_X(opcode)];
    chip8->memory[chip8->I]     = value / 100;
    chip8->memory[chip8->I + 1] = (value / 10) % 10;
    chip8->memory[chip8->I + 2] = value % 10;
}

/**
 * Fx55 - LD [I], Vx
 * Store registers V0 through Vx in memory starting at I.
 */
void op_Fx55(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = OPCODE_X(opcode);
    for (int i = 0; i <= Vx; i++)
        chip8->memory[chip8->I + i] = chip8->V[i];
}

/**
 * Fx65 - LD Vx, [I]
 * Load registers V0 through Vx from memory starting at I.
 */
void op_Fx65(Chip8 *chip8, uint16_t opcode) {
    uint8_t Vx = OPCODE_X(opcode);
    for (int i = 0; i <= Vx; i++)
        chip8->V[i] = chip8->memory[chip8->I + i];
}
