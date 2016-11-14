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
bool function_has_return = false;
// has return, ignore in void function
token_t t;
token_buffer_t token_buffer;
#define PARSE_ERROR 0
#define PARSED_OK 1
#define LEXICAL_ANALYSIS_ERROR 1
#define SYNTACTIC_ANALYSIS_ERROR 2

#define CONSTANT -1

string_t param_data_types;
symbol_table_item_t current_variable;
symbol_table_item_t function_variable;
symbol_table_item_t current_function;
char* current_class;


int get_token() {
        if (is_first_pass) {
                t.string_value = NULL;
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


/*    PARSING   */


int parse_expression(bool ends_semicolon) {

        if (t.type == SEMICOLON || t.type == RIGHT_ROUNDED_BRACKET || t.type == COMMA) {
                return PARSE_ERROR;
        }

        // ID co je funkcia alebo premenna
        // ak funkcia, parsuje takto:

        if (is_second_pass) {

                bool is_function = false;
                if (t.type == ID) {
                        symbol_table_item_t * item = get_symbol_table_class_item(current_class, t.string_value);
                        is_function = item && item->is_function;
                } else if (t.type == SPECIAL_ID) {
                        symbol_table_item_t * item = get_symbol_table_special_id_item(t.string_value);
                        is_function = item && item->is_function;
                }

                if (is_function) {
                        get_token();
                        if (t.type == LEFT_ROUNDED_BRACKET) {
                                if (parse_param_value()) {
                                        if (t.type == RIGHT_ROUNDED_BRACKET) {
                                                if (get_token() == SEMICOLON) {
                                                        return PARSED_OK;
                                                }
                                        }
                                }
                        }
                }
        }

        int br_cnt = 0;

        if (is_second_pass) printf("IN EXPR: ");
        while (1) {
                if (ends_semicolon) {
                        if (t.type == SEMICOLON) break;
                        // pop
                } else {
                        if (t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE || t.type == LEFT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                                // popni token, lebo to bolo prava zatvorka ifu, whilu

                                // if ((())) return
                                // foo (exp)
                                //break;
                        }

                        if (t.type == RIGHT_ROUNDED_BRACKET) {
                                br_cnt--;
                                //printf("Pocet zatvoriek %d\n", br_cnt);
                                if (br_cnt == 0) {
                                        //printf("nemam breakovat\n");
                                } else {
                                        //printf("breakovat\n");
                                        break;
                                }
                        }

                        if (t.type == LEFT_ROUNDED_BRACKET) {
                                br_cnt++;
                        }

                        // pop ???
                        if (t.type == COMMA) break;
                }

                if (is_second_pass) printf("%s, ", t_names[t.type]);

                if (is_second_pass) {
                        if (t.type == SPECIAL_ID) {
                                printf("EXPR: SPECIAL ID %s is declared %d\n", t.string_value, is_special_id_declared(t.string_value));
                        }
                        else if (t.type == ID) {
                                if (!is_declared(t.string_value)) {
                                        symbol_table_t *function_symbol_table = get_symbol_table_for_function(current_class, current_function.id_name);
                                        if(!is_declared_in_function(function_symbol_table, t.string_value)) {
                                                printf("EXPR: VAR IN FUNCTION (global or local) NOT DECLARED %s\n", t.string_value);
                                        }
                                }
                        }
                }
                get_token();
        }

        if (is_second_pass) printf("\n");
        return PARSED_OK;

}

int parse_return_value() {
        if (t.type == RETURN) {
                function_has_return = true;
                get_token();
                if (t.type == LEFT_ROUNDED_BRACKET || t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) {
                        if (current_function.function.return_type == VOID) {
                                printf("return E in VOID error\n");
                        }

                        return parse_expression(true);
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
                if (t.type == LEFT_ROUNDED_BRACKET || t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) { // EXPR HACK
                        if (parse_expression(false)) { // just for ifj16.print
                                if (t.type == COMMA) {
                                        return parse_next_param_value();
                                } else if (t.type == RIGHT_ROUNDED_BRACKET) {
                                        return PARSED_OK;
                                }
                        }
                } else if (t.type == RIGHT_ROUNDED_BRACKET) {
                        return PARSED_OK;
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
                        if (t.type == LEFT_ROUNDED_BRACKET || t.type == SEMICOLON || t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) {
                                return parse_expression(true);
                        }
                } else if (t.type == LEFT_ROUNDED_BRACKET) {
                        if (parse_param_value()) {
                                if (t.type == RIGHT_ROUNDED_BRACKET) {
                                        if (get_token() == SEMICOLON) {
                                                return PARSED_OK;
                                        }
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
        if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == RETURN || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
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
                if (is_second_pass) {

                        if (t.type == SPECIAL_ID) {
                                if (!is_special_id_declared(t.string_value)) {
                                        printf("SPECIAL ID NOT DECLARED\n");
                                }
                        } else if (t.type == ID) {
                                if (!is_declared(t.string_value)) {
                                        symbol_table_t *function_symbol_table = get_symbol_table_for_function(current_class, current_function.id_name);
                                        if(!is_declared_in_function(function_symbol_table, t.string_value)) {
                                                printf("VAR IN FUNCTION (global or local) NOT DECLARED %s\n", t.string_value);
                                        }
                                }
                        }
                }
                if(parse_call_assign()) {
                        get_token();
                        if (t.type == ELSE || t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                return PARSED_OK;
                        }
                }

        } else if (t.type == IF) {
                if (get_token() == LEFT_ROUNDED_BRACKET) {
                        get_token();
                        if (t.type == LEFT_ROUNDED_BRACKET || t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) { // EXPR HACK
                                if (parse_expression(false)) {
                                        get_token();
                                        if (t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE || t.type == LEFT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
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
                        if (t.type == LEFT_ROUNDED_BRACKET || t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) { // EXPR HACK
                                if (parse_expression(false)) {
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
                        if (current_function.function.return_type != VOID) {
                                if (!function_has_return) {
                                        printf("FUNKCIA NEMA RETURN\n");
                                }
                        }
                        current_function.function.param_data_types = (param_data_types.length > 0) ? param_data_types.data : NULL; /* for consistency */
                        current_function.function.params_count = param_data_types.length;
                        printf("name=%s, ret_type=%d, data_types=%s, params_count=%d, local_vars_count=%d\n", current_function.id_name, current_function.function.return_type, current_function.function.param_data_types, current_function.function.params_count, current_function.function.local_vars_count);
                        if (!is_declared(current_function.id_name)) {
                                insert_function_symbol_table(current_function.id_name, current_function.function.return_type, current_function.function.params_count, current_function.function.local_vars_count, current_function.function.param_data_types, current_function.function.symbol_table);
                        } else {
                                printf("FUNCTION REDECLARED\n");
                        }

                        clear_string(&param_data_types); /* vyprazdni string ale neuvolni */
                        current_function.function.local_vars_count = 0;
                        function_variable.variable.offset = 0;
                }
                return PARSED_OK;
        } else if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if (is_first_pass) {
                        current_function.function.local_vars_count++;
                        function_variable.variable.data_type = t.type;
                }
                if (parse_param()) {
                        if (is_first_pass) {
                                printf("local fun var .. name %s type %d offset %d\n", function_variable.id_name, function_variable.variable.data_type, function_variable.variable.offset);
                                if (!is_declared_in_function(current_function.function.symbol_table, function_variable.id_name)) {
                                        insert_function_variable_symbol_table(current_function.function.symbol_table, function_variable.id_name, function_variable.variable.data_type, function_variable.variable.offset);
                                        function_variable.variable.offset++;
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
                                function_variable.variable.data_type = t.type;
                        }
                        if (parse_param()) {
                                if (is_first_pass) {
                                        printf("local fun var .. name %s type %d offset %d\n", function_variable.id_name, function_variable.variable.data_type, function_variable.variable.offset);
                                        if (!is_declared_in_function(current_function.function.symbol_table, function_variable.id_name)) {
                                                insert_function_variable_symbol_table(current_function.function.symbol_table, function_variable.id_name, function_variable.variable.data_type, function_variable.variable.offset);
                                                function_variable.variable.offset++;
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
                        function_variable.variable.data_type = t.type;
                }
                if (parse_param()) {
                        if (is_first_pass) {
                                printf("local fun var .. name %s type %d offset %d\n", function_variable.id_name, function_variable.variable.data_type, function_variable.variable.offset);
                                if (!is_declared_in_function(current_function.function.symbol_table, function_variable.id_name)) {
                                        insert_function_variable_symbol_table(current_function.function.symbol_table, function_variable.id_name, function_variable.variable.data_type, function_variable.variable.offset);
                                        function_variable.variable.offset++;
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
                if (t.type == LEFT_ROUNDED_BRACKET || t.type == SEMICOLON || t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) {
                        return parse_expression(true);
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
                                insert_variable_symbol_table(current_variable.id_name, current_variable.variable.data_type, CONSTANT);
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
                                current_variable.id_name = t.string_value;
                                function_variable.id_name = t.string_value;
                        }
                        return PARSED_OK;
                }
        }
        return PARSE_ERROR;

}

int parse_declaration_element() {
        get_token();
        if (t.type == VOID) {
                if (is_first_pass) {
                        current_function.function.return_type = t.type;
                }
                if (get_token() == ID) {
                        current_function.id_name = t.string_value; /* potrebne pre oba prechody */

                        if (is_first_pass) {
                                function_has_return = false;
                                current_function.function.symbol_table = create_function_symbol_table();
                        }
                        if (get_token() == LEFT_ROUNDED_BRACKET) {
                                return parse_method_declaration();
                        }
                }
        } else if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                if (is_first_pass) {
                        current_variable.variable.data_type = t.type;
                        current_function.function.return_type = t.type;
                }

                if (parse_param()) {
                        current_function.id_name = t.string_value; /* potrebne pre oba prechody */

                        if (is_first_pass) {
                                function_has_return = false;
                                current_function.function.symbol_table = create_function_symbol_table();
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
                        current_class = t.string_value; /* potrebne pre oba prechody */
                        if (is_first_pass) {
                                if (!exists_class(t.string_value)) {
                                        insert_class(t.string_value);
                                        set_current_class(t.string_value);
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
        char *ifj_class = copy_string("ifj16");
        insert_class(ifj_class);
        set_current_class(ifj_class);

        insert_function_symbol_table(copy_string("sort"), STRING, 1, 0, copy_string("s"), NULL);
        insert_function_symbol_table(copy_string("find"), INT, 2, 0, copy_string("ss"), NULL);
        insert_function_symbol_table(copy_string("length"), INT, 1, 0, copy_string("s"), NULL);
        insert_function_symbol_table(copy_string("compare"), INT, 2, 0, copy_string("ss"), NULL);
        insert_function_symbol_table(copy_string("substr"), INT, 3, 0, copy_string("ssi"), NULL);
        insert_function_symbol_table(copy_string("print"), VOID, 1, 0, copy_string("s"), NULL);

        insert_function_symbol_table(copy_string("readInt"), INT, 0, 0, NULL, NULL);
        insert_function_symbol_table(copy_string("readDouble"), DOUBLE, 0, 0, NULL, NULL);
        insert_function_symbol_table(copy_string("readString"), INT, 0, 0, NULL, NULL);

        /*symbol_table_item_t * run_method = get_symbol_table_class_item("ifj16", "sort");
           printf("NAME %s\n", run_method->id_name);*/
}
int parse() {

        if (is_first_pass) {
                init_string(&param_data_types);
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
                                symbol_table_item_t * run_method = get_symbol_table_class_item("Main", "run");
                                if (run_method == NULL) {
                                        printf("No RUN in Main class or no MAIN class\n");
                                } else {
                                        if (!(run_method->function.return_type == VOID) || !(run_method->function.params_count == 0)) {
                                                printf("Weird RUN in Main class\n");
                                        }
                                }
                        } else /* second pass */ {
                                free_class_list(); // uvolni zoznam tried
                        }

                        if (is_first_pass) {
                                is_first_pass = false;
                                is_second_pass = true;
                                free_string(&param_data_types); // uvolni string s parametrami pre funkcie
                        } else /* second pass */ {
                                free_token_buffer(&token_buffer); // uvolni zoznam tokenov urceny pre druhy prechod
                        }
                        return PARSED_OK;
                } else {
                        /* Ak prvy prechod zlyhal... */
                        free_string(&param_data_types);
                        free_token_buffer(&token_buffer);
                        return SYNTACTIC_ANALYSIS_ERROR;
                }
        } else {
                return SYNTACTIC_ANALYSIS_ERROR;
        }

}
