CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -lm

all:test

test:test.o hash_table.o
	$(CC) $(CFLAGS) -o $@ $^ 
test.o:test.c hash_table.h
	$(CC) $(CFLAGS) -c -o $@ $<
htab.o:hash_table.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm *.o test
zip:

.PHONY:all test clean
