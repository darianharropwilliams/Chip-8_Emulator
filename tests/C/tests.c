#include <stdio.h>
#include <stdbool.h>

// Forward declarations of your test suites
bool test_chip8_cycle();

int main() {
    int failed = 0;

    printf("Running CHIP-8 Emulator Tests...\n\n");

    failed += !test_chip8_cycle();

    printf("\n---------------------------------\n");
    if (failed) {
        printf("FAILED: %d test suites failed.\n", failed);
        return 1;
    } else {
        printf("SUCCESS: All tests passed!\n");
        return 0;
    }
}
