#include <stdlib.h>
#include "instructions.h"
#include "interpret.h"

#define UNUSED(x) (void)(x)

tInst * generate(tInst_name instruction, tVar *op1, tVar *op2, tVar *result){
    tInst * new_inst;

    if((new_inst = malloc(sizeof(tInst))) == NULL){
        //TODO
    }

    new_inst->op1_st = op1;           
    new_inst->op2_st = op2;           
    new_inst->result_st = result;

    new_inst->op1 = op1;           
    new_inst->op2 = op2;           
    new_inst->result = result;
    
    if(instruction == I_ADD)
        new_inst->f = i_add;
    else
        new_inst->f = i_assign;

    return new_inst;
    
}

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
void i_jnc(tVar *op1, tVar *op2, tVar *result){
    if(!op1->b){
        ((tDLList *)op2->s)->Act = (tDLElemPtr)(result->s);
        //todo set active
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
    

    UNUSED(op1);
    UNUSED(op2); 
    //interpret_tac((tDLList *)op1->s);
  
    if(result != NULL){
        //uloz vysledok TODO
    }
//    pop_frame(&frame_stack);

}














