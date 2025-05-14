#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#include <stdbool.h>

// Initialize SDL and related platform components
void platform_init(void);

// Render the framebuffer to the window
void platform_update_display(const uint8_t *pixels);  // 64x32 resolution

// Poll for input events and update the keypad
void platform_poll_input(uint8_t *keypad);

// Play a beep sound while sound_timer > 0
void platform_play_beep(bool active);

// Shut down and clean up platform-related resources
void platform_quit(void);

#endif
