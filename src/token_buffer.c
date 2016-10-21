#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token_buffer.h"

int test(void) {
        token_buffer_t tb;
        init_token_buffer(&tb);
        token_t t;
        t.type = 42;

        add_token_to_buffer(&tb, &t);
        t.type = 43;
        t.attr.string_value = "hello";
        add_token_to_buffer(&tb, &t);
        t.type = 44;
        add_token_to_buffer(&tb, &t);
        add_token_to_buffer(&tb, &t);
        add_token_to_buffer(&tb, &t);
        add_token_to_buffer(&tb, &t);
        add_token_to_buffer(&tb, &t);
        add_token_to_buffer(&tb, &t);
        add_token_to_buffer(&tb, &t);
        add_token_to_buffer(&tb, &t);
        add_token_to_buffer(&tb, &t);
        add_token_to_buffer(&tb, &t);
        printf("cislo %d\n", tb.list[11]->type);
        printf("cislo %d\n", tb.allocated_size);
        printf("cislo %d\n", get_next_token_buffer(&tb).type);
        printf("data %s\n", get_next_token_buffer(&tb).attr.string_value);
        printf("cislo %d\n", get_next_token_buffer(&tb).type);
        free_token_buffer(&tb);

        return 0;
}

bool init_token_buffer(token_buffer_t *token_buf) {
        token_buf->list = (token_t **) malloc(TOKEN_BUFFER_INITIAL_ALLOCATION * sizeof(token_t));
        if (token_buf->list == NULL) {
                return false;
        }
        token_buf->length = 0;
        token_buf->allocated_size = TOKEN_BUFFER_INITIAL_ALLOCATION;
        return true;
}

bool add_token_to_buffer(token_buffer_t *token_buf, token_t *t) {
        if (token_buf->length == token_buf->allocated_size) {
                int new_size = token_buf->allocated_size + TOKEN_BUFFER_INITIAL_ALLOCATION;
                token_buf->list = (token_t **) realloc(token_buf->list, new_size * sizeof(token_t));
                if (token_buf->list == NULL) {
                        return false;
                }
                token_buf->allocated_size = new_size;

        }

        int i = token_buf->length;
        token_buf->list[i] =(token_t *) malloc(sizeof(token_t));
        memcpy(token_buf->list[i], t, sizeof(token_t));
        token_buf->length++;
        return true;
}

token_t get_next_token_buffer(token_buffer_t *token_buf) {
        static int i = 0;
        return *token_buf->list[i++];
}

void free_token_buffer(token_buffer_t *token_buf) {
        for (int i = 0; i < token_buf->length; i++) {
                free(token_buf->list[i]);
        }

        free(token_buf->list);
}
