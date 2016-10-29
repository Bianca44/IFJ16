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
char *t_names[TOKENS_COUNT] = { "LEXICAL_ERROR", "ID", "INT_LITERAL", "DOUBLE_LITERAL", "ADD", "SUB", "MUL",  // 6
                                "DIV", "SEMICOLON", "LEFT_CURVED_BRACKET", "RIGHT_CURVED_BRACKET",      // 10
                                "LEFT_ROUNDED_BRACKET", "RIGHT_ROUNDED_BRACKET", "ASSIGN", "LOGICAL_AND",
                                "LOGICAL_OR", "COMMA", "NEG",  "STRING_LITERAL", "COLON",      // 16
                                "NOT_EQUAL", "LESS_EQUAL", "LESS", "GREATER_EQUAL", "GREATER", "EQUAL",
                                "SPECIAL_ID", "BOOLEAN", "BREAK", "CLASS", "CONTINUE", "DO", "DOUBLE", "ELSE",
                                "FALSE", "FOR", "IF", "INT", "RETURN", "STRING", "STATIC", "TRUE", "VOID", "WHILE" };


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

        // TODO

        while (t.type != SEMICOLON && t.type != RIGHT_ROUNDED_BRACKET && t.type != COMMA) {
                get_next_token(&t, file);
        }
        return OK;

}

int parse_return_value() {
        if (t.type == RETURN) {
                get_next_token(&t, file);
                if (t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) { // HACK expr
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
                if (t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) { // EXPR HACK
                        get_next_token(&t, file);
                        if (t.type == RIGHT_ROUNDED_BRACKET) {
                                return OK;
                        } else if (t.type == COMMA) {
                                return parse_next_param_value();
                        }
                }
        }
        if (t.type == RIGHT_ROUNDED_BRACKET) {
                return OK;
        }
        return SYN_ERROR;
}

int parse_param_value () {
        if (t.type == LEFT_ROUNDED_BRACKET) {
                get_next_token(&t, file);
                if (t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) { // EXPR HACK
                        if (parse_expression()) { // just for ifj16.print
                                if (t.type == COMMA) {
                                        return parse_next_param_value();
                                } else if (t.type == RIGHT_ROUNDED_BRACKET) {
                                        return OK;
                                }
                        }
                }
        } else if (t.type == RIGHT_ROUNDED_BRACKET) {
                return OK;
        }

        return SYN_ERROR;
}

int parse_call_assign() {
        if (t.type == ID || t.type == SPECIAL_ID) {
                get_next_token(&t, file);
                if (t.type == ASSIGN) {
                        get_next_token(&t, file);
                        if (t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) {
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

int parse_statement();
int parse_statement_list();

int parse_condition_list() {
        get_next_token(&t, file);
        if (t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                if (parse_statement()) {
                        return OK;
                }
        } else if (t.type == LEFT_CURVED_BRACKET) {
                get_next_token(&t, file);
                if (parse_statement_list()) {
                        get_next_token(&t, file);
                        return OK;
                }
        } else if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if (parse_param()) {
                        get_next_token(&t, file);
                        if (t.type == ASSIGN || t.type == SEMICOLON) {
                                return parse_value();
                        }
                }
        }
        return SYN_ERROR;
}

int parse_else() {
        if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                return OK;
        } if (t.type == ELSE) {
                if(parse_condition_list()) {
                        if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                return OK;
                        }
                }
        }
        return SYN_ERROR;
}

int parse_statement() {
        if (t.type == SEMICOLON) {
                get_next_token(&t, file);
                if (t.type == ELSE || t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                        return OK;
                }
        }
        else if (t.type == RETURN) {
                if(parse_return_value()) {
                        get_next_token(&t, file);
                        if (t.type == ELSE || t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                return OK;
                        }
                }
        }

        else if (t.type == ID || t.type == SPECIAL_ID) {
                if(parse_call_assign()) {
                        get_next_token(&t, file);
                        if (t.type == ELSE || t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                return OK;
                        }
                }

        } else if (t.type == IF) {
                if (get_next_token(&t, file) == LEFT_ROUNDED_BRACKET) {
                        if (parse_expression()) {
                                if (parse_condition_list()) {
                                    return parse_else();
                                }

                        }
                }
        } else if (t.type == WHILE) {
                if (get_next_token(&t, file) == LEFT_ROUNDED_BRACKET) {
                        if (parse_expression()) {
                                return parse_condition_list();
                        }
                }
        }
        return SYN_ERROR;
}

int parse_element_list() {
        if (t.type == RIGHT_CURVED_BRACKET) {
                return OK;
        }
        else if (t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                return parse_statement();
        }
        else if (t.type == LEFT_CURVED_BRACKET) {
                get_next_token(&t, file);
                if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                        if(parse_statement_list()) {
                                get_next_token(&t, file);
                                if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                        return OK;
                                }
                        }
                }
        }

        return SYN_ERROR;
}

int parse_statement_list() {
        if (t.type == RIGHT_CURVED_BRACKET) {
                return OK;
        } else if (t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                if (parse_statement()) {
                        if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                return parse_statement_list();
                        }
                }
        } else if (t.type == LEFT_CURVED_BRACKET) {
                get_next_token(&t, file);
                if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                        if(parse_statement_list()) {
                                get_next_token(&t, file);
                                if (t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON ||t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == LEFT_CURVED_BRACKET || t.type == IF || t.type == WHILE) {
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
                                        if (t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON ||t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == LEFT_CURVED_BRACKET || t.type == IF || t.type == WHILE) {
                                                return parse_method_element();
                                        }
                                }
                        }
                }
        } else if (t.type == LEFT_CURVED_BRACKET || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                if(parse_element_list()) {
                        if (t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON ||t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == LEFT_CURVED_BRACKET || t.type == IF || t.type == WHILE) {
                                return parse_method_element();
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
                        if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                return parse_method_element();
                        }
                }
        } else if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if (parse_param()) {
                        get_next_token(&t, file);
                        if (t.type == LEFT_CURVED_BRACKET) {
                                get_next_token(&t, file);
                                if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                        return parse_method_element();
                                }
                        } else if (t.type == COMMA || t.type == RIGHT_ROUNDED_BRACKET) {
                                if (parse_next_param()) {
                                        if (get_next_token(&t, file) == LEFT_CURVED_BRACKET) {
                                                get_next_token(&t, file);
                                                if (t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
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
                if (parse_expression()) {
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
        if (t.type == CLASS) {
                if (get_next_token(&t, file) == ID) {
                        if (get_next_token(&t, file) == LEFT_CURVED_BRACKET) {
                                get_next_token(&t, file);
                                if (parse_class_element()) {
                                        if (t.type == RIGHT_CURVED_BRACKET) {
                                                get_next_token(&t, file);
                                                if (t.type == CLASS) {
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
        get_next_token(&t, file);
        if (t.type == CLASS || t.type == EOF) {
                return parse_class_list();
        } else {
                return SYN_ERROR;
        }

}
