# CHIP-8 Emulator Tests

This folder contains unit and integration tests for the CHIP-8 emulator.

## Structure

- `/C/`: Basic unit tests written in C using `assert()`
- `/python/`: Higher-level tests using Python and `ctypes`
- `/fixtures/`: Sample ROMs and opcode sequences
- `/dumps/`: Memory/register dumps for regression testing

## Running C Tests

```bash
cd tests/C
make test
./chip8_tests
