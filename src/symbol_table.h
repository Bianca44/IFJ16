#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "ial.h"

enum data_types {TINT, TDOUBLE, TSTRING, TBOOLEAN};

typedef struct class {
    // TS pre triedy
    int a ;
    tHTable *class_symbol_table;
} class_t;

void init_class_list();
bool insert_class(char* class_name);
int get(char* class_name);
void free_class_list();
bool exists_class(char* class_name);
bool insert_symbol_table_item(char * id_name, void * data);
void set_current_class(char *class_name);

#endif //SYMBOL_TABLE_H
