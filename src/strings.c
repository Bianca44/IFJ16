#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "strings.h"

bool init_string(string_t *str) {
        if ((str->data = (char *) malloc(STRING_INITIAL_ALLOCATION * sizeof(char))) == NULL) {
                return false;
        }
        str->data[0] = '\0';
        str->length = 0;
        str->allocated_size = STRING_INITIAL_ALLOCATION;
        return true;

}

void clear_string(string_t *str) {
        free(str->data);
        str->length = 0;
        str->allocated_size = 0;
}

bool copy_string(string_t *str, char *text) {
        int new_length = strlen(text);
        if (new_length + 1 >= str->allocated_size) {
                if ((str->data =
                             (char *) realloc(str->data, new_length * sizeof(char))) == NULL) {
                        return false;
                }
                str->allocated_size = new_length;
        }
        strcpy(str->data, text);
        str->length = new_length;
        str->data[str->length] = '\0';
        return true;
}

bool append_char(string_t *str, char c) {
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
