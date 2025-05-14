#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "chip8.h"

// Debug macros that print only if test mode is active
#define DEBUG_PRINT(chip8, fmt, ...) \
    do { if ((chip8)->test_mode) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)

#define DEBUG_PRINT_STDOUT(chip8, fmt, ...) \
    do { if ((chip8)->test_mode) printf(fmt, ##__VA_ARGS__); } while (0)

// Utility functions
int load_rom(const char *filename, uint8_t *memory, uint16_t memory_size);  // Load a ROM file into memory
void memory_copy(uint8_t *dest, const uint8_t *src, size_t size);           // Copy a block of memory
uint8_t swap_bytes(uint8_t byte);                                           // Swap high/low bits (bit reversal)
void print_registers(const uint8_t *V, uint16_t I, uint16_t pc, uint8_t delay_timer, uint8_t sound_timer); // Print state
void dump_memory(Chip8 *chip8, const char *rom_path);                       // Save full memory dump
void test_halt(Chip8 *chip8, const char *rom_path);                         // Halt execution during test

#endif
