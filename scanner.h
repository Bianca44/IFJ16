#ifndef SCANNER_H
#define SCANNER_H

char *keywords[17] = { "boolean", "break", "class", "continue", "do", "double", "else", "false", "for", "if", "int", "return", "String", "static", "true", "void", "while"};

char *token_names[31] = { "ID", "KEYWORD", "NUMBER", "DOUBLE", "ADD", "SUB", "MUL", "DIV", "SEMICOLON",  "LEFT_CURVED_BRACKET", "RIGHT_CURVED_BRACKET", "LEFT_ROUNDED_BRACKET", "RIGHT_ROUNDED_BRACKET",  "ASSIGN",  "LINE_COMMENT", "BLOCK_COMMENT",  "LOGICAL_AND", "LOGICAL_OR", "COMMA", "NEG", "SIMPLE_QUOTE", "STRING_LITERAL",  "COLON", "NOT_EQUAL", "LESS_EQUAL", "LESS", "GREATER_EQUAL", "GREATER",  "EQUAL", "SPECIAL_ID", "LEXICAL_ERROR"};

typedef struct {
        int type;       // token type
        char attr[256]; // tmp 256
} Token;

enum tokens{
        ID,
        KEYWORD,
        NUMBER,
        DOUBLE,
        ADD,
        SUB,
        MUL,
        DIV,
        SEMICOLON,

        LEFT_CURVED_BRACKET,
        RIGHT_CURVED_BRACKET,
        LEFT_ROUNDED_BRACKET,
        RIGHT_ROUNDED_BRACKET,

        ASSIGN,

        LINE_COMMENT,
        BLOCK_COMMENT,

        LOGICAL_AND,
        LOGICAL_OR,
        COMMA,
        NEG,
        SIMPLE_QUOTE,
        STRING_LITERAL,

        COLON,
        NOT_EQUAL,
        LESS_EQUAL,
        LESS,
        GREATER_EQUAL,
        GREATER,

        EQUAL,
        SPECIAL_ID, /* plne kvatifikovany */
        LEXICAL_ERROR

};

#endif // SCANNER_H
