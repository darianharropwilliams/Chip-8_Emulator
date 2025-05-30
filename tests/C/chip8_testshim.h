#ifndef CHIP8_TESTSHIM_H
#define CHIP8_TESTSHIM_H

#include "chip8.h"

/**
 * Dump the state of a Chip8 instance to a binary file.
 * This function is used during automated testing to validate emulator behavior.
 *
 * @param chip8    Pointer to the emulator instance.
 * @param filename Path to the output dump file.
 */
void chip8_dump_state(Chip8 *chip8, const char *filename);

#endif // CHIP8_TESTSHIM_H
