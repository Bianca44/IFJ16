/*
 * Project: Interpret for IFJ16
 * Date: November 2016, FIT VUT
 * Team: 026
 * Authors: Dávid Bolvanský, xbolva00
 *          Martin Marušiak, xmarus07
 */


#include <stdio.h>
#include <stdlib.h>
#include "symbol_table.h"
#include "memory_constants.h"
#include "scanner.h"
#include "debug.h"


/* Inserts integer number to the list of constants */
tVar *insert_int_const(constant_t ** const_arr, int num) {
        tVar *new = (tVar *) malloc(sizeof(tVar));
        if (new == NULL) {
                return new;
        }
        new->data_type = INT;
        new->i = num;
        new->initialized = true;
        new->offset = CONSTANT;

        constant_t *item = (constant_t *) malloc(sizeof(constant_t));
        if (item == NULL) {
                return NULL;
        }
        item->data = new;
        item->next = *const_arr;
        *const_arr = item;
        return new;
}

/* Inserts double number to the list of constants */
tVar *insert_double_const(constant_t ** const_arr, double num) {
        tVar *new = (tVar *) malloc(sizeof(tVar));
        if (new == NULL) {
                return new;
        }
        new->data_type = DOUBLE;
        new->d = num;
        new->initialized = true;
        new->offset = CONSTANT;

        constant_t *item = (constant_t *) malloc(sizeof(constant_t));
        if (item == NULL) {
                return NULL;
        }
        item->data = new;
        item->next = *const_arr;
        *const_arr = item;
        return new;
}

/* Inserts string value to the list of constants */
tVar *insert_string_const(constant_t ** const_arr, char *string) {
        tVar *new = (tVar *) malloc(sizeof(tVar));
        if (new == NULL) {
                return new;
        }
        new->data_type = STRING;
        new->s = string;
        new->initialized = true;
        new->offset = CONSTANT;

        constant_t *item = (constant_t *) malloc(sizeof(constant_t));
        if (item == NULL) {
                return NULL;
        }
        item->data = new;
        item->next = *const_arr;
        *const_arr = item;
        return new;
}

/* Inserts boolean value to the list of constants */
tVar *insert_boolean_const(constant_t ** const_arr, bool state) {
        tVar *new = (tVar *) malloc(sizeof(tVar));
        if (new == NULL) {
                return new;
        }
        new->data_type = BOOLEAN;
        new->b = state;
        new->initialized = true;
        new->offset = CONSTANT;

        constant_t *item = (constant_t *) malloc(sizeof(constant_t));
        if (item == NULL) {
                return NULL;
        }
        item->data = new;
        item->next = *const_arr;
        *const_arr = item;
        return new;
}

/* Inserts special structure used by interpret to the list of constants */
tVar *insert_special_const(constant_t ** const_arr, void *special) {
        tVar *new = (tVar *) malloc(sizeof(tVar));
        if (new == NULL) {
                return new;
        }
        new->data_type = RETURN;
        new->s = special;
        new->initialized = true;
        new->offset = CONSTANT;

        constant_t *item = (constant_t *) malloc(sizeof(constant_t));
        if (item == NULL) {
                return NULL;
        }
        item->data = new;
        item->next = *const_arr;
        *const_arr = item;

        return new;
}

/* Frees all constants from the list */
void free_constants(constant_t ** const_arr) {
        if (const_arr == NULL) {
                return;
        }
        constant_t *tmp = *const_arr;
        constant_t *next = NULL;
        while (tmp != NULL) {
                if (tmp->data->data_type == STRING) {
                        free(tmp->data->s);
                }

                free(tmp->data);
                tmp->data = NULL;
                next = tmp->next;
                free(tmp);
                tmp = next;
        }
        *const_arr = NULL;
}
