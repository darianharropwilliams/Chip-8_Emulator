"""
test_chip8.py

Automated regression test runner for the CHIP-8 emulator.

Each ROM fixture executes in test mode, and its post-execution state is 
dumped to a binary file. This script validates selected parts of the
emulator state (e.g., register values, memory contents) against expected values.

Run from project root:
    $ python tests/python/test_chip8.py
    $ python tests/python/test_chip8.py ld_vx

Requires:
- chip8.exe in project root (built emulator)
- helpers.read_dump to parse binary dump format
"""

import os
import sys
import subprocess
from helpers import read_dump

# Define directory structure relative to this script
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
ROM_DIR = os.path.join(SCRIPT_DIR, "fixtures")
DUMP_DIR = os.path.join(SCRIPT_DIR, "dumps")

# Expected outcomes for each test ROM
expected_behaviors = {
    "ld_vx.rom": {"V0": 0x0A},
    "add_vx.rom": {"V0": 0x03},
    "mem_store_load.rom": {"V0": 0x0A, "V1": 0x14},
    "jump.rom": {"V0": 0x0F},
    "call_ret.rom": {"V0": 0x0B},
    "cls.rom": {},
    "draw_sprite.rom": {},
    "key_skip.rom": {},
    "bcd.rom": {"memory": {0x300: 0, 0x301: 1, 0x302: 5}},
    # Note: timer_set.rom intentionally excluded due to timing nondeterminism
}


def run_single_test(rom: str) -> bool:
    """
    Runs the emulator in test mode for a single ROM and verifies output state.

    Args:
        rom: Filename of the test ROM to run (e.g., "add_vx.rom")

    Returns:
        True if the test passes; False otherwise.
    """
    exe = os.path.abspath(os.path.join(SCRIPT_DIR, "..", "..", "chip8.exe"))
    rom_path = os.path.join(ROM_DIR, rom)
    dump_path = os.path.join(DUMP_DIR, rom.replace(".rom", ".bin"))

    print(f"[TEST] {rom}")
    print("CMD:", f'"{exe}" "{rom_path}" --test')

    result = subprocess.run([exe, rom_path, "--test"]).returncode
    if result != 0:
        print(f"  [FAIL] Emulator exited with code {result}")
        return False

    try:
        chip = read_dump(dump_path)
    except FileNotFoundError:
        print(f"  [FAIL] Missing dump file: {dump_path}")
        return False

    checks = expected_behaviors.get(rom, {})
    success = True

    for key, expected in checks.items():
        if key == "memory":
            for addr, val in expected.items():
                actual = chip.memory[addr]
                if actual != val:
                    print(f"  [FAIL] Mem[{addr:04X}] = {actual:02X}, expected {val:02X}")
                    success = False
        elif key.startswith("V"):
            vx = int(key[1], 16)
            actual = chip.V[vx]
            if actual != expected:
                print(f"  [FAIL] V{vx:X} = {actual:02X}, expected {expected:02X}")
                success = False
        elif key == "delay_timer":
            if chip.delay_timer != expected:
                print(f"  [FAIL] Delay Timer = {chip.delay_timer:02X}, expected {expected:02X}")
                success = False
        elif key == "sound_timer":
            if chip.sound_timer != expected:
                print(f"  [FAIL] Sound Timer = {chip.sound_timer:02X}, expected {expected:02X}")
                success = False

    if success:
        print(f"  [PASS] {rom} passed.\n")
    else:
        print(f"  [FAIL] {rom} failed.\n")

    return success


def run_tests(selected_rom: str = None) -> None:
    """
    Runs the test suite. If a ROM is specified, only that test is executed.

    Args:
        selected_rom: Optional ROM filename (without extension) to run individually.

    Exits:
        Code 0 on success, 1 on any failure.
    """
    if selected_rom:
        if not selected_rom.endswith(".rom"):
            selected_rom += ".rom"
        if not os.path.exists(os.path.join(ROM_DIR, selected_rom)):
            print(f"[ERROR] ROM '{selected_rom}' not found.")
            sys.exit(1)
        passed = int(run_single_test(selected_rom))
        failed = 1 - passed
    else:
        roms = sorted([f for f in os.listdir(ROM_DIR) if f.endswith(".rom")])
        passed = failed = 0
        for rom in roms:
            if run_single_test(rom):
                passed += 1
            else:
                failed += 1

    print("---------------------------------")
    print(f"Results: {passed} Passed / {failed} Failed.")
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    # Clean and ensure output dump directory exists
    os.makedirs(DUMP_DIR, exist_ok=True)
    for f in os.listdir(DUMP_DIR):
        os.remove(os.path.join(DUMP_DIR, f))

    rom_arg = sys.argv[1] if len(sys.argv) > 1 else None
    run_tests(rom_arg)
