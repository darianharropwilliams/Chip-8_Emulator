#include "input.h"
#include "utils.h"
#include "platform.h"
#include <string.h>

/**
 * Initialize the keypad state.
 * Sets all keys to "not pressed" (0).
 */
void keypad_init(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "keypad_init called on null Chip8 pointer\n");
        return;
    }
    memset(chip8->keypad, 0, sizeof(chip8->keypad));  // Clear keypad state
}

/**
 * Poll for key input from the platform layer (e.g. SDL or WASM).
 * Fills the `chip8->keypad` array with current key states.
 *
 * This abstracts away platform-specific input polling logic.
 */
void keypad_scan(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "keypad_scan: chip8 pointer is null\n");
        return;
    }

    platform_poll_input(chip8->keypad);  // Delegate to platform
}

/**
 * Update the key state for a specific CHIP-8 key.
 *
 * @param key   The CHIP-8 key index (0x0 to 0xF)
 * @param state True (pressed) or False (released)
 */
void keypad_map(Chip8 *chip8, uint8_t key, bool state) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "keypad_map called on null Chip8 pointer\n");
        return;
    }

    if (key >= KEYPAD_SIZE) {
        DEBUG_PRINT(chip8, "Invalid key index: %d\n", key);
        return;
    }

    chip8->keypad[key] = state;
}

/**
 * Check whether a specific CHIP-8 key is currently pressed.
 *
 * @param key The CHIP-8 key index (0x0 to 0xF)
 * @return    True if pressed, false otherwise
 */
bool is_key_pressed(Chip8 *chip8, uint8_t key) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "is_key_pressed called on null Chip8 pointer\n");
        return false;
    }

    if (key < KEYPAD_SIZE) {
        return chip8->keypad[key];
    }

    DEBUG_PRINT(chip8, "Invalid key index: %d\n", key);
    return false;
}
