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
            switch(((tVar *)result)->data_type){
                case INT:
                    return i_e_i;
                    break;
                case DOUBLE:
                    return i_e_d;
                    break;
            }
			break;
        case I_NE:
            switch(((tVar *)result)->data_type){
                case INT:
                    return i_ne_i;
                    break;
                case DOUBLE:
                    return i_ne_d;
                    break;
            }
			break;
        case I_L:
            switch(((tVar *)result)->data_type){
                case INT:
                    return i_l_i;
                    break;
                case DOUBLE:
                    return i_l_d;
                    break;
            }
			break;
        case I_G:
            switch(((tVar *)result)->data_type){
                case INT:
                    return i_g_i;
                    break;
                case DOUBLE:
                    return i_g_d;
                    break;
            }
			break;
        case I_LE:
            switch(((tVar *)result)->data_type){
                case INT:
                    return i_le_i;
                    break;
                case DOUBLE:
                    return i_le_d;
                    break;
            }
			break;
        case I_GE:
            switch(((tVar *)result)->data_type){
                case INT:
                    return i_ge_i;
                    break;
                case DOUBLE:
                    return i_ge_d;
                    break;
            }
			break;
        case I_NOT:
                return i_not;
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
            return i_goto;
			break;
        case I_JNT:
            return i_jnt;
			break;
        case I_JT:
            return i_jt;
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
//first parameter instruction tape
//op1 - bool
//op2 - label

void i_goto(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op1);
    ((tDLList *)op2)->Act = (tDLElemPtr)result;
    d_inst_name();
}

void i_jnt(tVar *op1, tVar *op2, tVar *result){
    if(!op1->b){
        ((tDLList *)op2)->Act = (tDLElemPtr)result;
        //DLSetActive((tDLList *)op2, (tDLElemPtr)result);
        d_print("%p",(void *)result);
        //todo set active
    }
    d_inst_name();
}

void i_jt(tVar *op1, tVar *op2, tVar *result){
    if(op1->b){
        ((tDLList *)op2)->Act = (tDLElemPtr)result;
        //DLSetActive((tDLList *)op2, (tDLElemPtr)result);
        d_print("%p", (void *) result);
        //todo set active
    }
    d_inst_name();
}


/*
void i_label(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op1);
    UNUSED(op2);
    UNUSED(result);
    d_inst_name();
}*/
//LOGICAL
//equal
void i_e_i(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->i == op2->i);
    d_inst_name();
}

void i_e_d(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->d == op2->d);
    d_inst_name();
}
//not equal
void i_ne_i(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->i != op2->i);
    d_inst_name();
}

void i_ne_d(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->d != op2->d);
    d_inst_name();
}

//less
void i_l_i(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->i < op2->i);
    d_inst_name();
}

void i_l_d(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->d < op2->d);
    d_inst_name();
}

//greater
void i_g_i(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->i > op2->i);
    d_inst_name();
}

void i_g_d(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->d > op2->d);
    d_inst_name();
}

//less equal
void i_le_i(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->i <= op2->i);
    d_inst_name();
}

void i_le_d(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->d <= op2->d);
    d_inst_name();
}

//greater equal
void i_ge_i(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->i >= op2->i);
    d_inst_name();
}

void i_ge_d(tVar *op1, tVar *op2, tVar *result){
    result->b = (bool)(op1->d >= op2->d);
    d_inst_name();
}

//not
void i_not(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    result->b = !op1->b;
    d_inst_name();
}

//FUNCTIONS
//initialization of frame
//op1 size of frame
void i_init_frame(tVar *op1, tVar *op2, tVar *result){
    UNUSED(op2);
    UNUSED(result);
    frame_stack.prepared = init_frame(*(int *)op1);
    d_inst_name();
}
//push
void i_push_param(tVar *op1, tVar *op2, tVar *result){
    frame_stack.prepared->local[push_counter] = *op1;
    //todo string
    push_counter++;
    d_inst_name(); 
}
//
void i_f_call(tVar *op1, tVar *op2, tVar *result){
    
    push_counter = 0;
    UNUSED(op1);
    UNUSED(op2); 
    //interpret_tac((tDLList *)op1->s);
  
    if(result != NULL){
        //uloz vysledok TODO
    }
//    pop_frame(&frame_stack);

}
