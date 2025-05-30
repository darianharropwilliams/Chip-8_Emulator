/**
 * display.c
 *
 * CHIP-8 Display Module
 *
 * Handles the virtual framebuffer, drawing operations, and display backend
 * integration (SDL or WebAssembly). Implements the rendering behavior
 * of CHIP-8 as specified in the instruction set.
 */

#include "display.h"
#include "utils.h"
#include "platform.h"
#include <string.h>

#define SCALE 10  // Used internally for resolution scaling (e.g., SDL)

 /**
  * Initialize the display system.
  *
  * - Clears the framebuffer.
  * - Sets the draw flag to force a redraw.
  * - Initializes platform-specific rendering backend (SDL, WASM, etc.).
  *
  * @param chip8 Pointer to the CHIP-8 emulator instance.
  */
void display_init(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "display_init called on null Chip8 pointer\n");
        return;
    }

    memset(chip8->display, 0, sizeof(chip8->display));  // Clear framebuffer
    chip8->draw_flag = true;                            // Flag for initial redraw

    platform_init();  // Initialize SDL/WASM rendering backend
}

/**
 * Clear the display.
 *
 * - Resets all pixels in the framebuffer.
 * - Sets the draw flag so the display updates on the next frame.
 *
 * Typically invoked by opcode 0x00E0 (CLS).
 *
 * @param chip8 Pointer to the CHIP-8 emulator instance.
 */
void clear_display(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "clear_display called on null Chip8 pointer\n");
        return;
    }

    memset(chip8->display, 0, sizeof(chip8->display));  // Set all pixels to 0
    chip8->draw_flag = true;                            // Request display refresh
}

/**
 * Draw a sprite to the screen using XOR rendering.
 *
 * Each byte of the sprite corresponds to one horizontal row of 8 pixels.
 * If any drawn pixel flips from 1 to 0 (collision), this function returns 1.
 * Otherwise, returns 0.
 *
 * Wrapping occurs automatically if the sprite exceeds screen boundaries.
 *
 * @param chip8  Pointer to the CHIP-8 emulator instance.
 * @param x      Horizontal position (Vx).
 * @param y      Vertical position (Vy).
 * @param height Number of bytes (rows) to draw.
 * @param sprite Pointer to sprite data in memory.
 * @return       1 if any pixel was unset (collision), 0 otherwise.
 */
int draw_sprite(Chip8 *chip8, uint8_t x, uint8_t y, uint8_t height, const uint8_t *sprite) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "draw_sprite called on null Chip8 pointer\n");
        return 0;
    }

    if (chip8->I + height > MEMORY_SIZE) {
        DEBUG_PRINT(chip8, "draw_sprite error: sprite read out of bounds (I=%04X, height=%d)\n", chip8->I, height);
        return 0;
    }

    int collision = 0;

    for (uint8_t row = 0; row < height; ++row) {
        uint8_t sprite_byte = sprite[row];

        for (uint8_t col = 0; col < 8; ++col) {
            // Check if this bit should be drawn
            if ((sprite_byte & (0x80 >> col)) != 0) {
                uint8_t px = (x + col) % DISPLAY_WIDTH;
                uint8_t py = (y + row) % DISPLAY_HEIGHT;
                uint16_t index = py * DISPLAY_WIDTH + px;

                if (chip8->display[index] == 1)
                    collision = 1;  // Erasing an existing pixel

                chip8->display[index] ^= 1;  // Toggle pixel using XOR
            }
        }
    }

    return collision;
}

/**
 * Update the physical display.
 *
 * Delegates to platform-specific rendering to copy the framebuffer
 * contents to the screen. Should only be called when draw_flag is set.
 *
 * @param chip8 Pointer to the CHIP-8 emulator instance.
 */
void update_display(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "update_display called on null Chip8 pointer\n");
        return;
    }

    platform_update_display(chip8->display);  // Delegate to SDL or Web backend
}

/**
 * Shutdown and clean up display resources.
 *
 * Destroys SDL window and renderer or equivalent platform components.
 */
void display_quit() {
    platform_quit();  // Cleanup handled by platform backend
}
