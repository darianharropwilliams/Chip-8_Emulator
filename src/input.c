/**
 * input.c
 *
 * CHIP-8 Input Module
 *
 * Handles input from the platform's physical or virtual keyboard.
 * Provides abstractions for:
 * - Initializing the keypad state
 * - Polling for input
 * - Setting and querying individual key states
 */

#include "input.h"
#include "utils.h"
#include "platform.h"
#include <string.h>

/**
 * Initialize the CHIP-8 keypad state.
 *
 * Sets all 16 keys (0x0 to 0xF) to unpressed (0).
 *
 * @param chip8 Pointer to the CHIP-8 emulator state.
 */
void keypad_init(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "keypad_init called on null Chip8 pointer\n");
        return;
    }

    memset(chip8->keypad, 0, sizeof(chip8->keypad));  // Reset all keys to 0
}

/**
 * Poll current key states from the platform layer.
 *
 * Updates the internal `chip8->keypad` array with the status of each key.
 * Delegates to `platform_poll_input` which handles SDL or WASM input.
 *
 * @param chip8 Pointer to the CHIP-8 emulator state.
 */
void keypad_scan(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "keypad_scan: chip8 pointer is null\n");
        return;
    }

    platform_poll_input(chip8->keypad);  // Platform-specific polling
}

/**
 * Set the state of an individual key in the CHIP-8 keypad.
 *
 * This is typically used for emulated input (e.g., during testing or injection).
 *
 * @param chip8 Pointer to the CHIP-8 emulator state.
 * @param key   Key index (0x0 to 0xF).
 * @param state Boolean flag: true = pressed, false = released.
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

    chip8->keypad[key] = state ? 1 : 0;
}

/**
 * Query whether a specific CHIP-8 key is currently pressed.
 *
 * Used by opcodes 0xEx9E (SKP Vx) and 0xExA1 (SKNP Vx).
 *
 * @param chip8 Pointer to the CHIP-8 emulator state.
 * @param key   Key index (0x0 to 0xF).
 * @return      true if the key is pressed; false otherwise.
 */
bool is_key_pressed(Chip8 *chip8, uint8_t key) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "is_key_pressed called on null Chip8 pointer\n");
        return false;
    }

    if (key < KEYPAD_SIZE) {
        return chip8->keypad[key] != 0;
    }

    DEBUG_PRINT(chip8, "Invalid key index: %d\n", key);
    return false;
}
