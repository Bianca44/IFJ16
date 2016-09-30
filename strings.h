#ifndef STRINGS_H
#define STRINGS_H

#include <stdbool.h>

#define STRING_INITIAL_ALLOCATION 20

typedef struct {
	char *data;
	int length;
	int allocated_size;
} string;




bool init_string(string * str);

void free_string(string * str);
void clear_string(string * str);

bool set_string(string *str, char *text);

bool copy_string(string *str, char *text);

bool append_string(string *str, char *text);

char *get_string(string * str);
int get_length(string * str);

bool is_empty(string * str);

void remove_last_char(string * str);

bool append_char(string * str, char c);


#endif
