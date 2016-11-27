CFLAGS=-std=c11 -pedantic -Wall -Wextra -O2 -DNDEBUG
CC=gcc

BINARY=ifj16

.PHONY: all clean

all:
	$(CC) $(CFLAGS) main.c scanner.c parser.c symbol_table.c builtin.c strings.c ial.c expr.c precedence_stack.c list.c interpret.c instructions.c memory_constants.c token_buffer.c -o $(BINARY)

clean:
	rm -f $(BINARY)
