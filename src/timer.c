// timer.c
#include "timer.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>

// ----------------------------------------------------------
// Initialize both timers to 0
// ----------------------------------------------------------
void timer_init(Chip8 *chip8) {

    if (!chip8) {
        DEBUG_PRINT(chip8, "timer_init called with null pointer\n");
        return;
    }
    
    chip8->delay_timer = 0;
    chip8->sound_timer = 0;
}

// ----------------------------------------------------------
// Update timers: decrement each if greater than 0
// Called once per emulation frame (ideally at 60Hz)
// ----------------------------------------------------------
void timer_update(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "timer_update called with null pointer\n");
        return;
    }
    

    if (chip8->delay_timer > 0) {
        chip8->delay_timer--;
    }

    if (chip8->sound_timer > 0) {
        chip8->sound_timer--;
        // Optional: hook for sound output when sound_timer > 0
        // You could interface with SDL sound here if needed.
    }
}

// ----------------------------------------------------------
// Return current delay timer value
// ----------------------------------------------------------
uint8_t get_delay_timer(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "get_delay_timer called with null pointer\n");
        return 0;
    }
    

    return chip8->delay_timer;
}

// ----------------------------------------------------------
// Return current sound timer value
// ----------------------------------------------------------
uint8_t get_sound_timer(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "get_sound_timer called with null pointer\n");
        return 0;
    }
    
    return chip8->sound_timer;
}

// ----------------------------------------------------------
// Set delay timer
// ----------------------------------------------------------
void set_delay_timer(Chip8 *chip8, uint8_t value) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "set_delay_timer called with null pointer\n");
        return;
    }
    
    chip8->delay_timer = value;
}

// ----------------------------------------------------------
// Set sound timer
// ----------------------------------------------------------
void set_sound_timer(Chip8 *chip8, uint8_t value) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "set_sound called with null pointer\n");
        return;
    }
    
    chip8->sound_timer = value;
}
