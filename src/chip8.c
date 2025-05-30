/**
 * chip8.c
 *
 * Core logic for initializing and running the CHIP-8 virtual machine.
 * This module handles:
 * - Memory and register setup
 * - Fontset loading
 * - Subsystem initialization (display, timers, input)
 * - ROM loading
 * - Fetch-decode-execute cycle
 */

#include "chip8.h"
#include "dispatch.h"
#include "input.h"
#include "display.h"
#include "timer.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

/**
 * Built-in CHIP-8 fontset (0–F).
 * Each character is 5 bytes tall, representing 4x5 pixel sprites.
 * Stored in memory starting at address 0x000.
 */
static const uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

/**
 * Initializes a CHIP-8 instance.
 * Clears memory, resets registers and timers, and prepares subsystems.
 *
 * @param chip8 Pointer to the Chip8 structure to initialize.
 */
void chip8_init(Chip8 *chip8) {
    // Zero the entire structure (including memory, registers, etc.)
    memset(chip8, 0, sizeof(Chip8));

    // CHIP-8 programs start at memory address 0x200
    chip8->pc = 0x200;

    // Initialize display, timers, and keypad subsystems
    display_init(chip8);
    timer_init(chip8);
    keypad_init(chip8);

    // Load the fontset into the beginning of memory (0x000–0x04F)
    memcpy(chip8->memory, fontset, FONTSET_SIZE);

    // Set up opcode dispatch table for instruction decoding
    opcode_dispatch_init();
}

/**
 * Loads a ROM binary into the emulator's memory at address 0x200.
 * This wraps the generic `load_rom` utility for CHIP-8 semantics.
 *
 * @param chip8    Pointer to the emulator state.
 * @param filename Path to the ROM file to load.
 * @return         0 on success, -1 on failure.
 */
int chip8_load_rom(Chip8 *chip8, const char *filename) {
    return load_rom(filename, chip8->memory + 0x200, MEMORY_SIZE);
}

/**
 * Executes a single emulation cycle of the CHIP-8 virtual machine.
 *
 * This cycle performs the following steps:
 * - Fetch: Reads the next 2-byte instruction from memory.
 * - Decode + Execute: Uses the dispatch table to invoke the opcode handler.
 * - Update: Advances timers and polls input.
 *
 * Increments the program counter before execution.
 * Updates to the display are flagged via `chip8->draw_flag` and handled externally.
 *
 * @param chip8 Pointer to the emulator state.
 */
void chip8_cycle(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "chip8_cycle called on null Chip8 pointer\n");
        return;
    }

    if (chip8->pc >= MEMORY_SIZE - 1) {
        DEBUG_PRINT(chip8, "PC out of bounds: 0x%04X\n", chip8->pc);
        return;
    }

    // Fetch 16-bit instruction (big-endian)
    uint16_t opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];

    DEBUG_PRINT_STDOUT(chip8, "[DEBUG] PC=0x%04X  Executing: 0x%04X\n", chip8->pc, opcode);

    // Advance PC before executing (some handlers may override it)
    chip8->pc += 2;

    // Decode and execute instruction
    if (!dispatch_opcode(chip8, opcode)) {
        DEBUG_PRINT(chip8, "Registers after unknown opcode:\n");
    }

    // Timers and input are updated after each instruction
    timer_update(chip8);
    keypad_scan(chip8);

    // draw_flag is checked and acted on externally in the main loop
}
