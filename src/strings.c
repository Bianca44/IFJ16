/*
 * Project: Interpret for IFJ16
 * Date: November 2016, FIT VUT
 * Team: 026
 * Author: Dávid Bolvanský, xbolva00
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "strings.h"
#include "debug.h"

/* String initializiation, allocates initial string size */
bool init_string(string_t * str) {
        if ((str->data = (char *) malloc(STRING_INITIAL_ALLOCATION * sizeof(char))) == NULL) {
                return false;
        }
        str->data[0] = '\0';
        str->length = 0;
        str->allocated_size = STRING_INITIAL_ALLOCATION;

        return true;

}

/* Clears string */
void clear_string(string_t * str) {
        str->length = 0;
        str->allocated_size = 0;
}

/* Creates a copy of the string */
char *copy_string(char *text) {
        if (text == NULL) {
                return NULL;
        }
        char *ptr;
        int length = strlen(text);
        if ((ptr = (char *) malloc((length + 1) * sizeof(char))) == NULL) {
                return false;
        }
        memcpy(ptr, text, length);
        ptr[length] = '\0';
        return ptr;
}

/* Appends char to the string */
bool append_char(string_t * str, char c) {
        if (str->length + 1 >= str->allocated_size) {
                int new_size = (str->length + STRING_INITIAL_ALLOCATION) * sizeof(char);
                if ((str->data = (char *) realloc(str->data, new_size)) == NULL) {
                        return false;
                }
                str->allocated_size = new_size;
        }
        str->data[str->length] = c;
        str->length++;
        str->data[str->length] = '\0';
        return true;
}

/* Frees string from the memory */
void free_string(string_t * str) {
        if (str->data != NULL) {
                free(str->data);
                str->data = NULL;
        }
        clear_string(str);
}
