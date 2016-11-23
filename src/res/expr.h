#ifndef EXPR_Hr
#define EXPR_H
#include "token_buffer.h"
#include "scanner.h"
#include "symbol_table.h"
#include "instructions.h"

int get_psa(token_buffer_t *buffer,symbol_table_item_t * st_item, tVar** expr_result);
tVar * generate_tmp_var(int data_type);
void free_token_buffer_exit(int exit_code);

#endif // EXPR_H
