#include "platform.h"
#include <emscripten.h>
#include <stdio.h>

/**
 * JavaScript binding to render the CHIP-8 display buffer to a canvas.
 *
 * @param pixels Pointer to a 64x32 uint8_t buffer in WASM memory.
 *               Each pixel should be either 0 (off) or 1 (on).
 */
EM_JS(void, js_update_display, (const uint8_t *pixels), {
  const display = new Uint8Array(Module.HEAPU8.buffer, pixels, 64 * 32);
  Module.renderToCanvas(display);
});

/**
 * JavaScript binding to read the current key state from the browser.
 *
 * @param keypad Pointer to a 16-byte array to receive key states (1 = pressed).
 */
EM_JS(void, js_poll_input, (uint8_t *keypad), {
  for (let i = 0; i < 16; i++) {
    Module.HEAPU8[keypad + i] = Module.keyState[i];
  }
});

/**
 * JavaScript binding to enable or disable sound.
 *
 * @param active Whether the tone should be playing.
 */
EM_JS(void, js_beep, (bool active), {
  if (typeof Module.toggleBeep === "function") {
    Module.toggleBeep(active);
  }
});

/**
 * Platform initialization stub for WebAssembly.
 * No-op because the browser handles canvas and input setup.
 */
void platform_init(void) {}

/**
 * Render the CHIP-8 display to the browser using the JavaScript binding.
 */
void platform_update_display(const uint8_t *pixels) {
  js_update_display(pixels);
}

/**
 * Poll the current key state and update the CHIP-8 keypad array.
 */
void platform_poll_input(uint8_t *keypad) {
  js_poll_input(keypad);
}

/**
 * Play or stop the sound using the browser's audio system.
 */
void platform_play_beep(bool active) {
  js_beep(active);
}

/**
 * Platform cleanup stub for WebAssembly.
 * No resources to release in this implementation.
 */
void platform_quit(void) {}
