CC = emcc
CFLAGS = -O3 -s WASM=1 \
         -s MODULARIZE=1 \
         -s EXPORT_NAME=Chip8Emulator \
         -s EXPORTED_FUNCTIONS="['_wasm_init','_wasm_cycle','_wasm_load_rom','_malloc','_free']" \
         -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap','HEAPU8']" \
         -I../../include

SRC = ../../src/chip8.c ../../src/dispatch.c ../../src/display.c \
      ../../src/input.c ../../src/opcodes.c ../../src/timer.c ../../src/utils.c \
      wasm_bindings.c platform_wasm.c

OUT_BASE = chip8
OUT = $(OUT_BASE).js

.PHONY: all clean

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT_BASE).js $(OUT_BASE).wasm $(OUT_BASE).data
