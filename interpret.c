#include <stdio.h>
#include <stdlib.h>
#include "error_codes.h"
#include "scanner.c"

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "bad arguments\n");
		return ERROR_INTERPRET;
	}

	int internal_error = OK;

	init_scanner(argv[1]);

	// bla bla

	printf("path: %s \n", argv[1]);
	return internal_error;
}
