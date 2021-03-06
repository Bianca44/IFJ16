/*
 * Project: Interpret for IFJ16
 * Date: November 2016, FIT VUT
 * Team: 026
 * Author: Dávid Bolvanský, xbolva00
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "strings.h"
#include "scanner.h"
#include "parser.h"
#include "memory_constants.h"

/* Linked list of constants in an interpreted code */
extern constant_t *mem_constants;

/* Source file */
FILE *file;

/* List of keywords */
char *keywords[KEYWORDS_COUNT] = { "boolean", "break", "class", "continue", "do", "double", "else", "false",
                                   "for", "if", "int", "return", "String", "static", "true", "void", "while"};


/* Saves the token. Frees the attr part in case we do not need it */
int save_token(token_t * t, int type, string_t * attr) {
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

/* Checks if a certain indentificator is not a keyword. */
int detect_keyword(string_t * str) {
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

/* Checks if fully qualified identificator does not contain any keyword */
int is_special_id_ok(string_t * str) {
        char *special_id = copy_string(str->data);
        char *delimeter = ".";
        char *class = strtok(special_id, delimeter);
        char *method = strtok(NULL, delimeter);

        for (int i = 0; i < KEYWORDS_COUNT; i++) {
                if (strcmp(keywords[i], class) == 0 || strcmp(keywords[i], method) == 0) {
                        return 0;
                }
        }
        free(special_id);
        return 1;
}

/* Gets next token from the input file */
int get_next_token(token_t * t) {

        int c = 0;
        int state = 0;
        int octal_number_length = 0;
        char octal_escape[OCTAL_STRING_SIZE] = { '\0' };

        string_t s;

        while (1) {
                c = fgetc(file);
                if (c == EOF) {
                        if (state > SPACE && state != LINE_COMMENT_END) {
                                return save_token(t,LEXICAL_ERROR, NULL);
                        }
                        else {
                                return save_token(t,  EOF, NULL);
                        }
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
                        } else if (c == 'e' || c == 'E') {
                                append_char(&s, c);
                                state = SCIENTIC_DOUBLE;
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
                                ungetc(c, file);
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
                        } else if (c == '\\') {
                                state = LITERAL_SLASH;
                        } else if (c <= 31) {
                                ungetc(c, file);
                                return save_token(t, LEXICAL_ERROR, NULL);
                        } else {
                                append_char(&s, c);
                        }
                        break;

                case LITERAL_SLASH:
                        if (isdigit(c)) {
                                if (octal_number_length < 3) {
                                        int digit = c - '0';
                                        if (digit > 7) {
                                                ungetc(c, file);
                                                return save_token(t, LEXICAL_ERROR, NULL);
                                        }

                                        octal_escape[octal_number_length] = c;
                                        octal_number_length++;
                                        if (octal_number_length == 3) {
                                                octal_escape[octal_number_length] = '\0';
                                                int octal_num = atoi(octal_escape);
                                                if (octal_num >= 1 && octal_num <= 377) {
                                                        int c = strtol(octal_escape, NULL, 8);
                                                        append_char(&s, c);
                                                        octal_number_length = 0;
                                                        octal_escape[octal_number_length] = '\0';
                                                        state = LITERAL;
                                                } else {
                                                        ungetc(c, file);
                                                        return save_token(t, LEXICAL_ERROR, NULL);
                                                }
                                        }
                                }
                        } else if (c == '"' || c == 'n' || c == 't' || c == '\\') {
                                if (octal_number_length == 0) {
                                        if (c == 'n') {
                                                c = '\n';
                                        } else if (c == 't') {
                                                c = '\t';
                                        } else if (c == '\\') {
                                                c = '\\';
                                        } else if (c == '"') {
                                                c = '\"';
                                        }
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
                                if (is_special_id_ok(&s)) {
                                        return save_token(t, SPECIAL_ID, &s);
                                } else {
                                        ungetc(c, file);
                                        return save_token(t, LEXICAL_ERROR, NULL);
                                }
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
                        } else {
                                state = COMMENT;
                        }
                        break;

                case SIMPLE_DOUBLE:
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

                case SCIENTIC_DOUBLE_EXP:
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

/* Inits the scanner, sets file in the scanner */
FILE *init_scanner(char *filename) {
        file = fopen(filename, "r");
        /* for debug */
        // file = stdin;
        return file;
}
