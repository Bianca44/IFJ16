// test
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "ial.h"
#include "tokens_table.h"
#include <string.h>
#include "strings.h"


// enum pre stavy automatu

typedef struct {
    int type;			// token type
    union {
	int value_int;
	double value_double;
	string value_string;
    };
} TToken;

// Todo
int is_keyword(char *str) {
    return 0;
}

int get_next_token(FILE * file) {

    int c;
    int state = 0;

    char str[256] = { '\0' };	// rework to use string Object
    int i = 0;
    while (1) {
	c = fgetc(file);
	if (c == EOF) {
	    return c;
	}

	switch (state) {
	case 0:
	    if (isspace(c)) {
		state = 0;	// stav medzier
	    } else {
		if (isalpha(c)) {
		    state = 1;	// mozno string alebo cislo
		} else if (isdigit(c)) {
		    state = 2;

		    // * / atd
		} else {
		    printf(" nieco ine ");
		    return 5;
		}
		ungetc(c, file);

	    }
	    break;

	case 1:
	    if (isalnum(c)) {
		str[i++] = c;
	    } else {
		printf("string: %s \n", str);
		ungetc(c, file);
		str[i] = '\0';
		state = 0;
		return ID;
	    }
	    break;

	case 2:
	    if (isdigit(c)) {
		str[i++] = c;
	    } else {
		printf("number: %s \n", str);
		ungetc(c, file);
		str[i] = '\0';
		state = 0;
		return NUMBER;
	    }
	    break;
	default:
	    break;
	}
    }


}

int init_scanner(char *filename) {

    FILE *file;
    file = fopen(filename, "r");
    TToken new;
    new.type = 42;
    //strcpy(new.value_string, "hi");
    string h;
    init_string(&h);
    append_char(&h, 'c');
    printf("%d %s  < \n", new.type, h.data);

    int s;
    while ((s = get_next_token(file)) != EOF) {
	printf("token: %d\n", s);
    }

    fclose(file);

    return 1;
}
