#include "datatypes.h"
#include "instructions.h"

void i_add(tVar *op1, tVar *op2, tVar *result){
    result->i = op1->i + op2->i;
}

void i_sub(tVar *op1, tVar *op2, tVar *result){
    result->i = op1->i - op2->i;
}

void i_mul(tVar *op1, tVar *op2, tVar *result){
    result->i = op1->i * op2->i;
}
void i_div(tVar *op1, tVar *op2, tVar *result){
    result->i = op1->i / op2->i;
}
