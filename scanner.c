// test
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "ial.h"
#include "tokens_table.h"
#include <string.h>
#include "strings.h"


// enum pre stavy automatu

char *keywords[17] = { "boolean", "break", "class", "continue", "do", "double", "else", "false", "for", "if", "int", "return", "String", "static", "true", "void", "while"};

typedef struct {
        int type;       // token type
        union {
                int value_int;
                double value_double;
                string value_string;
        };
} TToken;

/* is_data_type */

bool is_keyword(char *str) {
        for (int i = 0; i < 17; i++) {
                if (strcmp(keywords[i], str) == 0) {
                        return true;
                }
        }
        return false;
}

int get_next_token(FILE * file) {

        int c;
        int state = 0;

        int octal_counter = 0;

        char str[256] = { '\0' }; // rework to use string
        int i = 0;

        while (1) {
                c = fgetc(file);
                if (c == EOF) {
                        return c;
                }

                switch (state) {
                case 0: /* START */
                        if (isspace(c)) {
                                state = 0; // stav medzier
                        } else {
                                if (isalpha(c) || c == '_' || c == '$') {
                                        state = 1; // mozno string alebo cislo
                                        str[i++] = c;
                                } else if (isdigit(c)) {
                                        state = 2;
                                        str[i++] = c;
                                } else if (c == '.') {
                                        return DOT;
                                } else if (c == ',') {
                                        return COMMA;
                                } else if (c == '\'') {
                                        return SIMPLE_QUOTE;
                                } else if (c == '"') {
                                        return DOUBLE_QUOTE;
                                } else if (c == '+') {
                                        return ADD;
                                } else if (c == '-') {
                                        return SUB;
                                } else if (c == ':') { /* < >  8strana pdf */
                                        return COLON;
                                } else if (c == '/') {
                                        state = 5;
                                } else if (c == '*') {
                                        state = 6;
                                } else if (c == '\\') {
                                        str[i++] = '\\';
                                        state = 7;
                                } else if (c == '!') {
                                        state = 9;
                                } else if (c == '<') {
                                        state = 10;
                                } else if (c == '>') {
                                        state = 11;
                                } else if (c == ';') {
                                        printf("semicolon \n");
                                        return SEMICOLON;
                                } else if (c == '{') {
                                        printf("left curved bracket \n");
                                        return LEFT_CURVED_BRACKET;
                                } else if (c == '}') {
                                        printf("right curved bracket \n");
                                        return RIGHT_CURVED_BRACKET;
                                } else if (c == '(') {
                                        printf("left round bracket \n");
                                        return LEFT_ROUNDED_BRACKET;
                                } else if (c == ')') {
                                        printf("right rounded bracket \n");
                                        return RIGHT_ROUNDED_BRACKET;
                                } else if (c == '=') {
                                        state = 12;
                                } else {
                                        printf(" nieco ine ");
                                        return LEXICAL_ERROR;
                                }

                        }
                        break;

                case 1: /* STRING, ID */
                        if (c == '.') {
                                str[i++] = c;
                                state = 13;
                        } else if (isalnum(c) || c == '_' || c == '$') {
                                str[i++] = c;
                        } else {
                                printf("string: %s is keyword = %d \n", str, is_keyword(str));
                                ungetc(c, file);
                                str[i] = '\0';
                                state = 0;
                                return ID;
                        }
                        break;

                case 2: /* INTEGER */
                        if (isalpha(c)) {
                                ungetc(c, file);
                                state = 0;
                                return LEXICAL_ERROR;
                        } else if (isdigit(c)) {
                                str[i++] = c;
                        } else if (c == '.') {
                                str[i++] = c;
                                state = 3;
                        } else {
                                printf("number: %s \n", str);
                                ungetc(c, file);
                                str[i] = '\0';
                                state = 0;
                                return NUMBER;
                        }
                        break;

                case 3 /* maybe float */:
                        if (isdigit(c)) {
                                str[i++] = c;
                        } else if (c == 'e' || c == 'E') {
                                str[i++] = c;
                                state = 4;
                        } else {
                                printf("double: %s \n", str);
                                ungetc(c, file);
                                str[i] = '\0';
                                state = 0;
                                return DOUBLE;
                        }
                        break;

                case 4 /* with e */:
                        if (isdigit(c)) {
                                str[i++] = c;
                        } else if (c == '-' || c == '+') {
                                str[i++] = c;

                        } else {
                                printf("double: %s \n", str);
                                ungetc(c, file);
                                str[i] = '\0';
                                state = 0;
                                return DOUBLE; /* TODO DOUBLE */
                        }
                        break;

                case 5:
                        state = 0;
                        if (c == '/') {
                                printf("line comment \n");
                                return LINE_COMMENT;
                        } else if (c == '*') {
                                printf("block comment start \n");
                                return BLOCK_COMMENT_START;
                        } else {
                                printf("div \n");
                                state = 0;
                                ungetc(c, file);
                                return DIV;
                        }
                        break;

                case 6:
                        state = 0;
                        if (c == '/') {
                                printf("block comment end \n");
                                return BLOCK_COMMENT_END;
                        } else {
                                printf("mul \n");
                                ungetc(c, file);
                                return MUL;
                        }
                        break;

                case 7:

                        state = 0;
                        if (c == 'n') {
                                printf("\\n\n");
                                return NEW_LINE;
                        } else if (c == 't') {
                                printf("\\t\n");
                                return TAB;
                        } /*else if (c == '\\') {
                             printf(" dvojite slash\n");
                             return DOUBLE_BACKSLASH;
                             } */
                        else if (isalpha(c)) {
                                ungetc(c, file);
                                return LEXICAL_ERROR; /* napr. \p */
                        } else if (isdigit(c)) {
                                state = 8;
                                ungetc(c, file);
                        } else {
                                ungetc(c, file);
                                return BACKSLASH;
                        }
                        break;

                case 8:

                        if (octal_counter < 3) {
                                int digit = c - '0';
                                if ((octal_counter == 0 && digit >= 0
                                     && digit <= 3) || (octal_counter == 1
                                                        && digit >= 0 && digit <= 7)
                                    || (octal_counter == 2 && digit >= 1 && digit <= 7)) {
                                        str[i++] = c;
                                        octal_counter++;
                                } else {
                                        state = 0;
                                        ungetc(c, file);
                                        return LEXICAL_ERROR; /* \567 */
                                }

                        } else {
                                state = 0;
                                octal_counter = 0;
                                ungetc(c, file);
                                printf("octal: %s \n", str);
                                return OCTAL;
                        }


                        break;

                case 9:
                        state = 0;
                        if (c == '=') {
                                return NOT_EQUAL;
                        } else {
                                ungetc(c, file);
                                return LEXICAL_ERROR; /* BOOLOP */
                        }
                        break;
                case 10:
                        state = 0;
                        if (c == '=') {
                                return LESS_EQUAL;
                        } else {
                                ungetc(c, file);
                                return LESS;
                        }
                        break;
                case 11:
                        state = 0;
                        if (c == '=') {
                                return GREATER_EQUAL;
                        } else {
                                ungetc(c, file);
                                return GREATER;
                        }
                        break;

                case 12:
                        state = 0;
                        if (c == '=') {
                                return EQUAL;
                        } else {
                                ungetc(c, file);
                                return ASSIGN;
                        }
                        break;


                case 13:
                        if (isalnum(c) || c == '_' || c == '$') {
                                str[i++] = c;
                        } else {
                                printf("string special id: %s \n", str);
                                ungetc(c, file);
                                str[i] = '\0';
                                state = 0;
                                return SPECIAL_ID;
                        }
                        break;
                default:
                        /* should not happen */
                        break;
                }
        }


}

int init_scanner(char *filename) {

        FILE *file;

        file = fopen(filename, "r");

        int s;

        while ((s = get_next_token(file)) != EOF) {
                printf("token: %d ", s);
                if (s == LEXICAL_ERROR) {
                        printf(" (lexikalna chyba) \n");
                        //break;
                } else {
                        printf("\n");
                }
        }

        fclose(file);

        return 1;
}
