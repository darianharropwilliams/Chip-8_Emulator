#ifndef INPUT_H
#define INPUT_H

#include "chip8.h"

// Initialize the keypad (set all keys to unpressed state)
void keypad_init(Chip8 *chip8);

// Scan the keypad and update key states (called each cycle)
void keypad_scan(Chip8 *chip8);

// Map the key states into the Chip8 keypad array (keypad[16])
void keypad_map(Chip8 *chip8, uint8_t key, bool state);

// Check if a specific key is pressed (returns true if the key is pressed)
bool is_key_pressed(Chip8 *chip8, uint8_t key);

#endif
