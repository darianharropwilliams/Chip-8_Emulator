// utils.c
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ----------------------------------------------------------
// Load a ROM from file into the given memory buffer.
// Returns 0 on success, -1 on failure.
// ----------------------------------------------------------
int load_rom(const char *filename, uint8_t *memory, uint16_t memory_size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open ROM file: %s\n", filename);
        return -1;
    }
    // Seek to end to get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Check that file fits in memory starting at 0x200
    if (file_size > memory_size - 0x200) {
        fprintf(stderr, "ROM too large to fit in memory.\n");
        fclose(file);
        return -1;
    }

    // Read ROM into memory at 0x200
    size_t bytes_read = fread(memory, 1, file_size, file);
    if (bytes_read != file_size) {
        fprintf(stderr, "Failed to read complete ROM.\n");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

// ----------------------------------------------------------
// Copy memory from src to dest
// ----------------------------------------------------------
void memory_copy(uint8_t *dest, const uint8_t *src, size_t size) {
    memcpy(dest, src, size);
}

// ----------------------------------------------------------
// Swap bits in a byte (e.g., reverse 0b10110000 -> 0b00001101)
// Can be used for mirrored sprite drawing if needed.
// ----------------------------------------------------------
uint8_t swap_bytes(uint8_t byte) {
    byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
    byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
    byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;
    return byte;
}

// ----------------------------------------------------------
// Debug function to print internal state of CHIP-8
// ----------------------------------------------------------
#ifdef DEBUG
void print_registers(const uint8_t *V, uint16_t I, uint16_t pc, uint8_t delay_timer, uint8_t sound_timer) {
    printf("PC: 0x%04X  I: 0x%04X  DT: %d  ST: %d\n", pc, I, delay_timer, sound_timer);
    for (int i = 0; i < 16; i++) {
        printf("V[%X]: %02X  ", i, V[i]);
        if ((i + 1) % 4 == 0) printf("\n");
    }
    printf("\n");
}
#endif