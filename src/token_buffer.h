#ifndef TOKEN_BUFFER_H
#define TOKEN_BUFFER_H

#include <stdbool.h>
#include "scanner.h"

typedef struct {
    token_t **list;
    int length;
    int allocated_size;
} token_buffer_t;

#define TOKEN_BUFFER_INITIAL_ALLOCATION 100

bool init_token_buffer(token_buffer_t *token_buf);
bool add_token_to_buffer(token_buffer_t *token_buf, token_t *t);
void free_token_buffer(token_buffer_t *token_buf);
token_t* get_next_token_buffer(token_buffer_t *token_buf);

#endif //TOKEN_BUFFER_H
