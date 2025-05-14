# CHIP-8 Emulator — Modular, Testable, Web-Compatible

This project is a **feature-complete CHIP-8 emulator** written in **pure C**, designed with **testability**, **modular dispatching**, and **browser deployment** in mind.

It goes beyond basic emulation to offer:

- A **clean VM core** with full `fetch-decode-execute` cycle management  
- A **multi-level opcode dispatch system** for readable instruction handling  
- **SDL2-based graphics/input** abstractions for native execution  
- **Test mode** with memory dumps, instruction tracing, and fixture support  
- A working **WebAssembly (WASM)** build with real-time canvas rendering and JS bindings  


---

## Project Layout
```
CHIP-8/
├── disasm/         # ROM disassembly reference
├── fixtures/       # Sample ROMs for correctness testing
├── include/        # Modular subsystem headers
├── platform/       # SDL2 and WASM platform backends for display, input, audio
├── roms/           # Full game ROMs for real-world testing
├── src/            # Core emulator logic
├── tests/          # C + Python testing suite (test ROMs, dumps, assertions)
└── Makefile        # One-line build setup

```
---

## Architecture Deep Dive

### VM Core (`chip8.h/.c`)

Encapsulates the full emulator state:
- 4KB memory, 16 8-bit registers (`V0`–`VF`)
- 16-level call stack and pointer
- 2 timers (`delay_timer`, `sound_timer`) ticking at 60Hz
- Monochrome 64×32 display buffer
- 16-key keypad
- Flags for redraw and test mode
- ROM path used for memory dumps in test mode

### Instruction Cycle

Every `chip8_cycle()` call performs:

1. **Fetch**: Read 2-byte opcode from `memory[pc]`  
2. **Decode**: Route opcode using multi-level dispatch tables  
3. **Execute**: Run associated handler (e.g., `op_7xkk`)  
4. **Timers**: Decrement delay/sound timers if > 0  
5. **Input**: Scan keyboard and update key states  
6. **Render**: Refresh display if `draw_flag` is set  

---

## Opcode Dispatch System

Implemented in `dispatch.c`, using **hierarchical function pointer tables** for readable, efficient decoding.

### Lookup Tables
- `main_table[0x10]`: High nibble (`0xXNNN`)
- `table_0[0x100]`: Special opcodes (`0x00E0`, `0x00EE`)
- `table_8[0x10]`: Bitwise/arithmetic (`8xy0`–`8xyE`)
- `table_E[0x100]`: Key skips (`Ex9E`, `ExA1`)
- `table_F[0x100]`: Timer, memory, input ops (`Fx07`–`Fx65`)

Each table routes to clearly named functions like `op_8xy4()` or `op_Fx1E()` for maintainability.

**All 35+ CHIP-8 opcodes** are implemented and tested.

---

## Display Module (`display.c`)

- Renders the CHIP-8 64×32 framebuffer by delegating to the active platform backend
- Uses **SDL2** in native builds or **JavaScript canvas** in WASM builds
- Each pixel is drawn using XOR logic with wraparound and VF collision detection
- Pixel scaling (default: `10x`) is applied in `platform_sdl.c` for modern screens
- Functions:
  - `draw_sprite()` — Core draw logic with collision return
  - `update_display()` — Triggers `platform_update_display()` for actual rendering
  - `clear_display()` — Clears framebuffer and marks for redraw

> Rendering is abstracted through `platform_update_display()`, which dynamically selects SDL2 or JS/WASM based on build target.


---

## Input Module (`input.c`)

- Maps input to the 16-key CHIP-8 keypad using the platform layer
- Uses `SDL_GetKeyboardState()` on native platforms or `Module.keyState[]` in WASM
- Supports:
  - `keypad_scan()` — Calls `platform_poll_input()` to update the key state array
  - `keypad_map()` — Sets individual key states manually (WASM-compatible)
  - `is_key_pressed()` — For conditional branching and the `Fx0A` blocking wait

> All input is routed through `platform_poll_input()`, which delegates to SDL2 or Emscripten depending on the platform.


```
Layout:
1 2 3 C      →    1 2 3 4
4 5 6 D      →    Q W E R
7 8 9 E      →    A S D F
A 0 B F      →    Z X C V
```


---

## Timer Module (`timer.c`)

- Two 8-bit timers (`delay_timer`, `sound_timer`)
- Updated at **60Hz**
- SDL sound output hook is stubbed (easily extensible)
- Exposed functions for `Fx07`, `Fx15`, `Fx18`, etc.

---

## Utility Module (`utils.c`)

- `load_rom()` — Loads ROM to `memory + 0x200`
- `memory_copy()` — Internal memcpy wrapper
- `swap_bytes()` — Bitwise mirror utility (for sprite mods)
- `print_registers()` — Debug CPU state
- `test_halt()` — Debug HALT triggered on RET in test mode
- `dump_memory()` — Dumps memory snapshot for ROM to `tests/python/dumps/*.bin`

---

## Testing & Debugging

**Test mode** (`--test`) runs the emulator deterministically:

- Executes a fixed number of frames at locked timing
- Dumps internal state and memory snapshot on exit
- Compatible with `tests/python/` harness:
  - `generate_test_roms.py` — Generates test ROMs
  - `test_chip8.py` — Loads dumps and asserts correctness
  - `fixtures/` — ROMs + disassemblies for visual inspection

> This enables true **ROM-level regression testing**, a rare feature in most CHIP-8 emulators.

---

## Engineering Highlights

Here’s why this emulator stands out from the crowd:

| Feature                    | Description                                                  |
|---------------------------|--------------------------------------------------------------|
| **Modular Opcode Dispatch**    | Multi-table architecture promotes clarity and scalability     |
| **Test Harness Integration**  | Python + C testing pipeline with fixture dumps                |
| **Debug Infrastructure**      | `DEBUG_PRINT()` macros gated by `test_mode`                   |
| **Separation of Concerns**    | Each subsystem — input, display, timing, memory — is isolated |
| **Cycle-accurate Emulation**  | Full support for instruction set and timer resolution         |
| **WASM-Ready**                | Platform-agnostic VM core, with `platform_wasm.c` support      |
| **Memory Safety Aware**       | Bounds checks on stack, memory, and I register usage          |
| **Real-time Feedback**        | Sprite collisions, timer warnings, and overflow logs built-in |

---

### ROM Attribution

The ROMs used in this project are part of a well-known set of public domain CHIP-8 programs historically used for emulator development and testing. These files were originally sourced from a GitHub repository that I can no longer identify due to cleared browser data.

If you recognize the original maintainer or repo, please reach out — I'd be happy to add proper credit and a link.

## Future Roadmap

### In Progress
- Fragment `opcodes.c` into `opcodes_0x.c`, `opcodes_8x.c`, etc.
- `memory_check()` centralized bounds enforcement
- Swap `fprintf()` with `assert()` where appropriate
- Add comprehensive instruction unit tests

---

## WebAssembly Support

This emulator runs natively **in the browser** via a working WebAssembly build, powered by Emscripten.

### Features
- Fully functional `platform_wasm.c` backend for display, input, and sound
- JavaScript bindings to `wasm_init()`, `wasm_cycle()`, and `wasm_load_rom()` via `wasm_bindings.c`
- Memory-safe ROM loading and rendering
- Clean canvas-based display via `Module.renderToCanvas(display)`
- Keyboard input mapped to the CHIP-8 keypad via `Module.keyState[]`
- Beep support stubbed via `Module.toggleBeep(true/false)`

### Integration Example
The WASM version is suitable for:
- Portfolio embedding
- In-browser ROM debugging
- Live demos and web-based regression test harnesses

To run:
1. Build with `emcc` using your Makefile or `.sh` script
2. Serve with any static web server
3. Load a ROM using JavaScript and call `wasm_load_rom()`, then `wasm_cycle()` per frame

---

## License

This project is currently unlicensed. Use the structure, code, or test infrastructure freely for **learning, hacking, or building your own emulators**.

---

## Author Notes

This project was built as a **systems-level showcase** — combining low-level memory manipulation, graphics handling, input polling, and CPU emulation — all while remaining modular, portable, and *fun*.

If you're a recruiter or engineer reviewing this, feel free to explore the opcode handlers, test framework, or platform abstractions. Feedback is welcome, and contributions are open to all who want to tinker with classic emulation.
