#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H


#include "symbol_table.h"
#include "scanner.h"
#include "memory_constants.h"
#include "interpret.h"

typedef void tInst_fun(tVar *op1, tVar *op2, tVar *result);

extern constant_t * labels;
extern constant_t * tape_ref;

typedef struct tInst{
    tInst_fun *f;
    //tVar *op1_st;
    tVar *op1;
    //tVar *op2_st;
    tVar *op2;
    //tVar *result_st;
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
    I_F_CALL,
    I_RETURN,
    //JUMPS
    I_GOTO,
    I_JNT,
    I_JT,
}tInstId;

tInst * generate(tInstId instruction, void *op1, void *op2, void *result);
tInst_fun * find_fun(tInstId instruction, void * result, void *op1);

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
//function
void i_init_frame(tVar *op1, tVar *op2, tVar *result);
void i_push_param(tVar *op1, tVar *op2, tVar *result);
void i_f_call(tVar *op1, tVar *op2, tVar *result);
void i_return(tVar *op1, tVar *op2, tVar *result);
//built-in
void i_print(tVar *op1, tVar *op2, tVar *result);

//logical
//equal
void i_e_i(tVar *op1, tVar *op2, tVar *result);
void i_e_d(tVar *op1, tVar *op2, tVar *result);
//not equal
void i_ne_i(tVar *op1, tVar *op2, tVar *result);
void i_ne_d(tVar *op1, tVar *op2, tVar *result);
//less
void i_l_i(tVar *op1, tVar *op2, tVar *result);
void i_l_d(tVar *op1, tVar *op2, tVar *result);
//greater
void i_g_i(tVar *op1, tVar *op2, tVar *result);
void i_g_d(tVar *op1, tVar *op2, tVar *result);
//less equal
void i_le_i(tVar *op1, tVar *op2, tVar *result);
void i_le_d(tVar *op1, tVar *op2, tVar *result);
//greater equal
void i_ge_i(tVar *op1, tVar *op2, tVar *result);
void i_ge_d(tVar *op1, tVar *op2, tVar *result);
//not
void i_not(tVar *op1, tVar *op2, tVar *result);
//jumps
void i_goto(tVar *op1, tVar *op2, tVar *result);
void i_jnt(tVar *op1, tVar *op2, tVar *result);
void i_jt(tVar *op1, tVar *op2, tVar *result);


void set_label(tDLElemPtr jump, tDLElemPtr where);


#endif //INSTRUCTIONS_H
