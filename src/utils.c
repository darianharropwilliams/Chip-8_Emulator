/**
 * utils.c
 *
 * Utility functions for CHIP-8 emulator support:
 * - ROM loading
 * - Bitwise operations
 * - Memory inspection and dumping
 * - Test mode state handling
 *
 * These functions are used both during normal execution and automated testing.
 */

#include "chip8.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <direct.h>     // For _mkdir on Windows
#include <libgen.h>     // For basename and dirname
#include <sys/stat.h>   // For mkdir on Unix
#include <sys/types.h>

/**
 * Loads a CHIP-8 ROM file from disk into memory.
 *
 * @param filename     Path to the ROM file on disk.
 * @param memory       Pointer to the memory buffer (starts at 0x200).
 * @param memory_size  Total size of CHIP-8 memory (typically 4096 bytes).
 *
 * @return 0 on success, -1 on failure (file not found, too large, etc.).
 */
int load_rom(const char *filename, uint8_t *memory, uint16_t memory_size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("[ERROR] fopen");
        fprintf(stderr, "[ERROR] Failed to open ROM file: %s\n", filename);
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    if (file_size <= 0) {
        fprintf(stderr, "[ERROR] ROM is empty or unreadable.\n");
        fclose(file);
        return -1;
    }

    if (file_size > memory_size - 0x200) {
        fprintf(stderr,
                "[ERROR] ROM too large to fit in CHIP-8 memory. Max allowed: %d bytes\n",
                memory_size - 0x200);
        fclose(file);
        return -1;
    }

    size_t bytes_read = fread(memory, 1, file_size, file);
    fclose(file);

    if (bytes_read != file_size) {
        fprintf(stderr,
                "[ERROR] fread incomplete: expected %ld bytes, got %zu\n",
                file_size, bytes_read);
        return -1;
    }

    return 0;
}

/**
 * Copies a block of memory from source to destination.
 *
 * @param dest  Destination memory buffer.
 * @param src   Source memory buffer.
 * @param size  Number of bytes to copy.
 */
void memory_copy(uint8_t *dest, const uint8_t *src, size_t size) {
    memcpy(dest, src, size);
}

/**
 * Reverses the bits in a single byte.
 *
 * Useful for bit-level sprite operations or mirroring.
 *
 * @param byte  Byte to reverse.
 * @return      Byte with bits reversed.
 */
uint8_t swap_bytes(uint8_t byte) {
    byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
    byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
    byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;
    return byte;
}

#ifdef DEBUG
/**
 * Debug utility to print the CHIP-8 register and timer state.
 *
 * @param V            Pointer to the 16 V-registers (V0–VF).
 * @param I            Index register.
 * @param pc           Program counter.
 * @param delay_timer  Delay timer value.
 * @param sound_timer  Sound timer value.
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
 * Dumps the emulator's memory and register state to a binary file.
 *
 * This is used during automated tests to capture final execution state
 * for post-analysis in Python.
 *
 * Dump format:
 *   - 0x0000–0x0FFF: memory (4096 bytes)
 *   - 0x1000–0x100F: V registers (16 bytes)
 *   - 0x1010–0x1011: I register (2 bytes, little endian)
 *   - 0x1012–0x1013: PC register (2 bytes, little endian)
 *   - 0x1014: delay_timer (1 byte)
 *   - 0x1015: sound_timer (1 byte)
 *
 * @param chip8     Pointer to the emulator state.
 * @param rom_path  Path to the loaded ROM (used to name the output file).
 */
void dump_memory(Chip8 *chip8, const char *rom_path) {
    // Extract base ROM name from path
    const char *rom_name = strrchr(rom_path, '/');
    if (!rom_name) rom_name = strrchr(rom_path, '\\');
    rom_name = rom_name ? rom_name + 1 : rom_path;

    // Strip file extension
    char name_buf[64] = {0};
    strncpy(name_buf, rom_name, sizeof(name_buf) - 1);
    char *dot = strrchr(name_buf, '.');
    if (dot) *dot = '\0';

    // Build absolute path to dumps directory, relative to ROM
    char base_dir[256];
    strncpy(base_dir, rom_path, sizeof(base_dir) - 1);
    char *last_sep = strrchr(base_dir, '\\');
    if (!last_sep) last_sep = strrchr(base_dir, '/');
    if (last_sep) *last_sep = '\0';

    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s/../dumps/%s.bin", base_dir, name_buf);

    // Create output directories if needed
#ifdef _WIN32
    _mkdir("tests");
    _mkdir("tests\\python");
    _mkdir("tests\\python\\dumps");
#else
    mkdir("tests", 0755);
    mkdir("tests/python", 0755);
    mkdir("tests/python/dumps", 0755);
#endif

    FILE *f = fopen(full_path, "wb");
    if (!f) {
        perror("Failed to open dump file");
        return;
    }

    // Write binary dump in test-harness compatible layout
    fwrite(chip8->memory, 1, 4096, f);
    fwrite(chip8->V, 1, 16, f);
    fwrite(&chip8->I, sizeof(uint16_t), 1, f);
    fwrite(&chip8->pc, sizeof(uint16_t), 1, f);
    fwrite(&chip8->delay_timer, sizeof(uint8_t), 1, f);
    fwrite(&chip8->sound_timer, sizeof(uint8_t), 1, f);

    fclose(f);
    printf("Memory dumped to %s\n", full_path);
}

/**
 * Triggers an emulator halt from test mode and writes state to a dump file.
 *
 * Called automatically during RET if chip8->test_mode is enabled.
 *
 * @param chip8     Pointer to emulator state.
 * @param rom_path  Path to the currently executing ROM.
 */
void test_halt(Chip8 *chip8, const char *rom_path) {
    fprintf(stderr, "[TEST_MODE] End of test reached after RET — exiting emulator.\n");

#ifdef DEBUG
    print_registers(chip8->V, chip8->I, chip8->pc, chip8->delay_timer, chip8->sound_timer);
#endif

    dump_memory(chip8, rom_path);
    exit(0);
}
