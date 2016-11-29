/*
 * Project: Interpret for IFJ16
 * Date: November 2016, FIT VUT
 * Team: 026
 * Authors: Tamara Krestianková, xkrest07
 *          Veronika Svoradová, xsvora01
 *          Dávid Bolvanský, xbolva00
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "builtin.h"
#include "strings.h"
#include "parser.h"
#include "error_codes.h"
#include "ial.h"
#include "scanner.h"


/* Sorts chars in a string */
void sort(char *str) {
        quick_sort(str, 0, (strlen(str) - 1));
}

/* Finds a substring in a string */
int find(char *text, char *search) {
        return find_bma(search, text);
}

/* Prints values to output stream */
void print(tVar * var) {
        switch (var->data_type) {
        case STRING:
                printf("%s", var->s);
                break;
        case INT:
                printf("%d", var->i);
                break;
        case DOUBLE:
                printf("%g", var->d);
                break;

        case BOOLEAN:
                printf("%s", (var->b) ? "true" : "false");
                break;
        }
}

/* Reads integer from the input stream, exists with RUN_INPUT_ERROR if not a valid integer number */
int read_int() {
        string_t s;
        init_string(&s);
        int c = 0;
        while (1) {
                c = getchar();
                if (c == '\n' || c == EOF) {
                        int num = atoi(s.data);
                        free_string(&s);
                        return num;
                }

                if (isdigit(c)) {
                        append_char(&s, c);
                } else {
                        free_string(&s);
                        fprintf(stderr, "Wrongly entered integer from input.\n");
                        exit(RUN_INPUT_ERROR);
                }
        }
}


/* Reads double from input stream, exists with RUN_INPUT_ERROR if not a valid double number */
double read_double() {
        string_t s;
        init_string(&s);
        int c = 0;
        int state = 0;
        while (1) {
                c = getchar();
                if (c == '\n' || c == EOF) {
                        double num = atof(s.data);
                        free_string(&s);
                        return num;
                }
                switch (state) {
                case 0:
                        if (isdigit(c)) {
                                append_char(&s, c);
                                state = 1;
                        } else {
                                free_string(&s);
                                fprintf(stderr, "Wrongly entered double from input.\n");
                                exit(RUN_INPUT_ERROR);
                        }
                        break;

                case 1:
                        if (isdigit(c)) {
                                append_char(&s, c);
                        } else if (c == '.') {
                                append_char(&s, c);
                                state = 2;
                        } else {
                                free_string(&s);
                                fprintf(stderr, "Wrongly entered double from input.\n");
                                exit(RUN_INPUT_ERROR);
                        }
                        break;
                case 2:
                        if (isdigit(c)) {
                                append_char(&s, c);
                                state = 3;
                        } else {
                                free_string(&s);
                                fprintf(stderr, "Wrongly entered double from input.\n");
                                exit(RUN_INPUT_ERROR);
                                return -1;
                        }
                        break;

                case 3:
                        if (isdigit(c)) {
                                append_char(&s, c);
                        } else if (c == 'e' || c == 'E') {
                                append_char(&s, c);
                                state = 4;
                        } else {
                                free_string(&s);
                                fprintf(stderr, "Wrongly entered double from input.\n");
                                exit(RUN_INPUT_ERROR);
                                return -1;
                        }
                        break;

                case 4:
                        if (isdigit(c)) {
                                append_char(&s, c);
                        } else if (c == '+' || c == '-') {
                                append_char(&s, c);
                                state = 0;
                        }
                        break;
                }
        }
}

/* Reads string from the input stream */
char *read_string() {
        string_t s;
        init_string(&s);
        int c = 0;
        while (1) {
                c = getchar();
                if (c == '\n' || c == EOF) {
                        return s.data;
                }
                append_char(&s, c);
        }
}

/* Compares two strings */
int compare(char *s1, char *s2) {
        int comparison = strcmp(s1, s2);
        if (comparison < 0) {
                return -1;
        } else if (comparison > 0) {
                return 1;
        } else if (comparison == 0) {
                return 0;
        } else {
                return -1;
        }
}

/* Finds and returns certain substring of the given string */
char *substr(char *s, int i, int n) {
        /* in case the length of a substring is less than zero
        ** or the beginnig index of the substring is less than zero - prints error
        **/
        printf("TROLOL\n");
        if (i < 0 || n < 0) {
                fprintf(stderr, "substr: index or length of search string is negative value.\n");
                exit(RUN_OTHER_ERROR);
        }
        char *new_str = (char *) malloc((n + 1) * sizeof(char));
        if (new_str == NULL) {
                exit(INTERNAL_INTERPRET_ERROR);
        }
        /* in case the beginning index of the substring is greater than
        ** the length of the given string - prints error
        */
        if (i > (int) strlen(s)) {
                fprintf(stderr, "substr: index is greater than length of the string.\n");
                free(new_str);
                exit(RUN_OTHER_ERROR);
        }
        /* in case the length of the substring is greater than the
        ** length of the given string - prints error
        */
        if (n > (int) strlen(s) - i) {
                fprintf(stderr, "substr: length of search string is longer than length of the string.\n");
                free(new_str);
                exit(RUN_OTHER_ERROR);
        }

        printf("TROLOL\n");

        memcpy(new_str, s, n);
        new_str[n] = '\0';
        return new_str;
}
