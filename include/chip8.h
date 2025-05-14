#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>

// CHIP-8 system configuration constants
#define MEMORY_SIZE 4096           // Total RAM size (4KB)
#define REGISTER_COUNT 16          // General-purpose registers (V0 to VF)
#define STACK_SIZE 16              // Maximum call stack depth
#define DISPLAY_WIDTH 64           // Display width in pixels
#define DISPLAY_HEIGHT 32          // Display height in pixels
#define KEYPAD_SIZE 16             // 16-key hexadecimal keypad
#define FONTSET_SIZE 80            // Size of the built-in fontset

// Core CHIP-8 system state
typedef struct {
    uint8_t memory[MEMORY_SIZE];      // RAM
    uint8_t V[REGISTER_COUNT];        // Registers V0 through VF
    uint16_t I;                       // Index register (typically used for memory addresses)
    uint16_t pc;                      // Program counter

    uint8_t delay_timer;             // Delay timer (ticks at 60Hz)
    uint8_t sound_timer;             // Sound timer (ticks at 60Hz, beeps when non-zero)

    uint16_t stack[STACK_SIZE];      // Stack for subroutine calls
    uint8_t sp;                      // Stack pointer

    uint8_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT];  // Monochrome framebuffer (1 byte per pixel)
    uint8_t keypad[KEYPAD_SIZE];     // Key states: 1 = pressed, 0 = not pressed

    bool draw_flag;                  // True if the screen needs to be redrawn

    bool test_mode;                  // Enables debugging and test features
    char rom_path[128];             // Path to the loaded ROM (for test logging)
} Chip8;

// Core functions

void chip8_init(Chip8 *chip8);                       // Initialize a new CHIP-8 instance
int chip8_load_rom(Chip8 *chip8, const char *filename); // Load a ROM into memory
void chip8_cycle(Chip8 *chip8);                      // Execute one emulation cycle

#endif
