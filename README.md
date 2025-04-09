# CHIP-8 Emulator

This project is a modular and extensible CHIP-8 emulator written in C, built with a focus on clean separation of responsibilities. It features a dynamic opcode dispatch system, modular subsystems for input, display, and timers, and a core emulation loop driven by a centralized `chip8.c` interface.

## Project Structure

```
├── chip8.h         # Core emulator state and high-level control
├── dispatch.h      # Opcode dispatch mechanism (main + subtables)
├── opcodes.h       # Opcode handler function declarations
├── display.h       # Monochrome display emulation (64x32)
├── input.h         # Keypad handling (16-key HEX)
├── timer.h         # Delay and sound timers (60Hz decrement)
├── utils.h         # ROM loading, memory utilities, debugging
```

## Architecture Overview

### Core: `chip8.h`
Defines the `Chip8` struct representing the full state of the virtual machine, including:

- Memory (`4K`)
- General purpose registers (`V0`–`VF`)
- Index register `I`
- Program counter `pc`
- Stack and stack pointer
- Timers: `delay_timer` and `sound_timer`
- Display buffer
- Keypad state array
- A `draw_flag` to indicate screen updates

### Execution Cycle: `chip8_cycle`
Fetches, decodes, and dispatches opcodes using the dispatch table defined in `dispatch.h`.

---

## Dispatch Table: `dispatch.h` + `opcodes.h`

### `dispatch_opcode`
Uses a function pointer table (`OpcodeHandler`) to quickly decode and route opcodes to the correct handler function based on the high nibble (e.g., `0x1NNN`, `0x6XKK`).

### Subtables
Special cases (e.g., `0x00E0`, `0x8XYE`, `0xEX9E`, `0xFX65`) are further delegated to sub-dispatch functions within handlers like `op_0xxx`, `op_8xxx`, etc.

### Benefits
- Fast opcode routing with minimal branching
- Easy to extend or debug
- Clean separation between opcode categories

---

## Display: `display.h`
- Emulates a 64x32 monochrome display.
- Sprites are drawn 8 bits wide and up to 15 pixels tall.
- Handles XOR drawing and collision detection.
- Includes a `draw_flag` to signal the need for redraw.

---

## Input: `input.h`
- Handles a 16-key hexadecimal keypad.
- Scanning, key mapping, and state queries are encapsulated.
- Used to implement instructions like `EX9E` and `FX0A`.

---

## Timers: `timer.h`
- `delay_timer` and `sound_timer` decrement at 60Hz.
- Accessible and settable via `FX15`, `FX18`, `FX07`, etc.

---

## Utilities: `utils.h`
- ROM loading
- Memory operations
- Byte-swapping helper
- Debug tools for printing register state

---

## Coming Next
- Implement `opcode_dispatch_init()` and fill the dispatch/sub-dispatch tables.
- Build out opcode logic in `opcodes.c`.
- Add SDL2 or other backend for visual/audio output and key input.

---

## License
This project is currently unlicensed — feel free to use or adapt the structure for learning or experimentation.
