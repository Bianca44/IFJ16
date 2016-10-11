#include "strings.h"
#include "scanner.h"

void parse_class(FILE *file); // will return error code
void parse_method(FILE *file);
void parse_param_list(FILE *file);

void parse(FILE *file) {
        token t;
        int state = 0;
        int type = get_next_token(&t, file);

        //  Useless automat for now.
        switch (state) {
        case 0:     // parse class
                while (type == CLASS) {
                        parse_class(file);
                        type = get_next_token(&t, file);
                }

                printf("AST created, all is OK\nCheck previous lines for error.\n");
                break;

        default: break;
        }

}

void parse_class(FILE *file) {

        token t;
        int type = get_next_token(&t, file);
        if (type != ID) {
                printf("No CLASS ID\n");
        }
        type = get_next_token(&t, file);
        if (type != LEFT_CURVED_BRACKET) {
                printf("Missing {\n");
        }

        while (1) {

                type = get_next_token(&t, file);

                if (type == RIGHT_CURVED_BRACKET) {
                        break; // Koniec classy

                }

                if (type != STATIC) {
                        printf("No STATIC\n");
                        break;
                }

                type = get_next_token(&t, file);

                if (type == VOID) {
                        type = get_next_token(&t, file);
                        if (type != ID) {
                                printf("No METHOD ID\n");
                        }
                        parse_method(file);
                }

                if (type != INT && type !=  DOUBLE) {
                        printf("Bad return type / data type\n");
                        break;
                }

                type = get_next_token(&t, file);
                if (type != ID) {
                        printf("No METHOD ID\n");
                }

                type = get_next_token(&t, file);
                if (type != ASSIGN) {
                        parse_method(file);
                } else {

                        //parse_expr(file);
                        //printf("hi\n");
                        break;
                }
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
