#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "symbol_table.h"

class_t *class_list;

void print_list() {
        class_t  * current = class_list;
        while (current != NULL) {
                printf("%s\n", current->class_name);
                current = current->next;
        }
}

bool insert_class(char* class_name) {
        class_t * new_class = NULL;
        new_class = malloc(sizeof(class_t));
        if (new_class == NULL) {
                return false;
        }
        new_class->class_name = class_name;
        new_class->a = 5;
        new_class->next = class_list;
        class_list = new_class;
        return true;
}

int get_symbol_table_for_class(char *class_name) {
    class_t  * current = class_list;
    while (current != NULL) {
            printf("%s\n", current->class_name);

            if (strcmp(current->class_name, class_name) == 0) {
                return current->a;
            }
            current = current->next;
    }

    return -1;
}

void free_class_list() {
    if (class_list == NULL) {
        return;
    }
    class_t  * current = class_list;
    while (current != NULL) {
            class_t* tmp = current->next;
            free(current);
            current = tmp;
    }
}
