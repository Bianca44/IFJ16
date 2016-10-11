#ifndef SCANNER_H
#define SCANNER_H

#include "strings.h"

#define LONGEST_KEYWORD 8
#define KEYWORDS_COUNT 17
#define TOKENS_COUNT 46
#define KEYWORD_TOKENS_OFFSET (TOKENS_COUNT - KEYWORDS_COUNT)

char *keywords[KEYWORDS_COUNT] = { "boolean", "break", "class", "continue", "do", "double", "else", "false",
                                   "for", "if", "int", "return", "String", "static", "true", "void", "while" };

char *token_names[TOKENS_COUNT] = { "LEXICAL_ERROR", "ID", "KEYWORD", "INTEGER", "FLOAT", "ADD", "SUB", "MUL",
                                    "DIV", "SEMICOLON", "LEFT_CURVED_BRACKET", "RIGHT_CURVED_BRACKET",
                                    "LEFT_ROUNDED_BRACKET", "RIGHT_ROUNDED_BRACKET", "ASSIGN", "LOGICAL_AND",
                                    "LOGICAL_OR", "COMMA", "NEG", "SIMPLE_QUOTE", "STRING_LITERAL", "COLON",
                                    "NOT_EQUAL", "LESS_EQUAL", "LESS", "GREATER_EQUAL", "GREATER", "EQUAL",
                                    "SPECIAL_ID", "BOOLEAN", "BREAK", "CLASS", "CONTINUE", "DO", "DOUBLE", "ELSE",
                                    "FALSE", "FOR", "IF", "INT", "RETURN", "STRING", "STATIC", "TRUE", "VOID", "WHILE" };

typedef struct {
        int type;
        string attr;
} token;

enum tokens { LEXICAL_ERROR, ID, KEYWORD, INTEGER, FLOAT, ADD, SUB, MUL, DIV, SEMICOLON, LEFT_CURVED_BRACKET,
              RIGHT_CURVED_BRACKET, LEFT_ROUNDED_BRACKET, RIGHT_ROUNDED_BRACKET, ASSIGN, LOGICAL_AND, LOGICAL_OR,
              COMMA, NEG, SIMPLE_QUOTE, STRING_LITERAL, COLON, NOT_EQUAL, LESS_EQUAL, LESS, GREATER_EQUAL,
              GREATER, EQUAL, SPECIAL_ID, BOOLEAN, BREAK, CLASS, CONTINUE, DO, DOUBLE, ELSE, FALSE, FOR, IF,
              INT, RETURN, STRING, STATIC, TRUE, VOID, WHILE };


int get_next_token (token * t, FILE * file);

#endif // SCANNER_H
