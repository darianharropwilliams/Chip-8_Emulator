#include "input.h"
#include <string.h>  // for memset

// Initialize the keypad (set all keys to unpressed)
void keypad_init(Chip8 *chip8) {
    memset(chip8->keypad, 0, sizeof(chip8->keypad));
}

// Poll the physical keyboard and update chip8->keypad[]
void keypad_scan(Chip8 *chip8) {
    // TODO: Integrate SDL2 event polling and map keys to chip8->keypad[]
}

// Set a key state manually (used by SDL event loop)
void keypad_map(Chip8 *chip8, uint8_t key, bool state) {
    if (key < KEYPAD_SIZE) {
        chip8->keypad[key] = state;
    }
}

// Return whether a key is currently pressed
bool is_key_pressed(Chip8 *chip8, uint8_t key) {
    if (key < KEYPAD_SIZE) {
        return chip8->keypad[key];
    }
    return false;
}
