#ifndef SCANNER_H
#define SCANNER_H

#include "strings.h"

#define LONGEST_KEYWORD 8
#define KEYWORDS_COUNT 17
#define TOKENS_COUNT 44
#define KEYWORD_TOKENS_OFFSET (TOKENS_COUNT - KEYWORDS_COUNT)

char *keywords[KEYWORDS_COUNT] = { "boolean", "break", "class", "continue", "do", "double", "else", "false",
                                   "for", "if", "int", "return", "String", "static", "true", "void", "while" };

char *token_names[TOKENS_COUNT] = { "LEXICAL_ERROR", "ID", "INT_LITERAL", "DOUBLE_LITERAL", "ADD", "SUB", "MUL",
                                    "DIV", "SEMICOLON", "LEFT_CURVED_BRACKET", "RIGHT_CURVED_BRACKET",
                                    "LEFT_ROUNDED_BRACKET", "RIGHT_ROUNDED_BRACKET", "ASSIGN", "LOGICAL_AND",
                                    "LOGICAL_OR", "COMMA", "NEG",  "STRING_LITERAL", "COLON",
                                    "NOT_EQUAL", "LESS_EQUAL", "LESS", "GREATER_EQUAL", "GREATER", "EQUAL",
                                    "SPECIAL_ID", "BOOLEAN", "BREAK", "CLASS", "CONTINUE", "DO", "DOUBLE", "ELSE",
                                    "FALSE", "FOR", "IF", "INT", "RETURN", "STRING", "STATIC", "TRUE", "VOID", "WHILE" };

typedef struct {
        int type;
        union {
                char * string_value;
                int int_value;
                double double_value;
        } attr;
} token_t;

int get_next_token (token_t * t, FILE * file);
int init_scanner(char *filename);
int save_token(token_t *t, int type, string_t *attr);
int detect_keyword(string_t *str);

enum states { SPACE, IDENTIFICATOR, NUMBER, NUM_DOUBLE, SCIENTIC_DOUBLE, SLASH, COMMENT, EXCLAMATION, COMPARE_LESS,  COMPARE_GREATER,
              COMPARE_ASSIGN, QUALIFIED_ID, LITERAL, QUALIFIED_ID_END, LINE_COMMENT_END, DOUBLE_END, BLOCK_COMMENT_END, SIMPLE_DOUBLE, SCIENTIC_DOUBLE_EXP, OR, AND };

enum tokens { LEXICAL_ERROR, ID, INT_LITERAL, DOUBLE_LITERAL, ADD, SUB, MUL, DIV, SEMICOLON, LEFT_CURVED_BRACKET,
              RIGHT_CURVED_BRACKET, LEFT_ROUNDED_BRACKET, RIGHT_ROUNDED_BRACKET, ASSIGN, LOGICAL_AND, LOGICAL_OR,
              COMMA, NEG, STRING_LITERAL, COLON, NOT_EQUAL, LESS_EQUAL, LESS, GREATER_EQUAL,
              GREATER, EQUAL, SPECIAL_ID, BOOLEAN, BREAK, CLASS, CONTINUE, DO, DOUBLE, ELSE, FALSE, FOR, IF,
              INT, RETURN, STRING, STATIC, TRUE, VOID, WHILE };

#endif // SCANNER_H
