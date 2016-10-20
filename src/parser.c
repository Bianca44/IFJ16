#include "strings.h"
#include "scanner.h"

void parse_class(FILE *file); // will return error code
void parse_method(FILE *file);
void parse_param_list(FILE *file);

FILE* source = NULL;

void parse(FILE *file) {
        source = FILE;

}

token check_token(token *t, int expected_token) {
    token t;
    int type = get_next_token(&t, file);
    if (type == expected_token) {
        return t;
    } else {
        return NULL;
    }
}

void parse_class() {
    token t;
    if (check_token(&t, CLASS) == NULL) {
        // chyba
    }
    if (check_token(&t, ID) == NULL) {
        // chyba
    }

}

void parse_class_element() {
    token t;
    if (check_token(&t, STATIC) == NULL) {
        // chyba
    }

}

void parse_declaration_element() {
    token t;
    if (check_token(&t, CLASS) != NULL) {
        if (check_token(&t, CLASS) != NULL) {
            parse_method_declaration();
        }
    } else {
        parse_param();
        parse_declaration();
    }
}

void parse_param() {
    parse_data_type();
    if (check_token(&t, ID) == NULL) {
        // chyba
    }
}

void parse_method(FILE *file) {
        token t;
        parse_param_list(file); // foo(params)

        int type = get_next_token(&t, file);
        if (type != LEFT_CURVED_BRACKET) {
                printf("Missing {\n");
        }

        // ignore body for now

        while (type != RIGHT_CURVED_BRACKET) {
                type = get_next_token(&t, file);
        }

        if (type != RIGHT_CURVED_BRACKET) {
                printf("Missing }\n");
        }

}

void parse_param_list(FILE *file) {
        token t;
        int type = get_next_token(&t, file);
        if (type != LEFT_ROUNDED_BRACKET) {
                printf("Missing (\n");
        }

        while (1) {

                type = get_next_token(&t, file);
                if (type != INT && type != STRING && type != DOUBLE) {
                        printf("LP Missing data type\n");
                }

                type = get_next_token(&t, file);
                if (type != ID) {
                        printf("Missing param ID\n");
                }

                type = get_next_token(&t, file);

                if (type == RIGHT_ROUNDED_BRACKET) {
                        break;
                }

                if (type != COMMA) {
                        printf("Missing comma\n");
                        break;
                }
        }

}
