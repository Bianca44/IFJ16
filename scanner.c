// test
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "ial.h"
#include "tokens_table.h"
#include <string.h>
#include "strings.h"


// enum pre stavy automatu

typedef struct {
    int type;            // token type
    union {
    int value_int;
    double value_double;
    string value_string;
    };
} TToken;

// Todo
int is_keyword(char *str) {
    return 0;
}

int get_next_token(FILE * file) {

    int c;
    int state = 0;

    int octal_counter = 0;

    char str[256] = { '\0' };    // rework to use string
    int i = 0;

    while (1) {
    c = fgetc(file);
    if (c == EOF) {
        return c;
    }

    switch (state) {
    case 0:
        if (isspace(c)) {
        state = 0;    // stav medzier
        } else {
        if (isalpha(c)) {
            state = 1;    // mozno string alebo cislo
            ungetc(c, file);
        } else if (isdigit(c)) {
            state = 2;
            ungetc(c, file);
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
        } else if (c == ':') { /* < >  8strana pdf*/
            return COLON;
        } else if (c == '/') {
            state = 5;
        } else if (c == '*') {
            state = 6;
        } else if (c == '\\') {
            str[i++] = '\\';
            state = 7;
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
            printf("assign\n");
            return ASSIGN;
        } else {
            printf(" nieco ine ");
            return LEXICAL_ERROR;
        }

        }
        break;

    case 1:
        if (isalnum(c)) {
        str[i++] = c;
        } else {
        printf("string: %s \n", str);
        ungetc(c, file);
        str[i] = '\0';
        state = 0;
        return ID;
        }
        break;

    case 2:
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

    case 3 /* maybe float */ :
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

    case 4 /* with e */ :
        if (isdigit(c)) {
        str[i++] = c;
        } else if (c == '-' || c == '+') {
        str[i++] = c;

        } else {
        printf("double: %s \n", str);
        ungetc(c, file);
        str[i] = '\0';
        state = 0;
        return DOUBLE;    /* TODO DOUBLE */
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
        return MUL;
        }
        break;

    case 7:

        state = 0;
        if (c == 'n') {
        printf("\\n\n");
        return 123;
        } else if (c == 't') {
        printf("\\t\n");
        return 124;
        } else if (c == '\\') {
        printf(" dvojite slash\n");
        return 125;
        } else if (isalnum(c)) {
            state = 0;
            ungetc(c, file);
            return LEXICAL_ERROR; /* napr. \p */
        } else if (isdigit(c)) {
            state = 8;
            ungetc(c, file);
        } else {
            return BACKSLASH;
        }
        break;

    case 8:

        if (isdigit(c)) {
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
            octal_counter = 0;
            ungetc(c, file);
            return BACKSLASH;
            }

        } else {
            state = 0;
            octal_counter = 0;
            ungetc(c, file);
            printf("octal: %s \n", str);
            return OCTAL;
        }
        } else {
        state = 0;
        octal_counter = 0;
        ungetc(c, file);
        printf("octalovska chyba \n");
        return LEXICAL_ERROR;
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
