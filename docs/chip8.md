# Chip8 Architecture and Instruction Cycle

## CHIP-8 Core Virtual Machine

This module defines and implements the core state and execution logic of the CHIP-8 emulator. It encapsulates memory, registers, timers, the display buffer, and input state, and provides the entry points for system initialization, ROM loading, and emulation cycles.

---

## Header: `chip8.h`

### Constants

```c
#define MEMORY_SIZE     4096
#define REGISTER_COUNT  16
#define STACK_SIZE      16
#define DISPLAY_WIDTH   64
#define DISPLAY_HEIGHT  32
#define KEYPAD_SIZE     16
#define FONTSET_SIZE    80
```

Defines the constraints of the CHIP-8 architecture, including memory layout, screen resolution, and keypad size.

### Struct: `Chip8`

```c
typedef struct {
    uint8_t  memory[MEMORY_SIZE];
    uint8_t  V[REGISTER_COUNT];
    uint16_t I;
    uint16_t pc;

    uint8_t  delay_timer;
    uint8_t  sound_timer;

    uint16_t stack[STACK_SIZE];
    uint8_t  sp;

    uint8_t  display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
    uint8_t  keypad[KEYPAD_SIZE];

    bool     draw_flag;
    bool     test_mode;
    char     rom_path[128];
} Chip8;
```

Represents the complete system state of a CHIP-8 interpreter:

- `memory`: 4KB RAM
- `V[0x0–0xF]`: General-purpose 8-bit registers
- `I`: Index register
- `pc`: Program counter
- `delay_timer`, `sound_timer`: Tick at 60Hz
- `stack` + `sp`: 16-level subroutine call stack
- `display`: 64x32 monochrome framebuffer (0 or 1 per pixel)
- `keypad`: 16-key hexadecimal input
- `draw_flag`: Indicates screen needs to be redrawn
- `test_mode`: Enables deterministic, debug-friendly execution
- `rom_path`: Saved for test logging and dump naming

### API

```c
void chip8_init(Chip8 *chip8);
int  chip8_load_rom(Chip8 *chip8, const char *filename);
void chip8_cycle(Chip8 *chip8);
```

- `chip8_init`: Initializes memory, registers, fontset, and subsystems
- `chip8_load_rom`: Loads a ROM binary into memory at offset 0x200
- `chip8_cycle`: Runs one fetch-decode-execute-update cycle

---

## Implementation: `chip8.c`

### `chip8_init`

- Clears all fields in the `Chip8` struct
- Sets program counter `pc` to 0x200
- Loads the default fontset (80 bytes) into memory at 0x000
- Initializes display, timer, keypad, and opcode dispatch system

### `chip8_load_rom`

- Uses `load_rom()` from `utils.c` to read a binary file into memory starting at 0x200
- Ensures ROM fits within the memory bounds
- Sets up for execution via `chip8_cycle()`

### `chip8_cycle`

This is the core interpreter loop executed repeatedly:

1. **Fetch**  
   Reads a 16-bit opcode from `memory[pc]` and `memory[pc+1]`

2. **Increment**  
   Increments `pc` by 2 bytes

3. **Decode & Execute**  
   Passes opcode to `dispatch_opcode()` which routes it to the proper handler

4. **Update Timers**  
   Calls `timer_update()`

5. **Poll Input**  
   Calls `keypad_scan()` to update key states

6. **Render Display**  
   If `draw_flag` is set, calls `update_display()` then resets the flag

Includes debug printing macros that are only active in `test_mode`.

---

## Integration: `main.c`

### Entry Points

- Uses a global `Chip8 chip8;` instance
- Parses command-line args to select a ROM and optionally enable `--test` mode
- Registers a SIGINT handler for clean shutdown
- Saves the ROM path into `chip8.rom_path` for test dump use

### Modes

- **Normal Mode**: Runs the emulator in a loop, cycling 100 times before checking for user interrupt
- **Test Mode**: Runs a fixed number of frames (e.g., 10), each with a fixed number of cycles (e.g., 60), to ensure deterministic output and proper memory dump

Test mode ensures repeatable behavior for automated testing tools.

---

## Fontset

The CHIP-8 fontset is hardcoded and loaded into the first 80 bytes of RAM during `chip8_init`. Each digit (0–F) is 5 bytes tall and represents a 4x5 pixel sprite used for drawing hexadecimal digits.

---

## Notes

- The emulator starts executing at memory address `0x200` per CHIP-8 convention
- `draw_flag`, `test_mode`, and `rom_path` are nonstandard fields added for testing and display coordination
- The VM is decoupled from the platform via subsystem abstractions (display, input, timer)

---
