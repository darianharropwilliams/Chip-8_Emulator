#include "input.h"
#include "utils.h"
#include <string.h>  // for memset
#include <SDL.h>
#include <stdio.h>


// CHIP-8 key mapping (modern keyboard to CHIP-8 layout)
static const SDL_Scancode keymap[KEYPAD_SIZE] = {
    SDL_SCANCODE_X,    // 0
    SDL_SCANCODE_1,    // 1
    SDL_SCANCODE_2,    // 2
    SDL_SCANCODE_3,    // 3
    SDL_SCANCODE_Q,    // 4
    SDL_SCANCODE_W,    // 5
    SDL_SCANCODE_E,    // 6
    SDL_SCANCODE_A,    // 7
    SDL_SCANCODE_S,    // 8
    SDL_SCANCODE_D,    // 9
    SDL_SCANCODE_Z,    // A
    SDL_SCANCODE_C,    // B
    SDL_SCANCODE_4,    // C
    SDL_SCANCODE_R,    // D
    SDL_SCANCODE_F,    // E
    SDL_SCANCODE_V     // F
};

// Initialize the keypad (set all keys to unpressed)
void keypad_init(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "keypad_init called on null Chip8 pointer\n");
        return;
    }
    
    memset(chip8->keypad, 0, sizeof(chip8->keypad));
}

// Poll the physical keyboard and update chip8->keypad[]
void keypad_scan(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "keypad_scan: chip8 pointer is null\n");
        return;
    }

    SDL_PumpEvents();

    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    if (!keystate) {
        DEBUG_PRINT(chip8, "keypad_scan: SDL_GetKeyboardState returned NULL\n");
        return;
    }

    for (uint8_t i = 0; i < KEYPAD_SIZE; i++) {
        chip8->keypad[i] = keystate[keymap[i]] ? 1 : 0;
    }
}


// Set a key state manually (used by SDL event loop)
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

// Return whether a key is currently pressed
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
