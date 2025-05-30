# Delegating to an Output Platform

## Platform Abstraction Layer

The platform module provides an abstraction over hardware-specific operations such as rendering, input polling, and audio playback. This allows the emulator core to remain platform-independent while supporting both native (SDL2) and WebAssembly (via JavaScript) targets.

All platform interaction is routed through the interface declared in `platform.h`.

---

## Header: `platform.h`

### Interface

```c
void platform_init(void);
void platform_update_display(const uint8_t *pixels);
void platform_poll_input(uint8_t *keypad);
void platform_play_beep(bool active);
void platform_quit(void);
```

These functions must be implemented per platform.

---

## Native: `platform_sdl.c`

### Initialization

```c
void platform_init(void)
```

- Initializes SDL video and audio subsystems
- Creates a window and hardware-accelerated renderer
- Sets up an audio callback for square wave playback

### Display Rendering

```c
void platform_update_display(const uint8_t *pixels)
```

- Clears the screen
- Draws each `1` bit in the framebuffer as a white square (scaled 10x)
- Uses `SDL_RenderFillRect` to draw pixel-sized rectangles
- Presents the rendered frame with `SDL_RenderPresent`

### Input Polling

```c
void platform_poll_input(uint8_t *keypad)
```

- Uses `SDL_GetKeyboardState` to read key states
- Maps physical keys to CHIP-8 keypad:

  ```
  CHIP-8     SDL Scancode
  -------     ------------
  1 2 3 C     1 2 3 4
  4 5 6 D     Q W E R
  7 8 9 E     A S D F
  A 0 B F     Z X C V
  ```

- Fills the `keypad[16]` array with 0 or 1 values

### Audio

```c
void platform_play_beep(bool active)
```

- Starts or stops SDL audio playback
- Generates a 440Hz square wave using an `audio_callback`
- Audio is lazily initialized if needed
- **Note**: To improve compatibility on some Windows systems (especially when using antivirus like Norton), the platform layer explicitly enforces the SDL audio driver to use `directsound` by calling:

  ```c
  SDL_setenv("SDL_AUDIODRIVER", "directsound", 1);
  ```

  This avoids initialization failures and silent audio issues caused by problematic WASAPI configurations.


### Shutdown

```c
void platform_quit(void)
```

- Destroys the SDL renderer and window
- Closes audio device
- Calls `SDL_Quit`

---

## WebAssembly: `platform_wasm.c`

### Initialization

```c
void platform_init(void)
```

- Stub function (no-op)
- Assumes HTML and JS handle canvas and input setup

### Display Rendering

```c
void platform_update_display(const uint8_t *pixels)
```

- Calls into JavaScript via `Module.renderToCanvas()`
- Passes the framebuffer (64x32) from WASM memory to the JS side

### Input Polling

```c
void platform_poll_input(uint8_t *keypad)
```

- Calls into JS via `Module.keyState[]`
- JS sets each of the 16 keypad entries to 0 or 1

### Audio

```c
void platform_play_beep(bool active)
```

- Calls `Module.toggleBeep(true or false)` in JavaScript
- Stub function unless JS defines this method

### Shutdown

```c
void platform_quit(void)
```

- Stub function (no resources to release)

---

## Notes

- The display and input modules are decoupled from the platform by calling only this API
- This allows the same core emulator code to run on desktop and in browsers
- Additional platforms (e.g., terminal or embedded) could implement the same API

---
