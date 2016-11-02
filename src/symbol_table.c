#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "symbol_table.h"
#include "ial.h"

tHTable *class_list;
char* current_class;

void dispose_class_list(tData data) {
        ht_free((tHTable *)(data));
}

void dispose_class_symbol_table(tData data) {
        free((char *)(data)); // TODO hack
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

int get(char* class_name) {
        tHTable * p = ht_read(class_list, class_name);
        if (p == NULL) return -1;
        return p->n_items;
}

tHTable * get_symbol_table_for_class(char* class_name) {
        tHTable * p = ht_read(class_list, class_name);
        if (p == NULL) return NULL;
        return p;
}

bool insert_symbol_table_item(char * id_name, void * data) {
    tHTable * sym_table = get_symbol_table_for_class(current_class);
    ht_insert(sym_table, id_name, data);
    return true;

}

bool exists_class(char* class_name) {
    return get_symbol_table_for_class(class_name) != NULL;
}

void free_class_list() {
        ht_free(class_list);
}
