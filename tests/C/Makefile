CC=gcc
CFLAGS=-Wall -Wextra -I../../C
LDFLAGS=-lSDL2
SRC=tests.c test_chip8_cycle.c chip8_testshim.c ../../chip8.c ../../display.c ../../input.c ../../timer.c ../../dispatch.c ../../utils.c ../../opcodes.c
OUT=test_runner

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)
