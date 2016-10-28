#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* TODO

   <statement> -> if (<E>) <condition-list> <else>
   <statement> -> while (<E>) <condition-list>
   <else> -> epsilon
   <else> -> else <condition-list>
   <condition-list> -> {<statement-list>}
   <condition-list> -> <statement>


   parse expr


 */

token_t t;
FILE *file;
#define SYN_ERROR 0
#define OK 1

#define PSEUDO_E 66

int parse_class_element();
int parse_param();
int parse_value();
int parse_method_element();

int parse_expression() {

        /*if (get_next_token(&t, file) == INT_LITERAL) { // HACK!
                if (get_next_token(&t, file) == SEMICOLON) {
                        return OK;
                }
           } else if (t.type == SEMICOLON) {
                return OK;
           }*/
        // TODO

        while (t.type != SEMICOLON) {
                get_next_token(&t, file);
        }
        return OK;

}

//

int parse_return_value() {
        if (t.type == RETURN) {
                get_next_token(&t, file);
                if (t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) { // HACK expr
                        return parse_expression();
                } else if (t.type == SEMICOLON) {
                        return OK;
                }
        }

        return SYN_ERROR;
}


int parse_next_param_value() {
        if (t.type == COMMA) {
                get_next_token(&t, file);
                if (t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) { // EXPR HACK
                        get_next_token(&t, file);
                        if (t.type == RIGHT_ROUNDED_BRACKET) {
                                return OK;
                        } else if (t.type == COMMA) {
                                return parse_next_param_value();
                        }
                }
                /*if (t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) { // HACK expr
                        if(parse_expression()) {
                            get_next_token(&t, file);
                            if (t.type == RIGHT_ROUNDED_BRACKET) {
                                    return OK;
                            } else if (t.type == COMMA) {
                                    return parse_next_param_value();
                            }
                        }
                   }*/
        }
        if (t.type == RIGHT_ROUNDED_BRACKET) {
                return OK;
        }
        return SYN_ERROR;
}

int parse_param_value () {
        if (t.type == LEFT_ROUNDED_BRACKET) {
                get_next_token(&t, file);
                if (t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) { // EXPR HACK
                        get_next_token(&t, file);
                        if (t.type == COMMA) {
                                return parse_next_param_value();
                        } else if (t.type == RIGHT_ROUNDED_BRACKET) {
                                return OK;
                        }
                }
        } if (t.type == RIGHT_ROUNDED_BRACKET) {
                return OK;
        }

        return SYN_ERROR;
}

int parse_call_assign() {
        if (t.type == ID || t.type == SPECIAL_ID) {
                get_next_token(&t, file);
                if (t.type == ASSIGN) {
                        get_next_token(&t, file);
                        if (t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) {
                                return parse_expression();
                        } else if (t.type == SEMICOLON) {
                                return OK;
                        }
                }
        } if (t.type == LEFT_ROUNDED_BRACKET) {
                if (parse_param_value()) {
                        if (t.type == RIGHT_ROUNDED_BRACKET) {
                                if (get_next_token(&t, file) == SEMICOLON) {
                                        return OK;
                                }
                        }
                }
        }

        return SYN_ERROR;
}

int parse_statement() {
        if (t.type == SEMICOLON) {
                return OK;
        }
        else if (t.type == RETURN) {
                if(parse_return_value()) {
                        return OK;
                }
        }

        else if (t.type == ID || t.type == SPECIAL_ID) {
                if(parse_call_assign()) {
                        return OK;
                }

        } else if (t.type == IF) {
                printf("som tu\n");
        }
        return SYN_ERROR;
}

int parse_element_list() {
        if (t.type == RIGHT_CURVED_BRACKET) {
                return OK;
        }
        else if (t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF) {
                return parse_statement();
        } /*else if (t.type == LEFT_CURVED_BRACKET) {
                get_next_token(&t, file);
                if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF) {
                        if(parse_statement_list()) {
                                get_next_token(&t, file);
                                if (t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON ||t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == LEFT_CURVED_BRACKET || t.type == IF) {
                                        return parse_method_element();
                                }
                        }
                }
        }*/

        return SYN_ERROR;
}

int parse_statement_list() {
        if (t.type == RIGHT_CURVED_BRACKET) {
                return OK;
        } else if (t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF) {
                if (parse_statement()) {
                        get_next_token(&t, file);
                        if (t.type == RIGHT_CURVED_BRACKET || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF) {
                                return parse_statement_list();
                        }
                }
        } else if (t.type == LEFT_CURVED_BRACKET) {
                get_next_token(&t, file);
                if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF) {
                        if(parse_statement_list()) {
                                get_next_token(&t, file);
                                if (t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON ||t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == LEFT_CURVED_BRACKET || t.type == IF) {
                                        return parse_method_element();
                                }
                        }
                }
        }
        return SYN_ERROR;
}

int parse_method_element() {
        if (t.type == RIGHT_CURVED_BRACKET) {
                return OK;
        } else if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if (parse_param()) {
                        get_next_token(&t, file);
                        if (t.type == ASSIGN || t.type == SEMICOLON) {
                                if (parse_value()) {
                                        get_next_token(&t, file);
                                        if (t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON ||t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == LEFT_CURVED_BRACKET || t.type == IF) {
                                                return parse_method_element();
                                        }
                                }
                        }
                }
        } else if (t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF) {
                if(parse_element_list()) {
                        get_next_token(&t, file);
                        if (t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON ||t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == LEFT_CURVED_BRACKET || t.type == IF) {
                                return parse_method_element();
                        }
                }
        } else if (t.type == LEFT_CURVED_BRACKET) {
                get_next_token(&t, file);
                if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF) {
                        if(parse_statement_list()) {
                                get_next_token(&t, file);
                                if (t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON ||t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == LEFT_CURVED_BRACKET || t.type == IF) {
                                        return parse_method_element();
                                }
                        }
                }
        }
        return SYN_ERROR;

}

int parse_next_param() {
        if (t.type == RIGHT_ROUNDED_BRACKET) {
                return OK;
        } else if (t.type == COMMA) {
                get_next_token(&t, file);
                if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                        if (parse_param()) {
                                get_next_token(&t, file);
                                if (t.type== RIGHT_ROUNDED_BRACKET || t.type == COMMA) {
                                        return parse_next_param();
                                }
                        }
                }
        }

        return SYN_ERROR;
}

int parse_param_list() {
        get_next_token(&t, file);
        if (t.type == RIGHT_ROUNDED_BRACKET) {
                if (get_next_token(&t, file) == LEFT_CURVED_BRACKET) {
                        get_next_token(&t, file);
                        if (t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF) {
                                return parse_method_element();
                        }
                }
        } else if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if (parse_param()) {
                        get_next_token(&t, file);
                        if (t.type == LEFT_CURVED_BRACKET) {
                                get_next_token(&t, file);
                                if (t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF) {
                                        return parse_method_element();
                                }
                        } else if (t.type == COMMA || t.type == RIGHT_ROUNDED_BRACKET) {
                                if (parse_next_param()) {
                                        if (get_next_token(&t, file) == LEFT_CURVED_BRACKET) {
                                                get_next_token(&t, file);
                                                if (t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF) {
                                                        return parse_method_element();
                                                }
                                        }
                                }
                        }
                }
        }
        return SYN_ERROR;

}

int parse_method_declaration () {
        if (t.type == LEFT_ROUNDED_BRACKET) {
                if (parse_param_list()) {
                        if (get_next_token(&t, file) == STATIC) {
                                return parse_class_element();
                        } else {
                                return OK;
                        }
                }

        }
        return SYN_ERROR;
}

int parse_value() {
        if (t.type == ASSIGN) {
                if (parse_expression()) { /* parse exp */
                        printf(" %d\n", t.attr.int_value);
                        if (t.type == SEMICOLON) {
                                return OK;
                        }
                }
        } if (t.type == SEMICOLON) {
                return OK;
        }

        return SYN_ERROR;
}

int parse_declaration() {
        if (t.type == LEFT_ROUNDED_BRACKET) {

                if (parse_method_declaration ()) {
                        return parse_class_element();
                }
        } else if (t.type == RIGHT_ROUNDED_BRACKET) {
                if (get_next_token(&t, file) == STATIC) {
                        return parse_class_element();
                } else {
                        return OK;
                }
        }  else if (t.type == ASSIGN || t.type == SEMICOLON) {
                if (parse_value()) {
                        if (t.type == SEMICOLON) {
                                if (get_next_token(&t, file) == STATIC) {
                                        return parse_class_element();
                                } else {
                                        return OK;
                                }
                        }
                }
        }

        return SYN_ERROR;
}

int parse_param() {
        if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if (get_next_token(&t, file) == ID) {
                        printf("id %s = ", t.attr.string_value);
                        return OK;
                }
        }
        return SYN_ERROR;

}

int parse_declaration_element() {
        get_next_token(&t, file);
        if (t.type == VOID) {
                if (get_next_token(&t, file) == ID) {
                        if (get_next_token(&t, file) == LEFT_ROUNDED_BRACKET) {
                                return parse_method_declaration();
                        }
                }
        } else if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if(parse_param()) {
                        get_next_token(&t, file);
                        if (t.type == LEFT_ROUNDED_BRACKET || t.type == RIGHT_ROUNDED_BRACKET || t.type == ASSIGN || t.type == SEMICOLON) {
                                return parse_declaration();
                        } else if (t.type == COMMA) {
                                get_next_token(&t, file);
                                if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                                        return parse_next_param();
                                }
                        }
                }
        }

        return SYN_ERROR;
}


int parse_class_element() {
        if (t.type == RIGHT_CURVED_BRACKET) {
                return OK;
        }

        else if (t.type == STATIC) {
                return parse_declaration_element();
        }
        return SYN_ERROR;
}

int parse_class_list() {
        if (get_next_token(&t, file) == CLASS) {
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
        } else if (t.type == EOF) {
                return OK;
        } return SYN_ERROR;
}
int parse(FILE *source) {
        file = source;
        return parse_class_list();

}
