#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "scanner.h"
#include "parser.h"
#include "symbol_table.h"
#include "strings.h"
#include "token_buffer.h"

char *t_names[TOKENS_COUNT] = { "LEXICAL_ERROR", "ID", "INT_LITERAL", "DOUBLE_LITERAL", "ADD", "SUB", "MUL",
                                "DIV", "SEMICOLON", "LEFT_CURVED_BRACKET", "RIGHT_CURVED_BRACKET",
                                "LEFT_ROUNDED_BRACKET", "RIGHT_ROUNDED_BRACKET", "ASSIGN", "LOGICAL_AND",
                                "LOGICAL_OR", "COMMA", "NEG",  "STRING_LITERAL", "NOT_EQUAL", "LESS_EQUAL",
                                "LESS", "GREATER_EQUAL", "GREATER", "EQUAL", "SPECIAL_ID", "BOOLEAN", "BREAK",
                                "CLASS", "CONTINUE", "DO", "DOUBLE", "ELSE", "FALSE", "FOR", "IF", "INT", "RETURN",
                                "STRING", "STATIC", "TRUE", "VOID", "WHILE" };


int parser_error_flag = 0; // no error
int function_offset = 0;
bool is_first_pass = true;
bool is_second_pass = false;
token_t t;
token_buffer_t token_buffer;
#define PARSE_ERROR 0
#define PARSED_OK 1
#define LEXICAL_ANALYSIS_ERROR 1
#define SYNTACTIC_ANALYSIS_ERROR 2

extern string_t param_data_types;
extern variable_t current_variable;
extern variable_t function_variable;
extern function_t current_function;


int get_token() {
        if (is_first_pass) {
                t.attr.string_value = NULL;
                get_next_token(&t);
                add_token_to_buffer(&token_buffer, &t);
        } else /* second pass */ {
                t = *get_next_token_buffer(&token_buffer);
        }
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

        if (t.type == SEMICOLON) {
                return PARSE_ERROR;
        }

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
                if (t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) {
                        if (current_function.data_type == VOID) {
                                printf("return E in VOID error\n");
                        }

                        return parse_expression();
                } else if (t.type == SEMICOLON) {
                        return PARSED_OK;
                }
        }

        return PARSE_ERROR;
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
        return PARSE_ERROR;
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

        return PARSE_ERROR;
}

int parse_call_assign() {
        if (t.type == ID || t.type == SPECIAL_ID) {
                get_token();
                if (t.type == ASSIGN) {
                        get_token();
                        if (t.type == SEMICOLON || t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) {
                                return parse_expression();
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

        return PARSE_ERROR;
}

int parse_condition_list() {
        if (t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                if (parse_statement()) {
                        return PARSED_OK;
                }
        } else if (t.type == LEFT_CURVED_BRACKET) {
                get_token();
                if (parse_statement_list()) {
                        get_token();
                        if (t.type == ELSE || t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                return PARSED_OK;
                        }
                }
        } else if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if (parse_param()) {
                        get_token();
                        if (t.type == ASSIGN || t.type == SEMICOLON) {
                                if (parse_value()) {
                                        get_token();
                                        if (t.type == ELSE || t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                                return PARSED_OK;
                                        }
                                }
                        }
                }
        }
        return PARSE_ERROR;
}

int parse_else() {
        if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                return PARSED_OK;
        } if (t.type == ELSE) {
                get_token();
                if (t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE || t.type == LEFT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                        if(parse_condition_list()) {
                                if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                        return PARSED_OK;
                                }
                        }
                }
        }
        return PARSE_ERROR;
}

int parse_statement() {
        if (t.type == RETURN) {
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
                        get_token();
                        if (t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) { // EXPR HACK
                                if (parse_param_expression()) {
                                        get_token();
                                        if (t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE || t.type == LEFT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                                                if (parse_condition_list()) {
                                                        return parse_else();
                                                }
                                        }

                                }
                        }
                }
        } else if (t.type == WHILE) {
                if (get_token() == LEFT_ROUNDED_BRACKET) {
                        get_token();
                        if (t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) { // EXPR HACK
                                if (parse_param_expression()) {
                                        get_token();
                                        if (t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE || t.type == LEFT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                                                return parse_condition_list();
                                        }
                                }
                        }
                }
        }
        return PARSE_ERROR;
}

int parse_element_list() {
        if (t.type == RIGHT_CURVED_BRACKET) {
                return PARSED_OK;
        }
        else if (t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                return parse_statement();
        }
        else if (t.type == LEFT_CURVED_BRACKET) {
                get_token();
                if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                        if(parse_statement_list()) {
                                get_token();
                                if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                        return PARSED_OK;
                                }
                        }
                }
        }

        return PARSE_ERROR;
}

int parse_statement_list() {
        if (t.type == RIGHT_CURVED_BRACKET) {
                return PARSED_OK;
        } else if (t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                if (parse_statement()) {
                        if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                return parse_statement_list();
                        }
                }
        } else if (t.type == LEFT_CURVED_BRACKET) {
                get_token();
                if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                        if(parse_statement_list()) {
                                get_token();
                                if (t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN ||t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == LEFT_CURVED_BRACKET || t.type == IF || t.type == WHILE) {
                                        return parse_method_element();
                                }
                        }
                }
        }
        return PARSE_ERROR;
}

int parse_method_element() {
        if (t.type == RIGHT_CURVED_BRACKET) {
                if (is_first_pass) {
                        printf("name=%s, ret_type=%d, data_types=%s, params_count=%d, local_vars_count=%d\n", current_function.id_name, current_function.data_type, current_function.param_data_types, current_function.params_count, current_function.local_vars_count);
                        if (!is_declared(current_function.id_name)) {
                                put_function_symbol_table(current_function.id_name, current_function.data_type, current_function.params_count, current_function.local_vars_count, current_function.param_data_types, current_function.symbol_table);
                        } else {
                                printf("FUNCTION REDECLARED\n");
                        }
                        current_function.local_vars_count = 0;
                        function_variable.offset = 0;
                }
                return PARSED_OK;
        } else if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if (is_first_pass) {
                        current_function.local_vars_count++;
                        function_variable.data_type = t.type;
                }
                if (parse_param()) {
                        if (is_first_pass) {
                                printf("local fun var .. name %s type %d offset %d\n", function_variable.id_name, function_variable.data_type, function_variable.offset);
                                if (!is_declared_in_function(current_function.symbol_table, function_variable.id_name)) {
                                        put_function_variable_symbol_table(current_function.symbol_table, function_variable.id_name, function_variable.data_type, function_variable.offset);
                                        function_variable.offset++;
                                } else {
                                        printf("VAR IN FUNC REDECLARED\n");
                                }
                        }
                        get_token();
                        if (t.type == ASSIGN || t.type == SEMICOLON) {
                                if (parse_value()) {
                                        get_token();
                                        if (t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == LEFT_CURVED_BRACKET || t.type == IF || t.type == WHILE) {
                                                return parse_method_element();
                                        }
                                }
                        }
                }
        } else if (t.type == LEFT_CURVED_BRACKET || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                if(parse_element_list()) {
                        if (t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == LEFT_CURVED_BRACKET || t.type == IF || t.type == WHILE) {
                                return parse_method_element();
                        }
                }
        }
        return PARSE_ERROR;

}

int parse_next_param() {
        if (t.type == RIGHT_ROUNDED_BRACKET) {
                return PARSED_OK;
        } else if (t.type == COMMA) {
                get_token();
                if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                        if (is_first_pass) {
                                append_param_data_types(t.type);
                                function_variable.data_type = t.type;
                        }
                        if (parse_param()) {
                                if (is_first_pass) {
                                        printf("local fun var .. name %s type %d offset %d\n", function_variable.id_name, function_variable.data_type, function_variable.offset);
                                        if (!is_declared_in_function(current_function.symbol_table, function_variable.id_name)) {
                                                put_function_variable_symbol_table(current_function.symbol_table, function_variable.id_name, function_variable.data_type, function_variable.offset);
                                                function_variable.offset++;
                                        } else {
                                                printf("VAR IN FUNC REDECLARED\n");
                                        }
                                }
                                get_token();
                                if (t.type== RIGHT_ROUNDED_BRACKET || t.type == COMMA) {
                                        return parse_next_param();
                                }
                        }
                }
        }

        return PARSE_ERROR;
}

int parse_param_list() {
        get_token();
        init_string(&param_data_types);
        if (t.type == RIGHT_ROUNDED_BRACKET) {
                if (get_token() == LEFT_CURVED_BRACKET) {
                        get_token();
                        if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                return parse_method_element();
                        }
                }
        } else if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if (is_first_pass) {
                        append_param_data_types(t.type);
                        function_variable.data_type = t.type;
                }
                if (parse_param()) {
                        if (is_first_pass) {
                                printf("local fun var .. name %s type %d offset %d\n", function_variable.id_name, function_variable.data_type, function_variable.offset);
                                if (!is_declared_in_function(current_function.symbol_table, function_variable.id_name)) {
                                        put_function_variable_symbol_table(current_function.symbol_table, function_variable.id_name, function_variable.data_type, function_variable.offset);
                                        function_variable.offset++;
                                } else {
                                        printf("VAR IN FUNC REDECLARED\n");
                                }
                        }

                        get_token();
                        if (t.type == LEFT_CURVED_BRACKET) {
                                get_token();
                                if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                        return parse_method_element();
                                }
                        } else if (t.type == COMMA || t.type == RIGHT_ROUNDED_BRACKET) {
                                if (parse_next_param()) {
                                        if (is_first_pass) {
                                                current_function.param_data_types = param_data_types.data;
                                                current_function.params_count = param_data_types.length;
                                        }
                                        if (get_token() == LEFT_CURVED_BRACKET) {
                                                get_token();
                                                if (t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                                        return parse_method_element();
                                                }
                                        }
                                }
                        }
                }
        }
        return PARSE_ERROR;

}

int parse_method_declaration () {
        if (t.type == LEFT_ROUNDED_BRACKET) {
                if (parse_param_list()) {
                        get_token();
                        if (t.type == STATIC || t.type == RIGHT_CURVED_BRACKET) {
                                return parse_class_element();
                        }
                }

        }
        return PARSE_ERROR;
}

int parse_value() {
        if (t.type == ASSIGN) {
                get_token();
                if (t.type == SEMICOLON || t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) {
                        return parse_expression();
                }
        } if (t.type == SEMICOLON) {
                return PARSED_OK;
        }

        return PARSE_ERROR;
}

int parse_declaration() {
        if (t.type == LEFT_ROUNDED_BRACKET) {
                return parse_method_declaration ();
        } else if (t.type == RIGHT_ROUNDED_BRACKET) {
                if (get_token() == STATIC) {
                        return parse_class_element();
                } else {
                        return PARSED_OK;
                }
        }  else if (t.type == ASSIGN || t.type == SEMICOLON) {
                if (is_first_pass) {
                        if (!is_declared(current_variable.id_name)) {
                                put_variable_symbol_table(current_variable.id_name, current_variable.data_type, -1);
                        } else {
                                printf("VAR REDECLARED\n");
                        }
                }

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

        return PARSE_ERROR;
}

int parse_param() {
        if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if (get_token() == ID) {
                        if (is_first_pass) {
                                current_variable.id_name = t.attr.string_value;
                                function_variable.id_name = t.attr.string_value;
                        }
                        return PARSED_OK;
                }
        }
        return PARSE_ERROR;

}

int parse_declaration_element() {
        get_token();
        if (t.type == VOID) {
                if (is_first_pass) {current_function.data_type = t.type; }
                if (get_token() == ID) {
                        if (is_first_pass) {
                                current_function.id_name = t.attr.string_value;
                                current_function.symbol_table = create_function_symbol_table();
                        }
                        if (get_token() == LEFT_ROUNDED_BRACKET) {
                                return parse_method_declaration();
                        }
                }
        } else if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if (is_first_pass) {
                        current_variable.data_type = t.type;
                        current_function.data_type = t.type;
                }

                if (parse_param()) {
                        if (is_first_pass) {
                                current_function.id_name = t.attr.string_value;
                                current_function.symbol_table = create_function_symbol_table();
                                /*put_function_variable_symbol_table(current_function.symbol_table, "test", 25, 5);
                                   symbol_table_item_t * o = ht_read(current_function.symbol_table, "test");
                                   printf("%d\n", o->data_type);*/
                        }
                        get_token();
                        if (t.type == LEFT_ROUNDED_BRACKET || t.type == RIGHT_ROUNDED_BRACKET || t.type == ASSIGN || t.type == SEMICOLON) {
                                return parse_declaration();
                        }
                }
        }

        return PARSE_ERROR;
}


int parse_class_element() {
        if (t.type == RIGHT_CURVED_BRACKET) {
                return PARSED_OK;
        }

        else if (t.type == STATIC) {
                return parse_declaration_element();
        }
        return PARSE_ERROR;
}

int parse_class_list() {
        if (t.type == CLASS) {
                if (get_token() == ID) {
                        if (is_first_pass) {
                                if (!exists_class(t.attr.string_value)) {
                                        insert_class(t.attr.string_value);
                                        set_current_class(t.attr.string_value);
                                } else {
                                        printf("CLASS REDECLARED\n");
                                }
                        }
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
                                                        return PARSE_ERROR;
                                                }
                                        }
                                }
                        }
                }
        } else if (t.type == EOF) {
                return PARSED_OK;
        } return PARSE_ERROR;
}


void add_builtin_functions() {
        // todo
}
int parse() {
        if (is_first_pass) {
                init_token_buffer(&token_buffer);
        }
        get_token();
        if (t.type == CLASS || t.type == EOF) {
                if (is_first_pass) {
                        init_class_list();
                        add_builtin_functions();
                }
                if (parse_class_list()) {

                        if (is_first_pass) {
                                // over ci je main a run v nej
                                symbol_table_item_t * run_method = get_function_symbol_table("Main", "run");
                                if (run_method == NULL) {
                                        printf("No RUN in Main class or no MAIN class\n");
                                } else {
                                        if (!(run_method->data_type == VOID) || !(run_method->content.function.params_count == 0)) {
                                                printf("Weird RUN in Main class\n");
                                        }
                                }
                        } else /* second pass */ {
                                free_class_list();
                        }

                        if (is_first_pass) {
                                is_first_pass = false;
                                is_second_pass = true;
                        } else /* second pass */ {
                                free_token_buffer(&token_buffer);
                        }
                        return PARSED_OK;
                } else {
                        free_token_buffer(&token_buffer);
                        return SYNTACTIC_ANALYSIS_ERROR;
                }
        } else {
                return SYNTACTIC_ANALYSIS_ERROR;
        }

}
