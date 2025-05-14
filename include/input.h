#ifndef INPUT_H
#define INPUT_H

#include "chip8.h"

// Set all keys to unpressed
void keypad_init(Chip8 *chip8);

// Scan physical key states and update CHIP-8 keypad
void keypad_scan(Chip8 *chip8);

// Update the key state array for a specific key
void keypad_map(Chip8 *chip8, uint8_t key, bool state);

// Check whether a key is currently pressed
bool is_key_pressed(Chip8 *chip8, uint8_t key);

#endif
