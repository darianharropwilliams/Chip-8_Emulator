#ifndef DISPLAY_H
#define DISPLAY_H

#include "chip8.h"

// Initialize display memory and state (clears screen)
void display_init(Chip8 *chip8);

// Draw a sprite from memory at (x, y) with height N
// Returns 1 if any pixels were flipped from set to unset (collision), 0 otherwise
int draw_sprite(Chip8 *chip8, uint8_t x, uint8_t y, uint8_t height, const uint8_t *sprite);

// Clear all display pixels
void clear_display(Chip8 *chip8);

// Trigger screen update (only if draw_flag is set)
void update_display(Chip8 *chip8);

// Shut down display-related resources
void display_quit();

#endif
