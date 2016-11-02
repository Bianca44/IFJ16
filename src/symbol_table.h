#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "ial.h"

typedef enum {TINT, TDOUBLE, TSTRING, TBOOLEAN} data_type;

typedef struct {
    char * id_name;
    int data_type;
} var_t;

typedef struct {
    char * id_name;
    int data_type;
    int params_count;
    int local_vars_count;
    char * param_data_types;
} function_t;

typedef struct symbol_table_item {
        char * id_name;
        int data_type;
        bool declared;
        union {
                struct {
                        int offset;
                        union {
                                char * string_value;
                                int int_value;
                                double double_value;
                                bool bool_value;
                        } value;
                } variable;

                struct {
                        int params_count;
                        int local_vars_count;
                        char * param_data_types; // e.g. idbs
                } function;
        } content;
} symbol_table_item_t;

void init_class_list();
bool insert_class(char* class_name);
int get(char* class_name);
void free_class_list();
bool exists_class(char* class_name);
bool insert_symbol_table_item_class(char * class_name, char * id_name, void * data);
bool insert_symbol_table_item(char * id_name, void * data);
void set_current_class(char *class_name);
symbol_table_item_t * create_symbol_table_item();
bool put_variable_symbol_table(char * id_name, int data_type, int offset);
bool put_function_symbol_table(char * id_name, int data_type, int params_count, int local_vars_count, char * param_data_types);
bool is_declared(char * id_name);
void append_param_data_types(int type);

#endif //SYMBOL_TABLE_H
