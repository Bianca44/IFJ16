/*
 * Project: Interpret for IFJ16
 * Date: November 2016, FIT VUT
 * Team: 026
 * Author: Martin Marušiak, xmarus07
 */

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "symbol_table.h"
#include "scanner.h"
#include "memory_constants.h"
#include "interpret.h"

/* instruction function */
typedef void tInst_fun(tVar *op1, tVar *op2, tVar *result);

/* used for storing labels */
extern constant_t * labels;

/* instruction */
typedef struct tInst{
    tInst_fun *f;
    tVar *op1;
    tVar *op2;
    tVar *result;
} tInst;

/* dispose function for instruction*/
void dispose_inst(void *);

typedef enum instructions {
    /* INPUT */
    I_RINT,
    I_RDBL,
    I_RSTR,
    /* ARITHMETIC */
    I_ADD,
    I_MUL,
    I_SUB,
    I_DIV,
    I_INC,
    I_DEC,
    /* CONVERSIONS */
    I_CONV_I_TO_D,
    I_TO_STRING,
    /* LOGICAL */
    I_E,
    I_NE,
    I_L,
    I_G,
    I_LE,
    I_GE,
    I_NOT,
    I_AND,
    I_OR,
    /* BUILT-IN AND OTHERS */
    I_ASSIGN,
    I_CAT,
    I_STRCMP,
    I_SUBSTR,
    I_FIND,
    I_SORT,
    I_PRINT,
    I_LEN,
    /* ASSOCIATED WITH FUNCTIONS */
    I_INIT_FRAME,
    I_PUSH_PARAM,
    I_F_CALL,
    I_RETURN,
    /* JUMPS */
    I_GOTO,
    I_JNT,
    I_JT,
    I_NOP,
}tInstId;

tInst * generate(tInstId instruction, void *op1, void *op2, void *result);
void set_label(tElemPtr jump, tElemPtr where);

/* INPUT */
void i_rint(tVar *op1, tVar *op2, tVar *result);
void i_rdbl(tVar *op1, tVar *op2, tVar *result);
void i_rstr(tVar *op1, tVar *op2, tVar *result);
/* ARITHMETIC */
void i_add_i(tVar *op1, tVar *op2, tVar *result);
void i_add_d(tVar *op1, tVar *op2, tVar *result);
void i_sub_i(tVar *op1, tVar *op2, tVar *result);
void i_sub_d(tVar *op1, tVar *op2, tVar *result);
void i_inc_i(tVar *op1, tVar *op2, tVar *result);
void i_dec_i(tVar *op1, tVar *op2, tVar *result);
void i_mul_i(tVar *op1, tVar *op2, tVar *result);
void i_mul_d(tVar *op1, tVar *op2, tVar *result);
void i_div_i(tVar *op1, tVar *op2, tVar *result);
void i_div_d(tVar *op1, tVar *op2, tVar *result);
/* Conversions */
void i_conv_i_to_d(tVar *op1, tVar *op2, tVar *result);
void i_to_str(tVar *op1, tVar *op2, tVar *result);
/* assigns */
void i_assign_i(tVar *op1, tVar *op2, tVar *result);
void i_assign_d(tVar *op1, tVar *op2, tVar *result);
void i_assign_i_to_d(tVar *op1, tVar *op2, tVar *result);
void i_assign_b(tVar *op1, tVar *op2, tVar *result);
void i_assign_s(tVar *op1, tVar *op2, tVar *result);
/* function */
void i_init_frame(tVar *op1, tVar *op2, tVar *result);
void i_push_param(tVar *op1, tVar *op2, tVar *result);
void i_f_call(tVar *op1, tVar *op2, tVar *result);
void i_return(tVar *op1, tVar *op2, tVar *result);
/* built-in */
void i_cat(tVar *op1, tVar *op2, tVar *result);
void i_strcmp(tVar *op1, tVar *op2, tVar *result);
void i_substr(tVar *op1, tVar *op2, tVar *result);
void i_find(tVar *op1, tVar *op2, tVar *result);
void i_sort(tVar *op1, tVar *op2, tVar *result);
void i_print(tVar *op1, tVar *op2, tVar *result);
void i_len(tVar *op1, tVar *op2, tVar *result);
/* logical */
/* equal */
void i_e_i(tVar *op1, tVar *op2, tVar *result);
void i_e_d(tVar *op1, tVar *op2, tVar *result);
void i_e_b(tVar *op1, tVar *op2, tVar *result);
/* not equal */
void i_ne_i(tVar *op1, tVar *op2, tVar *result);
void i_ne_d(tVar *op1, tVar *op2, tVar *result);
void i_ne_b(tVar *op1, tVar *op2, tVar *result);
/* less */
void i_l_i(tVar *op1, tVar *op2, tVar *result);
void i_l_d(tVar *op1, tVar *op2, tVar *result);
/* greater */
void i_g_i(tVar *op1, tVar *op2, tVar *result);
void i_g_d(tVar *op1, tVar *op2, tVar *result);
/* less or equal */
void i_le_i(tVar *op1, tVar *op2, tVar *result);
void i_le_d(tVar *op1, tVar *op2, tVar *result);
/* greater or equal */
void i_ge_i(tVar *op1, tVar *op2, tVar *result);
void i_ge_d(tVar *op1, tVar *op2, tVar *result);
/* not, and, or */
void i_not(tVar *op1, tVar *op2, tVar *result);
void i_and(tVar *op1, tVar *op2, tVar *result);
void i_or(tVar *op1, tVar *op2, tVar *result);
/* jumps */
void i_goto(tVar *op1, tVar *op2, tVar *result);
void i_jnt(tVar *op1, tVar *op2, tVar *result);
void i_jt(tVar *op1, tVar *op2, tVar *result);
void i_nop(tVar *op1, tVar *op2, tVar *result);
#endif //INSTRUCTIONS_H
