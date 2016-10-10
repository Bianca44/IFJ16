#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "ial.h"
#include <string.h>
#include "strings.h"
#include "scanner.h"

int save_token(token *t, int type, string *attr) {
        t->type = type;
        if (attr != NULL && type < KEYWORD_TOKENS_OFFSET) {
                t->attr.data = strdup(attr->data);
                t->attr.length = attr->length;
                t->attr.allocated_size = attr->length + 1;
                clear_string(attr);
        } else {
                t->attr.data =  NULL;
                t->attr.length = 0;
                t->attr.allocated_size = 0;
        }
        return type;
}

int detect_keyword(string *str) {
        if (str->length > LONGEST_KEYWORD) {
                return ID;
        }

        for (int i = 0; i < KEYWORDS_COUNT; i++) {
                if (strcmp(keywords[i], str->data) == 0) {
                        return i + KEYWORD_TOKENS_OFFSET;
                }
        }

        return ID;
}

int get_next_token(token *t, FILE * file) {

        int c;
        int state = 0;

        string s;

        while (1) {
                c = fgetc(file);
                if (c == EOF) {
                        return save_token(t, (state > 0) ? LEXICAL_ERROR : EOF, NULL);
                }

                switch (state) {
                case 0:
                        if (isspace(c)) {
                                state = 0;
                        } else {
                                if (isdigit(c)) {
                                        state = 2;
                                        init_string(&s);
                                        append_char(&s, c);
                                } else if (c == '.') {
                                        state = 3;
                                        init_string(&s);
                                        append_char(&s, c);
                                } else if (isalpha(c) || c == '_' || c == '$') {
                                        state = 1;
                                        init_string(&s);
                                        append_char(&s, c);
                                } else if (c == ',') {
                                        return save_token(t, COMMA, NULL);
                                } else if (c == '\'') {
                                        return save_token(t, SIMPLE_QUOTE, NULL);
                                } else if (c == '"') {
                                        state = 12;
                                        init_string(&s);
                                } else if (c == '+') {
                                        return save_token(t, ADD, NULL);
                                } else if (c == '-') {
                                        return save_token(t, SUB, NULL);
                                } else if (c == ':') {
                                        return save_token(t, COLON, NULL);
                                } else if (c == '/') {
                                        state = 5;
                                } else if (c == '*') {
                                        return save_token(t, MUL, NULL);
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
                                        return save_token(t, SEMICOLON, NULL);
                                } else if (c == '{') {
                                        return save_token(t, LEFT_CURVED_BRACKET, NULL);
                                } else if (c == '}') {
                                        return save_token(t, RIGHT_CURVED_BRACKET, NULL);
                                } else if (c == '(') {
                                        return save_token(t, LEFT_ROUNDED_BRACKET, NULL);
                                } else if (c == ')') {
                                        return save_token(t, RIGHT_ROUNDED_BRACKET, NULL);
                                } else if (c == '=') {
                                        state = 10;
                                } else {
                                        return save_token(t, LEXICAL_ERROR, NULL);
                                }

                        }
                        break;

                case 1:
                        if (c == '.') {
                                append_char(&s, c);
                                state = 11;
                        } else if (isalnum(c) || c == '_' || c == '$') {
                                append_char(&s, c);
                        } else {
                                ungetc(c, file);
                                return save_token(t, detect_keyword(&s), &s);
                        }
                        break;

                case 2:
                        if (c == '.') {
                                append_char(&s, c);
                                state = 3;
                        } else if (isdigit(c)) {
                                append_char(&s, c);
                        } else {
                                ungetc(c, file);
                                return save_token(t, INTEGER, &s);

                        }
                        break;

                case 3:
                        if (isdigit(c)) {
                                append_char(&s, c);
                                state = 17;
                        } else {
                                return save_token(t, LEXICAL_ERROR, NULL);
                        }

                        break;

                case 17:
                        if (isdigit(c)) {
                                append_char(&s, c);
                        } else if (c == 'e' || c == 'E') {
                                append_char(&s, c);
                                state = 4;
                        } else {
                                ungetc(c, file);
                                return save_token(t, FLOAT, &s);
                        }

                        break;

                case 4:
                        if (isdigit(c)) {
                                append_char(&s, c);
                                state = 15;
                        } else if (c == '-' || c == '+') {
                                append_char(&s, c);
                                state = 18;
                        } else {
                                return save_token(t, LEXICAL_ERROR, NULL);
                        }
                        break;

                case 18:
                        if (isdigit(c)) {
                                append_char(&s, c);
                                state = 15;
                        } else {
                                ungetc(c, file);
                                return save_token(t, LEXICAL_ERROR, NULL);
                        }
                        break;

                case 15:
                        if (isdigit(c)) {
                                append_char(&s, c);
                        } else {
                                ungetc(c, file);
                                return save_token(t, FLOAT, &s);
                        }
                        break;

                case 5:
                        if (c == '/') {
                                state = 14;
                        } else if (c == '*') {
                                state = 6;
                        } else {
                                ungetc(c, file);
                                return save_token(t, DIV, NULL);
                        }
                        break;

                case 14:
                        if (c == '\n') {
                                return save_token(t, LINE_COMMENT, NULL);
                        }
                        break;

                case 6:
                        if (c == '*') {
                                state = 16;
                        }
                        break;

                case 16:
                        if (c == '/') {
                                return save_token(t, BLOCK_COMMENT, NULL);
                        }
                        break;

                case 7:
                        if (c == '=') {
                                return save_token(t, NOT_EQUAL, NULL);
                        } else {
                                ungetc(c, file);
                                return save_token(t, NEG, NULL);
                        }
                        break;
                case 8:
                        if (c == '=') {
                                return save_token(t, LESS_EQUAL, NULL);
                        } else {
                                ungetc(c, file);
                                return save_token(t, LESS, NULL);
                        }
                        break;
                case 9:
                        if (c == '=') {
                                return save_token(t, GREATER_EQUAL, NULL);
                        } else {
                                ungetc(c, file);
                                return save_token(t, GREATER, NULL);
                        }
                        break;

                case 10:
                        if (c == '=') {
                                return save_token(t, EQUAL, NULL);
                        } else {
                                ungetc(c, file);
                                return save_token(t, ASSIGN, NULL);
                        }
                        break;


                case 11:
                        if (isalpha(c) || c == '_' || c == '$') {
                                append_char(&s, c);
                                state = 13;
                        } else {
                                ungetc(c, file);
                                return save_token(t, LEXICAL_ERROR, NULL);
                        }
                        break;


                case 12:
                        if (c == '"') {
                                return save_token(t, STRING_LITERAL, &s);

                        } else if (c == '\n') {
                                ungetc(c, file);
                                return save_token(t, LEXICAL_ERROR, NULL);
                        } else {
                                append_char(&s, c);
                        }
                        break;

                case 13:
                        if (isalnum(c) || c == '_' || c == '$') {
                                append_char(&s, c);
                        } else {
                                ungetc(c, file);
                                return save_token(t, SPECIAL_ID, &s);
                        }
                        break;

                case 19:
                        if (c == '|') {
                                return save_token(t, LOGICAL_OR, NULL);
                        } else {
                                ungetc(c, file);
                                return save_token(t, LEXICAL_ERROR, NULL);
                        }
                        break;

                case 20:
                        if (c == '&') {
                                return save_token(t, LOGICAL_AND, NULL);
                        } else {
                                ungetc(c, file);
                                return save_token(t, LEXICAL_ERROR, NULL);
                        }
                        break;
                default:
                        break;
                }
        }


}

int init_scanner(char *filename) {

        FILE *file;

        if (filename == NULL) {
                file = stdin;
        } else {

                file = fopen(filename, "r");
        }

        if (file == NULL) {
                printf("File not found\n");
                return -2;
        }

        token t;
        while (get_next_token(&t, file) != EOF) {

                #if LEXICAL_TESTS
                printf("[%s]", token_names[t.type]);
                if (t.attr.data != NULL) {
                        printf("[%s]\n", t.attr.data);
                } else {
                        printf("\n");
                }
                #endif

                if (t.attr.data != NULL) {
                        clear_string(&t.attr);
                }
        }

        fclose(file);

        return 0;
}
