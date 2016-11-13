#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "DLList.h"
#include "symbol_table.h"
#include "scanner.h"

typedef void tInst_fun(tVar *op1, tVar *op2, tVar *result);


typedef struct tInst{
    tInst_fun *f;
    tVar *op1_st;
    tVar *op1;
    tVar *op2_st;
    tVar *op2;
    tVar *result_st;
    tVar *result;
} tInst;

tInst *init_inst();
tInst *init_inst2();
void dispose_inst(void *);
void dispose_inst2(void *);


typedef enum instructions {
    //INPUT
    I_RINT,
    I_RDBL,
    I_RSTR,
    I_RBOOL,
    //ARITHMETIC
    I_ADD,
    I_MUL,
    I_SUB,
    I_DIV,
    //CONVERSIONS
    I_CONV_I_TO_D,
    I_TO_STRING,
    //LOGICAL
    I_E,
    I_NE,
    I_L,
    I_G,
    I_LE,
    I_GE,
    I_NOT,
    //BUILT-IN AND OTHER  
    I_ASSIGN,
    I_CAT,
    I_STRCMP,
    I_SUBSTR,
    I_FIND,
    I_SORT,
    I_PRINT,
    I_LEN,
    //ASSOCIATED WITH FUNCTIONS
    I_INIT_FRAME,
    I_PUSH_PARAM,
    I_CALL_F_AND_STORE,
    I_CALL_F,
    I_REMOVE_FRAME,
    I_RETURN,
    //JUMPS
    I_GOTO,
    I_JNT,
    I_JE,
    I_JLE,
    I_JG,
    I_JGE,
    I_LABEL
}tInstId;

tInst * generate(tInstId instruction, void *op1, void *op2, void *result);
tInst_fun * find_fun(tInstId instruction, void * result);

void i_add_i(tVar *op1, tVar *op2, tVar *result);
void i_add_d(tVar *op1, tVar *op2, tVar *result);
void i_sub_i(tVar *op1, tVar *op2, tVar *result);
void i_sub_d(tVar *op1, tVar *op2, tVar *result);
void i_mul_i(tVar *op1, tVar *op2, tVar *result);
void i_mul_d(tVar *op1, tVar *op2, tVar *result);
void i_div_i(tVar *op1, tVar *op2, tVar *result);
void i_div_d(tVar *op1, tVar *op2, tVar *result);
void i_assign_i(tVar *op1, tVar *op2, tVar *result);
void i_assign_d(tVar *op1, tVar *op2, tVar *result);
void i_assign_b(tVar *op1, tVar *op2, tVar *result);
void i_assign_s(tVar *op1, tVar *op2, tVar *result);
void i_g(tVar *op1, tVar *op2, tVar *result);
void i_jnt(tVar *op1, tVar *op2, tVar *result);
void i_goto(tVar *op1, tVar *op2, tVar *result);
void i_label(tVar *op1, tVar *op2, tVar *result);
void i_f_call(tVar *op1, tVar *op2, tVar *result);

#endif //INSTRUCTIONS_H
