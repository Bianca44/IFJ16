#include <stdlib.h>
#include "datatypes.h"
#include "instructions.h"
#include "interpret.h"
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
    op1->i = op1->i - 1; //TODO
}

void i_assign(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    result->i = op1->i;
}
// TODO 
// TODO
// TODO
void i_jnc(tVar *op1, tVar *op2, tVar *result){
    if(!op1->b){
        ((tDLList *)op2->s)->Act = (tDLElemPtr)(result->s);
    }
}

void i_goto(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op1);
    ((tDLList *)op2->s)->Act = (tDLElemPtr)(result->s);
}

void i_label(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op1);
    UNUSED(op2);
    UNUSED(result);
}

void i_f_call(tVar *op1, tVar *op2, tVar *result){
    
    interpret_tac((tDLList *)op1->s);
    if(result != NULL){
        //uloz vysledok TODO
    }
    pop_frame(&frame_stack);

}














