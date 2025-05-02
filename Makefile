# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -std=c99 -DDEBUG \
         -I./include \
         -I"C:/Program Files/SDL2-2.32.4/x86_64-w64-mingw32/include/SDL2"

LDFLAGS = -L"C:/Program Files/SDL2-2.32.4/x86_64-w64-mingw32/lib" \
          -lmingw32 -lSDL2main -lSDL2

# Source and object layout
SRC_DIR = src
OBJ_DIR = build/obj
OUT = chip8

# List of source files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Build output
$(OUT): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Compile each .c file to a .o in build/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build/ dir if not exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean build artifacts
clean:
	rm -f $(OBJ_DIR)/*.o $(OUT)
