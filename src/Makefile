CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror -pedantic

all:
	$(CC) $(CFLAGS) main.c scanner.c parser.c symbol_table.c strings.c token_buffer.c -o main
	$(CC) $(CFLAGS) main.c scanner.c parser.c symbol_table.c strings.c token_buffer.c -o main_lexical_tests -DLEXICAL_TESTS

test:
	$(CC) $(CFLAGS) ial.c -o ial
	$(CC) $(CFLAGS) token_buffer_test.c token_buffer.c scanner.c strings.c -o token_buffer_test

clean:
	rm -f main main_lexical_tests ial token_buffer_test *.c~ *.h~

zip:
	zip ifj16.zip *.c *.h Makefile
