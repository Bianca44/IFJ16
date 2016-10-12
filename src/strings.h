#ifndef STRINGS_H
#define STRINGS_H

#include <stdbool.h>

#define STRING_INITIAL_ALLOCATION 20

typedef struct {
    char *data;
    int length;
    int allocated_size;
} string;




bool init_string(string *str);

bool copy_string(string *str, char text[]);

void clear_string(string *str);

void remove_last_char(string *str);

bool append_char(string *str, char c);

char *clone_string(const char *s);


#endif
