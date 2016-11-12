#include <stdlib.h>
#include "instructions.h"
#include "interpret.h"
#include "debug.h"

#define UNUSED(x) (void)(x)

tInst * generate(tInstId instruction, void *op1, void *op2, void *result){
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

    new_inst->f = find_fun(instruction, result);
  
    return new_inst;
    
}

tInst_fun * find_fun(tInstId instruction, void * result){

    switch(instruction){
         //INPUT
        case I_RINT:
			break;
        case I_RDBL:
			break;
        case I_RSTR:
			break;
        case I_RBOOL:
			break;
        //ARITHMETIC
        case I_ADD:
            if(((tVar *)result)->data_type == INT)
                return i_add_i;
            else
                return i_add_d;
			break;
        case I_MUL:
            if(((tVar *)result)->data_type == INT)
                return i_mul_i;
            else
                return i_mul_d;
			break;
        case I_SUB:
            if(((tVar *)result)->data_type == INT)
                return i_sub_i;
            else
                return i_sub_d;
			break;
        case I_DIV:
            if(((tVar *)result)->data_type == INT)
                return i_div_i;
            else
                return i_div_d;
			break;
        //CONVERSIONS
        case I_CONV_I_TO_D:
			break;
        case I_TO_STRING:
			break;
        //LOGICAL
        case I_E:
			break;
        case I_NE:
			break;
        case I_L:
			break;
        case I_G:
			break;
        case I_LE:
			break;
        case I_GE:
			break;
        case I_NOT:
			break;
        //BUILT-IN AND OTHER  
        case I_ASSIGN:
            switch(((tVar *)result)->data_type){
                case INT:
                    return i_assign_i;
                    break;
                case DOUBLE:
                    return i_assign_d;
                    break;
                case BOOLEAN:
                    return i_assign_b;
                    break;
                case STRING:
                    return i_assign_s;
                    break;
            }
			break;
        case I_CAT:
			break;
        case I_STRCMP:
			break;
        case I_SUBSTR:
			break;
        case I_FIND:
			break;
        case I_SORT:
			break;
        case I_PRINT:
			break;
        case I_LEN:
			break;
        //ASSOCIATED WITH FUNCTIONS
        case I_INIT_FRAME:
			break;
        case I_PUSH_PARAM:
			break;
        case I_CALL_F_AND_STORE:
			break;
        case I_CALL_F:
			break;
        case I_REMOVE_FRAME:
			break;
        case I_RETURN:
			break;
        //JUMPS
        case I_GOTO:
			break;
        case I_JC:
			break;
        case I_JE:
			break;
        case I_JLE:
			break;
        case I_JG:
			break;
        case I_JGE:
			break;
        case I_LABEL:
            break;
    }   

    return NULL;
    
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
//ARITHMETIC
void i_add_i(tVar *op1, tVar *op2, tVar *result){
    result->i = op1->i + op2->i;
    d_inst_name();
}

void i_add_d(tVar *op1, tVar *op2, tVar *result){
    result->d = op1->d + op2->d;
    d_inst_name();
}

void i_sub_i(tVar *op1, tVar *op2, tVar *result){
    result->i = op1->i - op2->i;
    d_inst_name();
}

void i_sub_d(tVar *op1, tVar *op2, tVar *result){
    result->d = op1->d - op2->d;
    d_inst_name();
}

void i_mul_i(tVar *op1, tVar *op2, tVar *result){
    result->i = op1->i * op2->i;
    d_inst_name();
}

void i_mul_d(tVar *op1, tVar *op2, tVar *result){
    result->d = op1->d * op2->d;
    d_inst_name();
}

void i_div_i(tVar *op1, tVar *op2, tVar *result){
    result->i = op1->i / op2->i;
    d_inst_name();
}

void i_div_d(tVar *op1, tVar *op2, tVar *result){
    result->d = op1->d / op2->d;
    d_inst_name();
}

//CONVERSIONS
void i_conv_i_to_d(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    result->d = (double)op1->i;
    d_inst_name();
}

//OTHER
void i_assign_i(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    result->i = op1->i;
    d_inst_name();
}

void i_assign_d(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    result->d = op1->d;
    d_inst_name();
}

void i_assign_b(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    result->b = op1->b;
    d_inst_name();
}

void i_assign_s(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    //TODO copy
    result->s = op1->s;
    d_inst_name();
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

void i_g(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->i > op2->i);
    d_inst_name();
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













