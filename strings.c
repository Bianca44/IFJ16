#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>

#define STRING_INITIAL_ALLOCATION 20

typedef struct {
	char *data;
	int length;
	int allocated_size;
} string;

bool init_string(string * str)
{
	if ((str->data =
	     (char *) malloc(STRING_INITIAL_ALLOCATION * sizeof(char))) == NULL) {
		return false;
	}
	str->data[0] = '\0';
	str->length = 0;
	str->allocated_size = STRING_INITIAL_ALLOCATION;
	return true;

}

void free_string(string * str)
{
	free(str->data);
}

void clear_string(string * str)
{
	str->data[0] = '\0';
	str->length = 0;
}

char *get_string(string * str)
{
	return str->data;
}

int get_length(string * str)
{
	return str->length;
}

bool is_empty(string * str)
{
	if (str->length == 0) {
		return true;
	}
	return false;
}

void remove_last_char(string * str)
{
	if (is_empty(str)) {
		return;
	}

	str->data[--str->length] = '\0';
}

bool append_char(string * str, char c)
{
	if (str->length + 1 >= str->allocated_size) {
		if ((str->data =
		     (char *) realloc(str->data,
				      (str->length +
				       STRING_INITIAL_ALLOCATION) * sizeof(char))) ==
		    NULL) {
			return false;
		}
		str->allocated_size = str->length + STRING_INITIAL_ALLOCATION;
	}
	str->data[str->length++] = c;
	str->data[str->length] = '\0';
	return true;
}
