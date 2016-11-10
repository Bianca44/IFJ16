#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "strings.h"
#include "symbol_table.h"
#include "ial.h"
#include "scanner.h"

symbol_table_t *class_list;
char* current_class;
variable_t current_variable;
variable_t function_variable;
function_t current_function;

string_t param_data_types;

void dispose_class_list(tData data) {
        ht_free((tHTable *)(data));
}

void dispose_class_symbol_table(tData data) {
        free((symbol_table_item_t *)(data)); // TODO
}

void init_class_list() {
        class_list = ht_init(11, hash_code, dispose_class_list); /* TODO */
}

void set_current_class(char *class_name) {
        current_class = class_name;
}

bool insert_class(char* class_name) {
        tHTable *symbol_table = ht_init(11, hash_code, dispose_class_symbol_table);
        ht_insert(class_list, class_name, symbol_table);
        return true;
}

symbol_table_t * create_function_symbol_table() {
        symbol_table_t * p = ht_init(11, hash_code, dispose_class_symbol_table);
        if (p == NULL) return NULL;
        return p;
}


int get(char* class_name) {
        if (class_list == NULL) return -1;
        symbol_table_t * p = ht_read(class_list, class_name);
        if (p == NULL) return -1;
        return p->n_items;
}

symbol_table_t * get_symbol_table_for_class(char* class_name) {
        if (class_list == NULL) {
                return NULL;
        }
        symbol_table_t * p = ht_read(class_list, class_name);
        if (p == NULL) return NULL;
        return p;
}

bool insert_symbol_table_item_class(char * class_name, char * id_name, void * data) {
        symbol_table_t * symbol_table = get_symbol_table_for_class(class_name);
        ht_insert(symbol_table, id_name, data);
        return true;

}

bool insert_symbol_table_item(char * id_name, void * data) {
        return insert_symbol_table_item_class(current_class, id_name, data);

}

bool exists_class(char* class_name) {
        return get_symbol_table_for_class(class_name) != NULL;
}

symbol_table_item_t * create_symbol_table_item() {
        symbol_table_item_t * p = (symbol_table_item_t *) malloc(sizeof(struct symbol_table_item));
        return p;
}

bool put_variable_symbol_table(char * id_name, int data_type, int offset) {
        symbol_table_item_t * p = create_symbol_table_item();
        p->id_name = id_name;
        p->data_type = data_type;
        p->content.variable.offset = offset;
        p->is_function = false;
        p->declared = true;
        insert_symbol_table_item(id_name, p);
        return true;
}

bool put_function_variable_symbol_table(symbol_table_t *symbol_table, char * id_name, int data_type, int offset) {
        symbol_table_item_t * p = create_symbol_table_item();
        p->id_name = id_name;
        p->data_type = data_type;
        p->content.variable.offset = offset;
        p->is_function = false;
        p->declared = true;
        ht_insert(symbol_table, id_name, p);
        return true;
}

bool put_function_symbol_table(char * id_name, int data_type, int params_count, int local_vars_count, char * param_data_types) {
        symbol_table_item_t * p = create_symbol_table_item();
        p->id_name = id_name;
        p->data_type = data_type;
        p->content.function.params_count = params_count;
        p->content.function.local_vars_count = local_vars_count;
        p->content.function.param_data_types = param_data_types;
        p->is_function = true;
        p->declared = true;
        insert_symbol_table_item(id_name, p);
        return true;
}

symbol_table_item_t * get_function_symbol_table(char * class_name, char * id_name) {
        symbol_table_t * sym_table = get_symbol_table_for_class(class_name);
        if (sym_table == NULL) {
                return NULL;
        }
        symbol_table_item_t * sym_table_item = ht_read(sym_table, id_name);
        return sym_table_item;
}

bool is_declared(char * id_name) {
        tHTable * sym_table = get_symbol_table_for_class(current_class);
        symbol_table_item_t * sym_table_item = ht_read(sym_table, id_name);
        if (sym_table_item == NULL) {
                return false;
        }
        return sym_table_item->declared;
}

void append_param_data_types(int type) {
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

        append_char(&param_data_types, data_type_char);
}

void free_class_list() {
        ht_free(class_list);
}
