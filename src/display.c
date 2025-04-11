#include "display.h"
#include <string.h>  // for memset

// Initialize the display (clear the screen and set draw_flag)
void display_init(Chip8 *chip8) {
    memset(chip8->display, 0, sizeof(chip8->display));
    chip8->draw_flag = true;
}

// Clear the display (set all pixels to 0)
void clear_display(Chip8 *chip8) {
    memset(chip8->display, 0, sizeof(chip8->display));
    chip8->draw_flag = true;
}

// Draw a sprite at (x, y) with height and memory pointer
// Returns 1 if any pixels were flipped (for collision)
int draw_sprite(Chip8 *chip8, uint8_t x, uint8_t y, uint8_t height, const uint8_t *sprite) {
    // TODO: Implement sprite drawing with XOR and wraparound logic
    // Set chip8->V[0xF] = collision flag in opcode, not here
    return 0;
}

// Update display (used if draw_flag is set)
void update_display(Chip8 *chip8) {
    // TODO: Render chip8->display[] buffer to screen using SDL2 or another backend
}
