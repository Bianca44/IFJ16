#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef struct class {
    char * class_name;
    // TS pre triedy
    int a ;
    struct class * next;
} class_t;

void print_list();
bool insert_class(char* class_name);
int get_symbol_table_for_class(char *class_name);
void free_class_list();

#endif //SYMBOL_TABLE_H
