#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "symbol_table.h"
#include "ial.h"

tHTable *class_list;

void dispose_class_list(tData data) {
        free((class_t *)(data));
}

void dispose_class_symbol_table(tData data) {
        //free((tHTable *)(data)); // TODO
        ht_free((tHTable *)(data));
}

void init_class_list() {
        class_list = ht_init(11, hash_code, dispose_class_list); /* TODO */
}

bool insert_class(char* class_name) {
        class_t * new_class = NULL;
        new_class = malloc(sizeof(class_t));
        if (new_class == NULL) {
                return false;
        }
        new_class->a = 5;
        new_class->class_symbol_table = ht_init(11, hash_code, dispose_class_symbol_table);
        ht_insert(class_list, class_name, new_class);
        return true;
}

int get(char* class_name) {
        class_t * p = ht_read(class_list, class_name);
        if (p == NULL) return -1;
        return p->a;
}

tHTable * get_symbol_table_for_class(char* class_name) {
        class_t * p = ht_read(class_list, class_name);
        if (p == NULL) return NULL;
        return p->class_symbol_table;
}

void free_class_list() {
        ht_free(class_list);
}

void free_class_symbol_table() {
        //ht_free(class_symbol_table);
}
