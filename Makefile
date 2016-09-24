CC=gcc
CFLAGS=-std=c99 -Wall -Wextra #-Werror -pedantic

all:
	$(CC) $(CFLAGS) scanner.c -o scanner $(LFLAGS)
    
clean:
	rm -f scanner
    
zip:
	zip ifj16.zip *.c *.h Makefile
