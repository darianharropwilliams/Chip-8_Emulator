/**
 * dispatch.c
 *
 * CHIP-8 opcode dispatch system.
 *
 * This module routes 16-bit CHIP-8 opcodes to the correct handler functions
 * using multi-level dispatch tables. It supports direct decoding of opcodes
 * and layered subdispatching for complex instruction groups (e.g., 0x8, 0xF).
 */

#include "chip8.h"
#include "dispatch.h"
#include "opcodes.h"
#include <stdio.h>
#include <string.h>

/**
 * Typedef for opcode handler functions.
 * All opcode handlers take a Chip8 pointer and the 16-bit opcode.
 */
typedef void (*OpcodeHandler)(Chip8 *chip8, uint16_t opcode);

/* ------------------------------------------------------------
 * Primary Dispatch Tables
 * ------------------------------------------------------------
 * main_table — routes based on top nibble (0x0 to 0xF)
 * table_0    — routes 0x00** opcodes (e.g., CLS, RET)
 * table_8    — routes 0x8xy* ALU instructions
 * table_E    — routes 0xEx** key input ops
 * table_F    — routes 0xFx** timers, memory, and I/O
 */

static OpcodeHandler main_table[0x10];
static OpcodeHandler table_0[0x100];
static OpcodeHandler table_8[0x10];
static OpcodeHandler table_E[0x100];
static OpcodeHandler table_F[0x100];

/**
 * Initializes the opcode dispatch tables.
 * Maps each opcode group to its corresponding handler.
 */
void opcode_dispatch_init(void) {
    // Clear all dispatch tables
    memset(main_table, 0, sizeof(main_table));
    memset(table_0, 0, sizeof(table_0));
    memset(table_8, 0, sizeof(table_8));
    memset(table_E, 0, sizeof(table_E));
    memset(table_F, 0, sizeof(table_F));

    // Main table — top nibble (0x0 to 0xF)
    main_table[0x0] = op_0xxx;
    main_table[0x1] = op_1nnn;
    main_table[0x2] = op_2nnn;
    main_table[0x3] = op_3xkk;
    main_table[0x4] = op_4xkk;
    main_table[0x5] = op_5xy0;
    main_table[0x6] = op_6xkk;
    main_table[0x7] = op_7xkk;
    main_table[0x8] = op_8xxx;
    main_table[0x9] = op_9xy0;
    main_table[0xA] = op_Annn;
    main_table[0xB] = op_Bnnn;
    main_table[0xC] = op_Cxkk;
    main_table[0xD] = op_Dxyn;
    main_table[0xE] = op_Exxx;
    main_table[0xF] = op_Fxxx;

    // Subtable: 0x0***
    table_0[0xE0] = op_00E0; // CLS
    table_0[0xEE] = op_00EE; // RET

    // Subtable: 0x8***
    table_8[0x0] = op_8xy0;
    table_8[0x1] = op_8xy1;
    table_8[0x2] = op_8xy2;
    table_8[0x3] = op_8xy3;
    table_8[0x4] = op_8xy4;
    table_8[0x5] = op_8xy5;
    table_8[0x6] = op_8xy6;
    table_8[0x7] = op_8xy7;
    table_8[0xE] = op_8xyE;

    // Subtable: 0xE***
    table_E[0x9E] = op_Ex9E; // SKP Vx
    table_E[0xA1] = op_ExA1; // SKNP Vx

    // Subtable: 0xF***
    table_F[0x07] = op_Fx07;
    table_F[0x0A] = op_Fx0A;
    table_F[0x15] = op_Fx15;
    table_F[0x18] = op_Fx18;
    table_F[0x1E] = op_Fx1E;
    table_F[0x29] = op_Fx29;
    table_F[0x33] = op_Fx33;
    table_F[0x55] = op_Fx55;
    table_F[0x65] = op_Fx65;
}

/**
 * Dispatches a single CHIP-8 opcode to its corresponding handler function.
 *
 * @param chip8  Pointer to the CHIP-8 emulator state.
 * @param opcode The 16-bit opcode to execute.
 * @return       true if the opcode was dispatched; false if unknown.
 */
bool dispatch_opcode(Chip8 *chip8, uint16_t opcode) {
    if (!chip8) {
        fprintf(stderr, "dispatch called on null Chip8 pointer\n");
        return false;
    }

    // Use top nibble to find main group handler
    uint8_t prefix = (opcode >> 12) & 0xF;
    OpcodeHandler opcode_function = main_table[prefix];

    if (opcode_function) {
        opcode_function(chip8, opcode);
        return true;
    } else {
        fprintf(stderr, "Unknown Opcode: 0x%04X\n", opcode);
        return false;
    }
}

/* ------------------------------------------------------------
 * Subdispatchers
 * ------------------------------------------------------------
 * These are called from the main table and delegate to subgroups
 * based on lower bits of the opcode.
 */

/**
 * Handles 0x0*** opcodes (system ops like CLS, RET).
 * Uses the lowest 8 bits to route within table_0.
 *
 * @param chip8  Pointer to CHIP-8 state.
 * @param opcode Full 16-bit opcode.
 */
void op_0xxx(Chip8 *chip8, uint16_t opcode) {
    OpcodeHandler handler = table_0[opcode & 0x00FF];
    if (handler) {
        handler(chip8, opcode);
    } else {
        fprintf(stderr, "Unknown sub-opcode in 0x0 group: 0x%04X\n", opcode);
    }
}

/**
 * Handles 0x8*** opcodes (arithmetic and bitwise).
 * Uses lowest nibble to dispatch within table_8.
 *
 * @param chip8  Pointer to CHIP-8 state.
 * @param opcode Full 16-bit opcode.
 */
void op_8xxx(Chip8 *chip8, uint16_t opcode) {
    OpcodeHandler handler = table_8[opcode & 0x000F];
    if (handler) {
        handler(chip8, opcode);
    } else {
        fprintf(stderr, "Unknown sub-opcode in 0x8 group: 0x%04X\n", opcode);
    }
}

/**
 * Handles 0xE*** opcodes (key press skips).
 * Uses lowest byte for dispatching within table_E.
 *
 * @param chip8  Pointer to CHIP-8 state.
 * @param opcode Full 16-bit opcode.
 */
void op_Exxx(Chip8 *chip8, uint16_t opcode) {
    OpcodeHandler handler = table_E[opcode & 0x00FF];
    if (handler) {
        handler(chip8, opcode);
    } else {
        fprintf(stderr, "Unknown sub-opcode in 0xE group: 0x%04X\n", opcode);
    }
}

/**
 * Handles 0xF*** opcodes (miscellaneous instructions).
 * Uses lowest byte for dispatching within table_F.
 *
 * @param chip8  Pointer to CHIP-8 state.
 * @param opcode Full 16-bit opcode.
 */
void op_Fxxx(Chip8 *chip8, uint16_t opcode) {
    OpcodeHandler handler = table_F[opcode & 0x00FF];
    if (handler) {
        handler(chip8, opcode);
    } else {
        fprintf(stderr, "Unknown sub-opcode in 0xF group: 0x%04X\n", opcode);
    }
}
