@echo off
call "C:\Users\kingo\emsdk\emsdk_env.bat"

emcc -O3 -s WASM=1 ^
  -s MODULARIZE=1 ^
  -s EXPORT_NAME=Chip8Emulator ^
  -s EXPORTED_FUNCTIONS="['_wasm_init','_wasm_cycle','_wasm_load_rom','_malloc','_free']" ^
  -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap','HEAPU8']" ^
  -I../../include ^
  -o chip8.js ^
  wasm_bindings.c platform_wasm.c ^
  ../../src/chip8.c ../../src/dispatch.c ../../src/display.c ^
  ../../src/input.c ../../src/opcodes.c ../../src/timer.c ../../src/utils.c

echo.
echo Build complete: chip8.js and chip8.wasm created.
