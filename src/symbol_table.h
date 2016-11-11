#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>
#include "ial.h"
#include "strings.h"
// TODO
#define SYMBOL_TABLE_SIZE 11

typedef tHTable symbol_table_t;

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

typedef struct {
    int data_type;
    union {
        int i;
        double d;
        char *s;
        bool b;
    };
    bool initialized;
    int offset;
} tVar;

typedef struct symbol_table_item {
        char * id_name;
        bool is_function;
        bool declared;
        union {
                tVar variable;
                struct {
                        int return_type;
                        int params_count;
                        int local_vars_count;
                        char * param_data_types;
                        symbol_table_t * symbol_table;
                        // TODO: Pointer na instrukcnu pasku
                } function;
        };
} symbol_table_item_t;

void init_class_list();
bool insert_class(char* class_name);
void free_class_list();
bool exists_class(char* class_name);
symbol_table_t * get_symbol_table_for_function(char *class_name, char * id_name);
bool insert_symbol_table_item_class(char * class_name, char * id_name, void * data);
bool insert_symbol_table_item(char * id_name, void * data);
void set_current_class(char *class_name);
symbol_table_item_t * create_symbol_table_item();
bool insert_variable_symbol_table(char * id_name, int data_type, int offset);
bool insert_function_symbol_table(char * id_name, int data_type, int params_count, int local_vars_count, char * param_data_types, symbol_table_t * symbol_table);
bool is_declared(char * id_name);
void append_param_data_types(int type);
symbol_table_t * get_symbol_table_for_class(char* class_name);
symbol_table_item_t * get_symbol_table_class_item(char * class_name, char * id_name);
symbol_table_t * create_function_symbol_table();
bool insert_function_variable_symbol_table(symbol_table_t *symbol_table, char * id_name, int data_type, int offset);
symbol_table_item_t * get_symbol_table_function_item(symbol_table_t * symbol_table, char * id_name);
bool is_declared_in_function(symbol_table_t * symbol_table, char * id_name);
bool is_special_id_declared(char * id_name);

extern symbol_table_item_t current_variable;
extern symbol_table_item_t function_variable;
extern symbol_table_item_t current_function;
extern string_t param_data_types;
extern char* current_class;
#endif //SYMBOL_TABLE_H
