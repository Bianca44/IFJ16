#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>
#include "strings.h"

/* Inicializacia stringu na STRING_INITIAL_ALLOCATION bajtov */
bool init_string(string * str) {
	if ((str->data =
	     (char *) malloc(STRING_INITIAL_ALLOCATION * sizeof(char))) == NULL) {
		return false;
	}
	str->data[0] = '\0';
	str->length = 0;
	str->allocated_size = STRING_INITIAL_ALLOCATION;
	return true;

}

/* Nastavi hodnotu stringu */
bool set_string(string * str, char *text) {
	clear_string(str);
	return copy_string(str, text);
}

/* Skopiruje text z jedneho stringu do druheho */
bool copy_string(string * str, char *text) {
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

/* Pripne text na koniec stringu */
bool append_string(string * str, char *text) {
	int new_length = strlen(text);
	if (str->length + new_length + 1 >= str->allocated_size) {
		if ((str->data =
		     (char *) realloc(str->data,
				      (str->length + new_length) * sizeof(char))) ==
		    NULL) {
			return false;
		}
		str->allocated_size = str->length + new_length;
	}
	strcat(str->data, text);
	str->length = str->length + new_length;
	str->data[str->length] = '\0';
	return true;
}

void free_string(string * str) {
	free(str->data);
}

void clear_string(string * str) {
	str->data[0] = '\0';
	str->length = 0;
}

char *get_string(string * str) {
	return str->data;
}

int get_length(string * str) {
	return str->length;
}

bool is_empty(string * str) {
	if (str->length == 0) {
		return true;
	}
	return false;
}

void remove_last_char(string * str) {
	if (is_empty(str)) {
		return;
	}

	str->data[--str->length] = '\0';
}

bool append_char(string * str, char c) {
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
