#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

// Utility function to load a ROM into the CHIP-8 memory
int load_rom(const char *filename, uint8_t *memory, uint16_t memory_size);

// Utility function to copy memory (for ROM loading, etc.)
void memory_copy(uint8_t *dest, const uint8_t *src, size_t size);

// Utility function to swap two bytes
uint8_t swap_bytes(uint8_t byte);

// Utility function to print CHIP-8 registers (for debugging)
void print_registers(const uint8_t *V, uint16_t I, uint16_t pc, uint8_t delay_timer, uint8_t sound_timer);

#endif
