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

char *t_names[TOKENS_COUNT] = { "LEXICAL_ERROR", "ID", "INT_LITERAL", "DOUBLE_LITERAL", "ADD", "SUB", "MUL",
                                "DIV", "SEMICOLON", "LEFT_CURVED_BRACKET", "RIGHT_CURVED_BRACKET",
                                "LEFT_ROUNDED_BRACKET", "RIGHT_ROUNDED_BRACKET", "ASSIGN", "LOGICAL_AND",
                                "LOGICAL_OR", "COMMA", "NEG",  "STRING_LITERAL", "NOT_EQUAL", "LESS_EQUAL",
                                "LESS", "GREATER_EQUAL", "GREATER", "EQUAL", "SPECIAL_ID", "BOOLEAN", "BREAK",
                                "CLASS", "CONTINUE", "DO", "DOUBLE", "ELSE", "FALSE", "FOR", "IF", "INT", "RETURN",
                                "STRING", "STATIC", "TRUE", "VOID", "WHILE" };


int parser_error_flag = 0; // no error
int function_offset = 0;
int params_counter = 0;
bool is_first_pass = true;
bool is_second_pass = false;
bool function_has_return = false;
bool static_var_declaration = false;
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
symbol_table_item_t psa_result;
char* current_class;
char* function_call_name;

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

        token_buffer_t tb;

        /* Prvy prechod vyhodnot vyraz u globalnej premmenej priamo */
        if (is_first_pass && static_var_declaration) {
                init_token_buffer(&tb);
                while (1) {
                        if (t.type == SEMICOLON) break;

                        printf("%s, ", t_names[t.type]);

                        add_token_to_buffer(&tb, &t);

                        if (t.type == SPECIAL_ID) {
                                if (!is_special_id_declared(t.string_value)) {
                                        fprintf(stderr,"EXPR: SPECIAL ID %s is not declared\n", t.string_value);
                                } else {
                                        symbol_table_item_t * item = get_symbol_table_special_id_item(t.string_value);
                                        if (item->is_function) {
                                                printf("EXPR: SPECIAL ID %s is function\n", t.string_value);
                                        }
                                }
                        }
                        else if (t.type == ID) {
                                if (!is_declared(t.string_value)) {
                                        fprintf(stderr,"EXPR: ID (global) NOT DECLARED %s\n", t.string_value);
                                } else {
                                        symbol_table_item_t * item = get_symbol_table_class_item(current_class, t.string_value);
                                        if (item->is_function) {
                                                fprintf(stderr,"EXPR: ID %s is function\n", t.string_value);
                                        }
                                }
                        }
                        get_token();
                }
                printf("\n");
                //get_psa(&tb);
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
                                        fprintf(stderr, "ID in EXPR not found %s\n", t.string_value);
                                        //exit(4);
                                } else {
                                        item = get_symbol_table_class_item(current_class, t.string_value);
                                }
                        } else {
                                item = get_symbol_table_function_item(function_symbol_table, t.string_value);
                        }

                        is_function = item->is_function;
                } else if (t.type == SPECIAL_ID) {
                        item = get_symbol_table_special_id_item(t.string_value);
                        if (item == NULL) {
                                fprintf(stderr, "SPECIAL ID in EXPR not found\n");
                                //exit(3);
                        }
                        is_function = item->is_function;
                }

                if (is_function) {
                        //printf("data type %d\n", item->function.return_type);
                        function_call_name = t.string_value;
                        psa_result = *item;
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
                                                                printf("ARGS: ocakava %d mame %d\n", function_item->function.params_count, params_counter);
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
                                        fprintf(stderr,"EXPR: SPECIAL ID %s is not declared\n", t.string_value);
                                } else {
                                        symbol_table_item_t * item = get_symbol_table_special_id_item(t.string_value);
                                        if (item->is_function) {
                                                fprintf(stderr,"EXPR: SPECIAL ID %s is function\n", t.string_value);
                                        }
                                }
                        }
                        else if (t.type == ID) {
                                symbol_table_t *function_symbol_table = get_symbol_table_for_function(current_class, current_function.id_name);
                                if (function_symbol_table == NULL || (function_symbol_table != NULL &&  !is_declared_in_function(function_symbol_table, t.string_value))) {
                                        if (!is_declared(t.string_value)) {
                                                fprintf(stderr, "EXPR: ID (global or local) NOT DECLARED %s\n", t.string_value);
                                                //exit(4);
                                        } else {
                                                symbol_table_item_t * item =  get_symbol_table_class_item(current_class, t.string_value);
                                                if (item->is_function) {
                                                        fprintf(stderr, "EXPR: SPECIAL ID %s is function\n", t.string_value);
                                                }
                                        }
                                }
                        }
                }
                get_token();
        }

        if (is_second_pass) {
                // PSA
                printf("\n");
                //get_psa(&tb);
        }
        return PARSED_OK;

}

int parse_return_value() {
        if (t.type == RETURN) {
                if (is_first_pass) {
                        function_has_return = true;
                }
                get_token();
                if (t.type == LEFT_ROUNDED_BRACKET || t.type == ID || t.type == SPECIAL_ID || t.type == INT_LITERAL || t.type == DOUBLE_LITERAL || t.type == STRING_LITERAL || t.type == TRUE || t.type == FALSE) {
                        if (is_first_pass) {
                                if (current_function.function.return_type == VOID) {
                                        fprintf(stderr,"return E in VOID error\n");
                                        //exit (8);
                                }
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
                        if (is_second_pass) {
                                symbol_table_item_t * item;
                                int data_type = 0;
                                if (t.type == SPECIAL_ID) {
                                        if (!is_special_id_declared(t.string_value)) {
                                                fprintf(stderr, "ARG: Special id %s is not declared\n", t.string_value);
                                                //exit(4);
                                        } else {
                                                item = get_symbol_table_special_id_item(t.string_value);
                                                if (item->is_function) {
                                                        fprintf(stderr, "ARG: Special id %s is function\n", t.string_value);
                                                        //exit(4);
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
                                                        fprintf(stderr, "ARG: VAR IN FUNCTION (global or local) NOT DECLARED %s\n", t.string_value);
                                                        //exit(4);
                                                } else {
                                                        item = get_symbol_table_class_item(current_class, t.string_value);
                                                        if (item->is_function) {
                                                                fprintf(stderr, "ARG: ID %s is function\n", t.string_value);
                                                                //exit(4);
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

                                if (params_counter + 1 > function_item->function.params_count) {
                                        fprintf(stderr, "Too many arguments\n");
                                }

                                char expected_param_type = function_item->function.param_data_types[params_counter];

                                switch (expected_param_type) {
                                case 's': if (data_type != STRING) {
                                                printf("fail\n");
                                }
                                        break;
                                case 'b': if (data_type != BOOLEAN) {
                                                printf("fail\n");
                                }
                                        break;
                                case 'i': if (data_type == DOUBLE) {
                                                printf("konvertuj\n");

                                } else if (data_type != INT) {
                                                printf("fail\n");
                                }
                                        break;
                                case 'd': if (data_type != DOUBLE) {
                                                printf("fail\n");
                                }
                                        break;
                                }

                                params_counter++;


                                //symbol_table_item_t *function_item = get_symbol_table_class_item(current_class, function_call_name);
                                //printf("fun %s %d datapyes %s\n", function_item->id_name, function_item->function.return_type, function_item->function.param_data_types);
                                printf("funkcia %s datovy typ argumentu %s pocet param %d\n", function_call_name, t_names[data_type], params_counter);
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
                                                        fprintf(stderr, "ARG: Special id %s is not declared\n", t.string_value);
                                                        //exit(4);
                                                } else {
                                                        item = get_symbol_table_special_id_item(t.string_value);
                                                        if (item->is_function) {
                                                                fprintf(stderr, "ARG: Special id %s is function\n", t.string_value);
                                                                //exit(4);
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
                                                                fprintf(stderr, "ARG: VAR IN FUNCTION (global or local) NOT DECLARED %s\n", t.string_value);
                                                                //exit(4);
                                                        } else {
                                                                item = get_symbol_table_class_item(current_class, t.string_value);
                                                                if (item->is_function) {
                                                                        fprintf(stderr, "ARG: ID %s is function\n", t.string_value);
                                                                        //exit(4);
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
                                                fprintf(stderr, "Param in no param function\n");
                                                // TODO
                                                //exit(3);
                                        }


                                        char expected_param_type = function_item->function.param_data_types[params_counter];

                                        switch (expected_param_type) {
                                        case 's': if (data_type != STRING) {
                                                        printf("fail\n");
                                        }
                                                break;
                                        case 'b': if (data_type != BOOLEAN) {
                                                        printf("fail\n");
                                        }
                                                break;
                                        case 'i': if (data_type == DOUBLE) {
                                                        printf("konvertuj\n");

                                        } else if (data_type != INT) {
                                                        printf("fail\n");
                                        }
                                                break;
                                        case 'd': if (data_type != DOUBLE) {
                                                        printf("fail\n");
                                        }
                                                break;
                                        }

                                        params_counter++;


                                        //symbol_table_item_t *function_item = get_symbol_table_class_item(current_class, function_call_name);
                                        //printf("fun %s %d datapyes %s\n", function_item->id_name, function_item->function.return_type, function_item->function.param_data_types);
                                        printf("funkcia %s datovy typ argumentu %s pocet param %d\n", function_call_name, t_names[data_type], params_counter);
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
                                                symbol_table_item_t *function_item = get_symbol_table_special_id_item(function_call_name);
                                                char expected_param_type = function_item->function.param_data_types[params_counter];
                                                printf("ifj16.print vyzaduje %c\n", expected_param_type);
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
                                                        printf("ARGS: ocakava %d mame %d\n", function_item->function.params_count, params_counter);
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
        } else if (t.type == INT || t.type == DOUBLE || t.type == STRING || t.type == BOOLEAN) {
                current_function.function.local_vars_count++;
                function_variable.variable.data_type = t.type;
                if (parse_param()) {
                        if (is_first_pass) {
                                //printf("local fun var .. name %s type %d offset %d\n", function_variable.id_name, function_variable.variable.data_type, function_variable.variable.offset);
                                if (!is_declared_in_function(current_function.function.symbol_table, function_variable.id_name)) {
                                        insert_function_variable_symbol_table(current_function.function.symbol_table, function_variable.id_name, function_variable.variable.data_type, function_variable.variable.offset);
                                        function_variable.variable.offset++;
                                        function_variable.id_name = NULL;
                                } else {
                                        fprintf(stderr, "VAR IN FUNC REDECLARED\n");
                                        //exit(3);
                                }
                        }
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
                                        fprintf(stderr, "SPECIAL ID NOT DECLARED\n");
                                        //exit(3);
                                } else {
                                        symbol_table_item_t * p = get_symbol_table_special_id_item(t.string_value);
                                        if (p->is_function) {
                                                //function_call_name = t.string_value;
                                        }
                                        function_variable.variable.data_type = p->is_function ? p->function.return_type : p->variable.data_type;
                                        //printf("MAM TYP %d\n", function_variable.variable.data_type);
                                }
                        } else if (t.type == ID) {
                                if (!is_declared(t.string_value)) {
                                        symbol_table_t *function_symbol_table = get_symbol_table_for_function(current_class, current_function.id_name);
                                        if(!is_declared_in_function(function_symbol_table, t.string_value)) {
                                                fprintf(stderr, "VAR IN FUNCTION (global or local) NOT DECLARED %s\n", t.string_value);
                                                //exit(3);
                                        } else {
                                                symbol_table_item_t * p = get_symbol_table_function_item(function_symbol_table, t.string_value);
                                                function_variable.variable.data_type = p->is_function ? p->function.return_type : p->variable.data_type;
                                                //printf("MAM TYP3 %d\n", function_variable.variable.data_type);
                                        }
                                } else {
                                        symbol_table_item_t * p = get_symbol_table_class_item(current_class, t.string_value);
                                        if (p->is_function) {
                                                //function_call_name = t.string_value;
                                        }
                                        function_variable.variable.data_type = p->is_function ? p->function.return_type : p->variable.data_type;
                                        //printf("MAM TYP2 %d\n", function_variable.variable.data_type);
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
                                if (!function_has_return) {
                                        fprintf(stderr, "FUNKCIA NEMA RETURN\n");
                                        //exit(8);
                                }
                                function_has_return = false;
                        }
                        current_function.function.param_data_types = (param_data_types.length > 0) ? param_data_types.data : NULL; /* for consistency */
                        current_function.function.params_count = param_data_types.length;
                        printf("name=%s, ret_type=%d, data_types=%s, params_count=%d, local_vars_count=%d\n", current_function.id_name, current_function.function.return_type, current_function.function.param_data_types, current_function.function.params_count, current_function.function.local_vars_count);
                        if (!is_declared(current_function.id_name)) {
                                insert_function_symbol_table(current_function.id_name, current_function.function.return_type, current_function.function.params_count, current_function.function.local_vars_count, current_function.function.param_data_types, current_function.function.symbol_table);
                                current_function.id_name = NULL;
                        } else {
                                fprintf(stderr, "FUNCTION REDECLARED\n");
                                //exit(3);
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
                                //printf("local fun var .. name %s type %d offset %d\n", function_variable.id_name, function_variable.variable.data_type, function_variable.variable.offset);
                                if (!is_declared_in_function(current_function.function.symbol_table, function_variable.id_name)) {
                                        insert_function_variable_symbol_table(current_function.function.symbol_table, function_variable.id_name, function_variable.variable.data_type, function_variable.variable.offset);
                                        function_variable.variable.offset++;
                                        function_variable.id_name = NULL;
                                } else {
                                        fprintf(stderr, "VAR IN FUNC REDECLARED\n");
                                        //exit(3);
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
                                        //printf("local fun var .. name %s type %d offset %d\n", function_variable.id_name, function_variable.variable.data_type, function_variable.variable.offset);
                                        if (!is_declared_in_function(current_function.function.symbol_table, function_variable.id_name)) {
                                                insert_function_variable_symbol_table(current_function.function.symbol_table, function_variable.id_name, function_variable.variable.data_type, function_variable.variable.offset);
                                                function_variable.variable.offset++;
                                                function_variable.id_name = NULL;
                                        } else {
                                                fprintf(stderr, "VAR IN FUNC REDECLARED\n");
                                                //exit(3);
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
                        append_param_data_types(t.type);
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
                                        fprintf(stderr, "VAR IN FUNC REDECLARED\n");
                                        //exit(3);
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
                                if (is_second_pass) {
                                        if (psa_result.id_name != NULL) {
                                                int expr_data_type = psa_result.is_function ? psa_result.function.return_type : psa_result.variable.data_type;
                                                printf("premna s typom %s, priradujem expr %s...\n", t_names[function_variable.variable.data_type], t_names[expr_data_type]);
                                                psa_result.id_name = NULL;

                                                // CHYBA 4 v konflikte
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
                        static_var_declaration = false;
                }
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
                                current_variable.id_name = NULL;
                        } else {
                                fprintf(stderr, "VAR REDECLARED\n");
                                //exit(3);
                        }
                }

                if (parse_value()) {
                        if (t.type == SEMICOLON) {
                                if (is_first_pass) {
                                        static_var_declaration = false;
                                }
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
                        static_var_declaration = true;
                } else /* second pass */ {
                        function_variable.variable.data_type = t.type;
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
                                        fprintf(stderr, "CLASS REDECLARED\n");
                                        //exit(3);;
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
                                        fprintf(stderr, "No RUN in Main class or no MAIN class\n");
                                        //exit(3);
                                } else {
                                        if (!(run_method->function.return_type == VOID) || !(run_method->function.params_count == 0)) {
                                                fprintf(stderr, "Weird RUN in Main class\n");
                                                //exit(3);
                                        }
                                }
                        } else /* second pass */ {
                                free_class_list(); // uvolni zoznam tried
                        }

                        if (is_first_pass) {
                                is_first_pass = false;
                                is_second_pass = true;
                        } else /* second pass */ {
                                free_token_buffer(&token_buffer); // uvolni zoznam tokenov urceny pre druhy prechod
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
