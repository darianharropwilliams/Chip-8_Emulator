// chip8.c
#include "chip8.h"
#include "dispatch.h"
#include "input.h"
#include "display.h"
#include "timer.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

// ----------------------------------------------------------
// Built-in CHIP-8 fontset for hexadecimal digits (0–F).
// Each character is 5 bytes tall, stored as bitmapped rows.
// These are typically loaded into memory starting at address 0x000.
// ----------------------------------------------------------
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
 * Initialize the CHIP-8 virtual machine.
 * Clears memory and registers, sets PC to 0x200,
 * loads fontset, and initializes subsystems.
 */
void chip8_init(Chip8 *chip8) {
    // Zero the entire CHIP-8 struct, including memory, registers, etc.
    memset(chip8, 0, sizeof(Chip8));

    // CHIP-8 programs start at memory location 0x200
    chip8->pc = 0x200;

    // Initialize subsystems
    display_init(chip8);
    timer_init(chip8);
    keypad_init(chip8);

    // Load fontset into the first 80 bytes of memory
    memcpy(chip8->memory, fontset, FONTSET_SIZE);

    // Initialize opcode dispatch tables
    opcode_dispatch_init();
}

/**
 * Load a ROM from file into CHIP-8 memory starting at 0x200.
 * Wrapper around the utility function `load_rom`.
 *
 * @param chip8 Pointer to CHIP-8 state
 * @param filename Path to ROM file
 * @return 0 on success, -1 on failure
 */
int chip8_load_rom(Chip8 *chip8, const char *filename) {
    // Load ROM into memory starting at 0x200
    return load_rom(filename, chip8->memory + 0x200, MEMORY_SIZE);
}

/**
 * Execute one emulation cycle.
 * This function:
 * - Fetches the next instruction (2 bytes)
 * - Increments the program counter
 * - Decodes and executes the instruction
 * - Updates timers
 * - Scans input
 * - Refreshes display if needed
 */
void chip8_cycle(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "chip8_cycle called on null Chip8 pointer\n");
        return;
    }

    // Make sure the program counter is within valid memory bounds
    if (chip8->pc >= MEMORY_SIZE - 1) {
        DEBUG_PRINT(chip8, "PC out of bounds: 0x%04X\n", chip8->pc);
        return;
    }

    // Fetch opcode: two bytes from memory, big-endian
    uint16_t opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];

    DEBUG_PRINT_STDOUT(chip8, "[DEBUG] PC=0x%04X  Executing: 0x%04X\n", chip8->pc, opcode);

    // Increment PC before execution (each instruction is 2 bytes)
    chip8->pc += 2;

    // Decode and execute the opcode
    if (!dispatch_opcode(chip8, opcode)) {
        DEBUG_PRINT(chip8, "Registers after unknown opcode:\n");
        // Optionally print state or halt
        // print_registers(chip8->V, chip8->I, chip8->pc, chip8->delay_timer, chip8->sound_timer);
    }

    // Update timers (run at 60Hz)
    timer_update(chip8);

    // Poll for key inputs
    keypad_scan(chip8);

    // If a draw was requested by the last instruction, refresh the display
    if (chip8->draw_flag) {
        update_display(chip8);
        chip8->draw_flag = false;  // Reset flag after drawing
    }
}
