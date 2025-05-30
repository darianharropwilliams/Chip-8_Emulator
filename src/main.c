/**
 * main.c
 *
 * Entry point for the CHIP-8 emulator.
 * Supports both interactive and test-mode execution.
 *
 * In interactive mode, a ROM is executed in a 60 FPS loop using SDL2.
 * In test mode, the emulator runs a limited number of cycles and exits after a RET instruction.
 *
 * Usage:
 *     chip8 <ROM file> [--test]
 */

#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <SDL_stdinc.h>
#include <SDL_timer.h>

#include "chip8.h"
#include "utils.h"
#include "platform.h"
#include "display.h"

// Global CHIP-8 VM instance
Chip8 chip8;

// Signal-safe flag to support graceful shutdown on SIGINT
volatile sig_atomic_t quit_requested = 0;

/**
 * Signal handler for SIGINT (Ctrl+C).
 * Sets a flag to allow the main loop to terminate gracefully.
 */
void handle_signal(int signal) {
    const char msg[] = "\nCaught signal. Exiting cleanly...\n";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
    quit_requested = 1;
}

#ifdef __EMSCRIPTEN__
/**
 * Emscripten entry point (browser-based builds).
 */
int main(int argc, char *argv[])
#else
/**
 * SDL-based entry point (native desktop).
 */
int SDL_main(int argc, char *argv[])
#endif
{
    bool test_mode = false;

    // Parse command-line arguments
    if (argc == 3 && strcmp(argv[2], "--test") == 0) {
        test_mode = true;
    } else if (argc != 2) {
        fprintf(stderr, "Usage: %s <ROM file> [--test]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Initialize emulator state
    chip8_init(&chip8);

    // Enable test mode (used to trigger test_halt() on RET)
    if (test_mode) {
        chip8.test_mode = true;
    }

    // Store ROM path (used for dumping results in test mode)
    strncpy(chip8.rom_path, argv[1], sizeof(chip8.rom_path) - 1);
    chip8.rom_path[sizeof(chip8.rom_path) - 1] = '\0';

    // Load the ROM into memory starting at 0x200
    if (chip8_load_rom(&chip8, argv[1])) {
        fprintf(stderr, "Failed to load ROM: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    // Register signal handler for graceful termination
    if (signal(SIGINT, handle_signal) == SIG_ERR) {
        fprintf(stderr, "Failed to register SIGINT handler\n");
        return EXIT_FAILURE;
    }

    /**
     * -------------------
     * TEST MODE EXECUTION
     * -------------------
     * Executes a fixed number of frames and relies on RET to call test_halt(),
     * which writes a binary dump and exits. This path is used for automated testing.
     */
    if (test_mode) {
        const int CYCLES_PER_FRAME = 10;
        const int FRAME_DELAY_MS = 1000 / 60;
        const int TEST_FRAMES = 10;

        for (int frame = 0; frame < TEST_FRAMES && !quit_requested; frame++) {
            clock_t start = clock();

            for (int i = 0; i < CYCLES_PER_FRAME; i++) {
                chip8_cycle(&chip8);
            }

            clock_t end = clock();
            int elapsed_ms = (int)((end - start) * 1000 / CLOCKS_PER_SEC);

            // Frame pacing to simulate ~60Hz
            if (elapsed_ms < FRAME_DELAY_MS) {
                struct timespec ts = {
                    .tv_sec = 0,
                    .tv_nsec = (FRAME_DELAY_MS - elapsed_ms) * 1000000
                };
                nanosleep(&ts, NULL);
            }
        }

        // Clean shutdown after test run
        display_quit();
        return EXIT_SUCCESS;
    }

#ifndef __EMSCRIPTEN__
    /**
     * ------------------------
     * INTERACTIVE MODE EXECUTION
     * ------------------------
     * Main event loop that runs continuously, cycling the VM and updating display.
     * Timed to simulate ~700 instructions per second.
     */
    const int CYCLES_PER_SECOND = 700;
    const int FRAME_RATE = 60;
    const int CYCLES_PER_FRAME = CYCLES_PER_SECOND / FRAME_RATE;

    Uint32 last_time = SDL_GetTicks();
    Uint32 accumulator = 0;

    while (!quit_requested) {
        Uint32 current_time = SDL_GetTicks();
        Uint32 delta = current_time - last_time;
        last_time = current_time;
        accumulator += delta;

        // Run cycles for each frame slice
        while (accumulator >= (1000 / FRAME_RATE)) {
            for (int i = 0; i < CYCLES_PER_FRAME; i++) {
                chip8_cycle(&chip8);
            }

            if (chip8.draw_flag) {
                update_display(&chip8);
                chip8.draw_flag = false;
            }

            accumulator -= (1000 / FRAME_RATE);
        }

        // Avoid maxing out CPU
        SDL_Delay(1);
    }

    display_quit();
    return EXIT_SUCCESS;
#else
    return EXIT_SUCCESS;
#endif
}
