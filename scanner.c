// test
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "ial.h"

int main(int argc, char *args[])
{
	printf("IFJ 16 %d \n", argc);

	int c;
	int state = 0;
	int i = 0;
	char s[128] = "";
	while ((c = fgetc(stdin)) != EOF) {
		printf("char: %c \n", c);

		switch (state) {
			printf("ahoj space pre %c \n", c);;
		case 0 /*START*/:
			if (isspace(c)) {
				state = 0;
				printf("ahoj space pre %c \n", c);;
			}

			else if (isdigit(c)) {
				s[i++] = c;
				state = 1 /* number ? */ ;
				printf("mozno cislo %c \n", c);
			}

			else if (isalpha(c)) {
				state = 2 /* string ? */ ;
				printf("mozno string %c \n", c);
			}
			break;
		case 1:	/* moybe number */
			if (isdigit(c)) {
				s[i++] = c;
				state = 1 /* number ? */ ;
			} else {
				// chyba
				state = 0;
			}
			break;
		}
	}

	s[i] = '\0';
	printf("cislo %s \n", s);
	return 0;

}
