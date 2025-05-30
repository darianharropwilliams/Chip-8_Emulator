# Opcode Execution

## CHIP-8 Opcode Handlers

This document details the implementation of all CHIP-8 instructions supported by the emulator. Each opcode is 2 bytes (16 bits) and typically begins with a 4-bit identifier used for dispatching. Handlers are implemented in `opcodes.c` and declared in `opcodes.h`.

Opcodes are decoded and routed using the dispatch system documented in `dispatch.md`.

---

## Format Conventions

| Pattern | Description                                |
|---------|--------------------------------------------|
| `nnn`   | 12-bit address                             |
| `n`     | 4-bit number                               |
| `x`, `y`| 4-bit register indexes                     |
| `kk`    | 8-bit immediate value                      |
| `Vx`    | Register Vx                                |
| `Vy`    | Register Vy                                |
| `I`     | Index register                             |

---

## Instruction Set

### `00E0` - CLS

**Clear the display.**

```c
void op_00E0(Chip8 *chip8, uint16_t opcode);
```

- Sets all pixels in `chip8->display` to 0
- Triggers redraw by setting `chip8->draw_flag = true`

---

### `00EE` - RET

**Return from subroutine.**

```c
void op_00EE(Chip8 *chip8, uint16_t opcode);
```

- Pops an address off the stack and sets `pc` to that address
- In test mode, triggers `test_halt()` if stack is empty (used for debug exit)

---

### `1nnn` - JP addr

**Jump to address `nnn`.**

```c
void op_1nnn(Chip8 *chip8, uint16_t opcode);
```

- Sets `pc = nnn`

---

### `2nnn` - CALL addr

**Call subroutine at address `nnn`.**

```c
void op_2nnn(Chip8 *chip8, uint16_t opcode);
```

- Pushes current `pc` onto the stack
- Sets `pc = nnn`

---

### `3xkk` - SE Vx, byte

**Skip next instruction if `Vx == kk`.**

```c
void op_3xkk(Chip8 *chip8, uint16_t opcode);
```

- If equal, `pc += 2` to skip next opcode

---

### `4xkk` - SNE Vx, byte

**Skip next instruction if `Vx != kk`.**

```c
void op_4xkk(Chip8 *chip8, uint16_t opcode);
```

---

### `5xy0` - SE Vx, Vy

**Skip next instruction if `Vx == Vy`.**

```c
void op_5xy0(Chip8 *chip8, uint16_t opcode);
```

---

### `6xkk` - LD Vx, byte

**Set `Vx = kk`.**

```c
void op_6xkk(Chip8 *chip8, uint16_t opcode);
```

---

### `7xkk` - ADD Vx, byte

**Set `Vx = Vx + kk`. Does not affect carry.**

```c
void op_7xkk(Chip8 *chip8, uint16_t opcode);
```

---

### `8xy0` - LD Vx, Vy

**Set `Vx = Vy`.**

```c
void op_8xy0(Chip8 *chip8, uint16_t opcode);
```

---

### `8xy1` - OR Vx, Vy

**Set `Vx = Vx | Vy`.**

```c
void op_8xy1(Chip8 *chip8, uint16_t opcode);
```

---

### `8xy2` - AND Vx, Vy

**Set `Vx = Vx & Vy`.**

```c
void op_8xy2(Chip8 *chip8, uint16_t opcode);
```

---

### `8xy3` - XOR Vx, Vy

**Set `Vx = Vx ^ Vy`.**

```c
void op_8xy3(Chip8 *chip8, uint16_t opcode);
```

---

### `8xy4` - ADD Vx, Vy

**Set `Vx = Vx + Vy`, set `VF = carry`.**

```c
void op_8xy4(Chip8 *chip8, uint16_t opcode);
```

- If sum > 255, set `VF = 1`, else `VF = 0`

---

### `8xy5` - SUB Vx, Vy

**Set `Vx = Vx - Vy`, set `VF = NOT borrow`.**

```c
void op_8xy5(Chip8 *chip8, uint16_t opcode);
```

- If `Vx > Vy`, `VF = 1`; else `VF = 0`

---

### `8xy6` - SHR Vx

**Shift `Vx` right by 1, store LSB in `VF`.**

```c
void op_8xy6(Chip8 *chip8, uint16_t opcode);
```

---

### `8xy7` - SUBN Vx, Vy

**Set `Vx = Vy - Vx`, set `VF = NOT borrow`.**

```c
void op_8xy7(Chip8 *chip8, uint16_t opcode);
```

---

### `8xyE` - SHL Vx

**Shift `Vx` left by 1, store MSB in `VF`.**

```c
void op_8xyE(Chip8 *chip8, uint16_t opcode);
```

---

### `9xy0` - SNE Vx, Vy

**Skip next instruction if `Vx != Vy`.**

```c
void op_9xy0(Chip8 *chip8, uint16_t opcode);
```

---

### `Annn` - LD I, addr

**Set index register `I = nnn`.**

```c
void op_Annn(Chip8 *chip8, uint16_t opcode);
```

---

### `Bnnn` - JP V0, addr

**Jump to address `nnn + V0`.**

```c
void op_Bnnn(Chip8 *chip8, uint16_t opcode);
```

---

### `Cxkk` - RND Vx, byte

**Set `Vx = (rand byte) & kk`.**

```c
void op_Cxkk(Chip8 *chip8, uint16_t opcode);
```

---

### `Dxyn` - DRW Vx, Vy, nibble

**Draw `n`-byte sprite at (`Vx`, `Vy`), set `VF = collision`.**

```c
void op_Dxyn(Chip8 *chip8, uint16_t opcode);
```

- Uses `draw_sprite()` and sets `draw_flag`
- Handles wraparound and collision detection

---

### `Ex9E` - SKP Vx

**Skip next instruction if key in `Vx` is pressed.**

```c
void op_Ex9E(Chip8 *chip8, uint16_t opcode);
```

---

### `ExA1` - SKNP Vx

**Skip next instruction if key in `Vx` is not pressed.**

```c
void op_ExA1(Chip8 *chip8, uint16_t opcode);
```

---

### `Fx07` - LD Vx, DT

**Set `Vx = delay_timer`.**

```c
void op_Fx07(Chip8 *chip8, uint16_t opcode);
```

---

### `Fx0A` - LD Vx, K

**Wait for a key press, store result in `Vx`.**

```c
void op_Fx0A(Chip8 *chip8, uint16_t opcode);
```

- Repeats the instruction until a key is pressed

---

### `Fx15` - LD DT, Vx

**Set `delay_timer = Vx`.**

```c
void op_Fx15(Chip8 *chip8, uint16_t opcode);
```

---

### `Fx18` - LD ST, Vx

**Set `sound_timer = Vx`.**

```c
void op_Fx18(Chip8 *chip8, uint16_t opcode);
```

---

### `Fx1E` - ADD I, Vx

**Add `Vx` to `I`.**

```c
void op_Fx1E(Chip8 *chip8, uint16_t opcode);
```

---

### `Fx29` - LD F, Vx

**Set `I = location of sprite for digit in Vx`.**

```c
void op_Fx29(Chip8 *chip8, uint16_t opcode);
```

- Uses fontset location: `I = Vx * 5`

---

### `Fx33` - LD B, Vx

**Store BCD representation of `Vx` in memory at `I`, `I+1`, `I+2`.**

```c
void op_Fx33(Chip8 *chip8, uint16_t opcode);
```

- Hundreds digit at `I`, tens at `I+1`, ones at `I+2`

---

### `Fx55` - LD [I], Vx

**Store registers `V0` through `Vx` in memory starting at `I`.**

```c
void op_Fx55(Chip8 *chip8, uint16_t opcode);
```

---

### `Fx65` - LD Vx, [I]

**Read into `V0` through `Vx` from memory starting at `I`.**

```c
void op_Fx65(Chip8 *chip8, uint16_t opcode);
```

---

## Notes

- All handlers take a `Chip8*` instance and a raw 16-bit `opcode`
- Common fields extracted with macros: `OPCODE_X`, `OPCODE_Y`, `OPCODE_N`, `OPCODE_KK`, `OPCODE_NNN`
- Collision, overflow, and carry conditions are properly handled using register `VF`
- Most opcodes use memory, register, or display state directly, which makes them easy to test in isolation

---
