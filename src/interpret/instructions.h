#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H
#include "datatypes.h"

enum instructions {
    //INPUT
    I_RINT,
    I_RDBL,
    I_RSTR,
    I_RBOOL,
    //ARITHMETIC
    I_ADD,
    I_ADD_DBL,
    I_MUL,
    I_MUL_DBL,
    I_SUB,
    I_SUB_DBL,
    I_DIV,
    I_DIV_DBL,
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
    I_JT,
    I_JF,
};

void i_add(tVar *op1, tVar *op2, tVar *result);
void i_sub(tVar *op1, tVar *op2, tVar *result);
void i_mul(tVar *op1, tVar *op2, tVar *result);
void i_div(tVar *op1, tVar *op2, tVar *result);




#endif //INSTRUCTIONS_H

