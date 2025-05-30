# Opcode Dispatching

## Opcode Dispatch System

This module provides the decoding and routing logic for CHIP-8 opcodes. It maps fetched 16-bit instructions to specific handler functions via a structured, multi-level dispatch table design.

---

## Header: `dispatch.h`

### Function Pointer Type

```c
typedef void (*OpcodeHandler)(Chip8 *chip8, uint16_t opcode);
```

Defines the signature of all opcode handler functions.

### API

```c
void opcode_dispatch_init(void);
bool dispatch_opcode(Chip8 *chip8, uint16_t opcode);

void op_0xxx(Chip8 *chip8, uint16_t opcode);
void op_8xxx(Chip8 *chip8, uint16_t opcode);
void op_Exxx(Chip8 *chip8, uint16_t opcode);
void op_Fxxx(Chip8 *chip8, uint16_t opcode);
```

- `opcode_dispatch_init`: Initializes all dispatch tables
- `dispatch_opcode`: Top-level decoder, uses opcode prefix to select a handler
- `op_0xxx`, `op_8xxx`, `op_Exxx`, `op_Fxxx`: Specialized dispatchers for opcode families requiring further decoding

---

## Implementation: `dispatch.c`

### Main Dispatch Table

```c
static OpcodeHandler main_table[0x10];
```

Routes opcodes based on the high nibble (top 4 bits). For example:
- `0x1NNN` maps to `op_1nnn`
- `0x8XYE` maps to `op_8xxx`, which uses subdispatching

### Subdispatch Tables

```c
static OpcodeHandler table_0[0x100];
static OpcodeHandler table_8[0x10];
static OpcodeHandler table_E[0x100];
static OpcodeHandler table_F[0x100];
```

Used for opcode families where the lower bits determine the exact instruction:

- `table_0`: Handles `0x00E0` (CLS) and `0x00EE` (RET)
- `table_8`: Routes bitwise/arithmetic ops (`8XY0`–`8XYE`) using the lowest nibble
- `table_E`: Handles input-related skips (`EX9E`, `EXA1`) using full lower byte
- `table_F`: Maps all `FX**` instructions like timers, memory, and BCD logic

### `opcode_dispatch_init`

Initializes all entries in the main and subdispatch tables to NULL, then explicitly assigns known handlers:

Example:

```c
main_table[0x1] = op_1nnn;
main_table[0x8] = op_8xxx;
table_8[0x4]    = op_8xy4;  // Vx = Vx + Vy (with carry)
```

### `dispatch_opcode`

Called from `chip8_cycle`, this function:
1. Extracts the upper nibble of the opcode
2. Looks up the corresponding handler from `main_table`
3. Calls the handler if found, or logs an error if not

### Subdispatch Functions

Each subdispatch function takes an opcode group and routes it further:

#### `op_0xxx`

```c
OpcodeHandler handler = table_0[opcode & 0x00FF];
```

For opcodes like `0x00E0` (clear screen) or `0x00EE` (return).

#### `op_8xxx`

```c
OpcodeHandler handler = table_8[opcode & 0x000F];
```

Used for bitwise, arithmetic, and shift operations.

#### `op_Exxx` and `op_Fxxx`

Use the full lower byte (`opcode & 0x00FF`) to index into their respective tables.

---

## Design Rationale

CHIP-8 opcodes follow predictable binary patterns, making dispatch tables a clean and fast solution. This structure:

- Promotes modular handler implementations
- Avoids deep `switch` or `if` trees
- Supports extension (e.g., adding custom opcodes)
- Enables instruction tracing and debugging by group

---

## Example: Executing `0x8XY4`

1. `chip8_cycle` fetches opcode `0x8234`
2. `dispatch_opcode` sees high nibble `0x8` and calls `op_8xxx`
3. `op_8xxx` uses low nibble `0x4` to find `op_8xy4`
4. `op_8xy4` adds `Vy` to `Vx` and sets VF on overflow

---

## Notes

- All dispatch handlers must validate opcode bits as needed
- Unknown opcodes are logged to stderr
- Safe to extend dispatch tables for compatibility quirks or debugging features

---
