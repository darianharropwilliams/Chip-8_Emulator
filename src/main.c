#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h> // for write()
#include "chip8.h"
#include "display.h"

Chip8 chip8;

// Global shutdown flag
volatile sig_atomic_t quit_requested = 0;

// Async-signal-safe handler (only sets flag and writes raw message)
void handle_signal(int signal) {
    const char msg[] = "\nCaught signal. Exiting cleanly...\n";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
    quit_requested = 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ROM file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Initialize CHIP-8 state
    chip8_init(&chip8);

    // Load the ROM file into memory
    if (chip8_load_rom(&chip8, argv[1])) {
        fprintf(stderr, "Failed to load ROM: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    // Timing settings
    const int CYCLES_PER_FRAME = 10;
    const int TARGET_FPS = 60;
    const int FRAME_DELAY_MS = 1000 / TARGET_FPS;

    // Register clean shutdown handler for Ctrl+C
    if (signal(SIGINT, handle_signal) == SIG_ERR) {
        fprintf(stderr, "Failed to register SIGINT handler\n");
        return EXIT_FAILURE;
    }

    // Main emulation loop
    while (!quit_requested) {
        clock_t start = clock();

        for (int i = 0; i < CYCLES_PER_FRAME; i++) {
            chip8_cycle(&chip8); // Core emulation logic
        }

        clock_t end = clock();
        int elapsed_ms = (int)((end - start) * 1000 / CLOCKS_PER_SEC);

        if (elapsed_ms < FRAME_DELAY_MS) {
            struct timespec ts = {
                .tv_sec = 0,
                .tv_nsec = (FRAME_DELAY_MS - elapsed_ms) * 1000000
            };
            nanosleep(&ts, NULL);
        }

        // Optional: debug state dump
        // print_registers(chip8.V, chip8.I, chip8.pc, chip8.delay_timer, chip8.sound_timer);
    }

    // Safe cleanup after breaking out of the loop
    display_quit();
    return EXIT_SUCCESS;
}
