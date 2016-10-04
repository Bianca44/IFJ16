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
        char attr[256]; // tmp 256
} Token;

/* is_data_type */

bool is_keyword(char *str) {
        for (int i = 0; i < 17; i++) {
                if (strcmp(keywords[i], str) == 0) {
                        return true;
                }
        }
        return false;
}

Token create_token(int type, char attr[]) {
        Token t;
        t.type = type;
        strcpy(t.attr, attr);
        return t;
}

Token get_next_token(FILE * file) {

        int c;
        int state = 0;

        char str[256] = { '\0' }; // rework to use string
        int i = 0;

        while (1) {
                c = fgetc(file);
                if (c == EOF) {
                        if (state == 14) {
                                printf("line comment test\n");
                                return create_token(LINE_COMMENT, "");
                        } else {
                                return (state > 0) ? create_token(LEXICAL_ERROR, "") : create_token(EOF, "");
                        }
                }

                switch (state) {
                case 0: /* START */
                        if (isspace(c)) {
                                state = 0; // stav medzier
                        } else {
                                if (isdigit(c)) {
                                        state = 2;
                                        str[i++] = c;
                                } else if (c == '.') {
                                        state = 3;
                                        str[i++] = c;
                                } else if (isalpha(c) || c == '_' || c == '$') {
                                        state = 1; // mozno string alebo cislo
                                        str[i++] = c;
                                } else if (c == ',') {
                                        sprintf(str,"%c",c);
                                        return create_token(COMMA, str);
                                } else if (c == '\'') {
                                        sprintf(str,"%c",c);
                                        return create_token(SIMPLE_QUOTE, str);
                                } else if (c == '"') {
                                        state = 12;
                                } else if (c == '+') {
                                        return create_token(ADD, "+");
                                } else if (c == '-') {
                                        return create_token(SUB, "-");
                                } else if (c == ':') {
                                        return create_token(COLON, ":");
                                } else if (c == '/') {
                                        state = 5;
                                } else if (c == '*') {
                                        return create_token(MUL, "*");
                                } else if (c == '!') {
                                        state = 7;
                                } else if (c == '<') {
                                        state = 8;
                                } else if (c == '>') {
                                        state = 9;
                                } else if (c == '|') {
                                        state = 19;
                                } else if (c == '&') {
                                        state = 20;
                                } else if (c == ';') {
                                        sprintf(str,"%c",c);
                                        return create_token(SEMICOLON, str);
                                } else if (c == '{') {
                                        sprintf(str,"%c",c);
                                        return create_token(LEFT_CURVED_BRACKET, str);
                                } else if (c == '}') {
                                        sprintf(str,"%c",c);
                                        return create_token(RIGHT_CURVED_BRACKET, str);
                                } else if (c == '(') {
                                        sprintf(str,"%c",c);
                                        return create_token(LEFT_ROUNDED_BRACKET, str);
                                } else if (c == ')') {
                                        sprintf(str,"%c",c);
                                        return create_token(RIGHT_ROUNDED_BRACKET, str);
                                } else if (c == '=') {
                                        state = 10;
                                } else {
                                        //printf(" nieco ine ");
                                        sprintf(str,"%c",c);
                                        return create_token(LEXICAL_ERROR, str);
                                }

                        }
                        break;

                case 1: /* STRING, ID */
                        if (c == '.') {
                                str[i++] = c;
                                state = 11;
                        } else if (isalnum(c) || c == '_' || c == '$') {
                                str[i++] = c;
                        } else {
                                ungetc(c, file);
                                return create_token(ID, str);
                        }
                        break;

                case 2: /* INTEGER */
                        if (c == '.') {
                                str[i++] = c;
                                state = 3;
                        }  else if (isdigit(c)) {
                                str[i++] = c;
                        }  else {
                                ungetc(c, file);
                                str[i] = '\0';
                                return create_token(NUMBER, str);

                        }
                        break;

                case 3 /* maybe float */:
                        if (isdigit(c)) {
                                str[i++] = c;
                                state = 17;
                        } else {
                                return create_token(LEXICAL_ERROR, "");
                        }

                        break;

                case 17:
                        if (isdigit(c)) {
                                str[i++] = c;
                        } else if (c == 'e' || c == 'E') {
                                str[i++] = c;
                                state = 4;
                        } else {
                                ungetc(c, file);
                                str[i] = '\0';
                                return create_token(DOUBLE, str);
                        }

                        break;

                case 4 /* with e */:
                        if (isdigit(c)) {
                                str[i++] = c;
                                state = 15;
                        } else if (c == '-' || c == '+') {
                                str[i++] = c;
                                state = 18;
                        } else {
                                return create_token(LEXICAL_ERROR, "");
                        }
                        break;

                case 18: //plus, minus
                        if (isdigit(c)) {
                                str[i++] = c;
                                state = 15;
                        } else {
                                ungetc(c, file);
                                return create_token(LEXICAL_ERROR, "");
                        }
                        break;

                case 15 /* with e */:
                        if (isdigit(c)) {
                                str[i++] = c;
                        } else {
                                ungetc(c, file);
                                sprintf(str,"%c",c);
                                return create_token(DOUBLE, str);
                        }
                        break;

                case 5:
                        if (c == '/') {
                                state = 14;
                        } else if (c == '*') {
                                state = 6;
                        } else {
                                ungetc(c, file);
                                sprintf(str,"%c",c);
                                return create_token(DIV, str);
                        }
                        break;

                case 14:
                        if (c == '\n') {
                                return create_token(LINE_COMMENT, "");
                        }
                        break;

                case 6:
                        if (c == '*') {
                                state = 16;
                        }
                        break;

                case 16:
                        if (c == '/') {
                                return create_token(BLOCK_COMMENT, "");
                        }
                        break;

                case 7:
                        if (c == '=') {
                                return create_token(LESS_EQUAL, "!=");
                        } else {
                                ungetc(c, file);
                                sprintf(str,"%c",c);
                                return create_token(NEG, str);
                        }
                        break;
                case 8:
                        if (c == '=') {
                                return create_token(LESS_EQUAL, "<");
                        } else {
                                ungetc(c, file);
                                sprintf(str,"%c",c);
                                return create_token(LESS, str);
                        }
                        break;
                case 9:
                        if (c == '=') {
                                return create_token(GREATER_EQUAL, ">=");
                        } else {
                                ungetc(c, file);
                                sprintf(str,"%c",c);
                                return create_token(GREATER,  str);
                        }
                        break;

                case 10:
                        if (c == '=') {
                                return create_token(EQUAL, "==");
                        } else {
                                ungetc(c, file);
                                sprintf(str,"%c",c);
                                return create_token(ASSIGN, str);
                        }
                        break;


                case 11:
                        if (isalpha(c) || c == '_' || c == '$') {
                                str[i++] = c;
                                state = 13;
                        } else  {
                                ungetc(c, file);
                                return create_token(LEXICAL_ERROR, "");
                        } break;


                case 12:
                        if (c == '"') {
                                str[i] = '\0';
                                return create_token(STRING_LITERAL, str);

                        } else if (c == '\n') {
                                ungetc(c, file);
                                return create_token(LEXICAL_ERROR, "");
                        } else {
                                str[i++] = c;
                        }
                        break;

                case 13:
                        if (isalnum(c) || c == '_' || c == '$') {
                                str[i++] = c;
                        } else {
                                ungetc(c, file);
                                str[i] = '\0';
                                return create_token(SPECIAL_ID, str);
                        }
                        break;

                case 19:    // ||
                        if (c == '|') {
                                return create_token(LOGICAL_OR, "||");
                        } else {
                                ungetc(c, file);
                                return create_token(LEXICAL_ERROR, "");
                        }
                        break;

                case 20:          // &&
                        if (c == '&') {
                                return create_token(LOGICAL_AND, "&&");
                        } else {
                                ungetc(c, file);
                                return create_token(LEXICAL_ERROR, "");
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

        if (filename == NULL) {
                file = stdin;
        }
        else {

                file = fopen(filename, "r");
        }

        Token s;

        while ((s = get_next_token(file)).type != EOF) {
                printf("[%d]", s.type);
                if (strlen(s.attr) != 0) {
                        printf("[%s]\n", s.attr);
                } else {
                        printf("\n");
                }
        }

        fclose(file);

        return 1;
}
