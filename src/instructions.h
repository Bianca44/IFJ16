#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "DLList.h"
#include "symbol_table.h"


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
    I_TO_STRING,
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
    I_JC,
    I_JE,
    I_JLE,
    I_JG,
    I_JGE,
    I_LABEL
}tInst_name;

tInst * generate(tInst_name instruction, tVar *op1, tVar *op2, tVar *result);

void i_add(tVar *op1, tVar *op2, tVar *result);
void i_sub(tVar *op1, tVar *op2, tVar *result);
void i_mul(tVar *op1, tVar *op2, tVar *result);
void i_div(tVar *op1, tVar *op2, tVar *result);
void i_assign(tVar *op1, tVar *op2, tVar *result);
void i_g(tVar *op1, tVar *op2, tVar *result);
void i_jnc(tVar *op1, tVar *op2, tVar *result);
void i_goto(tVar *op1, tVar *op2, tVar *result);
void i_label(tVar *op1, tVar *op2, tVar *result);
void i_f_call(tVar *op1, tVar *op2, tVar *result);


#endif //INSTRUCTIONS_H

