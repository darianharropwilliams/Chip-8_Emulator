import struct

def load_rom(path):
    """Load a ROM file into a byte array."""
    with open(path, "rb") as f:
        return f.read()

import ctypes
import os

MEMORY_SIZE = 4096
NUM_REGS = 16
STACK_SIZE = 16
KEYPAD_SIZE = 16
DISPLAY_WIDTH = 64
DISPLAY_HEIGHT = 32

class Chip8(ctypes.LittleEndianStructure):  # Windows = little endian
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

def read_dump(path):
    with open(path, "rb") as f:
        data = f.read()

    class Chip8Dump:
        def __init__(self, raw):
            self.raw = raw
            self.memory = raw[0:4096]
            self.V = list(raw[4096:4096+16])
            self.I = int.from_bytes(raw[4112:4114], "little")
            self.pc = int.from_bytes(raw[4114:4116], "little")
            self.delay_timer = raw[4116]
            self.sound_timer = raw[4117]
            # Add more as needed...

    return Chip8Dump(data)


def compare_memory(actual, expected):
    """Simple memory comparison with report."""
    if actual == expected:
        return True
    for i in range(min(len(actual), len(expected))):
        if actual[i] != expected[i]:
            print(f"Mismatch at byte {i:04X}: actual={actual[i]:02X}, expected={expected[i]:02X}")
            return False
    return False
