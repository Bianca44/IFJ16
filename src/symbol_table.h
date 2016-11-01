#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "ial.h"

typedef struct class {
    // TS pre triedy
    int a ;
    tHTable *class_symbol_table;
} class_t;

void init_class_list();
bool insert_class(char* class_name);
int get(char* class_name);
void free_class_list();

#endif //SYMBOL_TABLE_H
