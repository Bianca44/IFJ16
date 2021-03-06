/*
 * Project: Interpret for IFJ16
 * Date: November 2016, FIT VUT
 * Team: 026
 * Author: Dávid Bolvanský, xbolva00
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token_buffer.h"
#include "debug.h"


/* Token buffer initializiation */
bool init_token_buffer(token_buffer_t * token_buf) {
        token_buf->list = (token_t **) malloc(TOKEN_BUFFER_INITIAL_ALLOCATION * sizeof(token_t));
        if (token_buf->list == NULL) {
                return false;
        }
        token_buf->length = 0;
        token_buf->allocated_size = TOKEN_BUFFER_INITIAL_ALLOCATION;
        return true;
}


/* Adds the token to the token buffer */
bool add_token_to_buffer(token_buffer_t * token_buf, token_t * t) {
        if (token_buf->length == token_buf->allocated_size) {
                int new_size = token_buf->allocated_size + TOKEN_BUFFER_INITIAL_ALLOCATION;
                token_buf->list = (token_t **) realloc(token_buf->list, new_size * sizeof(token_t));
                if (token_buf->list == NULL) {
                        return false;
                }
                token_buf->allocated_size = new_size;

        }

        int i = token_buf->length;
        token_buf->list[i] = (token_t *) malloc(sizeof(token_t));
        memcpy(token_buf->list[i], t, sizeof(token_t));
        token_buf->length++;
        return true;
}


/* Gets the token from the token buffer */
token_t *get_next_token_buffer(token_buffer_t * token_buf) {
        static int i = 0;

        if (i < token_buf->length) {
                return token_buf->list[i++];
        } else {
                return NULL;
        }
}


/* Frees the token buffer */
void free_token_buffer(token_buffer_t * token_buf) {
        if (token_buf == NULL)
                return;
        for (int i = 0; i < token_buf->length; i++) {
                if (token_buf->list[i]->type == ID || token_buf->list[i]->type == SPECIAL_ID)
                        free(token_buf->list[i]->string_value);

                free(token_buf->list[i]);
                token_buf->list[i] = NULL;
        }

        if (token_buf->list != NULL) {
                free(token_buf->list);
                token_buf->list = NULL;
        }

        token_buf->length = 0;
        token_buf->allocated_size = 0;

}


/* Frees the local token buffer used for expression parsing */
void free_token_buffer_local(token_buffer_t * token_buf) {
        if (token_buf == NULL)
                return;
        for (int i = 0; i < token_buf->length; i++) {
                free(token_buf->list[i]);
                token_buf->list[i] = NULL;
        }

        if (token_buf->list != NULL) {
                free(token_buf->list);
                token_buf->list = NULL;
        }

        token_buf->length = 0;
        token_buf->allocated_size = 0;

}
