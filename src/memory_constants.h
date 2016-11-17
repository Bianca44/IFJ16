#ifndef MEMORY_CONSTANTS_H
#define MEMORY_CONSTANTS_H

#include "symbol_table.h"

#define CONSTANT -1

typedef struct constant {
        tVar * data;
        struct constant * next;
} constant_t;

void insert_int_const(constant_t **const_arr, int num);
void insert_double_const(constant_t **const_arr, double num);
void insert_string_const(constant_t **const_arr, char * string);
void insert_boolean_const(constant_t **const_arr, bool state);

#endif // MEMORY_CONSTANTS_H
