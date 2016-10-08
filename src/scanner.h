#ifndef SCANNER_H
#define SCANNER_H

#include "strings.h"

char *keywords[17] =
    { "boolean", "break", "class", "continue", "do", "double", "else", "false",
    "for", "if", "int", "return", "String", "static", "true", "void", "while"
};

char *token_names[31] =
    { "LEXICAL_ERROR", "ID", "KEYWORD", "INTEGER", "DOUBLE", "ADD", "SUB", "MUL", "DIV",
    "SEMICOLON", "LEFT_CURVED_BRACKET", "RIGHT_CURVED_BRACKET",
	"LEFT_ROUNDED_BRACKET",
    "RIGHT_ROUNDED_BRACKET", "ASSIGN", "LINE_COMMENT", "BLOCK_COMMENT",
	"LOGICAL_AND",
    "LOGICAL_OR", "COMMA", "NEG", "SIMPLE_QUOTE", "STRING_LITERAL", "COLON",
	"NOT_EQUAL",
    "LESS_EQUAL", "LESS", "GREATER_EQUAL", "GREATER", "EQUAL", "SPECIAL_ID"
};

typedef struct {
    int type;
    string attr;
} token;

enum tokens { LEXICAL_ERROR, ID, KEYWORD, INTEGER, DOUBLE, ADD, SUB, MUL, DIV, SEMICOLON,
    LEFT_CURVED_BRACKET, RIGHT_CURVED_BRACKET, LEFT_ROUNDED_BRACKET,
    RIGHT_ROUNDED_BRACKET, ASSIGN, LINE_COMMENT, BLOCK_COMMENT, LOGICAL_AND,
    LOGICAL_OR, COMMA, NEG, SIMPLE_QUOTE, STRING_LITERAL, COLON, NOT_EQUAL,
    LESS_EQUAL, LESS, GREATER_EQUAL, GREATER, EQUAL, SPECIAL_ID };

#endif				// SCANNER_H
