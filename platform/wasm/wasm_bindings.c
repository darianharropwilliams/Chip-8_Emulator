#include <emscripten.h>
#include "chip8.h"
#include "platform.h"
#include "utils.h"

// Global CHIP-8 instance used by the browser
static Chip8 chip8;

/**
 * Exposed to JavaScript: Initialize the CHIP-8 system.
 * 
 * Clears memory, resets state, and prepares display, timers, and input.
 * Equivalent to calling `chip8_init` during native execution.
 */
EMSCRIPTEN_KEEPALIVE
void wasm_init() {
    chip8_init(&chip8);
}

/**
 * Exposed to JavaScript: Execute N cycles of the emulator.
 *
 * @param cycles Number of emulation cycles to execute
 *
 * This lets the browser control pacing (e.g., run 10–20 cycles per animation frame).
 */
EMSCRIPTEN_KEEPALIVE
void wasm_cycle(int cycles) {
    for (int i = 0; i < cycles; i++) {
        chip8_cycle(&chip8);
    }
}

/**
 * Exposed to JavaScript: Load a ROM into the CHIP-8 memory.
 *
 * @param data Pointer to a buffer containing ROM bytes
 * @param size Number of bytes to copy (max is MEMORY_SIZE - 0x200)
 * @return 0 on success, -1 on error (e.g., too large)
 *
 * Automatically resets state before loading.
 */
EMSCRIPTEN_KEEPALIVE
int wasm_load_rom(uint8_t *data, int size) {
    if (size > (MEMORY_SIZE - 0x200)) return -1;

    chip8_init(&chip8);  // Reset emulator state

    memory_copy(&chip8.memory[0x200], data, size);  // Load ROM into memory
    chip8.pc = 0x200;  // Reset program counter

    platform_update_display(chip8.display);  // Optional: trigger screen redraw

    return 0;
}
