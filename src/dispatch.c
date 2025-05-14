#include "chip8.h"
#include <stdio.h>
#include <string.h>
#include "dispatch.h"
#include "opcodes.h"

// Duplicate typedef for opcode function pointer, ensures compatibility with IntelliSense or code tools
typedef void (*OpcodeHandler)(Chip8 *chip8, uint16_t opcode);

// -----------------------------------------------------------------------------
// Dispatch Tables: Used to route opcodes to their correct handler functions
// -----------------------------------------------------------------------------

// Main dispatch table: uses the highest nibble (4 bits) of the opcode
static OpcodeHandler main_table[0x10];

// Sub-dispatch tables used for opcodes that need deeper decoding
static OpcodeHandler table_0[0x100]; // For opcodes 0x00E0 and 0x00EE
static OpcodeHandler table_8[0x10];  // For opcodes 0x8xy* (bitwise and arithmetic)
static OpcodeHandler table_E[0x100]; // For opcodes 0xEx9E and 0xExA1
static OpcodeHandler table_F[0x100]; // For opcodes 0xFx** (timer, memory, etc.)

/**
 * Initialize all dispatch tables with the appropriate opcode handlers.
 * Sets up the routing logic for both standard and extended opcode groups.
 */
void opcode_dispatch_init(void) {
    // Clear all dispatch tables
    memset(main_table, 0, sizeof(main_table));
    memset(table_0, 0, sizeof(table_0));
    memset(table_8, 0, sizeof(table_8));
    memset(table_E, 0, sizeof(table_E));
    memset(table_F, 0, sizeof(table_F));

    // Primary 4-bit opcode dispatch (0x0 to 0xF)
    main_table[0x0] = op_0xxx;  // SYS instructions → further dispatched
    main_table[0x1] = op_1nnn;  // JP addr
    main_table[0x2] = op_2nnn;  // CALL addr
    main_table[0x3] = op_3xkk;  // SE Vx, byte
    main_table[0x4] = op_4xkk;  // SNE Vx, byte
    main_table[0x5] = op_5xy0;  // SE Vx, Vy
    main_table[0x6] = op_6xkk;  // LD Vx, byte
    main_table[0x7] = op_7xkk;  // ADD Vx, byte
    main_table[0x8] = op_8xxx;  // ALU ops → further dispatched
    main_table[0x9] = op_9xy0;  // SNE Vx, Vy
    main_table[0xA] = op_Annn;  // LD I, addr
    main_table[0xB] = op_Bnnn;  // JP V0, addr
    main_table[0xC] = op_Cxkk;  // RND Vx, byte
    main_table[0xD] = op_Dxyn;  // DRW Vx, Vy, nibble
    main_table[0xE] = op_Exxx;  // Key input → further dispatched
    main_table[0xF] = op_Fxxx;  // Misc → further dispatched

    // Subdispatch table for 0x0 group
    table_0[0xE0] = op_00E0;    // CLS (clear display)
    table_0[0xEE] = op_00EE;    // RET (return from subroutine)

    // Subdispatch table for 0x8 group
    table_8[0x0] = op_8xy0;     // LD Vx, Vy
    table_8[0x1] = op_8xy1;     // OR Vx, Vy
    table_8[0x2] = op_8xy2;     // AND Vx, Vy
    table_8[0x3] = op_8xy3;     // XOR Vx, Vy
    table_8[0x4] = op_8xy4;     // ADD Vx, Vy (with carry)
    table_8[0x5] = op_8xy5;     // SUB Vx, Vy
    table_8[0x6] = op_8xy6;     // SHR Vx
    table_8[0x7] = op_8xy7;     // SUBN Vx, Vy
    table_8[0xE] = op_8xyE;     // SHL Vx

    // Subdispatch table for 0xE group (key input)
    table_E[0x9E] = op_Ex9E;    // SKP Vx
    table_E[0xA1] = op_ExA1;    // SKNP Vx

    // Subdispatch table for 0xF group (timers, memory, etc.)
    table_F[0x07] = op_Fx07;   // LD Vx, DT
    table_F[0x0A] = op_Fx0A;   // LD Vx, K
    table_F[0x15] = op_Fx15;   // LD DT, Vx
    table_F[0x18] = op_Fx18;   // LD ST, Vx
    table_F[0x1E] = op_Fx1E;   // ADD I, Vx
    table_F[0x29] = op_Fx29;   // LD F, Vx
    table_F[0x33] = op_Fx33;   // LD B, Vx
    table_F[0x55] = op_Fx55;   // LD [I], Vx
    table_F[0x65] = op_Fx65;   // LD Vx, [I]
}

/**
 * Decode a CHIP-8 opcode and dispatch it to the correct handler.
 *
 * @param chip8 Pointer to CHIP-8 state
 * @param opcode 16-bit opcode fetched from memory
 * @return true if successfully dispatched; false if unknown
 */
bool dispatch_opcode(Chip8 *chip8, uint16_t opcode) {
    if (!chip8) {
        fprintf(stderr, "dispatch called on null Chip8 pointer\n");
        return false;
    }

    // Use top 4 bits of opcode as index into main dispatch table
    uint8_t prefix = (opcode >> 12) & 0xF;
    OpcodeHandler opcode_function = main_table[prefix];

    if (opcode_function != NULL) {
        opcode_function(chip8, opcode);  // Call the handler
        return true;
    } else {
        fprintf(stderr, "Unknown Opcode: 0x%04X\n", opcode);
        return false;
    }
}

// -----------------------------------------------------------------------------
// Subdispatch handlers — called from main table for multi-level opcode groups
// -----------------------------------------------------------------------------

/**
 * Handle 0x0*** opcodes (SYS addr / CLS / RET).
 * Forwards to table_0 using lowest 8 bits.
 */
void op_0xxx(Chip8 *chip8, uint16_t opcode) {
    OpcodeHandler handler = table_0[opcode & 0x00FF];
    if (handler)
        handler(chip8, opcode);
    else
        fprintf(stderr, "Unknown sub-opcode in 0x0 group: 0x%04X\n", opcode);
}

/**
 * Handle 0x8*** opcodes (bitwise and arithmetic).
 * Dispatches based on lowest nibble.
 */
void op_8xxx(Chip8 *chip8, uint16_t opcode) {
    OpcodeHandler handler = table_8[opcode & 0x000F];
    if (handler)
        handler(chip8, opcode);
    else
        fprintf(stderr, "Unknown sub-opcode in 0x8 group: 0x%04X\n", opcode);
}

/**
 * Handle 0xE*** opcodes (keyboard skip instructions).
 * Dispatches based on lowest 8 bits.
 */
void op_Exxx(Chip8 *chip8, uint16_t opcode) {
    OpcodeHandler handler = table_E[opcode & 0x00FF];
    if (handler)
        handler(chip8, opcode);
    else
        fprintf(stderr, "Unknown sub-opcode in 0xE group: 0x%04X\n", opcode);
}

/**
 * Handle 0xF*** opcodes (miscellaneous: timers, memory, input).
 * Dispatches based on lowest 8 bits.
 */
void op_Fxxx(Chip8 *chip8, uint16_t opcode) {
    OpcodeHandler handler = table_F[opcode & 0x00FF];
    if (handler)
        handler(chip8, opcode);
    else
        fprintf(stderr, "Unknown sub-opcode in 0xF group: 0x%04X\n", opcode);
}
