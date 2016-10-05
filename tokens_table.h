#ifndef TOKENS_TABLE_H
#define TOKENS_TABLE_H

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

#endif // TOKENS_TABLE_H
