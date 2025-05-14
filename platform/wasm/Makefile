CC = emcc
CFLAGS = -O2 -s WASM=1 \
         -s EXPORTED_FUNCTIONS="['_wasm_init','_wasm_cycle']" \
         -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap']" \
         -I../include

SRC = ../../src/*.c wasm_bindings.c platform_wasm.c
OUT_BASE = chip8
OUT = $(OUT_BASE).js

.PHONY: all clean

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT_BASE).js $(OUT_BASE).wasm $(OUT_BASE).data
