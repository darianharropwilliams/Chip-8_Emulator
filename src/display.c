#include "display.h"
#include "utils.h"
#include <string.h>
#include <SDL.h>
#include <stdio.h>

#define SCALE 10  // Each CHIP-8 pixel is scaled up to a 10x10 square

// SDL window and renderer (static so only available within this file)
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

// ----------------------------------------------------------
// Initialize the display:
// - Clear the screen buffer
// - Set the draw flag
// - Initialize SDL, create window and renderer
// ----------------------------------------------------------
void display_init(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "display_init called on null Chip8 pointer\n");
        return;
    }

    memset(chip8->display, 0, sizeof(chip8->display));
    chip8->draw_flag = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        DEBUG_PRINT(chip8, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow("CHIP-8 Emulator",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              DISPLAY_WIDTH * SCALE,
                              DISPLAY_HEIGHT * SCALE,
                              SDL_WINDOW_SHOWN);

    if (!window) {
        DEBUG_PRINT(chip8, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        DEBUG_PRINT(chip8, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }
}

// ----------------------------------------------------------
// Clear the display buffer and set draw flag
// ----------------------------------------------------------
void clear_display(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "clear_display called on null Chip8 pointer\n");
        return;
    }

    memset(chip8->display, 0, sizeof(chip8->display));
    chip8->draw_flag = true;
}

// ----------------------------------------------------------
// Draw a sprite on the display at (x, y).
// - Each byte in `sprite` represents one row of 8 pixels.
// - XORs each pixel onto display buffer.
// - Wraps around screen edges.
// - Returns 1 if any pixels were flipped from 1 to 0.
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

        // Wrapping behavior: sprites that overflow past screen edges will wrap around
        for (uint8_t col = 0; col < 8; ++col) {
            if ((sprite_byte & (0x80 >> col)) != 0) {
                uint8_t px = (x + col) % DISPLAY_WIDTH;
                uint8_t py = (y + row) % DISPLAY_HEIGHT;
                uint16_t index = py * DISPLAY_WIDTH + px;

                if (chip8->display[index] == 1)
                    collision = 1;

                chip8->display[index] ^= 1;
            }
        }
    }

    return collision;
}

// ----------------------------------------------------------
// Render the CHIP-8 display buffer to the SDL window
// ----------------------------------------------------------
void update_display(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "update_display called on null Chip8 pointer\n");
        return;
    }

    if (!renderer) {
        DEBUG_PRINT(chip8, "update_display called before renderer was initialized\n");
        return;
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White pixels

    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            if (chip8->display[y * DISPLAY_WIDTH + x]) {
                SDL_Rect pixel = {
                    .x = x * SCALE,
                    .y = y * SCALE,
                    .w = SCALE,
                    .h = SCALE
                };
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

// ----------------------------------------------------------
// Clean up SDL resources (optional call at program end)
// ----------------------------------------------------------
void display_quit() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}
