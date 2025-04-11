#include "chip8.h"
#include <stdio.h>
#include "dispatch.h"
#include "opcodes.h"

// Duplicate workaround because vscode is stupid
typedef void (*OpcodeHandler)(Chip8 *chip8, uint16_t opcode);

// ----------------------------------------------------------
// Dispatch tables (main and subtables)
// Each entry is a function pointer to an opcode handler.
// ----------------------------------------------------------

// Main opcode dispatch table: maps the high nibble of the opcode
static OpcodeHandler main_table[0x10];

// Subtable for 0x0xxx opcodes: dispatches based on the low byte.
static OpcodeHandler table_0[0x100]; // key = opcode & 0x00FF

// Subtable for 0x8xxx opcodes: dispatches based on the lowest nibble.
static OpcodeHandler table_8[0x10]; // key = opcode & 0x000F

// Subtable for 0xExxx opcodes: dispatches based on the low byte.
static OpcodeHandler table_E[0x100]; // key = opcode & 0x00FF

// Subtable for 0xFxxx opcodes: dispatches based on the low byte.
static OpcodeHandler table_F[0x100]; // key = opcode & 0x00FF

// Initializes the dispatch tables (main and subtables).
// This function assigns opcode handler functions to each
// relevant entry in the lookup tables.
void opcode_dispatch_init(void) {
    
    memset(main_table, 0, sizeof(main_table));
    memset(table_0, 0, sizeof(table_0));
    memset(table_8, 0, sizeof(table_8));
    memset(table_E, 0, sizeof(table_E));
    memset(table_F, 0, sizeof(table_F));



    main_table[0x0] = op_0xxx; // SYS, or CLS, RET in table_0
    main_table[0x1] = op_1nnn; // JP addr
    main_table[0x2] = op_2nnn; // CALL addr
    main_table[0x3] = op_3xkk; // SE Vx, byte
    main_table[0x4] = op_4xkk; // SNE Vx, byte
    main_table[0x5] = op_5xy0; // SE Vx, Vy
    main_table[0x6] = op_6xkk; // LD Vx, byte
    main_table[0x7] = op_7xkk; // ADD Vx, byte
    main_table[0x8] = op_8xxx; // 9 8xxx functions in table_8
    main_table[0x9] = op_9xy0; // SNE Vx, Vy
    main_table[0xA] = op_Annn; // LD I, addr
    main_table[0xB] = op_Bnnn; // JP V0, addr
    main_table[0xC] = op_Cxkk; // RND Vx, byte
    main_table[0xD] = op_Dxyn; // DRW Vx, Vy, nibble
    main_table[0xE] = op_Exxx; // SKP, SKNP in table_E
    main_table[0xF] = op_Fxxx; // 9 Fxxx functions in table_F

    table_0[0xE0] = op_00E0; // CLS
    table_0[0xEE] = op_00EE; // RET

    table_8[0x0] = op_8xy0; // LD Vx, Vy
    table_8[0x1] = op_8xy1; // OR Vx, Vy
    table_8[0x2] = op_8xy2; // AND Vx, Vy
    table_8[0x3] = op_8xy3; // XOR Vx, Vy
    table_8[0x4] = op_8xy4; // ADD Vx, Vy
    table_8[0x5] = op_8xy5; // Sub Vx, Vy
    table_8[0x6] = op_8xy6; // SHR Vx {, Vy}
    table_8[0x7] = op_8xy7; // SubN Vx, Vy
    table_8[0xE] = op_8xyE; // SHL Vx {, Vy}


    table_E[0x9E] = op_Ex9E; // SKP Vx
    table_E[0xA1] = op_ExA1; // SKNP Vx

    table_F[0x07] = op_Fx07; // LD Vx, DT
    table_F[0x0A] = op_Fx0A; // LD Vx, K
    table_F[0x15] = op_Fx15; // LD DT, Vx
    table_F[0x18] = op_Fx18; // LD ST, Vx
    table_F[0x1E] = op_Fx1E; // ADD I, Vx
    table_F[0x29] = op_Fx29; // LD F, Vx
    table_F[0x33] = op_Fx33; // LD B, Vx
    table_F[0x55] = op_Fx55; // LD [I], Vx
    table_F[0x65] = op_Fx65; // LD Vx, [I]
}

// ----------------------------------------------------------
// Decodes a 16-bit CHIP-8 opcode and dispatches it to the correct
// handler using the lookup tables.
//
// Behavior:
// - Extracts the high nibble (opcode >> 12) to index main_table[]
// - If the handler for that nibble is a sub-dispatch (e.g., op_0xxx),
//   that handler should further dispatch based on finer opcode bits.
// 
// Responsibility of this function:
// - Validate that the main dispatch entry exists
// - Call the handler function with (chip8, opcode)
// - Print/log unhandled opcodes for debugging
// ----------------------------------------------------------
void dispatch_opcode(Chip8 *chip8, uint16_t opcode) {

    uint8_t prefix = (opcode >> 12) & 0xF;
    OpcodeHandler opcode_function = main_table[prefix];

    if (opcode_function != NULL)
        opcode_function(chip8, opcode);
    
    else
        fprintf(stderr, "Unknown Opcode: 0x%04X\n", opcode);
}


// === 0xxx: SYS / CLS / RET ===
/**
 * 0nnn - SYS addr
 * Jump to a machine code routine at nnn.
 * 
 * This instruction is only used on the old computers on which Chip-8 was
 * originally implemented. It is ignored by modern interpreters.
 */
static void op_0xxx(Chip8 *chip8, uint16_t opcode) {
    OpcodeHandler handler = table_0[opcode & 0x00FF];
    if (handler) handler(chip8, opcode);
    else /* SYS nnn - ignored */ ;
}

// === 8xxx Group (bitwise, arithmetic, etc.) ===
static void op_8xxx(Chip8 *chip8, uint16_t opcode) {
    OpcodeHandler handler = table_8[opcode & 0x000F];
    if (handler) handler(chip8, opcode);
}

// === Exxx: Keypad skip instructions ===
static void op_Exxx(Chip8 *chip8, uint16_t opcode) {
    OpcodeHandler handler = table_E[opcode & 0x00FF];
    if (handler) handler(chip8, opcode);
}

// === Fxxx: Timer, memory, input instructions ===
static void op_Fxxx(Chip8 *chip8, uint16_t opcode) {
    OpcodeHandler handler = table_F[opcode & 0x00FF];
    if (handler) handler(chip8, opcode);
}