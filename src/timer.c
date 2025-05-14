#include "timer.h"
#include "utils.h"
#include "platform.h"  // For platform_play_beep
#include <stdint.h>
#include <stdio.h>

/**
 * Initialize the delay and sound timers.
 * Sets both timers to 0.
 */
void timer_init(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "timer_init called with null pointer\n");
        return;
    }

    chip8->delay_timer = 0;
    chip8->sound_timer = 0;
}

/**
 * Decrement the delay and sound timers if they are greater than 0.
 * This function should be called at a 60Hz rate.
 * Also triggers the platform beep system when the sound timer is active.
 */
void timer_update(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "timer_update called with null pointer\n");
        return;
    }

    // Decrement delay timer if active
    if (chip8->delay_timer > 0) {
        chip8->delay_timer--;
    }

    // Decrement sound timer and toggle beep
    if (chip8->sound_timer > 0) {
        chip8->sound_timer--;
        platform_play_beep(true);   // Activate beep while sound_timer > 0
    } else {
        platform_play_beep(false);  // Silence when timer reaches 0
    }
}

/**
 * Get the current value of the delay timer.
 */
uint8_t get_delay_timer(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "get_delay_timer called with null pointer\n");
        return 0;
    }

    return chip8->delay_timer;
}

/**
 * Get the current value of the sound timer.
 */
uint8_t get_sound_timer(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "get_sound_timer called with null pointer\n");
        return 0;
    }

    return chip8->sound_timer;
}

/**
 * Set the delay timer to a specific value.
 */
void set_delay_timer(Chip8 *chip8, uint8_t value) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "set_delay_timer called with null pointer\n");
        return;
    }

    chip8->delay_timer = value;
}

/**
 * Set the sound timer to a specific value.
 */
void set_sound_timer(Chip8 *chip8, uint8_t value) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "set_sound called with null pointer\n");
        return;
    }

    chip8->sound_timer = value;
}
