#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scanner.h"
#include "parser.h"

char *t_names[TOKENS_COUNT] = { "LEXICAL_ERROR", "ID", "INT_LITERAL", "DOUBLE_LITERAL", "ADD", "SUB", "MUL",
                                "DIV", "SEMICOLON", "LEFT_CURVED_BRACKET", "RIGHT_CURVED_BRACKET",
                                "LEFT_ROUNDED_BRACKET", "RIGHT_ROUNDED_BRACKET", "ASSIGN", "LOGICAL_AND",
                                "LOGICAL_OR", "COMMA", "NEG",  "STRING_LITERAL", "NOT_EQUAL", "LESS_EQUAL",
                                "LESS", "GREATER_EQUAL", "GREATER", "EQUAL", "SPECIAL_ID", "BOOLEAN", "BREAK",
                                "CLASS", "CONTINUE", "DO", "DOUBLE", "ELSE", "FALSE", "FOR", "IF", "INT", "RETURN",
                                "STRING", "STATIC", "TRUE", "VOID", "WHILE" };


token_t t;
FILE *file;
int parser_error_flag = 0; // no error
#define PARSING_FAILED 1
#define PARSED_OK 0
#define LEXICAL_ANALYSIS_ERROR 1
#define SYNTACTIC_ANALYSIS_ERROR 2

int get_token() {
    get_next_token(&t, file);
    if (t.type == LEXICAL_ERROR) {
        parser_error_flag = LEXICAL_ANALYSIS_ERROR;
    }
    return t.type;
}

int parse_expression() {


        // ID co je funkcia alebo premenna
        // ak funkcia, parsuje takto:

        /*if (t.type == ID || t.type == SPECIAL_ID) {
                get_token();
                if (t.type == LEFT_ROUNDED_BRACKET) {
                        if (parse_param_value()) {
                                get_token();
                                if (t.type == SEMICOLON) {
                                    return PARSED_OK;
                                }
                        }
                }
           }*/

        while (t.type != SEMICOLON) {
                // ulozit
                get_token();
        }
        return PARSED_OK;

}

int parse_param_expression() {
        // riesi aj if (exp)
        // hlavne pre ifj16.print(str+str)
        // ale teda aj pre vsetky funkcie foo (exp, next_param);
        while (t.type != RIGHT_ROUNDED_BRACKET && t.type != COMMA) {
                // ulozit
                get_token();
        }
        return PARSED_OK;

}

int parse_return_value() {
        if (t.type == RETURN) {
                get_token();
                if (t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) { // HACK expr
                        return parse_expression();
                } else if (t.type == SEMICOLON) {
                        return PARSED_OK;
                }
        }

        return PARSING_FAILED;
}


int parse_next_param_value() {
        if (t.type == COMMA) {
                get_token();
                if (t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) { // EXPR HACK
                        get_token();
                        if (t.type == RIGHT_ROUNDED_BRACKET) {
                                return PARSED_OK;
                        } else if (t.type == COMMA) {
                                return parse_next_param_value();
                        }
                }
        }
        if (t.type == RIGHT_ROUNDED_BRACKET) {
                return PARSED_OK;
        }
        return PARSING_FAILED;
}

int parse_param_value () {
        if (t.type == LEFT_ROUNDED_BRACKET) {
                get_token();
                if (t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) { // EXPR HACK
                        if (parse_param_expression()) { // just for ifj16.print
                                if (t.type == COMMA) {
                                        return parse_next_param_value();
                                } else if (t.type == RIGHT_ROUNDED_BRACKET) {
                                        return PARSED_OK;
                                }
                        }
                }
        } else if (t.type == RIGHT_ROUNDED_BRACKET) {
                return PARSED_OK;
        }

        return PARSING_FAILED;
}

int parse_call_assign() {
        if (t.type == ID || t.type == SPECIAL_ID) {
                get_token();
                if (t.type == ASSIGN) {
                        get_token();
                        if (t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) {
                                return parse_expression();
                        } else if (t.type == SEMICOLON) {
                                return PARSED_OK;
                        }
                }
        } if (t.type == LEFT_ROUNDED_BRACKET) {
                if (parse_param_value()) {
                        if (t.type == RIGHT_ROUNDED_BRACKET) {
                                if (get_token() == SEMICOLON) {
                                        return PARSED_OK;
                                }
                        }
                }
        }

        return PARSING_FAILED;
}

int parse_condition_list() {
        if (t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                if (parse_statement()) {
                        return PARSED_OK;
                }
        } else if (t.type == LEFT_CURVED_BRACKET) {
                get_token();
                if (parse_statement_list()) {
                        get_token();
                        if (t.type == ELSE || t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                return PARSED_OK;
                        }
                }
        } else if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if (parse_param()) {
                        get_token();
                        if (t.type == ASSIGN || t.type == SEMICOLON) {
                                return parse_value();
                        }
                }
        }
        return PARSING_FAILED;
}

int parse_else() {
        if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                return PARSED_OK;
        } if (t.type == ELSE) {
                get_token();
                if (t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE || t.type == LEFT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                        if(parse_condition_list()) {
                                if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                        return PARSED_OK;
                                }
                        }
                }
        }
        return PARSING_FAILED;
}

int parse_statement() {
        if (t.type == SEMICOLON) {
                get_token();
                if (t.type == ELSE || t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                        return PARSED_OK;
                }
        }
        else if (t.type == RETURN) {
                if(parse_return_value()) {
                        get_token();
                        if (t.type == ELSE || t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                return PARSED_OK;
                        }
                }
        }

        else if (t.type == ID || t.type == SPECIAL_ID) {
                if(parse_call_assign()) {
                        get_token();
                        if (t.type == ELSE || t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                return PARSED_OK;
                        }
                }

        } else if (t.type == IF) {
                if (get_token() == LEFT_ROUNDED_BRACKET) {
                        if (parse_param_expression()) {
                                get_token();
                                if (t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE || t.type == LEFT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                                        if (parse_condition_list()) {
                                                return parse_else();
                                        }
                                }

                        }
                }
        } else if (t.type == WHILE) {
                if (get_token() == LEFT_ROUNDED_BRACKET) {
                        if (parse_param_expression()) {
                                get_token();
                                if (t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE || t.type == LEFT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                                        return parse_condition_list();
                                }
                        }
                }
        }
        return PARSING_FAILED;
}

int parse_element_list() {
        if (t.type == RIGHT_CURVED_BRACKET) {
                return PARSED_OK;
        }
        else if (t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                return parse_statement();
        }
        else if (t.type == LEFT_CURVED_BRACKET) {
                get_token();
                if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                        if(parse_statement_list()) {
                                get_token();
                                if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                        return PARSED_OK;
                                }
                        }
                }
        }

        return PARSING_FAILED;
}

int parse_statement_list() {
        if (t.type == RIGHT_CURVED_BRACKET) {
                return PARSED_OK;
        } else if (t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                if (parse_statement()) {
                        if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                return parse_statement_list();
                        }
                }
        } else if (t.type == LEFT_CURVED_BRACKET) {
                get_token();
                if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                        if(parse_statement_list()) {
                                get_token();
                                if (t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON ||t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == LEFT_CURVED_BRACKET || t.type == IF || t.type == WHILE) {
                                        return parse_method_element();
                                }
                        }
                }
        }
        return PARSING_FAILED;
}

int parse_method_element() {
        if (t.type == RIGHT_CURVED_BRACKET) {
                return PARSED_OK;
        } else if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if (parse_param()) {
                        get_token();
                        if (t.type == ASSIGN || t.type == SEMICOLON) {
                                if (parse_value()) {
                                        get_token();
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
        return PARSING_FAILED;

}

int parse_next_param() {
        if (t.type == RIGHT_ROUNDED_BRACKET) {
                return PARSED_OK;
        } else if (t.type == COMMA) {
                get_token();
                if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                        if (parse_param()) {
                                get_token();
                                if (t.type== RIGHT_ROUNDED_BRACKET || t.type == COMMA) {
                                        return parse_next_param();
                                }
                        }
                }
        }

        return PARSING_FAILED;
}

int parse_param_list() {
        get_token();
        if (t.type == RIGHT_ROUNDED_BRACKET) {
                if (get_token() == LEFT_CURVED_BRACKET) {
                        get_token();
                        if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                return parse_method_element();
                        }
                }
        } else if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if (parse_param()) {
                        get_token();
                        if (t.type == LEFT_CURVED_BRACKET) {
                                get_token();
                                if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                        return parse_method_element();
                                }
                        } else if (t.type == COMMA || t.type == RIGHT_ROUNDED_BRACKET) {
                                if (parse_next_param()) {
                                        if (get_token() == LEFT_CURVED_BRACKET) {
                                                get_token();
                                                if (t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                                        return parse_method_element();
                                                }
                                        }
                                }
                        }
                }
        }
        return PARSING_FAILED;

}

int parse_method_declaration () {
        if (t.type == LEFT_ROUNDED_BRACKET) {
                if (parse_param_list()) {
                        if (get_token() == STATIC) {
                                return parse_class_element();
                        } else {
                                return PARSED_OK;
                        }
                }

        }
        return PARSING_FAILED;
}

int parse_value() {
        if (t.type == ASSIGN) {
                if (parse_expression()) {
                        if (t.type == SEMICOLON) {
                                return PARSED_OK;
                        }
                }
        } if (t.type == SEMICOLON) {
                return PARSED_OK;
        }

        return PARSING_FAILED;
}

int parse_declaration() {
        if (t.type == LEFT_ROUNDED_BRACKET) {
                if (parse_method_declaration ()) {
                        return parse_class_element();
                }
        } else if (t.type == RIGHT_ROUNDED_BRACKET) {
                if (get_token() == STATIC) {
                        return parse_class_element();
                } else {
                        return PARSED_OK;
                }
        }  else if (t.type == ASSIGN || t.type == SEMICOLON) {
                if (parse_value()) {
                        if (t.type == SEMICOLON) {
                                if (get_token() == STATIC) {
                                        return parse_class_element();
                                } else {
                                        return PARSED_OK;
                                }
                        }
                }
        }

        return PARSING_FAILED;
}

int parse_param() {
        if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if (get_token() == ID) {
                        return PARSED_OK;
                }
        }
        return PARSING_FAILED;

}

int parse_declaration_element() {
        get_token();
        if (t.type == VOID) {
                if (get_token() == ID) {
                        if (get_token() == LEFT_ROUNDED_BRACKET) {
                                return parse_method_declaration();
                        }
                }
        } else if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if(parse_param()) {
                        get_token();
                        if (t.type == LEFT_ROUNDED_BRACKET || t.type == RIGHT_ROUNDED_BRACKET || t.type == ASSIGN || t.type == SEMICOLON) {
                                return parse_declaration();
                        } else if (t.type == COMMA) {
                                get_token();
                                if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                                        return parse_next_param();
                                }
                        }
                }
        }

        return PARSING_FAILED;
}


int parse_class_element() {
        if (t.type == RIGHT_CURVED_BRACKET) {
                return PARSED_OK;
        }

        else if (t.type == STATIC) {
                return parse_declaration_element();
        }
        return PARSING_FAILED;
}

int parse_class_list() {
        if (t.type == CLASS) {
                if (get_token() == ID) {
                        if (get_token() == LEFT_CURVED_BRACKET) {
                                get_token();
                                if (parse_class_element()) {
                                        if (t.type == RIGHT_CURVED_BRACKET) {
                                                get_token();
                                                if (t.type == CLASS) {
                                                        return parse_class_list();
                                                } else if (t.type == EOF) {
                                                        return PARSED_OK;
                                                } else {
                                                        return PARSING_FAILED;
                                                }
                                        }
                                }
                        }
                }
        } else if (t.type == EOF) {
                return PARSED_OK;
        } return PARSING_FAILED;
}

int parse(FILE *source) {
        file = source;
        get_token();
        if (t.type == CLASS || t.type == EOF) {
                if (parse_class_list()) {
                    return PARSED_OK;
                } else {
                    return SYNTACTIC_ANALYSIS_ERROR;
                }
        } else {
                return SYNTACTIC_ANALYSIS_ERROR;
        }

        return SYNTACTIC_ANALYSIS_ERROR;

}
