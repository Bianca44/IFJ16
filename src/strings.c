#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>
#include "strings.h"

bool init_string(string *str) {
        if ((str->data = malloc(STRING_INITIAL_ALLOCATION * sizeof(char))) == NULL) {
                return false;
        }
        str->data[0] = '\0';
        str->length = 0;
        str->allocated_size = STRING_INITIAL_ALLOCATION;
        return true;

}

bool copy_string(string *str, char text[]) {
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

void clear_string(string *str) {
        free(str->data);
        str->length = 0;
        str->allocated_size = 0;
}

void remove_last_char(string *str) {
        if (str->length == 0) {
                return;
        }
        str->length--;
        str->data[str->length] = '\0';
}

bool append_char(string *str, char c) {
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

char *clone_string(const char *s) {
        char *d = malloc (strlen (s) + 1);
        if (d == NULL) return NULL;
        strcpy (d,s);
        return d;
}
