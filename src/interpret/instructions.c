#include <stdlib.h>
#include "datatypes.h"
#include "instructions.h"


tInst *init_inst(){
    tInst *new;
    if((new = malloc(sizeof(tInst))) == NULL){
        //TODO
    }
    new->f = NULL;
    new->op1 = NULL;
    new->op2 = NULL;
    new->result = NULL;
    return new;
}

void dispose_inst(void * inst){
    free((tInst *)(inst));
}

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
