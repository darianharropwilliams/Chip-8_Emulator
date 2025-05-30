// chip8_testshim.c
#include "chip8_testshim.h"
#include <stdio.h>
#include <stdint.h>

/**
 * Dumps the current Chip8 emulator state to a binary file for testing.
 * This format is matched by `helpers.py::read_dump()` in the Python test suite.
 */
void chip8_dump_state(Chip8 *chip8, const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "[ERROR] Could not open dump file: %s\n", filename);
        return;
    }

    // Dump layout:
    // - 0x0000–0x0FFF: memory (4096 bytes)
    // - 0x1000–0x100F: V registers (16 bytes)
    // - 0x1010–0x1011: I register (2 bytes, little endian)
    // - 0x1012–0x1013: PC register (2 bytes, little endian)
    // - 0x1014: delay_timer (1 byte)
    // - 0x1015: sound_timer (1 byte)

    fwrite(chip8->memory, 1, 4096, f);                  // memory
    fwrite(chip8->V, 1, 16, f);                         // V[0–F]
    fwrite(&chip8->I, sizeof(uint16_t), 1, f);          // I
    fwrite(&chip8->pc, sizeof(uint16_t), 1, f);         // PC
    fwrite(&chip8->delay_timer, sizeof(uint8_t), 1, f); // delay_timer
    fwrite(&chip8->sound_timer, sizeof(uint8_t), 1, f); // sound_timer

    fclose(f);
}
