
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token_buffer.h"

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
        token_buf->list[i] = (token_t *) malloc(sizeof(token_t));
        memcpy(token_buf->list[i], t, sizeof(token_t));
        token_buf->length++;
        return true;
}

token_t* get_next_token_buffer(token_buffer_t *token_buf) {
        static int i = 0;

        if (i < token_buf->length) {
                return token_buf->list[i++];
        } else {
                return NULL;
        }
}
char *tr_names[TOKENS_COUNT] = { "LEXICAL_ERROR", "ID", "INT_LITERAL", "DOUBLE_LITERAL", "ADD", "SUB", "MUL",
                                 "DIV", "SEMICOLON", "LEFT_CURVED_BRACKET", "RIGHT_CURVED_BRACKET",
                                 "LEFT_ROUNDED_BRACKET", "RIGHT_ROUNDED_BRACKET", "ASSIGN", "LOGICAL_AND",
                                 "LOGICAL_OR", "COMMA", "NEG",  "STRING_LITERAL", "NOT_EQUAL", "LESS_EQUAL",
                                 "LESS", "GREATER_EQUAL", "GREATER", "EQUAL", "SPECIAL_ID", "BOOLEAN", "BREAK",
                                 "CLASS", "CONTINUE", "DO", "DOUBLE", "ELSE", "FALSE", "FOR", "IF", "INT", "RETURN",
                                 "STRING", "STATIC", "TRUE", "VOID", "WHILE" };

void free_token_buffer(token_buffer_t *token_buf) {
        if (token_buf == NULL) return;
        for (int i = 0; i < token_buf->length; i++) {
            free(token_buf->list[i]);
        }

        if (token_buf->list != NULL) {
                free(token_buf->list);
        }

        token_buf->length = 0;
        token_buf->allocated_size = 0;

}
