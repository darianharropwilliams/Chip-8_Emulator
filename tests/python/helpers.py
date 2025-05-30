"""
helpers.py

Support utilities for CHIP-8 emulator testing.

Includes:
- ROM loading
- Emulator state structure definition (Chip8)
- Binary dump parsing for test results
- Memory comparison utility
"""

import os
import struct
import ctypes

# Constants for CHIP-8 architecture
MEMORY_SIZE = 4096
NUM_REGS = 16
STACK_SIZE = 16
KEYPAD_SIZE = 16
DISPLAY_WIDTH = 64
DISPLAY_HEIGHT = 32


def load_rom(path: str) -> bytes:
    """
    Load the binary contents of a CHIP-8 ROM file.

    Args:
        path: Path to the ROM file.

    Returns:
        The ROM file contents as a byte string.
    """
    with open(path, "rb") as f:
        return f.read()


class Chip8(ctypes.LittleEndianStructure):
    """
    Native memory layout of a CHIP-8 virtual machine for testing.

    Used primarily to define the full emulator state in memory dumps.

    Attributes:
        V            -- 16 general-purpose 8-bit registers (V0–VF)
        I            -- 16-bit index register
        pc           -- 16-bit program counter
        sp           -- 8-bit stack pointer
        delay_timer  -- delay timer value
        sound_timer  -- sound timer value
        draw_flag    -- flag to signal redraw
        keypad       -- 16-key input array
        display      -- 64x32 monochrome pixel display buffer
        stack        -- 16-entry call stack
        memory       -- 4096-byte main memory
    """
    _fields_ = [
        ("V", ctypes.c_uint8 * NUM_REGS),
        ("I", ctypes.c_uint16),
        ("pc", ctypes.c_uint16),
        ("sp", ctypes.c_uint8),
        ("delay_timer", ctypes.c_uint8),
        ("sound_timer", ctypes.c_uint8),
        ("draw_flag", ctypes.c_uint8),
        ("keypad", ctypes.c_uint8 * KEYPAD_SIZE),
        ("display", ctypes.c_uint8 * (DISPLAY_WIDTH * DISPLAY_HEIGHT)),
        ("stack", ctypes.c_uint16 * STACK_SIZE),
        ("memory", ctypes.c_uint8 * MEMORY_SIZE),
    ]


def read_dump(path: str):
    """
    Parse a CHIP-8 binary memory dump file.

    Expected layout:
    - [0x0000–0x0FFF]   → 4096 bytes of memory
    - [0x1000–0x100F]   → V0–VF
    - [0x1010–0x1011]   → I register (little endian)
    - [0x1012–0x1013]   → PC register (little endian)
    - [0x1014]          → delay_timer
    - [0x1015]          → sound_timer

    Args:
        path: Path to the binary dump file.

    Returns:
        A Chip8Dump object containing the extracted state.
    """

    class Chip8Dump:
        """Python-side representation of parsed CHIP-8 dump data."""

        def __init__(self, raw: bytes):
            self.raw = raw
            self.memory = raw[0:4096]
            self.V = list(raw[4096:4096 + 16])
            self.I = int.from_bytes(raw[4112:4114], "little")
            self.pc = int.from_bytes(raw[4114:4116], "little")
            self.delay_timer = raw[4116]
            self.sound_timer = raw[4117]
            # Note: Add more fields here if extended in the C dump format

    with open(path, "rb") as f:
        data = f.read()

    return Chip8Dump(data)


def compare_memory(actual: bytes, expected: bytes) -> bool:
    """
    Compare two byte sequences and report the first mismatch, if any.

    Args:
        actual: Actual memory content.
        expected: Expected memory content.

    Returns:
        True if memory matches exactly, False otherwise.
    """
    if actual == expected:
        return True

    for i in range(min(len(actual), len(expected))):
        if actual[i] != expected[i]:
            print(f"Mismatch at byte {i:04X}: actual={actual[i]:02X}, expected={expected[i]:02X}")
            return False

    return False
