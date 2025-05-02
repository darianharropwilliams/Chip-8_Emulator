import os

# Folder to output ROMs and disassemblies
OUTPUT_DIR = "./fixtures/"
DUMP_DIR = "./disasm/"
os.makedirs(OUTPUT_DIR, exist_ok=True)
os.makedirs(DUMP_DIR, exist_ok=True)

# Helper: disassemble a CHIP-8 instruction
def disassemble(opcode):
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

# Write ROM file + disassembly
def write_rom(filename, instructions):
    path = os.path.join(OUTPUT_DIR, filename)
    disasm_path = os.path.join(DUMP_DIR, filename.replace(".rom", ".txt"))

    with open(path, "wb") as f, open(disasm_path, "w") as d:
        for inst in instructions:
            f.write(inst.to_bytes(2, byteorder="big"))
            d.write(f"{inst:04X}: {disassemble(inst)}\n")

    print(f"[Generated] {filename}")

def generate_roms():
    CALL_TEST = 0x2208     # CALL 0x208 — test body starts at 0x208
    TEST_END  = 0x00EE     # RET to exit in test_mode
    HALT_LOOP = 0x1200     # JP 0x200 — safe halt loop

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
        "timer_set.rom": [0x601E, 0xF015, 0xF018],
    }

    for name, body in roms_raw.items():
        padding_size = (0x208 - 0x202) // 2
        wrapper = [0x2208] + [0x6000] * padding_size  # NOPs until 0x208
        if name == "call_ret.rom":
            test = body  # no RET added; already included explicitly
        else:
            test = body + [TEST_END]
        write_rom(name, wrapper + test)

generate_roms()
