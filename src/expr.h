/*
 * Project: Interpret for IFJ16
 * Date: November 2016, FIT VUT
 * Team: 026
 * Author: Juraj Ondrej Dúbrava, xdubra03
 */

#ifndef EXPR_H
#define EXPR_H
#include "token_buffer.h"
#include "scanner.h"
#include "symbol_table.h"
#include "instructions.h"

int get_psa(token_buffer_t * buffer, symbol_table_item_t * st_item, tVar ** expr_result);
tVar *generate_tmp_var(int data_type);
void expr_exit(int exit_code);

#endif				// EXPR_H
