#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

token_t t;
FILE *file;
#define SYN_ERROR 0
#define OK 1

#define PSEUDO_E 66

int parse_class_element();
int parse_param();

int parse_expression() {

        if (get_next_token(&t, file) == INT_LITERAL) { // HACK!
                if (get_next_token(&t, file) == SEMICOLON) {
                        return OK;
                }
        } else if (t.type == SEMICOLON) {
                return OK;
        }
        return SYN_ERROR;

}

//

int parse_method_element() {
        if (t.type == LEFT_CURVED_BRACKET) {
                if (get_next_token(&t, file) == RIGHT_CURVED_BRACKET) {
                        return OK;
                }

        }

        return SYN_ERROR;

}

int parse_next_param() {
        if (get_next_token(&t, file) == RIGHT_ROUNDED_BRACKET) {
                return OK;
        }

        return SYN_ERROR;
}

int parse_param_list() {
        if (get_next_token(&t, file) == RIGHT_ROUNDED_BRACKET) {
                if (get_next_token(&t, file) == LEFT_CURVED_BRACKET) {
                        if (parse_method_element()) {
                                if (t.type == RIGHT_CURVED_BRACKET) {
                                        return OK;

                                }
                        }
                }
        } else if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if (parse_param()) {
                        return parse_method_element();


                }
        }
        return SYN_ERROR;

}

int parse_method_declaration () {
        if (t.type == LEFT_ROUNDED_BRACKET) {
                if (parse_param_list()) {
                        if (t.type == RIGHT_CURVED_BRACKET) {
                                if (get_next_token(&t, file) == STATIC) {
                                        return parse_class_element();
                                } else {
                                        return OK;
                                }
                        }

                }

        } //
        return SYN_ERROR;
}

int parse_value() {
        if (t.type == ASSIGN) {
                if (parse_expression()) { /* parse exp */
                        printf(" %d\n", t.attr.int_value);
                        if (t.type == SEMICOLON) {
                                if (get_next_token(&t, file) == STATIC) {
                                        return parse_class_element();
                                } else {
                                        return OK;
                                }
                        }
                }
        } else if (t.type == SEMICOLON) {
                if (get_next_token(&t, file) == STATIC) {
                        return parse_class_element();
                } else if (t.type == RIGHT_CURVED_BRACKET) {
                        return OK;

                }
        }
        return SYN_ERROR;
}

int parse_declaration() {
        if (get_next_token(&t, file) == LEFT_ROUNDED_BRACKET) {

                if (parse_method_declaration ()) {
                        return parse_class_element();
                }
        } else if (t.type == RIGHT_ROUNDED_BRACKET) {
                if (get_next_token(&t, file) == STATIC) {
                        return parse_class_element();
                } else {
                        return OK;
                }
        }

        else {
                return parse_value();
        }

        return SYN_ERROR;
}

int parse_param() {
        if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if (get_next_token(&t, file) == ID) {
                        printf("id %s = ", t.attr.string_value);
                        if (parse_declaration()) {
                                return OK;
                        }
                }

        }

        return SYN_ERROR;

}

int parse_declaration_element() {
        if (get_next_token(&t, file) == VOID) {
                if (get_next_token(&t, file) == ID) {
                        if (get_next_token(&t, file) == LEFT_ROUNDED_BRACKET) {
                                return parse_method_declaration();
                        }
                }
        } else {
                return parse_param();
        }

        return SYN_ERROR;
}


int parse_class_element() {
        if (t.type == RIGHT_CURVED_BRACKET) {
                return OK;
        }

        else if (t.type == STATIC) {
                if (parse_declaration_element()) {
                        return OK;
                }
        }
        return SYN_ERROR;
}

int parse_class_list() {
        if (t.type == CLASS) {
                if (get_next_token(&t, file) == ID) {
                        if (get_next_token(&t, file) == LEFT_CURVED_BRACKET) {
                                get_next_token(&t, file);
                                if (parse_class_element()) {
                                        if (t.type == RIGHT_CURVED_BRACKET) {
                                                if (get_next_token(&t, file) == CLASS) {
                                                        return parse_class_list();
                                                } else if (t.type == EOF) {
                                                        return OK;
                                                } else {
                                                        return SYN_ERROR;
                                                }
                                        }
                                }
                        }
                }
        }

        return SYN_ERROR;
}
int parse(FILE *source) {
        file = source;
        get_next_token(&t, file);
        if (t.type != LEXICAL_ERROR) {
                return parse_class_list();
        } else if (t.type == EOF) {
                return OK;
        } else {
                return SYN_ERROR;
        }

}
