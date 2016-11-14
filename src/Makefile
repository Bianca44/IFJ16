CFLAGS=-std=c11 -pedantic -Wall -Wextra -O2
CFLAGS_N=-std=c11 -pedantic -Wall -Wextra -DNDEBUG
CC=gcc

.PHONY: all clean r_test

all:test
test:test_debug.o instructions.o DLList.o interpret.o ial.o symbol_table.o strings.o
	$(CC) $(CFLAGS) -o $@ $^
instructions.o:instructions.c instructions.h interpret.h
	$(CC) $(CFLAGS) -c -o $@ $<
DLList.o:DLList.c DLList.h
	$(CC) $(CFLAGS) -c -o $@ $<
interpret.o:interpret.c interpret.h
	$(CC) $(CFLAGS) -c -o $@ $<
ial.o:ial.c ial.h
	$(CC) $(CFLAGS) -c -o $@ $<
symbol_table.o:symbol_table.c symbol_table.h ial.h
	$(CC) $(CFLAGS) -c -o $@ $<
strings.o:strings.c strings.h
	$(CC) $(CFLAGS) -c -o $@ $<

ntest:ntest_debug.o
	$(CC) $(CFLAGS) -o $@ $<
ntest_debug.o:test_debug.c
	$(CC) $(CFLAGS_N) -c -o $@ $<

parser:
	$(CC) $(CFLAGS) main.c scanner.c parser.c symbol_table.c builtin.c strings.c ial.c expr.c precedence_stack.c  token_buffer.c -o main

r_test:
	./test

clean:
	rm -f *.o
	rm -f test
	rm -f ntest
	rm -f main
