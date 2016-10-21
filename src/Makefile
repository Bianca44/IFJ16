CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror -pedantic

all:
	$(CC) $(CFLAGS) main.c scanner.c strings.c -o main
	$(CC) $(CFLAGS) main.c scanner.c strings.c -o main_lexical_tests -DLEXICAL_TESTS
	$(CC) $(CFLAGS) ial.c -o ial

clean:
	rm -f main main_lexical_tests ial *.c~ *.h~

zip:
	zip ifj16.zip *.c *.h Makefile
