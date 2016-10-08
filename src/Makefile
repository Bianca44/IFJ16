CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror -pedantic

all:
	$(CC) $(CFLAGS) main.c strings.c -o main
	$(CC) $(CFLAGS) main.c strings.c -o main_lexical_tests -DLEXICAL_TESTS

clean:
	rm -f main *.c~ *.h~

zip:
	zip ifj16.zip *.c *.h Makefile
