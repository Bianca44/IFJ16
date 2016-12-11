/*
 * Project: Interpret for IFJ16
 * Date: November 2016, FIT VUT
 * Team: 026
 * Author: Dávid Bolvanský, xbolva00
 */

#ifndef STRINGS_H
#define STRINGS_H

#include <stdbool.h>

#define STRING_INITIAL_ALLOCATION 20

typedef struct {
    char *data;
    int length;
    int allocated_size;
} string_t;




bool init_string(string_t * str);

void clear_string(string_t * str);

char *copy_string(char *text);

bool append_char(string_t * str, char c);

void free_string(string_t * str);

#endif				// STRINGS_H
