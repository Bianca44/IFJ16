#ifndef EXPR_H
#define EXPR_H
#include "token_buffer.h"
#include "scanner.h"
#include "symbol_table.h"
#include "instructions.h"

int get_psa(token_buffer_t *buffer, symbol_table_item_t * expr_result);



#endif // EXPR_H
