#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token_buffer.h"

int main(void) {
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
    t.attr.double_value = 2.5e-12;
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
    printf("cislo %d\n", get_next_token_buffer(&tb)->type);
    printf("data %s\n", get_next_token_buffer(&tb)->attr.string_value);
    printf("cislo %d\n", get_next_token_buffer(&tb)->type);
    printf("cislo %g\n", get_next_token_buffer(&tb)->attr.double_value);

    for (int i = 0; i < 1000; i++) {
	t.type = i;
	add_token_to_buffer(&tb, &t);
    }

    free_token_buffer(&tb);

    return 0;
}
