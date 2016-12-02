# Project: Interpret for IFJ16
# Date: November 2016, FIT VUT
# Team: 026
# Author: Dávid Bolvanský, xbolva00

CFLAGS=-std=c11 -pedantic -Wall -Wextra -O3 -DNDEBUG
CC=gcc

BINARY=ifj16

.PHONY: all clean test

all:
	$(CC) $(CFLAGS) main.c scanner.c parser.c symbol_table.c builtin.c strings.c ial.c expr.c precedence_stack.c list.c interpret.c instructions.c memory_constants.c token_buffer.c -o $(BINARY)

clean:
	rm -f $(BINARY)

test:
	chmod +x ./tests/run_tests.sh
	./tests/run_tests.sh
