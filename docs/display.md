# Outputting to the Display

## Display Subsystem

The display module handles the CHIP-8 64x32 monochrome framebuffer, including pixel manipulation, sprite rendering, screen clearing, and platform-agnostic drawing. It acts as the visual interface of the emulator and abstracts the rendering backend via platform hooks.

---

## Header: `display.h`

### API

```c
void display_init(Chip8 *chip8);
int  draw_sprite(Chip8 *chip8, uint8_t x, uint8_t y, uint8_t height, const uint8_t *sprite);
void clear_display(Chip8 *chip8);
void update_display(Chip8 *chip8);
void display_quit(void);
```

- `display_init`: Clears the framebuffer and initializes the rendering backend
- `draw_sprite`: Draws a sprite from memory to the display and reports collisions
- `clear_display`: Clears the framebuffer and sets the draw flag
- `update_display`: Renders the framebuffer if `draw_flag` is set
- `display_quit`: Shuts down rendering resources (SDL or WASM)

---

## Implementation: `display.c`

### `display_init`

- Clears `chip8->display` to 0
- Sets `chip8->draw_flag = true`
- Calls `platform_init()` to initialize SDL or WASM rendering

### `clear_display`

- Zeros out the framebuffer
- Sets `draw_flag` to true so the next cycle triggers a redraw

Used by opcode `00E0` (CLS).

### `draw_sprite`

```c
int draw_sprite(Chip8 *chip8, uint8_t x, uint8_t y, uint8_t height, const uint8_t *sprite)
```

- Renders an `N`-byte tall sprite at position `(x, y)`
- Each sprite byte encodes 8 horizontal pixels (most significant bit is leftmost)
- Pixels are XORed onto the screen (as per CHIP-8 spec)
- If any pixel changes from 1 to 0 (collision), sets VF to 1 and returns 1
- Handles screen wrap-around using modulo math

Used by opcode `DXYN`.

### `update_display`

- Calls `platform_update_display(chip8->display)`
- Resets `draw_flag` in the main cycle once rendering is complete

### `display_quit`

- Delegates to `platform_quit()` to release SDL or WASM display resources

---

## Framebuffer

```c
uint8_t display[64 * 32];
```

- Linear buffer, row-major order
- Each element is 0 (off) or 1 (on)
- The `draw_sprite` function operates directly on this array
- Backend rendering (SDL or WASM) maps these values to screen pixels

---

## Platform Abstraction

Rendering is delegated to the platform layer:

```c
void platform_update_display(const uint8_t *pixels);
```

- SDL implementation draws each pixel as a filled rectangle
- WASM implementation passes buffer to JavaScript which draws to canvas

The display module is agnostic to whether it's running on native or web.

---

## Collision Detection

- Collisions occur when drawing a sprite causes any pixel to change from 1 to 0
- `draw_sprite` returns `1` if a collision occurred, `0` otherwise
- This result is stored in register VF by the opcode handler (`Dxyn`)

---

## Notes

- All drawing logic is pure C and platform-independent
- Only the `update_display` and `display_quit` functions rely on platform hooks
- `draw_flag` is used to minimize unnecessary redraws and is managed by the emulator cycle

---
