// timer.c
#include "timer.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
// #include <SDL.h>

static SDL_AudioDeviceID audio_device = 0;
static bool audio_initialized = false;


#define SAMPLE_RATE 44100
#define TONE_FREQUENCY 440
#define TONE_VOLUME 64

void audio_callback(void *userdata, Uint8 *stream, int len) {
    static int phase = 0;
    int period = 44100 / 440;
    int half_period = period / 2;

    for (int i = 0; i < len; ++i) {
        stream[i] = (phase < half_period) ? 128 + 64 : 128 - 64;
        phase = (phase + 1) % period;
    }
}



void audio_init() {
    if (audio_initialized) return;

    if (!(SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO)) {
        fprintf(stderr, "SDL audio subsystem not initialized yet. Skipping audio init.\n");
        return;
    }

    SDL_AudioSpec desired_spec = {0};
    desired_spec.freq = 44100;
    desired_spec.format = AUDIO_U8;
    desired_spec.channels = 1;
    desired_spec.samples = 512;
    desired_spec.callback = audio_callback;

    const char *device_name = SDL_GetAudioDeviceName(0, 0);
    if (device_name) {
        audio_device = SDL_OpenAudioDevice(device_name, 0, &desired_spec, NULL, 0);
    }

    // Fallback if device_name failed or didn't work
    if (!audio_device && SDL_GetError()[0]) {
        fprintf(stderr, "[WARNING] Named device failed: %s\nTrying NULL fallback...\n", SDL_GetError());
        audio_device = SDL_OpenAudioDevice(NULL, 0, &desired_spec, NULL, 0);
    }

    if (!audio_device) {
        fprintf(stderr, "[ERROR] Final fallback failed: %s\n", SDL_GetError());
    } else {
        fprintf(stderr, "[INFO] Audio initialized using device ID %d\n", audio_device);
        audio_initialized = true;
    }
}


void audio_quit() {
    if (audio_initialized) {
        SDL_CloseAudioDevice(audio_device);
        audio_initialized = false;
    }
}



// ----------------------------------------------------------
// Initialize both timers to 0
// ----------------------------------------------------------
void timer_init(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "timer_init called with null pointer\n");
        return;
    }

    chip8->delay_timer = 0;
    chip8->sound_timer = 0;

    // Removed audio_init(); now called in display_init() after SDL_Init()
}


// ----------------------------------------------------------
// Update timers: decrement each if greater than 0
// Called once per emulation frame (ideally at 60Hz)
// ----------------------------------------------------------
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
        if (audio_initialized) SDL_PauseAudioDevice(audio_device, 0); // unpause
    } else {
        if (audio_initialized) SDL_PauseAudioDevice(audio_device, 1); // pause
    }
}

// ----------------------------------------------------------
// Return current delay timer value
// ----------------------------------------------------------
uint8_t get_delay_timer(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "get_delay_timer called with null pointer\n");
        return 0;
    }
    

    return chip8->delay_timer;
}

// ----------------------------------------------------------
// Return current sound timer value
// ----------------------------------------------------------
uint8_t get_sound_timer(Chip8 *chip8) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "get_sound_timer called with null pointer\n");
        return 0;
    }
    
    return chip8->sound_timer;
}

// ----------------------------------------------------------
// Set delay timer
// ----------------------------------------------------------
void set_delay_timer(Chip8 *chip8, uint8_t value) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "set_delay_timer called with null pointer\n");
        return;
    }
    
    chip8->delay_timer = value;
}

// ----------------------------------------------------------
// Set sound timer
// ----------------------------------------------------------
void set_sound_timer(Chip8 *chip8, uint8_t value) {
    if (!chip8) {
        DEBUG_PRINT(chip8, "set_sound called with null pointer\n");
        return;
    }
    
    chip8->sound_timer = value;
}
