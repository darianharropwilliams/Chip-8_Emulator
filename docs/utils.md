# Utility Functions

## Utility Functions and Debug Tools

This module provides a collection of helper functions and macros for ROM loading, memory manipulation, debugging, and test harness support. These utilities support emulator initialization, test mode logging, memory dumps, and diagnostics.

---

## Header: `utils.h`

### Debug Macros

```c
#define DEBUG_PRINT(chip8, fmt, ...)
#define DEBUG_PRINT_STDOUT(chip8, fmt, ...)
```

- Conditionally prints debug output if `chip8->test_mode == true`
- Outputs to `stderr` or `stdout` depending on macro
- Used throughout the codebase to log state, errors, or execution flow during tests

---

### API

```c
int load_rom(const char *filename, uint8_t *memory, uint16_t memory_size);
void memory_copy(uint8_t *dest, const uint8_t *src, size_t size);
uint8_t swap_bytes(uint8_t byte);

void print_registers(const uint8_t *V, uint16_t I, uint16_t pc, uint8_t delay_timer, uint8_t sound_timer);
void dump_memory(Chip8 *chip8, const char *rom_path);
void test_halt(Chip8 *chip8, const char *rom_path);
```

---

## Implementation: `utils.c`

### `load_rom`

```c
int load_rom(const char *filename, uint8_t *memory, uint16_t memory_size)
```

- Opens a binary ROM file and loads its contents into `memory + 0x200`
- Validates file size against remaining memory space
- Returns 0 on success, -1 on failure
- Called by `chip8_load_rom`

---

### `memory_copy`

```c
void memory_copy(uint8_t *dest, const uint8_t *src, size_t size)
```

- Thin wrapper over `memcpy`
- Used in WASM memory initialization and test tools

---

### `swap_bytes`

```c
uint8_t swap_bytes(uint8_t byte)
```

- Reverses the bits in a byte
- Useful for mirroring sprites or fonts
- Example: `0b10110000` becomes `0b00001101`

---

### `print_registers`

```c
void print_registers(const uint8_t *V, uint16_t I, uint16_t pc, uint8_t delay_timer, uint8_t sound_timer)
```

- Outputs CPU register and timer state to stdout
- Used for diagnostics in test mode and error logging
- Only active when compiled with `-DDEBUG`

---

### `dump_memory`

```c
void dump_memory(Chip8 *chip8, const char *rom_path)
```

- Dumps the entire `Chip8` struct to a binary file for later analysis
- Output path: `tests/python/dumps/<rom>.bin`
- Used by the Python test suite for validating expected state
- Extracts base filename from `rom_path` for naming consistency

---

### `test_halt`

```c
void test_halt(Chip8 *chip8, const char *rom_path)
```

- Terminates the emulator during test mode when a `RET` is encountered
- Dumps memory using `dump_memory`
- Emits final state logs to `stderr`
- Called by `op_00EE` when `chip8->test_mode == true`

---

## Usage in Test Mode

These utilities play a key role in the `--test` harness:

- ROMs are executed for a fixed number of cycles
- Final state is dumped to file
- Python scripts compare dumps with expected outputs

---

## Notes

- `dump_memory` enables full VM state serialization for regression testing
- Debug macros do not produce output during normal execution
- The utils module is test-focused but safe to use during normal runs

---
