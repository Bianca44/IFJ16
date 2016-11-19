#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "scanner.h"
#include "parser.h"
#include "symbol_table.h"
#include "strings.h"
#include "token_buffer.h"
#include "expr.h"
#include "memory_constants.h"
#include "error_codes.h"

char *t_names[TOKENS_COUNT] = { "LEXICAL_ERROR", "ID", "INT_LITERAL", "DOUBLE_LITERAL", "ADD", "SUB", "MUL",
                                "DIV", "SEMICOLON", "LEFT_CURVED_BRACKET", "RIGHT_CURVED_BRACKET",
                                "LEFT_ROUNDED_BRACKET", "RIGHT_ROUNDED_BRACKET", "ASSIGN", "LOGICAL_AND",
                                "LOGICAL_OR", "COMMA", "NEG",  "STRING_LITERAL", "NOT_EQUAL", "LESS_EQUAL",
                                "LESS", "GREATER_EQUAL", "GREATER", "EQUAL", "SPECIAL_ID", "BOOLEAN", "BREAK",
                                "CLASS", "CONTINUE", "DO", "DOUBLE", "ELSE", "FALSE", "FOR", "IF", "INT", "RETURN",
                                "STRING", "STATIC", "TRUE", "VOID", "WHILE" };

int params_counter = 0;
bool is_first_pass = true;
bool is_second_pass = false;
bool has_function_return = false;
bool is_static_variable_declaration = false;
bool skip_precedence_analysis = false;

constant_t * mem_constants = NULL;
tDLList * global_inst_tape;
token_t t;
token_buffer_t token_buffer;

#define PARSE_ERROR 0
#define PARSED_OK 1

string_t param_data_types;
string_t local_vars_data_types;
symbol_table_item_t current_variable;
symbol_table_item_t function_variable;
symbol_table_item_t current_function;
symbol_table_item_t expr_result;


tVar * expr_var_result;
char* current_class;
char* function_call_name;


void cleanup_exit(int exit_code) {
        //TODO free memory

        free_constants(&mem_constants);
        free_token_buffer(&token_buffer);
        free_class_list();
        exit(exit_code);
}

int get_token() {
        if (is_first_pass) {
                t.string_value = NULL;
                get_next_token(&t);
                add_token_to_buffer(&token_buffer, &t);
        } else /* second pass */ {
                t = *get_next_token_buffer(&token_buffer);
        }
        if (t.type == LEXICAL_ERROR) {
                fprintf(stderr, "Lexical error: unknown token\n");
                cleanup_exit(LEXICAL_ANALYSIS_ERROR);
        }
        return t.type;
}


/*    PARSING   */


int parse_expression(bool ends_semicolon) {

        if (t.type == SEMICOLON || t.type == RIGHT_ROUNDED_BRACKET || t.type == COMMA) {
                return PARSE_ERROR;
        }

        token_buffer_t tb;

        /* Prvy prechod vyhodnot vyraz u globalnej premmenej priamo */
        if (is_first_pass && is_static_variable_declaration) {
                init_token_buffer(&tb);
                while (1) {
                        if (t.type == SEMICOLON) break;

                        printf("%s, ", t_names[t.type]);

                        add_token_to_buffer(&tb, &t);

                        if (t.type == SPECIAL_ID) {
                                if (!is_special_id_declared(t.string_value)) {
                                        fprintf(stderr,"Expression: ID %s was not declared.\n", t.string_value);
                                        cleanup_exit(SEMANTIC_ANALYSIS_OTHER_ERROR);
                                } else {
                                        symbol_table_item_t * item = get_symbol_table_special_id_item(t.string_value);
                                        if (item->is_function) {
                                                fprintf(stderr, "Expression: ID \'%s\' is declared as function.\n", t.string_value);
                                                cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR); // TODO check forum
                                        }
                                }
                        }
                        else if (t.type == ID) {
                                if (!is_declared(t.string_value)) {
                                        fprintf(stderr,"Expression: ID %s was not declared.\n", t.string_value);
                                        cleanup_exit(SEMANTIC_ANALYSIS_OTHER_ERROR);
                                } else {
                                        symbol_table_item_t * item = get_symbol_table_class_item(current_class, t.string_value);
                                        if (item->is_function) {
                                                fprintf(stderr, "Expression: ID \'%s\' is declared as function.\n", t.string_value);
                                                cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                        }
                                }
                        }
                        get_token();
                }
                printf("\n");
                //printf("uvolnujem\n");
                //free_token_buffer(&tb);
                get_psa(&tb, &expr_result,  &expr_var_result);
                return PARSED_OK;
        }

        // ID co je funkcia alebo premenna
        // ak funkcia, parsuje takto:

        if (is_second_pass) {
                /* Mozno je to funkcia, takze kukni na id do TS ci je alebo nie je */
                bool is_function = false;
                symbol_table_item_t * item = NULL;
                if (t.type == ID) {
                        symbol_table_t *function_symbol_table = get_symbol_table_for_function(current_class, current_function.id_name);
                        if (function_symbol_table == NULL || (function_symbol_table != NULL &&  !is_declared_in_function(function_symbol_table, t.string_value))) {
                                if (!is_declared(t.string_value)) {
                                        fprintf(stderr,"Expression: ID %s was not declared.\n", t.string_value);
                                        cleanup_exit(SEMANTIC_ANALYSIS_PROGRAM_ERROR);
                                } else {
                                        item = get_symbol_table_class_item(current_class, t.string_value);
                                }
                        } else {
                                item = get_symbol_table_function_item(function_symbol_table, t.string_value);
                        }
                        if (item != NULL) {
                                is_function = item->is_function;
                        }
                } else if (t.type == SPECIAL_ID) {
                        item = get_symbol_table_special_id_item(t.string_value);
                        if (item == NULL) {
                                fprintf(stderr,"Expression: ID %s was not declared.\n", t.string_value);
                                cleanup_exit(SEMANTIC_ANALYSIS_PROGRAM_ERROR);
                        }
                        is_function = item->is_function;
                }

                if (is_function) {
                        //printf("data type %d\n", item->function.return_type);
                        function_call_name = t.string_value;
                        expr_result = *item;
                        get_token();
                        if (t.type == LEFT_ROUNDED_BRACKET) {
                                if (parse_param_value()) {
                                        if (t.type == RIGHT_ROUNDED_BRACKET) {
                                                if (get_token() == SEMICOLON) {
                                                        if (is_second_pass) {
                                                                symbol_table_item_t *function_item;
                                                                if (strchr(function_call_name, '.') != NULL) {
                                                                        function_item = get_symbol_table_special_id_item(function_call_name);
                                                                } else {
                                                                        function_item = get_symbol_table_class_item(current_class, function_call_name);
                                                                }

                                                                if (function_item->function.params_count != params_counter) {
                                                                        fprintf(stderr, "Too many arguments when calling function \'%s\'\n", function_call_name);
                                                                        fprintf(stderr, "Function \'%s\' requires %d param(s) but is called with %d param(s).\n", function_call_name, function_item->function.params_count, params_counter);
                                                                        cleanup_exit(SEMANTIC_ANALYSIS_PROGRAM_ERROR);
                                                                }
                                                                params_counter = 0;
                                                        }
                                                        function_call_name = NULL;
                                                        return PARSED_OK;
                                                }
                                        }
                                }
                        }
                }
        }

        int brackets_counter = 0;
        if (is_second_pass) {
                init_token_buffer(&tb);
                printf("IN EXPR: ");
        }

        while (1) {
                if (ends_semicolon) {
                        if (t.type == SEMICOLON) break;
                } else {
                        if (t.type == LEFT_CURVED_BRACKET || t.type == SEMICOLON ||t.type == EOF) return PARSE_ERROR;  /* prevent loop */

                        if (t.type == RIGHT_ROUNDED_BRACKET) {
                                brackets_counter--;
                                if (brackets_counter != 0) {
                                        break;
                                }
                        }

                        if (t.type == LEFT_ROUNDED_BRACKET) {
                                brackets_counter++;
                        }

                        if (t.type == COMMA) break;
                }

                if (is_second_pass) {
                        printf("%s, ", t_names[t.type]);
                        add_token_to_buffer(&tb, &t);
                }

                if (is_second_pass) {
                        if (t.type == SPECIAL_ID) {
                                if (!is_special_id_declared(t.string_value)) {
                                        fprintf(stderr,"Expression: ID %s was not declared.\n", t.string_value);
                                        cleanup_exit(SEMANTIC_ANALYSIS_PROGRAM_ERROR);
                                } else {
                                        symbol_table_item_t * item = get_symbol_table_special_id_item(t.string_value);
                                        if (item->is_function) {
                                                fprintf(stderr, "Expression: ID \'%s\' is declared as function.\n", t.string_value);
                                                cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                        }
                                }
                        }
                        else if (t.type == ID) {
                                symbol_table_t *function_symbol_table = get_symbol_table_for_function(current_class, current_function.id_name);
                                if (function_symbol_table == NULL || (function_symbol_table != NULL &&  !is_declared_in_function(function_symbol_table, t.string_value))) {
                                        if (!is_declared(t.string_value)) {
                                                fprintf(stderr, "Expression: ID \'%s\' was not declared in the function nor in the class \'%s\'.\n", t.string_value, current_class);
                                                cleanup_exit(SEMANTIC_ANALYSIS_PROGRAM_ERROR);
                                        } else {
                                                symbol_table_item_t * item =  get_symbol_table_class_item(current_class, t.string_value);
                                                if (item->is_function) {
                                                        fprintf(stderr, "Expression: ID \'%s\' is declared as function.\n", t.string_value);
                                                        cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                                }
                                        }
                                }
                        }
                }
                get_token();
        }

        if (is_second_pass) {
                // PSA
                //free_token_buffer(&tb);
                printf("\n");
                if (!skip_precedence_analysis) {
                    get_psa(&tb, &expr_result, &expr_var_result);
                }
        }
        return PARSED_OK;

}

int parse_return_value() {
        if (t.type == RETURN) {
                if (is_first_pass) {
                        has_function_return = true;
                }
                get_token();
                if (t.type == LEFT_ROUNDED_BRACKET || t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) {
                        if (is_first_pass) {
                                if (current_function.function.return_type == VOID) {
                                        fprintf(stderr, "Return in function '\'%s'\' with void return type.\n", current_function.id_name);
                                        cleanup_exit(RUN_UNINITIALIZED_VARIABLE_ERROR);
                                }
                        }

                        if (parse_expression(true)) {
                                if (is_second_pass) {
                                        if (expr_result.id_name != NULL) {
                                                int expr_data_type =  expr_result.variable.data_type;

                                                if (current_variable.variable.data_type != expr_data_type) {
                                                        fprintf(stderr, "Bad return expression.\n");
                                                        cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                                }
                                        }
                                }
                                expr_result.id_name = NULL;

                                if (t.type == SEMICOLON) {
                                        return PARSED_OK;
                                }
                        }
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
                        if (is_second_pass) {
                                symbol_table_item_t * item;
                                int data_type = 0;
                                if (t.type == SPECIAL_ID) {
                                        if (!is_special_id_declared(t.string_value)) {
                                                fprintf(stderr, "Param \'%s\' for function \'%s.%s\' was not declared.\n", t.string_value, current_class, function_call_name);
                                                cleanup_exit(SEMANTIC_ANALYSIS_PROGRAM_ERROR);
                                        } else {
                                                item = get_symbol_table_special_id_item(t.string_value);
                                                if (item->is_function) {
                                                        fprintf(stderr, "Param \'%s\' for function \'%s.%s\' is declared as function.\n", t.string_value, current_class, function_call_name);
                                                        cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                                }
                                                data_type = item->variable.data_type;
                                        }
                                }
                                else if (t.type == ID) {
                                        symbol_table_t *function_symbol_table = get_symbol_table_for_function(current_class, current_function.id_name);
                                        if(function_symbol_table != NULL && is_declared_in_function(function_symbol_table, t.string_value)) {
                                                item = get_symbol_table_function_item(function_symbol_table, t.string_value);
                                                data_type = item->variable.data_type;
                                        } else {
                                                if (!is_declared(t.string_value)) {
                                                        fprintf(stderr, "Neither param \'%s\' for function \'%s.%s\' was not declared nor in the class \'%s\'.\n", t.string_value, current_class, function_call_name, current_class);
                                                        cleanup_exit(SEMANTIC_ANALYSIS_PROGRAM_ERROR);
                                                } else {
                                                        item = get_symbol_table_class_item(current_class, t.string_value);
                                                        if (item->is_function) {
                                                                fprintf(stderr, "Param \'%s\' for function \'%s.%s\' is declared as function.\n", t.string_value, current_class, function_call_name);
                                                                cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                                        }
                                                        data_type = item->variable.data_type;
                                                }
                                        }
                                } else {
                                        switch (t.type) {
                                        case INT_LITERAL: data_type = INT;
                                                break;
                                        case DOUBLE_LITERAL: data_type = DOUBLE;
                                                break;
                                        case STRING_LITERAL: data_type = STRING;
                                                break;
                                        case TRUE:
                                        case FALSE: data_type = BOOLEAN;
                                                break;
                                        }
                                }

                                symbol_table_item_t *function_item;
                                if (strchr(function_call_name, '.') != NULL) {
                                        function_item = get_symbol_table_special_id_item(function_call_name);
                                } else {
                                        function_item = get_symbol_table_class_item(current_class, function_call_name);
                                }

                                char expected_param_type = function_item->function.param_data_types[params_counter];

                                params_counter++;

                                if (params_counter > function_item->function.params_count) {
                                        fprintf(stderr, "Too many arguments when calling function \'%s\'\n", function_call_name);
                                        fprintf(stderr, "Function \'%s\' requires %d param(s) but is called with %d param(s).\n", function_call_name, function_item->function.params_count, params_counter);
                                        cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                }

                                switch (expected_param_type) {
                                case 's':
                                        if (data_type != STRING) {
                                                printf("Parameter \'%s\' when calling function \'%s\' must be string.\n", t.string_value, function_call_name);
                                                cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                        }
                                        break;
                                case 'b':
                                        if (data_type != BOOLEAN) {
                                                printf("Parameter \'%s\' when calling function \'%s\' must be boolean.\n", t.string_value, function_call_name);
                                                cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                        }
                                        break;
                                case 'i':
                                        if (data_type != INT) {
                                                printf("Parameter \'%s\' when calling function \'%s\' must be int.\n", t.string_value, function_call_name);
                                                cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                        }
                                        break;
                                case 'd':
                                        if (data_type == INT) {
                                                printf("konvertuj\n");

                                        } else if (data_type != DOUBLE) {
                                                printf("Parameter \'%s\' when calling function \'%s\' must be double.\n", t.string_value, function_call_name);
                                                cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                        }
                                }


                                //symbol_table_item_t *function_item = get_symbol_table_class_item(current_class, function_call_name);
                                //printf("fun %s %d datapyes %s\n", function_item->id_name, function_item->function.return_type, function_item->function.param_data_types);
                                //printf("funkcia %s datovy typ argumentu %s pocet param %d\n", function_call_name, t_names[data_type], params_counter);
                        }
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
        if (is_second_pass) {
                params_counter = 0;
        }
        if (t.type == LEFT_ROUNDED_BRACKET) {
                get_token();
                if (t.type == LEFT_ROUNDED_BRACKET || t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) { // EXPR HACK
                        if (strcmp(function_call_name, "ifj16.print") != 0) {
                                if (is_second_pass) {
                                        symbol_table_item_t * item;
                                        int data_type = 0;
                                        if (t.type == SPECIAL_ID) {
                                                if (!is_special_id_declared(t.string_value)) {
                                                        fprintf(stderr, "Param \'%s\' for function \'%s.%s\' was not declared.\n", t.string_value, current_class, function_call_name);
                                                        cleanup_exit(SEMANTIC_ANALYSIS_PROGRAM_ERROR);
                                                } else {
                                                        item = get_symbol_table_special_id_item(t.string_value);
                                                        if (item->is_function) {
                                                                fprintf(stderr, "Param \'%s\' for function \'%s.%s\' is declared as function.\n", t.string_value, current_class, function_call_name);
                                                                cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                                        }
                                                        data_type = item->variable.data_type;
                                                }
                                        }
                                        else if (t.type == ID) {
                                                symbol_table_t *function_symbol_table = get_symbol_table_for_function(current_class, current_function.id_name);
                                                if(function_symbol_table != NULL && is_declared_in_function(function_symbol_table, t.string_value)) {
                                                        item = get_symbol_table_function_item(function_symbol_table, t.string_value);
                                                        data_type = item->variable.data_type;
                                                } else {
                                                        if (!is_declared(t.string_value)) {
                                                                fprintf(stderr, "Neither param \'%s\' for function \'%s.%s\' was not declared nor in the class \'%s\'.\n", t.string_value, current_class, function_call_name, current_class);
                                                                cleanup_exit(SEMANTIC_ANALYSIS_PROGRAM_ERROR);
                                                        } else {
                                                                item = get_symbol_table_class_item(current_class, t.string_value);
                                                                if (item->is_function) {
                                                                        fprintf(stderr, "Param \'%s\' for function \'%s.%s\' is declared as function.\n", t.string_value, current_class, function_call_name);
                                                                        cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                                                }
                                                                data_type = item->variable.data_type;
                                                        }
                                                }
                                        } else {
                                                switch (t.type) {
                                                case INT_LITERAL: data_type = INT;
                                                        break;
                                                case DOUBLE_LITERAL: data_type = DOUBLE;
                                                        break;
                                                case STRING_LITERAL: data_type = STRING;
                                                        break;
                                                case TRUE:
                                                case FALSE: data_type = BOOLEAN;
                                                        break;
                                                }
                                        }

                                        symbol_table_item_t *function_item;
                                        if (strchr(function_call_name, '.') != NULL) {
                                                function_item = get_symbol_table_special_id_item(function_call_name);
                                        } else {
                                                function_item = get_symbol_table_class_item(current_class, function_call_name);
                                        }

                                        if (function_item->function.param_data_types == NULL) {
                                                fprintf(stderr, "Calling function \'%s\' with param(s) but function has no parameters.\n", function_call_name);
                                                cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                        }

                                        bool is_var = (t.type == ID || t.type == SPECIAL_ID);
                                        printf("pushujem var %d\n", is_var);


                                        char expected_param_type = function_item->function.param_data_types[params_counter];

                                        switch (expected_param_type) {
                                        case 's':
                                                if (data_type != STRING) {
                                                        printf("Parameter \'%s\' when calling function \'%s\' must be string.\n", t.string_value, function_call_name);
                                                        cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                                }
                                                break;
                                        case 'b':
                                                if (data_type != BOOLEAN) {
                                                        printf("Parameter \'%s\' when calling function \'%s\' must be boolean.\n", t.string_value, function_call_name);
                                                        cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                                }
                                                break;
                                        case 'i':
                                                if (data_type != INT) {
                                                        printf("Parameter \'%s\' when calling function \'%s\' must be int.\n", t.string_value, function_call_name);
                                                        cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                                }
                                                break;
                                        case 'd':
                                                if (data_type == INT) {
                                                        printf("konvertuj\n");

                                                } else if (data_type != DOUBLE) {
                                                        printf("Parameter \'%s\' when calling function \'%s\' must be double.\n", t.string_value, function_call_name);
                                                        cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                                }
                                        }

                                        params_counter++;

                                        //printf("funkcia %s datovy typ argumentu %s pocet param %d\n", function_call_name, t_names[data_type], params_counter);
                                }
                                get_token();
                                if (t.type == COMMA) {
                                        return parse_next_param_value();
                                } else if (t.type == RIGHT_ROUNDED_BRACKET) {
                                        return PARSED_OK;
                                }
                        } else {
                                /* special case for ifj16.print */
                                if (parse_expression(false)) {
                                        if (is_second_pass) {
                                                printf("DATA print %s\n", expr_result.variable.s);
                                                DLInsertLast(global_inst_tape, generate(I_PRINT, &expr_result.variable, NULL, NULL));
                                                params_counter++;
                                        }
                                        if (t.type == RIGHT_ROUNDED_BRACKET) {
                                                return PARSED_OK;
                                        }
                                }
                        }
                } else if (t.type == RIGHT_ROUNDED_BRACKET) {
                        if (is_second_pass) {
                                symbol_table_item_t *function_item;
                                if (strchr(function_call_name, '.') != NULL) {
                                        function_item = get_symbol_table_special_id_item(function_call_name);
                                } else {
                                        function_item = get_symbol_table_class_item(current_class, function_call_name);
                                }

                                if (function_item->function.params_count > 0) {
                                        fprintf(stderr, "Missing arguments\n");
                                }
                        }
                        return PARSED_OK;
                }
        }
        else if (t.type == RIGHT_ROUNDED_BRACKET) {
                return PARSED_OK;
        }

        return PARSE_ERROR;
}

int parse_call_assign() {
        if (t.type == ID || t.type == SPECIAL_ID) {
                function_call_name = t.string_value;
                get_token();
                if (t.type == LEFT_ROUNDED_BRACKET) {
                        if (parse_param_value()) {
                                if (t.type == RIGHT_ROUNDED_BRACKET) {
                                        if (get_token() == SEMICOLON) {
                                                if (is_second_pass) {
                                                        symbol_table_item_t *function_item;
                                                        if (strchr(function_call_name, '.') != NULL) {
                                                                function_item = get_symbol_table_special_id_item(function_call_name);
                                                        } else {
                                                                function_item = get_symbol_table_class_item(current_class, function_call_name);
                                                        }
                                                        if (params_counter > function_item->function.params_count) {
                                                                fprintf(stderr, "Too many arguments when calling function \'%s\'\n", function_call_name);
                                                                fprintf(stderr, "Function \'%s\' requires %d param(s) but is called with %d param(s).\n", function_call_name, function_item->function.params_count, params_counter);
                                                                cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                                        }
                                                        params_counter = 0;
                                                }
                                                function_call_name = NULL;
                                                return PARSED_OK;
                                        }
                                }
                        }
                } else if (t.type == ASSIGN || t.type == SEMICOLON) {
                        return parse_value();
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
        }
        return PARSE_ERROR;
}

int parse_else() {
        if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                return PARSED_OK;
        } if (t.type == ELSE) {
                get_token();
                if (t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE || t.type == LEFT_CURVED_BRACKET) {
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
                                        fprintf(stderr, "ID \'%s\' in function \'%s.%s\' was not declared.\n", t.string_value, current_class, current_function.id_name);
                                        cleanup_exit(SEMANTIC_ANALYSIS_PROGRAM_ERROR);
                                } else {
                                        symbol_table_item_t * p = get_symbol_table_special_id_item(t.string_value);
                                        function_variable.variable.data_type = p->is_function ? p->function.return_type : p->variable.data_type;
                                        //printf("MAM TYP %d\n", function_variable.variable.data_type);
                                }
                        } else if (t.type == ID) {
                                if (!is_declared(t.string_value)) {
                                        symbol_table_t *function_symbol_table = get_symbol_table_for_function(current_class, current_function.id_name);
                                        if(!is_declared_in_function(function_symbol_table, t.string_value)) {
                                                fprintf(stderr, "Neither ID \'%s\' in function \'%s.%s\' was not declared nor in the class \'%s\'.\n", t.string_value, current_class, current_function.id_name, current_class);
                                                cleanup_exit(SEMANTIC_ANALYSIS_PROGRAM_ERROR);
                                        } else {
                                                symbol_table_item_t * p = get_symbol_table_function_item(function_symbol_table, t.string_value);
                                                function_variable.variable.data_type = p->is_function ? p->function.return_type : p->variable.data_type;
                                                //printf("MAM TYP3 %s\n", t_names[function_variable.variable.data_type]);
                                        }
                                } else {
                                        symbol_table_item_t * p = get_symbol_table_class_item(current_class, t.string_value);
                                        function_variable.variable.data_type = p->is_function ? p->function.return_type : p->variable.data_type;
                                        //printf("MAM TYP3 %s\n", t_names[function_variable.variable.data_type]);
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
                if(parse_statement_list()) {
                        get_token();
                        if (t.type == LEFT_CURVED_BRACKET || t.type == RIGHT_CURVED_BRACKET || t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN || t.type == SEMICOLON || t.type == RETURN || t.type == ID || t.type == SPECIAL_ID || t.type == IF || t.type == WHILE) {
                                return PARSED_OK;
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
                                        return parse_statement_list();
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
                                if (!has_function_return) {
                                        fprintf(stderr, "Return in function \'%s\' was not found\n", current_function.id_name);
                                        cleanup_exit(RUN_UNINITIALIZED_VARIABLE_ERROR);
                                }
                                has_function_return = false;
                        }

                        current_function.function.param_data_types = param_data_types.data;
                        current_function.function.local_vars_data_types = local_vars_data_types.data;
                        current_function.function.params_count = param_data_types.length;
                        printf("name=%s, ret_type=%d, data_types=%s, params_count=%d, local_vars_count=%d, all=%d, local_vars_data_types=%s\n", current_function.id_name, current_function.function.return_type, current_function.function.param_data_types, current_function.function.params_count, current_function.function.local_vars_count, current_function.function.params_count + current_function.function.local_vars_count, current_function.function.local_vars_data_types);
                        if (!is_declared(current_function.id_name)) {
                                insert_function_symbol_table(current_function.id_name, current_function.function.return_type, current_function.function.params_count, current_function.function.local_vars_count, current_function.function.param_data_types, current_function.function.local_vars_data_types, current_function.function.symbol_table);

                                // TODO
                                //symbol_table_item_t * p = insert_tmp_variable_symbol_table_function(current_function.id_name, BOOLEAN);
                                //symbol_table_item_t * sp = insert_tmp_variable_symbol_table_function(current_function.id_name, STRING);
                                //symbol_table_item_t * g = insert_tmp_variable_symbol_table_function(current_function.id_name, INT);

                                //printf("nenajdene %d\n", p->variable.offset);
                                //printf("test %d\n", p->variable.data_type);

                                current_function.id_name = NULL;
                        } else {
                                fprintf(stderr, "Function \'%s\' was redeclared.\n", current_function.id_name);
                                cleanup_exit(SEMANTIC_ANALYSIS_PROGRAM_ERROR);
                        }

                        clear_string(&param_data_types);
                        clear_string(&local_vars_data_types); /* vyprazdni string ale neuvolni */
                        current_function.function.local_vars_count = 0;
                        function_variable.variable.offset = 0;
                }
                return PARSED_OK;
        } else if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                function_variable.variable.data_type = t.type;
                append_type(&local_vars_data_types, t.type);

                if (is_first_pass) {
                        current_function.function.local_vars_count++;
                }
                if (parse_param()) {
                        if (is_first_pass) {
                                //printf("local fun var .. name %s type %d offset %d\n", function_variable.id_name, function_variable.variable.data_type, function_variable.variable.offset);
                                if (!is_declared_in_function(current_function.function.symbol_table, function_variable.id_name)) {
                                        insert_function_variable_symbol_table(current_function.function.symbol_table, function_variable.id_name, function_variable.variable.data_type, function_variable.variable.offset);
                                        function_variable.variable.offset++;
                                        function_variable.id_name = NULL;
                                } else {
                                        fprintf(stderr, "Variable \'%s\' in function \'%s\' was redeclared.\n", function_variable.id_name, current_function.id_name);
                                        cleanup_exit(SEMANTIC_ANALYSIS_PROGRAM_ERROR);
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
                if (parse_element_list()) {
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
                                append_type(&param_data_types, t.type);
                                function_variable.variable.data_type = t.type;
                        }
                        if (parse_param()) {
                                if (is_first_pass) {
                                        //printf("local fun var .. name %s type %d offset %d\n", function_variable.id_name, function_variable.variable.data_type, function_variable.variable.offset);
                                        if (!is_declared_in_function(current_function.function.symbol_table, function_variable.id_name)) {
                                                insert_function_variable_symbol_table(current_function.function.symbol_table, function_variable.id_name, function_variable.variable.data_type, function_variable.variable.offset);
                                                function_variable.variable.offset++;
                                                function_variable.id_name = NULL;
                                        } else {
                                                fprintf(stderr, "Variable \'%s\' in function \'%s\' was redeclared.\n", function_variable.id_name, current_function.id_name);
                                                cleanup_exit(SEMANTIC_ANALYSIS_PROGRAM_ERROR);
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
                        init_string(&param_data_types);
                        append_type(&param_data_types, t.type);
                        function_variable.variable.data_type = t.type;
                }
                if (parse_param()) {
                        if (is_first_pass) {
                                //printf("local fun var .. name %s type %d offset %d\n", function_variable.id_name, function_variable.variable.data_type, function_variable.variable.offset);
                                if (!is_declared_in_function(current_function.function.symbol_table, function_variable.id_name)) {
                                        insert_function_variable_symbol_table(current_function.function.symbol_table, function_variable.id_name, function_variable.variable.data_type, function_variable.variable.offset);
                                        function_variable.variable.offset++;
                                        function_variable.id_name = NULL;
                                } else {
                                        fprintf(stderr, "Variable \'%s\' in function \'%s\' was redeclared.\n", function_variable.id_name, current_function.id_name);
                                        cleanup_exit(SEMANTIC_ANALYSIS_PROGRAM_ERROR);
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
                        if (parse_expression(true)) {
                                if (is_first_pass) {
                                        if (expr_result.id_name != NULL) {
                                                int expr_data_type =  expr_result.variable.data_type;

                                                if (current_variable.variable.data_type != expr_data_type) {
                                                        fprintf(stderr, "Incompatible s types to assign value.\n");
                                                        cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                                }
                                                expr_result.id_name = NULL;
                                        }
                                } else {
                                        if (expr_result.id_name != NULL) {
                                                int expr_data_type = expr_result.is_function ? expr_result.function.return_type : expr_result.variable.data_type;
                                                if (function_variable.variable.data_type != expr_data_type) {
                                                        fprintf(stderr, "Incompatible types to assign value.\n");
                                                        cleanup_exit(SEMANTIC_ANALYSIS_TYPE_COMPATIBILITY_ERROR);
                                                }
                                                expr_result.id_name = NULL;
                                        }
                                }
                                return PARSED_OK;
                        }
                }
        } if (t.type == SEMICOLON) {
                return PARSED_OK;
        }
        return PARSE_ERROR;
}

int parse_declaration() {
        if (t.type == LEFT_ROUNDED_BRACKET) {
                if (is_first_pass) {
                        is_static_variable_declaration = false;
                } else {
                        skip_precedence_analysis = false;
                }
                return parse_method_declaration ();
        } else if (t.type == RIGHT_ROUNDED_BRACKET) {
                if (get_token() == STATIC) {
                        return parse_class_element();
                } else {
                        return PARSED_OK;
                }
        }  else if (t.type == ASSIGN || t.type == SEMICOLON) {
                current_function.id_name = NULL;

                if (parse_value()) {
                        if (t.type == SEMICOLON) {
                                if (is_first_pass) {
                                        is_static_variable_declaration = false;
                                        if (!is_declared(current_variable.id_name)) {
                                                insert_variable_symbol_table(current_variable.id_name, current_variable.variable.data_type, CONSTANT);
                                        } else {
                                                fprintf(stderr, "Variable \'%s\' in class \'%s\' was redeclared.\n", current_variable.id_name, current_class);
                                                cleanup_exit(SEMANTIC_ANALYSIS_PROGRAM_ERROR);
                                        }

                                            printf("VAR %s %p\n", current_variable.id_name, &expr_result.variable);
                                        tVar * to = &get_symbol_table_class_item(current_class, current_variable.id_name)->variable;
                                        to->initialized = true;
                                        tVar * from = expr_var_result;
                                        //printf("hodnota vysledku %s\n", from->s);


                                        DLInsertLast(global_inst_tape, generate(I_ASSIGN, from, NULL, to));
                                        DLInsertLast(global_inst_tape, generate(I_PRINT, to, NULL, NULL));

                                        //printf("init %d\n", to->initialized);
                                        //DLInsertLast(global_inst_tape, generate(I_PRINT, to, NULL, NULL));
                                } else {
                                        skip_precedence_analysis = false;
                                }
                                current_variable.id_name = NULL;
                                get_token();
                                if (t.type == STATIC || t.type == RIGHT_CURVED_BRACKET) {
                                        return parse_class_element();
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
                                has_function_return = false;
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
                        is_static_variable_declaration = true;
                } else /* second pass */ {
                        function_variable.variable.data_type = t.type;
                        skip_precedence_analysis = true;
                }

                if (parse_param()) {
                        current_function.id_name = t.string_value; /* potrebne pre oba prechody */
                        if (is_first_pass) {
                                has_function_return = false;
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
                                if (!exists_class(current_class)) {
                                        insert_class(current_class);
                                        set_current_class(current_class);

                                        /* TODO
                                           insert_tmp_variable_symbol_table_class(42);
                                           symbol_table_item_t * p = get_symbol_table_class_item(current_class, "#0");
                                           printf("test %d\n", p->variable.data_type);

                                         */
                                } else {
                                        fprintf(stderr, "Class \'%s\' was redeclared.\n", current_class);
                                        cleanup_exit(SEMANTIC_ANALYSIS_PROGRAM_ERROR);
                                }
                        }
                        if (get_token() == LEFT_CURVED_BRACKET) {
                                get_token();
                                if (parse_class_element()) {
                                        if (t.type == RIGHT_CURVED_BRACKET) {
                                                get_token();
                                                if (t.type == CLASS || t.type == EOF) {
                                                        return parse_class_list();
                                                } else {
                                                        return PARSE_ERROR;
                                                }
                                        }
                                }
                        }
                }
        } else if (t.type == EOF) {
                return PARSED_OK;
        }
        return PARSE_ERROR;
}


void add_builtin_functions() {
        char *ifj_class = copy_string("ifj16");
        insert_class(ifj_class);
        set_current_class(ifj_class);

        insert_function_symbol_table(copy_string("sort"), STRING, 1, 0, copy_string("s"), NULL, NULL);
        insert_function_symbol_table(copy_string("find"), INT, 2, 0, copy_string("ss"), NULL, NULL);
        insert_function_symbol_table(copy_string("length"), INT, 1, 0, copy_string("s"), NULL, NULL);
        insert_function_symbol_table(copy_string("compare"), INT, 2, 0, copy_string("ss"), NULL, NULL);
        insert_function_symbol_table(copy_string("substr"), INT, 3, 0, copy_string("ssi"), NULL, NULL);
        insert_function_symbol_table(copy_string("print"), VOID, 1, 0, copy_string("s"), NULL, NULL);

        insert_function_symbol_table(copy_string("readInt"), INT, 0, 0, NULL, NULL, NULL);
        insert_function_symbol_table(copy_string("readDouble"), DOUBLE, 0, 0, NULL, NULL, NULL);
        insert_function_symbol_table(copy_string("readString"), INT, 0, 0, NULL, NULL, NULL);
}

int parse(tDLList * inst_tape) {
        global_inst_tape = inst_tape;

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
                                symbol_table_item_t * run_method = get_symbol_table_class_item("Main", "run");
                                if (run_method == NULL) {
                                        fprintf(stderr, "Missing function\'Main.run\'.\n");
                                        cleanup_exit(SEMANTIC_ANALYSIS_PROGRAM_ERROR);
                                } else {
                                        if (!(run_method->function.return_type == VOID) || !(run_method->function.params_count == 0)) {
                                                fprintf(stderr, "Bad signature of \'Main.run\' method.\n");
                                                cleanup_exit(SEMANTIC_ANALYSIS_PROGRAM_ERROR);
                                        }
                                }
                        } else /* second pass */ {
                                // free_class_list(); // uvolni zoznam tried
                        }

                        if (is_first_pass) {
                                is_first_pass = false;
                                is_second_pass = true;
                        } else /* second pass */ {
                                // free_token_buffer(&token_buffer); // uvolni zoznam tokenov urceny pre druhy prechod
                        }
                        return PARSED_OK;
                } else {
                        // free_token_buffer(&token_buffer);
                        return SYNTACTIC_ANALYSIS_ERROR;
                }
        } else {
                return SYNTACTIC_ANALYSIS_ERROR;
        }
}
