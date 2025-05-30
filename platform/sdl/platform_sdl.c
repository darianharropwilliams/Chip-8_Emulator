#include "platform.h"
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

// Constants for CHIP-8 dimensions and scale factor
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define SCALE 10
#define KEYPAD_SIZE 16

// SDL objects for window and rendering
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

// Audio playback objects
static SDL_AudioDeviceID audio_device = 0;
static bool audio_initialized = false;

// Mapping modern keyboard keys to CHIP-8 keypad layout
static const SDL_Scancode keymap[KEYPAD_SIZE] = {
    SDL_SCANCODE_X,    // 0
    SDL_SCANCODE_1,    // 1
    SDL_SCANCODE_2,    // 2
    SDL_SCANCODE_3,    // 3
    SDL_SCANCODE_Q,    // 4
    SDL_SCANCODE_W,    // 5
    SDL_SCANCODE_E,    // 6
    SDL_SCANCODE_A,    // 7
    SDL_SCANCODE_S,    // 8
    SDL_SCANCODE_D,    // 9
    SDL_SCANCODE_Z,    // A
    SDL_SCANCODE_C,    // B
    SDL_SCANCODE_4,    // C
    SDL_SCANCODE_R,    // D
    SDL_SCANCODE_F,    // E
    SDL_SCANCODE_V     // F
};

/**
 * Initialize SDL subsystems and create window/renderer.
 */
void platform_init(void) {
    SDL_setenv("SDL_AUDIODRIVER", "directsound", 1);  // Force DirectSound
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "[SDL] Initialization failed: %s\n", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow("CHIP-8 Emulator",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              DISPLAY_WIDTH * SCALE,
                              DISPLAY_HEIGHT * SCALE,
                              SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "[SDL] Failed to create window: %s\n", SDL_GetError());
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1,
                                  SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "[SDL] Failed to create renderer: %s\n", SDL_GetError());
        exit(1);
    }

    // Optional: audio is initialized lazily
}

/**
 * SDL audio callback to generate a simple square wave tone.
 */
static void audio_callback(void *userdata, uint8_t *stream, int len) {
    static int phase = 0;
    const int sample_rate = 44100;
    const int tone_freq = 440;
    const int period = sample_rate / tone_freq;
    const int half_period = period / 2;

    for (int i = 0; i < len; ++i) {
        stream[i] = (phase < half_period) ? 128 + 64 : 128 - 64;  // Centered square wave
        phase = (phase + 1) % period;
    }
}

/**
 * Set up SDL audio playback if not already initialized.
 */
static void init_audio() {
    if (audio_initialized) return;

    if (!(SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO)) {
        fprintf(stderr, "[SDL] Audio not initialized. Skipping.\n");
        return;
    }

    SDL_AudioSpec desired_spec = {0};
    desired_spec.freq = 44100;
    desired_spec.format = AUDIO_U8;
    desired_spec.channels = 1;
    desired_spec.samples = 512;
    desired_spec.callback = audio_callback;

    // Try first available audio device, fallback to default
    const char *device_name = SDL_GetAudioDeviceName(0, 0);
    if (device_name) {
        audio_device = SDL_OpenAudioDevice(device_name, 0, &desired_spec, NULL, 0);
    }
    if (!audio_device && SDL_GetError()[0]) {
        fprintf(stderr, "[SDL] Named device failed: %s. Trying NULL...\n", SDL_GetError());
        audio_device = SDL_OpenAudioDevice(NULL, 0, &desired_spec, NULL, 0);
    }

    if (!audio_device) {
        fprintf(stderr, "[SDL] Audio device init failed: %s\n", SDL_GetError());
    } else {
        SDL_PauseAudioDevice(audio_device, 1);  // Start paused
        audio_initialized = true;
    }
}

/**
 * Render the CHIP-8 framebuffer to the SDL window.
 *
 * @param pixels  Pointer to 64x32 framebuffer (values should be 0 or 1)
 */
void platform_update_display(const uint8_t *pixels) {
    if (!renderer) {
        fprintf(stderr, "[SDL] platform_update_display called before renderer was initialized\n");
        return;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear to black
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White for pixels

    // Draw each pixel as a filled rectangle
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            if (pixels[y * DISPLAY_WIDTH + x]) {
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

    SDL_RenderPresent(renderer);  // Show the updated frame
}

/**
 * Start or stop the system beep depending on `play`.
 */
void platform_play_beep(bool play) {
    if (!audio_initialized) {
        init_audio();
    }

    if (audio_device) {
        SDL_PauseAudioDevice(audio_device, play ? 0 : 1);
    }
}

/**
 * Poll the current keyboard state and update the CHIP-8 keypad.
 *
 * @param keypad Pointer to CHIP-8's 16-key state array (0 or 1 per key)
 */
void platform_poll_input(uint8_t *keypad) {
    SDL_PumpEvents();  // Update SDL internal input state

    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    if (!keystate) return;

    for (int i = 0; i < KEYPAD_SIZE; i++) {
        keypad[i] = keystate[keymap[i]] ? 1 : 0;
    }
}

/**
 * Clean up all SDL resources.
 */
void platform_quit(void) {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    SDL_Quit();

    if (audio_initialized) {
        SDL_CloseAudioDevice(audio_device);
        audio_initialized = false;
    }
}
