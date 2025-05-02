// utils.c
#include "chip8.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>

// ----------------------------------------------------------
// Load a ROM from file into the given memory buffer.
// Returns 0 on success, -1 on failure.
// ----------------------------------------------------------
#include <stdio.h>
#include <stdint.h>

int load_rom(const char *filename, uint8_t *memory, uint16_t memory_size) {
    printf("[DEBUG] Attempting to load ROM: %s\n", filename);

    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("[ERROR] fopen");
        fprintf(stderr, "[ERROR] Failed to open ROM file: %s\n", filename);
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    printf("[DEBUG] ROM size = %ld bytes\n", file_size);

    if (file_size <= 0) {
        fprintf(stderr, "[ERROR] ROM is empty or unreadable.\n");
        fclose(file);
        return -1;
    }

    if (file_size > memory_size - 0x200) {
        fprintf(stderr, "[ERROR] ROM too large to fit in CHIP-8 memory. Max allowed: %d bytes\n",
                memory_size - 0x200);
        fclose(file);
        return -1;
    }

    size_t bytes_read = fread(memory, 1, file_size, file);
    fclose(file);

    if (bytes_read != file_size) {
        fprintf(stderr, "[ERROR] fread incomplete: expected %ld bytes, got %zu\n", file_size, bytes_read);
        return -1;
    }

    printf("[DEBUG] Successfully loaded %zu bytes into memory at 0x200\n", bytes_read);
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

void test_halt(Chip8 *chip8, const char *rom_path) {
    fprintf(stderr, "[TEST_MODE] End of test reached after RET — exiting emulator.\n");
    print_registers(chip8->V, chip8->I, chip8->pc, chip8->delay_timer, chip8->sound_timer);
    dump_memory(chip8, rom_path);
    exit(0);
}

#include <sys/stat.h> // For mkdir
#include <sys/types.h>

void dump_memory(Chip8 *chip8, const char *rom_path) {
    print_registers(chip8->V, chip8->I, chip8->pc, chip8->delay_timer, chip8->sound_timer);

    // Ensure dumps directory exists
#ifdef _WIN32
    _mkdir("tests\\python\\dumps");
#else
    mkdir("tests/python/dumps", 0755);
#endif

    const char *rom_name = strrchr(rom_path, '/');
    if (!rom_name) rom_name = strrchr(rom_path, '\\');
    rom_name = rom_name ? rom_name + 1 : rom_path;

    char name_buf[64] = {0};
    strncpy(name_buf, rom_name, sizeof(name_buf) - 1);
    char *dot = strrchr(name_buf, '.');
    if (dot) *dot = '\0';

    char path[128];
    snprintf(path, sizeof(path), "tests/python/dumps/%s.bin", name_buf);

    FILE *f = fopen(path, "wb");
    if (!f) {
        perror("Failed to open dump file");
        return;
    }

    fwrite(chip8, 1, sizeof(Chip8), f);
    fclose(f);
    printf("Memory dumped to %s\n", path);
}

