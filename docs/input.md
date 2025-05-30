# Keyboard Input

## Input Subsystem

The input module handles the CHIP-8 16-key hexadecimal keypad. It provides functions for polling physical input, setting key states programmatically, and checking for pressed keys. All platform-specific input logic is delegated to the platform layer.

---

## Header: `input.h`

### API

```c
void keypad_init(Chip8 *chip8);
void keypad_scan(Chip8 *chip8);
void keypad_map(Chip8 *chip8, uint8_t key, bool state);
bool is_key_pressed(Chip8 *chip8, uint8_t key);
```

- `keypad_init`: Resets all key states to unpressed (0)
- `keypad_scan`: Polls current platform key state and updates `chip8->keypad`
- `keypad_map`: Sets a specific key to pressed or released manually
- `is_key_pressed`: Checks if a specific key is currently down

---

## Implementation: `input.c`

### `keypad_init`

```c
void keypad_init(Chip8 *chip8)
```

- Uses `memset` to zero the `chip8->keypad` array
- Marks all keys as unpressed
- Called during `chip8_init`

### `keypad_scan`

```c
void keypad_scan(Chip8 *chip8)
```

- Calls `platform_poll_input(chip8->keypad)`
- This function is platform-agnostic; SDL or WASM handles the actual keyboard polling
- Should be called once per emulation cycle (done inside `chip8_cycle`)

### `keypad_map`

```c
void keypad_map(Chip8 *chip8, uint8_t key, bool state)
```

- Manually sets the pressed state of a key
- Safe for programmatic key injection (used in WebAssembly bindings)
- Validates that key index is within `0x0` to `0xF`

### `is_key_pressed`

```c
bool is_key_pressed(Chip8 *chip8, uint8_t key)
```

- Returns `true` if the given key index is currently pressed
- Used for implementing opcodes `EX9E` and `EXA1`

---

## Keypad Layout

The CHIP-8 keypad is a 4x4 hex-based layout:

```
1 2 3 C    →    1 2 3 4
4 5 6 D    →    Q W E R
7 8 9 E    →    A S D F
A 0 B F    →    Z X C V
```

Mapped using SDL scancodes or JavaScript arrays, depending on platform.

---

## Platform Integration

Polling and event handling are delegated to:

```c
void platform_poll_input(uint8_t *keypad);
```

- In `platform_sdl.c`: Uses `SDL_GetKeyboardState` to update keymap
- In `platform_wasm.c`: Uses `Module.keyState[]` (shared with JavaScript)

The input module never directly depends on platform-specific headers.

---

## Opcode Usage

Input functions support the following opcodes:

- `EX9E`: Skip next instruction if key in Vx is pressed
- `EXA1`: Skip next instruction if key in Vx is not pressed
- `FX0A`: Wait for key press, then store the key index in Vx

---

## Notes

- Key states are updated every cycle to ensure responsive input
- The input module is safe to use in both native and browser builds
- `keypad_map` enables input mocking and external input systems

---
