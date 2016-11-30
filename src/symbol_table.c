/*
 * Project: Interpret for IFJ16
 * Date: November 2016, FIT VUT
 * Team: 026
 * Author: Dávid Bolvanský, xbolva00
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "strings.h"
#include "symbol_table.h"
#include "ial.h"
#include "scanner.h"
#include "error_codes.h"
#include "parser.h"
#include "memory_constants.h"

symbol_table_t *class_list;
char *current_class;

/* Frees the class list table */
void dispose_class_list(tData data) {
        ht_free((symbol_table_t *) (data));
}

/* Frees the symbol table and its items */
void dispose_class_symbol_table(tData data) {
        symbol_table_item_t *item = (symbol_table_item_t *) data;

        if (item->is_function) {
                if (item->function.param_data_types != NULL) {
                        free(item->function.param_data_types);
                        item->function.param_data_types = NULL;
                }
                if (item->function.local_vars_data_types != NULL) {
                        free(item->function.local_vars_data_types);
                        item->function.local_vars_data_types = NULL;
                }
                if (item->function.symbol_table != NULL) {
                        ht_free((symbol_table_t *) (item->function.symbol_table));
                }

                if (item->function.instruction_tape != NULL) {
                        DisposeList(item->function.instruction_tape);
                        free(item->function.instruction_tape);
                }
        } else {
                if (item->variable.initialized && item->variable.data_type == STRING) {
                        free(item->variable.s);
                        item->variable.s = NULL;
                }
        }
        free(item);
}

/* Class list table initialization */
void init_class_list() {
        class_list = ht_init(CLASS_TABLE_SIZE, hash_code, dispose_class_list);
}

/* Sets current class */
void set_current_class(char *class_name) {
        current_class = class_name;
}

/* Inserts class to the class list table */
bool insert_class(char *class_name) {
        symbol_table_t *symbol_table = ht_init(SYMBOL_TABLE_SIZE, hash_code, dispose_class_symbol_table);
        ht_insert(class_list, class_name, symbol_table);
        return true;
}

/* Creates a local symbol table for a function */
symbol_table_t *create_function_symbol_table() {
        symbol_table_t *p = ht_init(SYMBOL_TABLE_SIZE, hash_code, dispose_class_symbol_table);
        if (p == NULL)
                return NULL;
        return p;
}

/* Gets a symbol table for the class */
symbol_table_t *get_symbol_table_for_class(char *class_name) {
        if (class_list == NULL) {
                return NULL;
        }
        symbol_table_t *p = ht_read(class_list, class_name);
        if (p == NULL)
                return NULL;
        return p;
}

/* Gets a local symbol table for the function */
symbol_table_t *get_symbol_table_for_function(char *class_name, char *id_name) {
        symbol_table_item_t *item = get_symbol_table_class_item(class_name, id_name);
        if (item == NULL) {
                return NULL;
        }
        return item->function.symbol_table;
}

/* Inserts the item to the symbol table of the given class */
bool insert_symbol_table_item_class(char *class_name, char *id_name, void *data) {
        symbol_table_t *symbol_table = get_symbol_table_for_class(class_name);
        ht_insert(symbol_table, id_name, data);
        return true;

}

/*Inserts the item into the symbol table of the current class */
bool insert_symbol_table_item(char *id_name, void *data) {
        return insert_symbol_table_item_class(current_class, id_name, data);

}

/* Checks if the class exists */
bool exists_class(char *class_name) {
        return get_symbol_table_for_class(class_name) != NULL;
}

/* Creates an item of the symbol table */
symbol_table_item_t *create_symbol_table_item() {
        symbol_table_item_t *p = (symbol_table_item_t *) calloc(1, sizeof(struct symbol_table_item));
        if (p == NULL) {
                exit(INTERNAL_INTERPRET_ERROR);
        }
        return p;
}

/* Inserts the item - global static variable - to the symbol table of the current class */
symbol_table_item_t *insert_variable_symbol_table(char *id_name, int data_type, int offset) {
        symbol_table_item_t *p = create_symbol_table_item();
        p->variable.data_type = data_type;
        p->variable.offset = offset;
        p->variable.initialized = false;
        if (data_type == STRING) {
                p->variable.s = NULL;
        }
        p->is_function = false;
        p->declared = true;
        insert_symbol_table_item(id_name, p);
        return p;
}

/* Inserts parameters or local variables of the function to the symbol table of this function */
symbol_table_item_t *insert_function_variable_symbol_table(symbol_table_t * symbol_table, char *id_name, int data_type,
                                                           int offset) {
        symbol_table_item_t *p = create_symbol_table_item();
        p->id_name = id_name;
        p->variable.data_type = data_type;
        p->variable.offset = offset;
        p->variable.initialized = false;
        if (data_type == STRING) {
                p->variable.s = NULL;
        }
        p->is_function = false;
        p->declared = true;
        ht_insert(symbol_table, id_name, p);
        return p;
}

/* Inserts a function to the symbol table of the current class */
symbol_table_item_t *insert_function_symbol_table(char *id_name, int data_type,
                                                  int params_count,
                                                  int local_vars_count, char *param_data_types,
                                                  char *local_vars_data_types, symbol_table_t * symbol_table) {
        symbol_table_item_t *p = create_symbol_table_item();
        p->id_name = id_name;
        p->function.return_type = data_type;
        p->function.params_count = params_count;
        p->function.local_vars_count = local_vars_count;
        p->function.params_local_vars_count = params_count + local_vars_count;
        p->function.param_data_types = param_data_types;
        if (local_vars_data_types == NULL) {
            p->function.local_vars_data_types = copy_string("");
        }
        p->function.symbol_table = symbol_table;
        p->is_function = true;
        p->declared = true;
        insert_symbol_table_item(id_name, p);
        return p;
}

/* Sets information about the function */
symbol_table_item_t *set_function_symbol_table(char *id_name, int data_type,
                                               int params_count,
                                               int local_vars_count, char *param_data_types,
                                               char *local_vars_data_types, symbol_table_t * symbol_table) {
        symbol_table_item_t *p = get_symbol_table_class_item(current_class, id_name);
        p->id_name = id_name;
        p->function.return_type = data_type;
        p->function.params_count = params_count;
        p->function.local_vars_count = local_vars_count;
        p->function.params_local_vars_count = params_count + local_vars_count;
        p->function.param_data_types = param_data_types;
        free(p->function.local_vars_data_types); /* clears empty string */
        p->function.local_vars_data_types = local_vars_data_types;
        p->function.symbol_table = symbol_table;
        p->is_function = true;
        p->declared = true;
        return p;
}

/* Inserts a instruction tape of the function to the symbol table of this function */
void insert_instr_tape_for_function(char *class_name, char *function_name, tList * tape) {
        symbol_table_item_t *function_item = get_symbol_table_class_item(class_name, function_name);
        function_item->function.instruction_tape = tape;
}

/* Gets an item from the symbol table of the given class */
symbol_table_item_t *get_symbol_table_class_item(char *class_name, char *id_name) {
        if (id_name == NULL) {
                return NULL;
        }
        symbol_table_t *symbol_table = get_symbol_table_for_class(class_name);
        if (symbol_table == NULL) {
                return NULL;
        }
        symbol_table_item_t *symbol_table_item = ht_read(symbol_table, id_name);
        return symbol_table_item;
}

/* Checks if a variable or a function exists in the symbol table of the current class */
bool is_declared(char *id_name) {
        symbol_table_item_t *symbol_table_item = get_symbol_table_class_item(current_class, id_name);
        if (symbol_table_item == NULL) {
                return false;
        }
        return symbol_table_item->declared;
}

/* Gets the item from the symbol table of the function */
symbol_table_item_t *get_symbol_table_function_item(symbol_table_t * symbol_table, char *id_name) {
        symbol_table_item_t *symbol_table_item = ht_read(symbol_table, id_name);
        if (symbol_table_item == NULL) {
                return NULL;
        }

        return symbol_table_item;
}

/* Check if a parameter or a local variable exists in the symbol table of the function */
bool is_declared_in_function(symbol_table_t * symbol_table, char *id_name) {
        symbol_table_item_t *symbol_table_item = get_symbol_table_function_item(symbol_table, id_name);
        if (symbol_table_item == NULL) {
                return false;
        }
        return symbol_table_item->declared;
}

/* Converts data type to specific char */
void append_type(string_t * str, int type) {
        char data_type_char = 0;
        switch (type) {
        case INT:
                data_type_char = 'i';
                break;
        case DOUBLE:
                data_type_char = 'd';
                break;
        case STRING:
                data_type_char = 's';
                break;
        case BOOLEAN:
                data_type_char = 'b';
                break;
        default:
                break;
        }

        append_char(str, data_type_char);
}


/* Gets an item from the symbol table for the fully qualified identificator */
symbol_table_item_t *get_symbol_table_special_id_item(char *id_name) {
        char *special_id = copy_string(id_name);
        char *delimeter = ".";
        char *class = strtok(special_id, delimeter);
        char *method = strtok(NULL, delimeter);

        symbol_table_item_t *item = get_symbol_table_class_item(class, method);
        free(special_id);
        if (item == NULL) {
                return NULL;
        }
        return item;
}

/* Checks if fully qualified identificator exists, whether there is a variable or function in the class */
bool is_special_id_declared(char *id_name) {
        symbol_table_item_t *item = get_symbol_table_special_id_item(id_name);
        if (item == NULL) {
                return false;
        }
        return item->declared;
}

/* Creates local temp variable in the symbol table of the current class */
symbol_table_item_t *insert_tmp_variable_symbol_table_class(int data_type) {
        symbol_table_item_t *p = create_symbol_table_item();
        char *id_name = (char *) malloc(TMP_VAR_NAME_SIZE * sizeof(char));
        if (id_name == NULL) {
                exit(INTERNAL_INTERPRET_ERROR);
        }
        insert_string_const(&mem_constants, id_name);
        static int tmp_id = 0;
        sprintf(id_name, "#%d", tmp_id);

        p->variable.data_type = data_type;
        p->variable.initialized = true;
        if (data_type == STRING) {
                p->variable.s = NULL;
        }
        p->is_function = false;
        p->declared = true;
        p->id_name = id_name;
        p->variable.offset = CONSTANT;
        insert_symbol_table_item(id_name, p);
        tmp_id++;
        return p;
}

/* Creates local temp variable in the symbol table of the current function */
symbol_table_item_t *insert_tmp_variable_symbol_table_function(char *function_name, int data_type) {
        symbol_table_item_t *p = create_symbol_table_item();
        symbol_table_item_t *function_item = get_symbol_table_class_item(current_class, function_name);
        symbol_table_t *function_table = function_item->function.symbol_table;

        int offset = function_item->function.params_local_vars_count;
        char *local_vars_data_types = function_item->function.local_vars_data_types;
        int new_len = strlen(local_vars_data_types) + 1;
        local_vars_data_types = (char *) realloc(local_vars_data_types, (new_len + 1) * sizeof(char));

        int c = 0;
        switch (data_type) {
        case STRING:
                c = 's';
                break;
        case INT:
                c = 'i';
                break;
        case DOUBLE:
                c = 'd';
                break;
        case BOOLEAN:
                c = 'b';
                break;
        }

        local_vars_data_types[new_len - 1] = c;
        local_vars_data_types[new_len] = '\0';
        function_item->function.local_vars_data_types = local_vars_data_types;

        char *id_name = (char *) malloc(TMP_VAR_NAME_SIZE * sizeof(char));
        if (id_name == NULL) {
                exit(INTERNAL_INTERPRET_ERROR);
        }
        insert_string_const(&mem_constants, id_name);
        static int tmp_id = 0;
        sprintf(id_name, "#%d", tmp_id);

        p->id_name = id_name;
        p->variable.data_type = data_type;
        p->variable.offset = offset;
        p->variable.initialized = true;
        if (data_type == STRING) {
                p->variable.s = NULL;
        }
        p->is_function = false;
        p->declared = true;

        function_item->function.params_local_vars_count++;
        function_item->function.local_vars_count++;
        ht_insert(function_table, id_name, p);
        tmp_id++;
        return p;
}

/* Creates instruction tape for a function */
tList *create_function_instr_tape() {
        tList *tape = (tList *) malloc(sizeof(tList));
        if (tape == NULL) {
                exit(INTERNAL_INTERPRET_ERROR);
        }
        return tape;
}

/* Frees class list table */
void free_class_list() {
        if (class_list == NULL) return;
        ht_free(class_list);
}

/* Stack for jumps initialization*/
void js_init() {
        head = NULL;
}

/* Pushes instruction into the jumop stack */
void js_push(tElemPtr instr) {
        js_item *p = malloc(sizeof(js_item));
        if (p == NULL) {
                exit(INTERNAL_INTERPRET_ERROR);
        }
        p->data = instr;
        p->next = head;
        head = p;
}

/* Gets the top jump stack item */
tElemPtr js_top() {
        return head->data;
}

/* Pops the top jump stack item */
void js_pop() {
        js_item *tmp = head;
        head = head->next;
        free(tmp);
}

/* Frees jump stack */
void js_free() {
        if (head == NULL)
                return;
        js_item *tmp = head;
        js_item *next = NULL;
        while (tmp != NULL) {
                next = tmp;
                tmp = tmp->next;
                free(next);
        }
}
