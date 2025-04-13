# CHIP-8 Emulator

This project is a **modular, extensible CHIP-8 emulator written in C**, built with clarity, testability, and eventual web deployment in mind. It features:

- A dynamic, table-driven opcode dispatch system
- Isolated subsystems for display, input, timers, and utility logic
- A clean VM core that orchestrates fetch-decode-execute cycles
- Readability and maintainability as core design principles

---

## Project Structure

```
├── include/
│   ├── chip8.h         # Core emulator state and control
│   ├── dispatch.h      # Opcode dispatch mechanism (main + subtables)
│   ├── opcodes.h       # Opcode handler declarations
│   ├── display.h       # SDL2-based 64x32 monochrome graphics
│   ├── input.h         # Keypad mapping (16-key hex)
│   ├── timer.h         # Delay and sound timer interface
│   └── utils.h         # ROM loading, memory utils, debug helpers
│
├── src/
│   ├── chip8.c         # Core VM: init, ROM load, emulation cycle
│   ├── dispatch.c      # Opcode decoding + dispatch table wiring
│   ├── opcodes.c       # Full implementation of 35+ CHIP-8 opcodes
│   ├── display.c       # SDL2 display with XOR sprite rendering
│   ├── input.c         # SDL2 key scanning and key state mapping
│   ├── timer.c         # 60Hz decrement logic for delay/sound
│   ├── utils.c         # ROM file I/O, memory copy, debugging
│   └── main.c          # CLI entry point, emulation loop, timing
```

---

## Architecture Overview

### `Chip8` VM Core
Defined in `chip8.h`, the VM struct contains:
- **Memory** (4K bytes)
- **Registers**: `V0`–`VF`, index register `I`, and program counter `PC`
- **Stack**: 16-level call stack + stack pointer
- **Timers**: 8-bit `delay_timer`, `sound_timer` (decrement at 60Hz)
- **Display**: Monochrome 64×32 screen (`display[]`)
- **Keypad**: 16-key input state
- **Flags**: `draw_flag` signals when screen needs redrawing

### Emulation Cycle (`chip8_cycle`)
Called every frame; performs:
1. Fetch 16-bit opcode from memory
2. Advance `PC` by 2
3. Decode and dispatch instruction
4. Update timers
5. Scan keypad state
6. Redraw display (if `draw_flag` is set)

---

## Opcode Dispatch System

### `dispatch.c` + `opcodes.c`
A hierarchical dispatch system routes instructions efficiently:

- **`main_table[16]`**: Dispatch by high nibble (`0xXNNN`)
- **Subtables**:
  - `table_0[256]`: `0x00E0`, `0x00EE`
  - `table_8[16]`: `8xy0`–`8xyE` (bitwise, arithmetic)
  - `table_E[256]`: Key skips `Ex9E`, `ExA1`
  - `table_F[256]`: Timers, input, memory (`Fx07`–`Fx65`)

Subgroups like `op_8xxx()` and `op_Fxxx()` perform nested decoding for maximum clarity and modularity.

---

## Display Module

- Renders 64×32 display buffer to an SDL2 window
- Each pixel scaled (10x default)
- XOR-based sprite drawing
- Collision detection sets `VF` register
- Supports display clear and wraparound rendering

---

## Input Module

- Polls host keyboard using SDL
- Maps host keys to CHIP-8 16-key layout
- Supports polling or manual state mapping
- `is_key_pressed()` used by opcodes for conditionals

---

##  Timer Module

- `delay_timer` and `sound_timer` initialized to 0
- Decremented at 60Hz each cycle
- Functions to get/set both timers
- Future hook for integrating audio output when `sound_timer > 0`

---

## Utility Module

- `load_rom()`: Reads ROM from file into memory at `0x200`
- `memory_copy()`: Wraps `memcpy()` for VM-internal transfers
- `swap_bytes()`: Byte mirror helper for potential sprite features
- `print_registers()`: Debugging tool for CPU state

---

## Current Status: Core Emulation Complete

### Implemented:
- All **35 documented CHIP-8 instructions** with overflow, bounds, and stack error checks
- Hierarchical **opcode dispatch** via lookup tables
- Display and input logic (via SDL2)
- 60Hz **timer decrement logic**
- ROM loading and cycle management

### In Progress / Upcoming:
- **Fragment `opcodes.c`** into multiple files (e.g., `opcodes_0x.c`, `opcodes_8x.c`, etc.)
- **Implement `memory_check()`** for centralized bounds checking
- Replace debug `fprintf()` with `assert()` and testable flags
- Add **unit tests** for instruction correctness
- Optional: track opcode usage for profiling/debug

---

## Future Plans: WebAssembly Support

The project is being written with eventual browser deployment in mind.

### Key Considerations:
- SDL2 will be replaced or abstracted (e.g., into `platform_sdl.c` vs `platform_wasm.c`)
- `fprintf(stderr, ...)` will be routed through `log_error()` for WebAssembly-safe output
- Fontset and ROM memory regions may be **marked as readonly** in strict/debug modes
- The core VM (`chip8.c`) will remain unchanged between platforms

### WASM Targets:
- Run the emulator in-browser via Emscripten
- Expose `chip8_cycle()` and `chip8_load_rom()` to JS bindings
- Embed on personal site for public demos

---

## Testing Plans

- Instruction-by-instruction test cases (e.g., test `op_7xkk` with known registers)
- VM snapshot validation (memory, display, registers after cycle)
- Debug toggle to log execution trace

---

## License

This project is currently unlicensed. Please feel free to adapt or use the structure and architecture for your own learning, emulator projects, or educational purposes.

---
