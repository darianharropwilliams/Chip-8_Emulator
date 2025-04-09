#ifndef TIMER_H
#define TIMER_H

#include "chip8.h"

// Initialize the timers (set both timers to 0)
void timer_init(Chip8 *chip8);

// Update the timers (decrement timers at 60Hz)
void timer_update(Chip8 *chip8);

// Get the current delay timer value
uint8_t get_delay_timer(Chip8 *chip8);

// Get the current sound timer value
uint8_t get_sound_timer(Chip8 *chip8);

// Set the delay timer value
void set_delay_timer(Chip8 *chip8, uint8_t value);

// Set the sound timer value
void set_sound_timer(Chip8 *chip8, uint8_t value);

#endif
