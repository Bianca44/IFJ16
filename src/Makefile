CFLAGS=-std=c11 -pedantic -Wall -Wextra -O2
CFLAGS_N=-std=c11 -pedantic -Wall -Wextra -DNDEBUG
CC=gcc

.PHONY: all clean r_test

all:test
test:test_debug.o instructions.o DLList.o interpret.o ial.o symbol_table.o strings.o memory_constants.o builtin.o parser.o expr.o token_buffer.o precedence_stack.o scanner.o
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
memory_constants.o:memory_constants.c memory_constants.h
	$(CC) $(CFLAGS) -c -o $@ $<
builtin.o:builtin.c builtin.h
	$(CC) $(CFLAGS) -c -o $@ $<
parser.o:parser.c parser.h
	$(CC) $(CFLAGS) -c -o $@ $<
expr.o:expr.c expr.h
	$(CC) $(CFLAGS) -c -o $@ $<
token_buffer.o:token_buffer.c
	$(CC) $(CFLAGS) -c -o $@ $<
precedence_stack.o:precedence_stack.c
	$(CC) $(CFLAGS) -c -o $@ $<
scanner.o:scanner.c
	$(CC) $(CFLAGS) -c -o $@ $<

parser:
	$(CC) $(CFLAGS) main.c scanner.c parser.c symbol_table.c builtin.c strings.c ial.c expr.c precedence_stack.c DLList.c interpret.c instructions.c memory_constants.c token_buffer.c -o main

r_test:
	./test

clean:
	rm -f *.o
	rm -f test
	rm -f ntest
	rm -f main
