#include "display.h"
#include "utils.h"
#include "platform.h"
#include <string.h>

#define SCALE 10  // Still used internally in some contexts (e.g. SDL)

// ----------------------------------------------------------
// Initializes the display system:
// - Clears the framebuffer
// - Sets draw_flag so the screen gets redrawn
// - Initializes the underlying platform layer (SDL or WASM)
// ----------------------------------------------------------
void display_init(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "display_init called on null Chip8 pointer\n");
        return;
    }

    memset(chip8->display, 0, sizeof(chip8->display));  // Clear framebuffer
    chip8->draw_flag = true;                            // Mark screen for update

    platform_init();  // Initialize SDL or WASM display backend
}

// ----------------------------------------------------------
// Clears the CHIP-8 framebuffer and triggers a redraw.
// This is typically used by the CLS (0x00E0) opcode.
// ----------------------------------------------------------
void clear_display(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "clear_display called on null Chip8 pointer\n");
        return;
    }

    memset(chip8->display, 0, sizeof(chip8->display));  // Set all pixels to 0
    chip8->draw_flag = true;                            // Request a redraw
}

// ----------------------------------------------------------
// Draws a sprite at (x, y) with a height of N bytes.
// Each sprite byte represents 8 horizontal pixels.
// Pixels are XORed onto the screen, and if any pixels are
// erased (from 1 to 0), the function returns 1 to indicate
// a collision, as per CHIP-8 spec (VF gets set accordingly).
//
// Sprite wrapping is handled using modulo on coordinates.
// ----------------------------------------------------------
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
            // If bit is set, process the pixel
            if ((sprite_byte & (0x80 >> col)) != 0) {
                uint8_t px = (x + col) % DISPLAY_WIDTH;
                uint8_t py = (y + row) % DISPLAY_HEIGHT;
                uint16_t index = py * DISPLAY_WIDTH + px;

                if (chip8->display[index] == 1)
                    collision = 1;  // Sprite pixel collided with existing pixel

                chip8->display[index] ^= 1;  // XOR toggle
            }
        }
    }

    return collision;
}

// ----------------------------------------------------------
// Sends the current framebuffer to the screen using the platform layer.
// This should be called when draw_flag is true.
// ----------------------------------------------------------
void update_display(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "update_display called on null Chip8 pointer\n");
        return;
    }

    platform_update_display(chip8->display);  // Backend-specific draw call
}

// ----------------------------------------------------------
// Shut down display resources (e.g. SDL window and renderer).
// ----------------------------------------------------------
void display_quit() {
    platform_quit();  // Cleanup handled by SDL or WASM
}
