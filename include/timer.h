#ifndef TIMER_H
#define TIMER_H

#include "chip8.h"
#include <stdint.h>

// Set delay and sound timers to 0
void timer_init(Chip8 *chip8);

// Decrement delay/sound timers at 60Hz (if > 0)
void timer_update(Chip8 *chip8);

// Accessor functions for the delay and sound timers
uint8_t get_delay_timer(Chip8 *chip8);
uint8_t get_sound_timer(Chip8 *chip8);

// Set the delay or sound timers manually
void set_delay_timer(Chip8 *chip8, uint8_t value);
void set_sound_timer(Chip8 *chip8, uint8_t value);

// SDL audio integration (optional)
void audio_callback(void *userdata, uint8_t *stream, int len);
void audio_init();    // Start audio engine
void audio_quit();    // Stop audio engine

#endif
