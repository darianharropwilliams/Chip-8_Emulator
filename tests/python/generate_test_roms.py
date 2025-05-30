"""
generate_test_roms.py

Generates a suite of test ROMs for the CHIP-8 emulator and outputs:
- Binary ROM files into ./fixtures/
- Disassembled text files into ./disasm/

Each ROM contains a short instruction sequence targeting a specific opcode or feature.
Test execution begins at address 0x208 via a CALL wrapper from 0x200.

Run this script once to regenerate all test ROMs:
    $ python generate_test_roms.py
"""

import os

# Output directories
OUTPUT_DIR = "./fixtures/"
DUMP_DIR = "./disasm/"
os.makedirs(OUTPUT_DIR, exist_ok=True)
os.makedirs(DUMP_DIR, exist_ok=True)


def disassemble(opcode: int) -> str:
    """
    Returns a human-readable disassembly for a given CHIP-8 opcode.

    Args:
        opcode: A 16-bit CHIP-8 instruction.

    Returns:
        A string representing the mnemonic and arguments.
    """
    nnn = opcode & 0x0FFF
    kk = opcode & 0x00FF
    x = (opcode >> 8) & 0x0F
    y = (opcode >> 4) & 0x0F
    n = opcode & 0x000F

    if opcode == 0x00E0:
        return "CLS"
    elif opcode == 0x00EE:
        return "RET"
    elif opcode & 0xF000 == 0x1000:
        return f"JP 0x{nnn:03X}"
    elif opcode & 0xF000 == 0x2000:
        return f"CALL 0x{nnn:03X}"
    elif opcode & 0xF000 == 0x3000:
        return f"SE V{x:X}, 0x{kk:02X}"
    elif opcode & 0xF000 == 0x4000:
        return f"SNE V{x:X}, 0x{kk:02X}"
    elif opcode & 0xF000 == 0x6000:
        return f"LD V{x:X}, 0x{kk:02X}"
    elif opcode & 0xF000 == 0x7000:
        return f"ADD V{x:X}, 0x{kk:02X}"
    elif opcode & 0xF000 == 0xA000:
        return f"LD I, 0x{nnn:03X}"
    elif opcode & 0xF000 == 0xD000:
        return f"DRW V{x:X}, V{y:X}, 0x{n:X}"
    elif opcode & 0xF0FF == 0xE09E:
        return f"SKP V{x:X}"
    elif opcode & 0xF0FF == 0xF033:
        return f"LD B, V{x:X}"
    elif opcode & 0xF0FF == 0xF015:
        return f"LD DT, V{x:X}"
    elif opcode & 0xF0FF == 0xF018:
        return f"LD ST, V{x:X}"
    elif opcode & 0xF0FF == 0xF055:
        return f"LD [I], V{x:X}"
    elif opcode & 0xF0FF == 0xF065:
        return f"LD V{x:X}, [I]"
    elif opcode & 0xF0FF == 0xF165:
        return f"LD V0–V{x:X}, [I]"
    elif opcode & 0xF0FF == 0xF155:
        return f"LD [I], V0–V{x:X}"
    else:
        return "UNKNOWN"


def write_rom(filename: str, instructions: list[int]) -> None:
    """
    Write a CHIP-8 ROM and its disassembly to output files.

    Args:
        filename: The target ROM file name (e.g., "add_vx.rom").
        instructions: A list of 16-bit CHIP-8 opcodes.
    """
    path = os.path.join(OUTPUT_DIR, filename)
    disasm_path = os.path.join(DUMP_DIR, filename.replace(".rom", ".txt"))

    with open(path, "wb") as f, open(disasm_path, "w") as d:
        for inst in instructions:
            f.write(inst.to_bytes(2, byteorder="big"))
            d.write(f"{inst:04X}: {disassemble(inst)}\n")

    print(f"[Generated] {filename}")


def generate_roms() -> None:
    """
    Generate all test ROMs with appropriate wrappers for test-mode execution.

    Each ROM includes:
    - CALL 0x208 at 0x200 (start of program)
    - NOP padding to reach 0x208
    - Instruction sequence
    - RET to exit cleanly from test mode
    """
    CALL_TEST = 0x2208     # CALL 0x208 — test body starts at 0x208
    TEST_END  = 0x00EE     # RET to exit in test_mode
    HALT_LOOP = 0x1200     # JP 0x200 — unused here, but useful for manual ROMs

    roms_raw = {
        "ld_vx.rom": [0x600A],
        "add_vx.rom": [0x6001, 0x7002],
        "mem_store_load.rom": [
            0x600A, 0x6114,
            0xA300, 0xF155,
            0x6000, 0x6100,
            0xA300, 0xF165
        ],
        "jump.rom": [0x120C, 0x6001, 0x6002, 0x600F],  # Jumps to 0x20C
        "call_ret.rom": [
            0x220C,      # CALL 0x20C
            0x600B,      # LD V0, 0x0B
            0x00EE       # RET
        ],
        "cls.rom": [0x00E0],
        "draw_sprite.rom": [0x6000, 0x6100, 0xA300, 0xD015],
        "key_skip.rom": [0x6005, 0xE09E, 0x60FF],
        "bcd.rom": [0x600F, 0xA300, 0xF033],
        "timer_set.rom": [0x601E, 0xF015, 0xF018],  # Note: not currently validated due to timing issues
    }

    for name, body in roms_raw.items():
        # Insert CALL wrapper and NOPs (LD V0, 0) until address 0x208
        padding_size = (0x208 - 0x202) // 2
        wrapper = [CALL_TEST] + [0x6000] * padding_size

        if name == "call_ret.rom":
            test = body  # already includes RET
        else:
            test = body + [TEST_END]  # ensure clean return

        write_rom(name, wrapper + test)


# Execute ROM generation if script is run directly
if __name__ == "__main__":
    generate_roms()
