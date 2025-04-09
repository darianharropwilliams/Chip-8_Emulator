#ifndef DISPLAY_H
#define DISPLAY_H

#include "chip8.h"

// Initialize the display (clear the screen and set the draw flag)
void display_init(Chip8 *chip8);

// Draw a sprite at the given coordinates (x, y) with a width of 8 pixels
// Returns 1 if any pixels are flipped (collision), otherwise 0
int draw_sprite(Chip8 *chip8, uint8_t x, uint8_t y, uint8_t height, const uint8_t *sprite);

// Clear the display (set all pixels to 0)
void clear_display(Chip8 *chip8);

// Update the display (trigger redrawing if needed)
void update_display(Chip8 *chip8);

#endif
