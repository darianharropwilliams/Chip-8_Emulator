# Compiler and Flags
CC = gcc
CFLAGS = -Wall -g -std=c99 -I./include
LDFLAGS =

# Directories
SRCDIR = src
INCDIR = include
BINDIR = build/bin
OBJDIR = build/obj

# Files
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
EXEC = $(BINDIR)/chip8

# Default target
all: $(EXEC)

# Linking the final executable
$(EXEC): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(EXEC)

# Compiling each .c file to .o
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the build artifacts
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Rebuild everything
rebuild: clean all

# Run the emulator
run: $(EXEC)
	./$(EXEC)

# Test the Makefile with a dummy target
.PHONY: all clean rebuild run

all: $(EXEC)
	# Copy SDL2.dll to output directory
	cp ./lib/SDL2-2.28.5/x86_64-w64-mingw32/bin/SDL2.dll $(BINDIR)/
