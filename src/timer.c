/**
 * timer.c
 *
 * CHIP-8 Timer Module
 *
 * Implements the behavior of the delay and sound timers as defined in the CHIP-8 specification.
 * Timers count down at a fixed 60Hz rate and can be accessed or modified by various opcodes.
 */

#include "timer.h"
#include "utils.h"
#include "platform.h"  // For platform_play_beep
#include <stdint.h>
#include <stdio.h>

/**
 * Initialize the CHIP-8 timers.
 *
 * Sets both the delay and sound timers to zero.
 * Should be called once during system startup/reset.
 *
 * @param chip8 Pointer to the CHIP-8 emulator state.
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
 * Update the CHIP-8 timers once per frame (typically at 60Hz).
 *
 * - Decrements the delay timer if it is greater than zero.
 * - Decrements the sound timer and plays a beep when active.
 * - Stops beeping when the sound timer reaches zero.
 *
 * @param chip8 Pointer to the CHIP-8 emulator state.
 */
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
        platform_play_beep(true);  // Activate tone while sound timer is active
    } else {
        platform_play_beep(false); // Silence when sound timer expires
    }
}

/**
 * Get the current value of the delay timer.
 *
 * @param chip8 Pointer to the CHIP-8 emulator state.
 * @return      Current value of the delay timer.
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
 *
 * @param chip8 Pointer to the CHIP-8 emulator state.
 * @return      Current value of the sound timer.
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
 *
 * Used by opcode 0xFx15.
 *
 * @param chip8 Pointer to the CHIP-8 emulator state.
 * @param value New value for the delay timer.
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
 *
 * Used by opcode 0xFx18.
 *
 * @param chip8 Pointer to the CHIP-8 emulator state.
 * @param value New value for the sound timer.
 */
void set_sound_timer(Chip8 *chip8, uint8_t value) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "set_sound called with null pointer\n");
        return;
    }

    chip8->sound_timer = value;
}
