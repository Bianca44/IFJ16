#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "symbol_table.h"
#include "ial.h"

tHTable *class_list;

void dispose_class_list(tData data) {
        ht_free((tHTable *)(data));
}

void dispose_class_symbol_table(tData data) {
        ht_free((tHTable *)(data)); // TODO
}

void init_class_list() {
        class_list = ht_init(11, hash_code, dispose_class_list); /* TODO */
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

void free_class_list() {
        ht_free(class_list);
}
