#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "builtin.h"
#include "strings.h"
#include "parser.h"
#include "error_codes.h"

int read_int() {
        string_t s;
        init_string(&s);
        int c = 0;
        while (1) {
                c = getchar();
                if (c =='\n' || c == EOF) {
                        int num = atoi(s.data);
                        free_string(&s);
                        return num;
                }

                if (isdigit(c)) {
                        append_char(&s, c);
                } else {
                        fprintf(stderr, "Wrongly entered integer from input.\n");
                        free_string(&s);
                        cleanup_exit(RUN_INPUT_ERROR);
                }
        }
}

double read_double() {
        string_t s;
        init_string(&s);
        int c = 0;
        int state = 0;
        while (1) {
                c = getchar();
                if (c =='\n' || c == EOF) {
                        double num = atof(s.data);
                        free_string(&s);
                        return num;
                }
                switch (state) {
                case 0: if (isdigit(c)) {
                                append_char(&s, c);
                                state = 1;
                        } else {
                                fprintf(stderr, "Wrongly entered double from input.\n");
                                free_string(&s);
                                cleanup_exit(RUN_INPUT_ERROR);
                        }
                        break;

                case 1: if (isdigit(c)) {
                                append_char(&s, c);
                        } else if (c=='.') {
                                append_char(&s, c);
                                state = 2;
                        } else {
                                fprintf(stderr, "Wrongly entered double from input.\n");
                                free_string(&s);
                                cleanup_exit(RUN_INPUT_ERROR);
                        }
                        break;
                case 2: if (isdigit(c)) {
                                append_char(&s, c);
                                state = 3;
                        } else {
                                fprintf(stderr, "Wrongly entered double from input.\n");
                                free_string(&s);
                                cleanup_exit(RUN_INPUT_ERROR);
                                return -1;
                        }
                        break;

                case 3: if (isdigit(c)) {
                                append_char(&s, c);
                        } else if (c=='e' || c == 'E') {
                                append_char(&s, c);
                                state = 4;
                        } else {
                                fprintf(stderr, "Wrongly entered double from input.\n");
                                free_string(&s);
                                cleanup_exit(RUN_INPUT_ERROR);
                                return -1;
                        }
                        break;

                case 4: if (isdigit(c)) {
                                append_char(&s, c);
                        } else if (c=='+' || c == '-') {
                                append_char(&s, c);
                                state = 0;
                        }
                        break;
                }
        }
}

char * read_string() {
        string_t s;
        init_string(&s);
        int c = 0;
        while (1) {
                c = getchar();
                if (c =='\n' || c == EOF) {
                        return s.data;
                }
                append_char(&s, c);
        }
}

int main2 () { /* TODO REMOVE */

	//length
	char str [] = "blablabla";
	int dlzka = length(str);
	printf("String length is: %d\n", dlzka);

	//compare
	char string1 [] = "ahoj";
	char string2 [] = "cau";
	int comparison = compare(string1, string2);
	printf("Vysledok porovnavania: %d\n", comparison);

	//substr
	char source [] = "Cauko volam sa Tamara";
	int position = 6;
	int length_of_substring = 4;
	char *ot = substr(source, position, length_of_substring);
    printf("Substr: %s\n", ot);

	return 0;
}

int length(char *string) {
        return strlen(string);
}

int compare (char *s1, char *s2) {
        int comparison = strcmp (s1, s2);
        if (comparison < 0) {
                return -1;
        }
        else if (comparison > 0) {
                return 1;
        }
        else if (comparison == 0) {
                return 0;
        } else {
                return -1;
        }
}

char *substr (char *s, int i, int n) {
        if (i<0 || n < 0) {
                fprintf(stderr, "substr: index or length of search string is negative value.\n");
                cleanup_exit(RUN_OTHER_ERROR);
        }
        char *new_str = (char *) malloc((n + 1) * sizeof(char));
        if (new_str == NULL) {
                return NULL;
        }
        //osetrenie stavu, kedy pozadujeme zacinajuci index > ako dlzka samotneho retazca
        if (i > (int) strlen(s)) {
                fprintf(stderr, "substr: index is greater than length of the string.\n");
                free(new_str);
                cleanup_exit(RUN_OTHER_ERROR);
        }

        //osetrenie kedy pozadovana dlzka hladaneho podretazca je dlhsia ako samotny retazec
        if (n > (int) strlen(s) - i) {
                fprintf(stderr, "substr: length of search string is longer than length of the string.\n");
                free(new_str);
                cleanup_exit(RUN_OTHER_ERROR);
        }

        for (int j = 0; j<n; j++, i++) {
                new_str[j] = s[i];
        }
        new_str[n] = '\0';
        return new_str;
}
