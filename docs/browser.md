# Browser Support

## WebAssembly and Browser Integration

This document describes how the CHIP-8 emulator is integrated into a browser environment using **Emscripten**, **WebAssembly**, and a JavaScript frontend. It includes rendering, audio, ROM loading, keyboard input, and frame pacing.

The implementation spans four key files:

- `platform_wasm.c`: WASM-native platform interface
- `wasm_bindings.c`: C-to-JS glue using `EMSCRIPTEN_KEEPALIVE`
- `index.html`: UI template with canvas and file input
- `index.js`: JavaScript frontend for audio, input, rendering, and control flow

---

## Overview

The browser emulator mimics the native structure but replaces:

- SDL with JavaScript Canvas API
- SDL audio with Web Audio API
- Keyboard polling with DOM event listeners
- ROM loading with file picker or HTTP

---

## Web Entry Point: `index.html`

This HTML file defines:

- A sidebar of loadable ROMs (`#rom-links`)
- A `canvas` for rendering the display (scaled to 640x320)
- An `<input type="file">` for loading local ROMs
- Script includes for Emscripten-generated `chip8.js` and custom `index.js`

Key element IDs:
- `#screen`: Target for canvas rendering
- `#rom-picker`: File input element
- `#rom-links`: Button list populated from `roms/index.json`

---

## JavaScript Controller: `index.js`

### Responsibilities

- Manages canvas rendering (`renderToCanvas`)
- Tracks keyboard state in `Module.keyState`
- Implements `Module.toggleBeep` using the Web Audio API
- Loads ROMs from local file picker or from HTTP (`roms/`)
- Coordinates WebAssembly exports: `wasm_init`, `wasm_cycle`, `wasm_load_rom`
- Runs a frame-paced loop (targeting ~700Hz execution rate)

### Key Structures

#### Audio

```js
function toggleBeep(active)
```

- Starts/stops a 440Hz square wave oscillator using Web Audio API
- Called from WASM via `platform_play_beep`

#### Key Mapping

```js
Module.keyState = new Array(16).fill(0);
```

- Mapped via DOM events (`keydown`/`keyup`)
- CHIP-8 keys are mapped to physical keys (e.g., `1` → `0x1`, `x` → `0x0`)

#### Canvas Rendering

```js
Module.renderToCanvas = function(pixels) { ... }
```

- Clears the screen, draws white squares for each set pixel
- Expects a 64x32 byte buffer from WASM

#### ROM Loading

- Supports:
  - File picker (`<input type="file">`)
  - HTTP fetch via `fetchROM`
  - Dynamic button list via `roms/index.json`

#### Emulation Loop

```js
function runLoop(now) { ... }
```

- Uses `requestAnimationFrame` for synchronization
- Runs enough `wasm_cycle(1)` calls to match target frequency (700Hz)

---

## WASM Bindings: `wasm_bindings.c`

### Public Exports (JS-visible)

```c
EMSCRIPTEN_KEEPALIVE void wasm_init(void);
EMSCRIPTEN_KEEPALIVE void wasm_cycle(int cycles);
EMSCRIPTEN_KEEPALIVE int  wasm_load_rom(uint8_t *data, int size);
```

- `wasm_init`: Initializes CHIP-8 state
- `wasm_cycle`: Executes N cycles (called every animation frame)
- `wasm_load_rom`: Loads ROM from JS memory into VM (at 0x200)

These are registered with Emscripten and callable via `Module.ccall`.

---

## Platform Glue: `platform_wasm.c`

This file implements the required platform API:

```c
void platform_update_display(const uint8_t *pixels);
void platform_poll_input(uint8_t *keypad);
void platform_play_beep(bool active);
```

#### `platform_update_display`

- Calls `js_update_display(pixels)` which invokes `Module.renderToCanvas()`

#### `platform_poll_input`

- Fills the CHIP-8 `keypad[16]` from `Module.keyState[]` in JS

#### `platform_play_beep`

- Calls `Module.toggleBeep(true or false)` in JS

#### `platform_init` and `platform_quit`

- Both are stubs — browser setup/cleanup is handled externally

---

## Building and Running

### Requirements

- Emscripten installed (`emcc`)
- Static web server to serve the generated files (e.g., Python's `http.server`)

### Build Example

```sh
cd platform/wasm
make
python3 -m http.server
```

Then open [http://localhost:8000](http://localhost:8000) in your browser.

---

## ROM Hosting

- ROMs are stored in `platform/wasm/roms/`
- `index.json` must list available ROM filenames
- Used by `populateROMButtons()` to dynamically create UI buttons

---

## Notes

- All browser interaction (canvas, audio, input) is mediated through `Module` bindings
- WASM codebase shares the exact same CHIP-8 core as native builds
- Deterministic cycle execution supports visual debugging and interactive demos

---
