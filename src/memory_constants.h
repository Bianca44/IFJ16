/*
 * Project: Interpret for IFJ16
 * Date: November 2016, FIT VUT
 * Team: 026
 * Authors: Dávid Bolvanský, xbolva00
 *          Martin Marušiak, xmarus07
 */

#ifndef MEMORY_CONSTANTS_H
#define MEMORY_CONSTANTS_H

#include "symbol_table.h"

typedef struct constant {
    tVar *data;
    struct constant *next;
} constant_t;

tVar *insert_int_const(constant_t ** const_arr, int num);
tVar *insert_double_const(constant_t ** const_arr, double num);
tVar *insert_string_const(constant_t ** const_arr, char *string);
tVar *insert_boolean_const(constant_t ** const_arr, bool state);
tVar *insert_special_const(constant_t ** const_arr, void *special);
void free_constants(constant_t ** const_arr);

#endif				// MEMORY_CONSTANTS_H
