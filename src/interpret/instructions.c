#include <stdlib.h>
#include "datatypes.h"
#include "instructions.h"
#define UNUSED(x) (void)(x)

tInst *init_inst2(){
    tInst *new;
    if((new = malloc(sizeof(tInst))) == NULL){
        //TODO
    }
    new->f = NULL;
    new->op1 = malloc(sizeof(tVar));
    new->op2 = malloc(sizeof(tVar));
    new->result = malloc(sizeof(tVar));
    return new;
}
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

void dispose_inst2(void * inst){
    free(((tInst *)(inst))->op1);
    free(((tInst *)(inst))->op2);
    free(((tInst *)(inst))->result);
    free((tInst *)(inst));
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

void i_g(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->i > op2->i);
}

void i_assign(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    result->i = op1->i;
}
// TODO 
// TODO
// TODO
void i_jc(tVar *op1, tDLList *op2, tDLElemPtr result){
    if(!op1->b){
        op2->Act = result;
    }
}

void i_goto(tVar *op1, tDLList *op2, tDLElemPtr result){
    UNUSED(op1);
    op2->Act = result;
}

void i_label(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op1);
    UNUSED(op2);
    UNUSED(result);
}















