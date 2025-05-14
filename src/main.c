#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>  // for basename()

#include "chip8.h"
#include "utils.h"
#include "platform.h"
#include "display.h"

// Global CHIP-8 system instance
Chip8 chip8;

// Flag for graceful shutdown triggered by SIGINT (Ctrl+C)
volatile sig_atomic_t quit_requested = 0;

/**
 * Signal handler for SIGINT (Ctrl+C).
 * Sets the quit_requested flag and writes a message to stderr.
 */
void handle_signal(int signal) {
    const char msg[] = "\nCaught signal. Exiting cleanly...\n";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);  // Safe in signal context
    quit_requested = 1;
}

#ifdef __EMSCRIPTEN__
/**
 * Entry point for WebAssembly build.
 * Emscripten uses a standard `main` function.
 */
int main(int argc, char *argv[])
#else
/**
 * Entry point for native SDL builds (SDL hijacks `main()`).
 */
int SDL_main(int argc, char *argv[])
#endif
{
    bool test_mode = false;

    // Parse command line args
    // Expected usage: ./emulator <ROM file> [--test]
    if (argc == 3 && strcmp(argv[2], "--test") == 0) {
        test_mode = true;
    } else if (argc != 2) {
        fprintf(stderr, "Usage: %s <ROM file> [--test]\n", argv[0]);
        return EXIT_FAILURE;
    }

    chip8_init(&chip8);

    // Enable test mode inside the CHIP-8 state
    if (argc >= 3 && strcmp(argv[2], "--test") == 0) {
        chip8.test_mode = true;
    }

    // Save ROM path in case test features need to reference it later (e.g., memory dump)
    strncpy(chip8.rom_path, argv[1], sizeof(chip8.rom_path) - 1);
    chip8.rom_path[sizeof(chip8.rom_path) - 1] = '\0';  // Ensure null termination

    // Load the ROM into memory at 0x200
    if (chip8_load_rom(&chip8, argv[1])) {
        fprintf(stderr, "Failed to load ROM: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    // Register SIGINT handler (Ctrl+C)
    if (signal(SIGINT, handle_signal) == SIG_ERR) {
        fprintf(stderr, "Failed to register SIGINT handler\n");
        return EXIT_FAILURE;
    }

    // If in test mode, run a fixed number of frames at a stable rate
    if (test_mode) {
        const int CYCLES_PER_FRAME = 60;     // Execute 60 CHIP-8 cycles per frame
        const int FRAME_DELAY_MS = 1000 / 60; // Target 60 FPS
        const int TEST_FRAMES = 10;           // Total number of test frames to run

        for (int frame = 0; frame < TEST_FRAMES && !quit_requested; frame++) {
            clock_t start = clock();

            for (int i = 0; i < CYCLES_PER_FRAME; i++) {
                chip8_cycle(&chip8);
            }

            // Wait to maintain stable frame rate
            clock_t end = clock();
            int elapsed_ms = (int)((end - start) * 1000 / CLOCKS_PER_SEC);

            if (elapsed_ms < FRAME_DELAY_MS) {
                struct timespec ts = {
                    .tv_sec = 0,
                    .tv_nsec = (FRAME_DELAY_MS - elapsed_ms) * 1000000
                };
                nanosleep(&ts, NULL);
            }
        }

        display_quit();
        return EXIT_SUCCESS;
    }

    // Normal execution mode — continuous, fast loop
    while (!quit_requested) {
        for (int i = 0; i < 100; i++) {
            chip8_cycle(&chip8);  // Run 100 cycles before checking for exit
        }
    }

    display_quit();  // Clean shutdown of SDL window and display
    return EXIT_SUCCESS;
}
