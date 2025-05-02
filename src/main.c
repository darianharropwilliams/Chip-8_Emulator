#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h> // for write()
#include <string.h>
#include <libgen.h>
#include "chip8.h"
#include "display.h"
#include "utils.h"

Chip8 chip8;

// Global shutdown flag
volatile sig_atomic_t quit_requested = 0;

// Async-signal-safe handler
void handle_signal(int signal) {
    const char msg[] = "\nCaught signal. Exiting cleanly...\n";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
    quit_requested = 1;
}

int SDL_main(int argc, char *argv[]) {
    bool test_mode = false;

    if (argc == 3 && strcmp(argv[2], "--test") == 0) {
        test_mode = true;
    } else if (argc != 2) {
        fprintf(stderr, "Usage: %s <ROM file> [--test]\n", argv[0]);
        return EXIT_FAILURE;
    }

    chip8_init(&chip8);

    if (argc >= 3 && strcmp(argv[2], "--test") == 0) {
        chip8.test_mode = true;
    }

    // 💡 Store rom path for test dump from op_00EE
    strncpy(chip8.rom_path, argv[1], sizeof(chip8.rom_path) - 1);
    chip8.rom_path[sizeof(chip8.rom_path) - 1] = '\0';

    if (chip8_load_rom(&chip8, argv[1])) {
        fprintf(stderr, "Failed to load ROM: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    strncpy(chip8.rom_path, argv[1], sizeof(chip8.rom_path) - 1);
    // printf("[DEBUG] Memory at 0x20A = 0x%02X%02X\n",
    //     chip8.memory[0x20A], chip8.memory[0x20B]); 
    if (signal(SIGINT, handle_signal) == SIG_ERR) {
        fprintf(stderr, "Failed to register SIGINT handler\n");
        return EXIT_FAILURE;
    }

    if (test_mode) {
        // Slow, consistent cycle count for test mode
        const int CYCLES_PER_FRAME = 60;
        const int FRAME_DELAY_MS = 1000 / 60;
        const int TEST_FRAMES = 10;

        for (int frame = 0; frame < TEST_FRAMES && !quit_requested; frame++) {
            clock_t start = clock();

            for (int i = 0; i < CYCLES_PER_FRAME; i++) {
                chip8_cycle(&chip8);
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
        }

        display_quit();
        return EXIT_SUCCESS;
    }

    // Normal execution mode — fast loop
    while (!quit_requested) {
        for (int i = 0; i < 100; i++) {
            chip8_cycle(&chip8);
        }
    }

    display_quit();
    return EXIT_SUCCESS;
}
