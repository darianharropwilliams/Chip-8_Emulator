#include "chip8.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

bool test_chip8_cycle() {
    printf("[TEST] chip8_cycle basic functionality...\n");

    Chip8 chip8;
    chip8_init(&chip8);

    // --- Setup ---
    // Insert simple opcode into memory manually: 6XKK = LD V0, 0x42
    chip8.memory[0x200] = 0x60; // 6Xkk: Load into V0
    chip8.memory[0x201] = 0x42; // value = 0x42

    uint16_t old_pc = chip8.pc;

    // --- Act ---
    chip8_cycle(&chip8);

    // --- Assert ---
    if (chip8.V[0] != 0x42) {
        printf("[FAIL] Expected V0 = 0x42, got 0x%02X\n", chip8.V[0]);
        return false;
    }
    if (chip8.pc != old_pc + 2) {
        printf("[FAIL] Expected PC to advance by 2. Got %04X -> %04X\n", old_pc, chip8.pc);
        return false;
    }

    printf("[PASS] chip8_cycle executed LD V0, 0x42 correctly.\n");
    return true;
}
