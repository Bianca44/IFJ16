#ifndef EXPR_Hr
#define EXPR_H
#include "token_buffer.h"
#include "scanner.h"
#include "symbol_table.h"
#include "instructions.h"

int get_psa(token_buffer_t *buffer,symbol_table_item_t * st_item, tVar** expr_result);
tVar * generate_tmp_var(int data_type);
tVar* op_1;
tVar* op_2;


#endif // EXPR_H
