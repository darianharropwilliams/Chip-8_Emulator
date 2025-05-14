#include "chip8.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>

/**
 * Load a ROM file into memory.
 *
 * @param filename     Path to the ROM file
 * @param memory       Pointer to CHIP-8 memory (starting at 0x200)
 * @param memory_size  Total size of CHIP-8 memory (should be 4096)
 * @return             0 on success, -1 on failure
 */
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

    // Ensure the ROM fits in memory starting at 0x200
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

/**
 * Copy a block of memory from `src` to `dest`.
 */
void memory_copy(uint8_t *dest, const uint8_t *src, size_t size) {
    memcpy(dest, src, size);
}

/**
 * Reverse the bits in a byte.
 *
 * Example: 0b10110000 → 0b00001101
 * Useful for optional sprite transformations.
 */
uint8_t swap_bytes(uint8_t byte) {
    byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
    byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
    byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;
    return byte;
}

#ifdef DEBUG
/**
 * Print CHIP-8 register and timer state to stdout.
 */
void print_registers(const uint8_t *V, uint16_t I, uint16_t pc, uint8_t delay_timer, uint8_t sound_timer) {
    printf("PC: 0x%04X  I: 0x%04X  DT: %d  ST: %d\n", pc, I, delay_timer, sound_timer);
    for (int i = 0; i < 16; i++) {
        printf("V[%X]: %02X  ", i, V[i]);
        if ((i + 1) % 4 == 0) printf("\n");
    }
    printf("\n");
}
#endif

/**
 * Dump the entire CHIP-8 memory and register state to a binary file.
 * Useful for automated testing.
 */
void dump_memory(Chip8 *chip8, const char *rom_path) {
#ifdef DEBUG
    print_registers(chip8->V, chip8->I, chip8->pc, chip8->delay_timer, chip8->sound_timer);
#endif

    // Cross-platform mkdir — works on Windows and Unix-like systems
#ifdef _WIN32
    mkdir("tests\\python\\dumps");
#else
    mkdir("tests/python/dumps", 0755);
#endif

    // Extract ROM filename from path
    const char *rom_name = strrchr(rom_path, '/');
    if (!rom_name) rom_name = strrchr(rom_path, '\\');
    rom_name = rom_name ? rom_name + 1 : rom_path;

    // Remove file extension (if any)
    char name_buf[64] = {0};
    strncpy(name_buf, rom_name, sizeof(name_buf) - 1);
    char *dot = strrchr(name_buf, '.');
    if (dot) *dot = '\0';

    // Construct output file path
    char path[128];
    snprintf(path, sizeof(path), "tests/python/dumps/%s.bin", name_buf);

    // Write memory to file
    FILE *f = fopen(path, "wb");
    if (!f) {
        perror("Failed to open dump file");
        return;
    }

    fwrite(chip8, 1, sizeof(Chip8), f);
    fclose(f);

    printf("Memory dumped to %s\n", path);
}

/**
 * Halt execution and dump state when test mode ends.
 * This is usually triggered by a RET instruction in test ROMs.
 */
void test_halt(Chip8 *chip8, const char *rom_path) {
    fprintf(stderr, "[TEST_MODE] End of test reached after RET — exiting emulator.\n");
#ifdef DEBUG
    print_registers(chip8->V, chip8->I, chip8->pc, chip8->delay_timer, chip8->sound_timer);
#endif
    dump_memory(chip8, rom_path);
    exit(0);
}
