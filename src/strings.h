#ifndef STRINGS_H
#define STRINGS_H

#include <stdbool.h>

#define STRING_INITIAL_ALLOCATION 20

typedef struct {
    char *data;
    int length;
    int allocated_size;
} string_t;




bool init_string(string_t *str);

bool copy_string(string_t *str, char *text);

void clear_string(string_t *str);

void remove_last_char(string_t *str);

bool append_char(string_t *str, char c);

#endif // STRINGS_H
