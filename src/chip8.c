// chip8.c
#include "chip8.h"
#include "dispatch.h"
#include "input.h"
#include "display.h"
#include "timer.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

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

// ----------------------------------------------------------
// Initializes the CHIP-8 system state before execution begins.
// This includes clearing memory, resetting registers,
// initializing display, input, timers, and dispatch tables.
// ----------------------------------------------------------
void chip8_init(Chip8 *chip8) {

    // Zero out the entire struct
    memset(chip8, 0, sizeof(Chip8));

    // Set program counter to 0x200 (start of most CHIP-8 programs)
    chip8->pc = 0x200;

    // Initialize display, input, timers
    display_init(chip8);
    timer_init(chip8);
    keypad_init(chip8);

    // Load fontset into memory (if done here)
    memcpy(chip8->memory, fontset, FONTSET_SIZE);

    // Initialize opcode dispatch tables
    opcode_dispatch_init();

}

// ----------------------------------------------------------
// Loads a ROM into the CHIP-8 memory starting at 0x200
// Uses the utility function `load_rom`.
// ----------------------------------------------------------
int chip8_load_rom(Chip8 *chip8, const char *filename) {

    // Call load_rom and pass chip8->memory and MEMORY_SIZE
    return load_rom(filename, chip8->memory + 0x200, MEMORY_SIZE);

}

// ----------------------------------------------------------
// Executes one emulation cycle:
// - Fetches the next opcode from memory
// - Increments PC
// - Decodes and dispatches the opcode
// - Updates timers
// - Scans input
// - Updates display if needed
// ----------------------------------------------------------
void chip8_cycle(Chip8 *chip8) {
    
    if (!chip8) {
        DEBUG_PRINT(chip8, "chip8_cycle called on null Chip8 pointer\n");
        return;
    }

    if (chip8->pc >= MEMORY_SIZE - 1) {
        DEBUG_PRINT(chip8, "PC out of bounds: 0x%04X\n", chip8->pc);
        return;
    }
    
    // Fetch opcode: combine two bytes from memory at PC and PC+1
    uint16_t opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];

    // Print debug before executing
    DEBUG_PRINT_STDOUT(chip8,"[DEBUG] PC=0x%04X  Executing: 0x%04X\n", chip8->pc, opcode);

    // Increment PC by 2
    chip8->pc += 2;
    
    // Dispatch the opcode using the dispatch_opcode function
    if (!dispatch_opcode(chip8, opcode)) {
        DEBUG_PRINT(chip8, "Registers after unknown opcode:\n");
        // print_registers(chip8->V, chip8->I, chip8->pc, chip8->delay_timer, chip8->sound_timer);
    }
    // if (chip8->test_mode) {
    //     print_registers(chip8->V, chip8->I, chip8->pc, chip8->delay_timer, chip8->sound_timer);
    // }    
    // Update delay and sound timers (60Hz)
    timer_update(chip8);    
    
    // Scan for key inputs (update chip8->keypad)
    keypad_scan(chip8);
    
    // If draw_flag is set, call update_display and clear draw_flag
    if (chip8->draw_flag) {
        update_display(chip8);
        chip8->draw_flag = false;
    }
}
