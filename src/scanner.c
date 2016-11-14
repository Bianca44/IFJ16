#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "strings.h"
#include "scanner.h"
#include "parser.h"

extern int parser_error_flag;
FILE *file;

char *token_names[TOKENS_COUNT] = { "LEXICAL_ERROR", "ID", "INT_LITERAL", "DOUBLE_LITERAL", "ADD", "SUB", "MUL",
                                    "DIV", "SEMICOLON", "LEFT_CURVED_BRACKET", "RIGHT_CURVED_BRACKET",
                                    "LEFT_ROUNDED_BRACKET", "RIGHT_ROUNDED_BRACKET", "ASSIGN", "LOGICAL_AND",
                                    "LOGICAL_OR", "COMMA", "NEG",  "STRING_LITERAL", "NOT_EQUAL", "LESS_EQUAL",
                                    "LESS", "GREATER_EQUAL", "GREATER", "EQUAL", "SPECIAL_ID", "BOOLEAN", "BREAK",
                                    "CLASS", "CONTINUE", "DO", "DOUBLE", "ELSE", "FALSE", "FOR", "IF", "INT", "RETURN",
                                    "STRING", "STATIC", "TRUE", "VOID", "WHILE" };


char *keywords[KEYWORDS_COUNT] = { "boolean", "break", "class", "continue", "do", "double", "else", "false",
                                   "for", "if", "int", "return", "String", "static", "true", "void", "while" };

int save_token(token_t *t, int type, string_t *attr) {
        t->type = type;
        if (attr != NULL) {
                switch (type) {
                case ID:
                case SPECIAL_ID:
                case STRING_LITERAL:
                        t->string_value = attr->data;
                        break;
                case INT_LITERAL:
                        t->int_value = atoi(attr->data);
                        free_string(attr);
                        break;

                case DOUBLE_LITERAL:
                        t->double_value = atof(attr->data);
                        free_string(attr);
                        break;
                default:
                        free_string(attr);
                        break;
                }
        }
        return type;
}

int detect_keyword(string_t *str) {
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

int get_next_token(token_t *t) {

        int c = 0;
        int state = 0;
        int octal_number_length = 0;

        string_t s;

        while (1) {
                c = fgetc(file);
                if (c == EOF) {
                        return save_token(t, (state > SPACE) ? LEXICAL_ERROR : EOF, NULL);
                }

                switch (state) {
                case SPACE:
                        if (!isspace(c)) {
                                if (isdigit(c)) {
                                        state = NUMBER;
                                        init_string(&s);
                                        append_char(&s, c);
                                } else if (isalpha(c) || c == '_' || c == '$') {
                                        state = IDENTIFICATOR;
                                        init_string(&s);
                                        append_char(&s, c);
                                } else if (c == ',') {
                                        return save_token(t, COMMA, NULL);
                                } else if (c == '"') {
                                        state = LITERAL;
                                        init_string(&s);
                                } else if (c == '+') {
                                        return save_token(t, ADD, NULL);
                                } else if (c == '-') {
                                        return save_token(t, SUB, NULL);
                                } else if (c == '/') {
                                        state = SLASH;
                                } else if (c == '*') {
                                        return save_token(t, MUL, NULL);
                                } else if (c == '!') {
                                        state = EXCLAMATION;
                                } else if (c == '<') {
                                        state = COMPARE_LESS;
                                } else if (c == '>') {
                                        state = COMPARE_GREATER;
                                } else if (c == '|') {
                                        state = OR;
                                } else if (c == '&') {
                                        state = AND;
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
                                        state = COMPARE_ASSIGN;
                                } else {
                                        return save_token(t, LEXICAL_ERROR, NULL);
                                }

                        }
                        break;

                case IDENTIFICATOR:
                        if (c == '.') {
                                append_char(&s, c);
                                state = QUALIFIED_ID;
                        } else if (isalnum(c) || c == '_' || c == '$') {
                                append_char(&s, c);
                        } else {
                                ungetc(c, file);
                                return save_token(t, detect_keyword(&s), &s);
                        }
                        break;

                case NUMBER:
                        if (c == '.') {
                                append_char(&s, c);
                                state = NUM_DOUBLE;
                        } else if (isdigit(c)) {
                                append_char(&s, c);
                        } else {
                                ungetc(c, file);
                                return save_token(t, INT_LITERAL, &s);

                        }
                        break;

                case NUM_DOUBLE:
                        if (isdigit(c)) {
                                append_char(&s, c);
                                state = SIMPLE_DOUBLE;
                        } else {
                                return save_token(t, LEXICAL_ERROR, NULL);
                        }

                        break;

                case SCIENTIC_DOUBLE:
                        if (isdigit(c)) {
                                append_char(&s, c);
                                state = DOUBLE_END;
                        } else if (c == '-' || c == '+') {
                                append_char(&s, c);
                                state = SCIENTIC_DOUBLE_EXP;
                        } else {
                                return save_token(t, LEXICAL_ERROR, NULL);
                        }
                        break;

                case SLASH:
                        if (c == '/') {
                                state = LINE_COMMENT_END;
                        } else if (c == '*') {
                                state = COMMENT;
                        } else {
                                ungetc(c, file);
                                return save_token(t, DIV, NULL);
                        }
                        break;

                case COMMENT:
                        if (c == '*') {
                                state = BLOCK_COMMENT_END;
                        }
                        break;

                case EXCLAMATION:
                        if (c == '=') {
                                return save_token(t, NOT_EQUAL, NULL);
                        } else {
                                ungetc(c, file);
                                return save_token(t, NEG, NULL);
                        }
                        break;

                case COMPARE_LESS:
                        if (c == '=') {
                                return save_token(t, LESS_EQUAL, NULL);
                        } else {
                                ungetc(c, file);
                                return save_token(t, LESS, NULL);
                        }
                        break;

                case COMPARE_GREATER:
                        if (c == '=') {
                                return save_token(t, GREATER_EQUAL, NULL);
                        } else {
                                ungetc(c, file);
                                return save_token(t, GREATER, NULL);
                        }
                        break;

                case COMPARE_ASSIGN:
                        if (c == '=') {
                                return save_token(t, EQUAL, NULL);
                        } else {
                                ungetc(c, file);
                                return save_token(t, ASSIGN, NULL);
                        }
                        break;


                case QUALIFIED_ID:
                        if (isalpha(c) || c == '_' || c == '$') {
                                append_char(&s, c);
                                state = QUALIFIED_ID_END;
                        } else {
                                ungetc(c, file);
                                return save_token(t, LEXICAL_ERROR, NULL);
                        }
                        break;

                case LITERAL:
                        if (c == '"') {
                                return save_token(t, STRING_LITERAL, &s);
                        } else if (c <= 31) {
                                ungetc(c, file);
                                return save_token(t, LEXICAL_ERROR, NULL);
                        } else if (c == '\\') {
                                append_char(&s, c);
                                state = LITERAL_SLASH;
                        } else {
                                append_char(&s, c);
                        }
                        break;

                case LITERAL_SLASH:
                        if (isdigit(c)) {
                                if (octal_number_length < 3) {
                                        int digit = c - '0';
                                        switch (octal_number_length) {
                                        case 0:
                                                if (digit > 3) {
                                                        ungetc(c, file);
                                                        return save_token(t, LEXICAL_ERROR, NULL);
                                                }
                                                break;
                                        case 1:
                                                if (digit > 7) {
                                                        ungetc(c, file);
                                                        return save_token(t, LEXICAL_ERROR, NULL);
                                                }
                                                break;
                                        case 2:
                                                if (digit == 0 || digit > 7) {
                                                        ungetc(c, file);
                                                        return save_token(t, LEXICAL_ERROR, NULL);
                                                }
                                                break;
                                        }

                                        append_char(&s, c);
                                        octal_number_length++;
                                        if (octal_number_length == 3) {
                                                octal_number_length = 0;
                                                state = LITERAL;
                                        }
                                }
                        } else if (c == '"' || c == 'n' || c == 't' || c == '\\') {
                                if (octal_number_length == 0 ) {
                                    append_char(&s, c);
                                    state = LITERAL;
                                } else {
                                    ungetc(c, file);
                                    return save_token(t, LEXICAL_ERROR, NULL);
                                }
                        } else {
                                ungetc(c, file);
                                return save_token(t, LEXICAL_ERROR, NULL);
                        }
                        break;

                case QUALIFIED_ID_END:
                        if (isalnum(c) || c == '_' || c == '$') {
                                append_char(&s, c);
                        } else {
                                ungetc(c, file);
                                return save_token(t, SPECIAL_ID, &s);
                        }
                        break;

                case LINE_COMMENT_END:
                        if (c == '\n') {
                                state = SPACE;
                        }
                        break;

                case DOUBLE_END:
                        if (isdigit(c)) {
                                append_char(&s, c);
                        } else {
                                ungetc(c, file);
                                return save_token(t, DOUBLE_LITERAL, &s);
                        }
                        break;

                case BLOCK_COMMENT_END:
                        if (c == '/') {
                                state = SPACE;
                        }
                        break;

                case SCIENTIC_DOUBLE_EXP:
                        if (isdigit(c)) {
                                append_char(&s, c);
                        } else if (c == 'e' || c == 'E') {
                                append_char(&s, c);
                                state = SCIENTIC_DOUBLE;
                        } else {
                                ungetc(c, file);
                                return save_token(t, DOUBLE_LITERAL, &s);
                        }

                        break;

                case SIMPLE_DOUBLE:
                        if (isdigit(c)) {
                                append_char(&s, c);
                                state = DOUBLE_END;
                        } else {
                                ungetc(c, file);
                                return save_token(t, LEXICAL_ERROR, NULL);
                        }
                        break;

                case OR:
                        if (c == '|') {
                                return save_token(t, LOGICAL_OR, NULL);
                        } else {
                                ungetc(c, file);
                                return save_token(t, LEXICAL_ERROR, NULL);
                        }
                        break;

                case AND:
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

        if (filename == NULL) {
                file = stdin;
        } else {

                file = fopen(filename, "r");
        }

        if (file == NULL) {
                printf("File not found\n");
                return -2;
        }

        #if LEXICAL_TESTS
        token_t t;
        while (get_next_token(&t) != EOF) {

                printf("[%s]", token_names[t.type]);
                switch (t.type) {
                case ID:
                case SPECIAL_ID:
                case STRING_LITERAL:
                        printf("[%s]\n", t.string_value);
                        break;
                case INT_LITERAL:
                        printf("[%d]\n", t.int_value);
                        break;

                case DOUBLE_LITERAL:
                        printf("[%g]\n", t.double_value);
                        break;
                default:
                        printf("\n");
                        break;
                }
                if (t.type == ID || t.type == SPECIAL_ID || t.type == STRING_LITERAL) {
                        free(t.string_value);
                }
        }
        rewind(file);
        #endif

        printf("\x1b[32mFIRST PASS\n\x1b[0m");
        if (parse(file) == 1) {
                printf("SYNTACTIC ANALYSIS\tOK\n");
                printf("\x1b[32mSECOND PASS\n\x1b[0m");
                //rewind(file);
                parse(file);
        } else {
                // chyba 2
                printf("SYNTACTIC ANALYSIS\tFAILED\n");
        }

        printf("LEXICAL ANALYSIS\t%s\n", (parser_error_flag == 0) ? "OK" : "FAILED");


        fclose(file);

        return 0;
}
