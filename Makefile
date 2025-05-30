# ========================
# CHIP-8 Emulator Makefile
# ========================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -std=c99 -DDEBUG \
         -I./include \
		 -I./tests/C \
         -I"$(SDL2_PATH)/include/SDL2"

LDFLAGS = -L"$(SDL2_PATH)/lib" \
          -lmingw32 -lSDL2main -lSDL2

# Directories
SRC_DIR = src
PLATFORM_DIR = platform/sdl
OBJ_DIR = build/obj
OUT = chip8

# Source files
SRC = $(wildcard $(SRC_DIR)/*.c) \
      $(PLATFORM_DIR)/platform_sdl.c \
      tests/C/chip8_testshim.c


# Object files (preserve folder structure)
OBJ = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))

# Default target
all: $(OUT)

# Link final binary
$(OUT): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Compile .c to .o
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(OUT)
