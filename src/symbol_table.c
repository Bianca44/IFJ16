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

bool insert_symbol_table_item_class(char * class_name, char * id_name, void * data) {
        tHTable * sym_table = get_symbol_table_for_class(class_name);
        ht_insert(sym_table, id_name, data);
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
    /*p->id_name = "hello";
    p->content.variable.offset = 5;
    printf("%s\n", p->id_name);
    printf("%d\n", p->content.variable.offset);*/
    return p;
}

bool put_variable_symbol_table(char * id_name, data_type, int offset) {
    symbol_table_item_t * p = create_symbol_table_item();
    p->id_name = "bb";
    p->data_type = 2;
    p->declared = true;
    p->content.variable.offset = offset;
    return true;
}

void free_class_list() {
        ht_free(class_list);
}
