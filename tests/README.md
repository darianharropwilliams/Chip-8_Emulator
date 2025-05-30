# CHIP-8 Emulator Test Suite

This test suite verifies the correctness of CHIP-8 opcode handling using automatically generated ROMs and binary state dumps.

## Test Types

- **Integration-level** testing via ROM execution.
- Asserts CHIP-8 state (registers, memory, timers) after executing small programs.
- Realistic — mimics how actual games stress the emulator.

### Timing Tests

The `timer_set.rom` test was removed from the automated test suite because the `delay_timer` and `sound_timer` are decremented during test execution, which leads to non-deterministic results.

Timers run at 60Hz, but the test executes multiple frames, so their values are always reduced by 1–2 ticks depending on timing. This is expected and not a bug in emulator logic.

To test timers manually:
- Run the ROM in test mode
- Inspect `DT` and `ST` from the final dump


---

## Directory Overview

- `C/`: C utilities for dumping emulator state in `--test` mode.
- `python/`:
  - `generate_test_roms.py`: Generates `.rom` test cases.
  - `test_chip8.py`: Runs all ROMs and validates final state.
  - `helpers.py`: Reads and interprets binary state dumps.
  - `fixtures/`: ROMs used for testing.
  - `dumps/`: Output state from `--test` mode.
  - `disasm/`: Optional disassemblies for visibility.

---

## Running Tests

From the `python/` directory:

```bash
python test_chip8.py
