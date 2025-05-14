# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -std=c99 -DDEBUG \
         -I./include \
         -I"C:/Program Files/SDL2-2.32.4/x86_64-w64-mingw32/include/SDL2"

LDFLAGS = -L"C:/Program Files/SDL2-2.32.4/x86_64-w64-mingw32/lib" \
          -lmingw32 -lSDL2main -lSDL2

# Directories
SRC_DIR = src
PLATFORM_DIR = platform/sdl
OBJ_DIR = build/obj
OUT = chip8

# Source files (full paths)
SRC = $(wildcard $(SRC_DIR)/*.c) $(PLATFORM_DIR)/platform_sdl.c

# Object files (preserving directory structure)
OBJ = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))

# Output binary
$(OUT): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Rule to compile .c -> .o while preserving directory structure
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(OBJ_DIR) $(OUT)
