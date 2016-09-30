#include <stdio.h>
#include <stdlib.h>
#include "error_codes.h"
#include "scanner.c"
#include "strings.h"

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "bad arguments\n");
		return ERROR_INTERPRET;
	}

	int internal_error = OK;

	init_scanner(argv[1]);

	string s;
	init_string(&s);
	append_char(&s, 'c');
	append_char(&s, 'a');
	append_char(&s, 'u');

	set_string(&s, "ahoj");
	set_string(&s, "ahojkk");
	append_string(&s, "pp");
	set_string(&s, "pkfgdnoklsnidANDOIAODnascznxcnpoaijf");
	sort(&s);

	//remove_last_char(&s);
	printf("> %s <", s.data);

	free_string(&s);

	// bla bla

	printf("path: %s \n", argv[1]);
	return internal_error;
}
