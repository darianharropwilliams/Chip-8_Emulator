# CHIP-8 Emulator

This project is a modular and extensible CHIP-8 emulator written in C, built with a focus on clean separation of responsibilities. It features a dynamic opcode dispatch system, modular subsystems for input, display, and timers, and a core emulation loop driven by a centralized `chip8.c` interface.

## Project Structure

```
├── include/
│   ├── chip8.h         # Core emulator state and control
│   ├── dispatch.h      # Opcode dispatch mechanism (main + subtables)
│   ├── opcodes.h       # Opcode handler function declarations
│   ├── display.h       # Monochrome display emulation (64x32)
│   ├── input.h         # Keypad handling (16-key HEX)
│   ├── timer.h         # Delay and sound timers (60Hz decrement)
│   └── utils.h         # ROM loading, memory utilities, debugging
│
├── src/
│   ├── chip8.c         # Core VM implementation
│   ├── dispatch.c      # Main and subdispatch tables
│   ├── opcodes.c       # Fully implemented CHIP-8 opcode set
│   ├── display.c       # (Stubbed) TODO
│   ├── input.c         # (Stubbed) TODO
│   ├── timer.c         # TODO
│   ├── utils.c         # TODO
│   └── main.c          # TODO
```

---

## Architecture Overview

### Core: `chip8.h`
Defines the `Chip8` struct representing full VM state:
- Memory (4K)
- Registers `V0`–`VF`, index register `I`, and PC
- Stack (16x 16-bit) + stack pointer
- Delay & sound timers (60Hz)
- Monochrome display buffer (64×32)
- 16-key keypad
- `draw_flag` used to signal screen updates

### Execution Cycle: `chip8_cycle`
Performs one emulation cycle:
1. Fetches opcode from memory
2. Advances program counter
3. Dispatches opcode
4. Updates timers
5. Polls input
6. Redraws display if needed

---

## Opcode Dispatch System

### Files: `dispatch.h/.c` + `opcodes.h/.c`

The dispatch mechanism uses a layered table-based system for speed and clarity:

#### `main_table[16]`
- Dispatches based on the high nibble (e.g., `0x1NNN`, `0x6XKK`)
- Routes to functions like `op_1nnn`, `op_8xxx`, `op_Fxxx`

#### Subdispatch Tables
- `table_0[256]`: Handles `0x00E0`, `0x00EE`
- `table_8[16]`: Handles arithmetic/bitwise ops `8xy0` to `8xyE`
- `table_E[256]`: Keypad skip instructions (`EX9E`, `EXA1`)
- `table_F[256]`: Timer/memory/input instructions like `FX07`, `FX55`, etc.

Each top-level handler (like `op_8xxx`) calls its subtable based on lower opcode bits.

### Benefits
- Fast: Constant-time dispatch via table lookups
- Modular: Easy to debug or extend individual opcode categories
- Clean: Keeps core logic readable and separated

---

## Display Module: `display.c`
- Currently stubbed
- Will render `chip8->display[]` buffer (64x32) to an SDL2 window
- Uses XOR-based sprite drawing with collision detection
- Clears display and tracks redraws via `chip8->draw_flag`

---

## Input Module: `input.c`
- Currently stubbed
- Will map host keyboard input to CHIP-8's 16-key hex keypad
- Includes polling, key state mapping, and key state checks

---

## Timers: `timer.c`
- Manages delay and sound timers (`delay_timer`, `sound_timer`)
- Decrements at 60Hz
- Supports reading and setting from opcodes

---

## Utilities: `utils.c`
- Loads ROMs into memory starting at 0x200
- Swaps bytes and prints VM state (for debugging)

---

## Status: Core Emulation Complete ✅

**Fully implemented:**
- Memory, registers, and instruction decoding
- 35 standard CHIP-8 opcodes
- Stack, keypad logic, timers, display buffer updates

**Pending:**
- SDL2 graphics rendering (64x32 scaled window)
- SDL2 input mapping to CHIP-8 keypad
- Audio (for sound timer, optional)

---

## License
This project is currently unlicensed — feel free to use or adapt the structure for learning or experimentation.
