#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>

#define MEMORY_SIZE 4096
#define REGISTER_COUNT 16
#define STACK_SIZE 16
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define KEYPAD_SIZE 16
#define FONTSET_SIZE 80

typedef struct {
    uint8_t memory[MEMORY_SIZE];
    uint8_t V[REGISTER_COUNT];     // V0 to VF
    uint16_t I;                    // Index register
    uint16_t pc;                   // Program counter

    uint8_t delay_timer;
    uint8_t sound_timer;

    uint16_t stack[STACK_SIZE];
    uint8_t sp;                    // Stack pointer

    uint8_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT];  // Monochrome display
    uint8_t keypad[KEYPAD_SIZE];   // Key states

    bool draw_flag;                // Set when the screen needs to be redrawn

    bool test_mode;
    char rom_path[128];  // For test dump tracing
} Chip8;

// Core functions
void chip8_init(Chip8 *chip8);
int chip8_load_rom(Chip8 *chip8, const char *filename);
void chip8_cycle(Chip8 *chip8);

#endif
