#include <stdio.h>
#include <stdlib.h>
#include "interpret.h"
#include "instructions.h"
#include "debug.h"

#define get_e_adr(adress) \
    ((adress != NULL) ? \
    (adress->offset == -1 ? adress : adress->offset + frame_stack.top->frame->local)\
    :\
    NULL)

tFrame * init_frame(unsigned size){
    
    tFrame * new_frame;
    if((new_frame = malloc(sizeof(tFrame) + size*sizeof(tVar))) == NULL){
        //TODO calloc ?
    }

//    tVar * ret_val;
//    if((ret_val = malloc(sizeof(tVar))) == NULL){
        //TODO
 //   }

    new_frame->ret_val = NULL;
    
    return new_frame;
}

void dispose_frame(tFrame *frame){
 //   free(frame->ret_val);
    free(frame);
}

void init_frame_stack(tFrameStack *stack){
    stack->top = NULL;
    stack->prepared = NULL;
}

tFrame * top_frame(tFrameStack *stack){
    return stack->top->frame;
}

void push_frame(tFrameStack *stack, tFrame * frame){
    
    tFSElem *ptr;
    if((ptr = malloc(sizeof(tFSElem))) == NULL){
        //TODO
    }
    
    ptr->frame = frame;
    //global_offset = (tVar*)(&frame->local);
    //adding new frame to stack top 
    ptr->next = stack->top;
    stack->top = ptr;
}
   
void pop_frame(tFrameStack *stack){
    
    tFSElem *tmp;

    if(stack->top != NULL){
        tmp = stack->top;
        //change stack top
        stack->top = stack->top->next;
        //free frame
        dispose_frame(tmp->frame);
        free(tmp);
    }
}

/*
void set_effective_adresess(tDLList *inst_tape){
    
    DLFirst(inst_tape);
    tDLElemPtr tmp1;
    while(DLActive(inst_tape)){
        //DLCopy(inst_tape, (void **)&tmp);
        tmp1 = DLActiveElem(inst_tape);
        tInst *tmp = tmp1->data;
        if(tmp->op1_st != NULL){
            tmp->op1 = frame_stack.prepared->local + tmp->op1_st->offset;
            d_print("%d",tmp->op1->i);
        }
        if(tmp->op2_st != NULL){
            tmp->op2 = frame_stack.prepared->local + tmp->op2_st->offset;
            d_print("%d",tmp->op2->i);
        }
        if(tmp->result != NULL){
            tmp->result = frame_stack.prepared->local + tmp->result_st->offset;
            d_print("%d",tmp->result->i);
        }
        DLSucc(inst_tape);
    }

}
*/

int interpret_tac(tDLList *inst_tape){
    
    DLFirst(inst_tape);
    tVar *op1, *op2, *result;
    tInst * inst;
    while(DLActive(inst_tape)){
        
        DLCopy(inst_tape, (void **)&inst);

        d_message("spracovanie adries");
        op1 = get_e_adr(inst->op1);         
        op2 = get_e_adr(inst->op2);         
        result = get_e_adr(inst->result);         
                
        d_message("vykonanie instrukcie");
        inst->f(op1, op2, result);
        d_message("instrukcia bola vykonana");

        if(result != NULL && op1 != NULL && op2 != NULL){
            if(inst->result->data_type == INT){

                d_print("%d", op1->i); //TODO
                d_print("%d", op2->i); //TODO
                d_print("%d", result->i); //TODO
            }
            else if (inst->result->data_type == DOUBLE){

                d_print("%f", op1->d); //TODO
                d_print("%f", op2->d); //TODO
                d_print("%f", result->d); //TODO 
            }
        }
        DLSucc(inst_tape);  
    } 
     
   return 0; //TODO
}
