#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "ial.h"

typedef tHTable symbol_table_t;
typedef enum {TINT, TDOUBLE, TSTRING, TBOOLEAN} data_type;

typedef struct {
    char * id_name;
    int data_type;
    int offset;
} variable_t;

typedef struct {
    char * id_name;
    int data_type;
    int params_count;
    int local_vars_count;
    char * param_data_types;
    symbol_table_t * symbol_table;
} function_t;

typedef struct symbol_table_item {
        char * id_name;
        int data_type;
        bool is_function;
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
                        char * param_data_types;
                        symbol_table_t * symbol_table;
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
bool put_function_symbol_table(char * id_name, int data_type, int params_count, int local_vars_count, char * param_data_types, symbol_table_t * symbol_table);
bool is_declared(char * id_name);
void append_param_data_types(int type);
symbol_table_t * get_symbol_table_for_class(char* class_name);
symbol_table_item_t * get_function_symbol_table(char * class_name, char * id_name);
symbol_table_t * create_function_symbol_table();
bool put_function_variable_symbol_table(symbol_table_t *symbol_table, char * id_name, int data_type, int offset);
bool is_declared_in_function(symbol_table_t * symbol_table, char * id_name);

#endif //SYMBOL_TABLE_H
