# CHIP-8 Emulator: Modular, Testable, Web-Compatible

A complete CHIP-8 emulator written in C — built for clarity, modularity, and cross-platform performance.

- Clean architecture with subsystem separation
- Full instruction set and accurate timing
- Native SDL2 and WebAssembly (browser) support
- Built-in test mode and memory state dumping
- Easily extendable, hackable, and inspectable

---

## Quick Start

### Native (SDL2)

Requires SDL2 development libraries.

```bash
make
./chip8 path/to/rom.ch8
```

On Windows: set the `SDL2_PATH` in the `Makefile` to your SDL2 install directory:

```bash
SDL2_PATH=C:/path/to/SDL2 make
```

### WebAssembly (Emscripten)

Requires [Emscripten](https://emscripten.org/):

```bash
cd platform/wasm
make
python3 -m http.server
# Visit http://localhost:8000 in your browser
```

---

## Test Mode (Deterministic Debugging)

Use `--test` to run in cycle-accurate mode and generate memory dumps:

```bash
./chip8 fixtures/add_vx.rom --test
```

Used to validate execution and automate test coverage via Python scripts.

---

## Project Structure

```
CHIP-8/
├── include/       # Public headers (API and constants)
├── src/           # Core CHIP-8 logic (VM, dispatch, opcodes, etc.)
├── platform/
│   ├── sdl/       # SDL2 rendering/audio/input backend
│   └── wasm/      # Emscripten bindings and JS platform glue
├── tests/         # C and Python test harnesses
├── fixtures/      # Mini test ROMs for validation
├── roms/          # Public domain CHIP-8 games
├── docs/          # Internal developer documentation
├── Makefile       # Native (SDL2) build
└── README.md      # You're here
```

---

## Features

| Feature                | Description |
|------------------------|-------------|
| Full Opcode Support | Implements all 35+ CHIP-8 instructions |
| Dispatch Architecture | Nested tables for opcode decoding |
| Pixel Display        | 64x32 framebuffer via SDL2 or JS Canvas |
| Sound Support       | Sound timer triggers buzzer via platform audio |
|  Key Input           | Platform-independent 16-key input |
| Test Mode           | Dumps memory/register state for test ROMs |
| Web Support         | Runs in-browser via WebAssembly |
| Memory Safety       | Bounds-checked stack and memory operations |

---

## Build System Overview

### Native (SDL2)

- Requires SDL2 headers and libraries
- `SDL2_PATH` should point to your local SDL2 install (used in `Makefile`)
- Uses `gcc` and `make`

### Web (WASM)

- Requires Emscripten (`emcc`) in `PATH`
- Builds to `.js` + `.wasm` via `make` in `platform/wasm/`
- Outputs JS module `Chip8Emulator` with exported methods:
  - `wasm_init()`
  - `wasm_cycle(N)`
  - `wasm_load_rom(ptr, size)`

---

## Technical Documentation

See the `docs/` folder for detailed breakdowns:

- `docs/chip8.md`: Virtual machine design, memory map
- `docs/opcodes.md`: Instruction set and decoding rules
- `docs/dispatch.md`: Hierarchical opcode routing
- `docs/display.md`: Framebuffer and rendering flow
- `docs/input.md`: Key mapping and polling abstraction
- `docs/timer.md`: 60Hz timers and audio integration
- `docs/platform.md`: SDL2 and WASM runtime differences
- `docs/testing.md`: Test harness, dumps, and validation tools

---

## ROM Licensing

ROMs in `/roms` and `/fixtures` are sourced from public domain collections for testing and education. If attribution is needed for a specific file, feel free to open an issue or PR.

---

## Roadmap

- Opcode unit tests by category
- Split opcodes into grouped files
- Test-driven input regression coverage
- Drop-in drag-and-run UI for web ROMs
- Real-time debug overlays (FPS, CPU, etc.)

---

## Credits

This emulator is a learning-oriented system-level project exploring:

- Low-level memory and stack architecture
- Bitwise operations and instruction decoding
- Platform rendering and input handling
- Deterministic testing and validation

Built for students, tinkerers, systems engineers, and anyone exploring emulator internals.

Contributions, questions, and improvements welcome!
