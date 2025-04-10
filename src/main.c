#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "chip8.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ROM file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Initialize CHIP-8 system
    Chip8 chip8;
    chip8_init(&chip8);

    // Load ROM
    if (chip8_load_rom(&chip8, argv[1])) { // Function returns 0 (false) if successful
        fprintf(stderr, "Failed to load ROM: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    // Optional: set up timing (for emulating ~60Hz)
    const int CYCLES_PER_FRAME = 10; // can be tuned
    const int TARGET_FPS = 60;
    const int FRAME_DELAY_MS = 1000 / TARGET_FPS;

    while (1) {
        clock_t start = clock();

        // Run designated cycles per frame
        for (int i = 0; i < CYCLES_PER_FRAME; i++) {
            chip8_cycle(&chip8); // Errors handled inside chip8_cycle()
        }

        // Optional: add delay to simulate 60Hz
        clock_t end = clock();
        int elapsed_ms = (int)((end - start) * 1000 / CLOCKS_PER_SEC); // Calculate time spent
        if (elapsed_ms < FRAME_DELAY_MS) {
            // Sleep the remaining time (not precise)
            struct timespec ts = {
                .tv_sec = 0,
                .tv_nsec = (FRAME_DELAY_MS - elapsed_ms) * 1000000
            };
            nanosleep(&ts, NULL); // Maintain consistent FPS
        }

        // Optional: print registers for debugging
        // print_registers(chip8.V, chip8.I, chip8.pc, chip8.delay_timer, chip8.sound_timer);
    }

    return EXIT_SUCCESS;
}
