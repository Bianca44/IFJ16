CFLAGS=-std=c11 -pedantic -Wall -Wextra
CFLAGS_N=-std=c11 -pedantic -Wall -Wextra -DNDEBUG
CC=gcc

.PHONY: all clean r_test

all:test
test:test_debug.o instructions.o DLList.o datatypes.h
	$(CC) $(CFLAGS) -o $@ $^
instructions.o:instructions.c instructions.h
	$(CC) $(CFLAGS) -c -o $@ $<
DLList.o:DLList.c DLList.h
	$(CC) $(CFLAGS) -c -o $@ $<
ntest:ntest_debug.o
	$(CC) $(CFLAGS) -o $@ $<
ntest_debug.o:test_debug.c	
	$(CC) $(CFLAGS_N) -c -o $@ $<

r_test:
	./test

clean:
	rm *.o 
	rm test
	rm ntest
