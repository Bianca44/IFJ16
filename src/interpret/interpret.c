#include <stdio.h>
#include <stdlib.h>
#include "interpret.h"
#include "datatypes.h"
#include "debug.h"

void dispose_frame(tFrame *frame){
    free(frame->result);
    free(frame);
}

tFrameStack init_frame_stack(tFrameStack *stack){
    stack->top = NULL;
    stack->prepared = NULL;
}

tFrame * top_frame(tFrameStack *stack){
    stack->top->frame;
}

void push_frame(tFrameStack *stack, tFrame * frame){
    
    tFSElem *ptr;
    if((ptr = malloc(sizeof(tFSElem))) == NULL){
        //TODO
    }
    
    ptr->frame = frame;
    
    ptr->next = stack->top;
    stack->top = ptr->next;
}
   
void pop_frame(tFrameStack *stack){
    
    if(stack->top != NULL){
        tFSElem *tmp;
        tmp = stack->top;
        stack->top = stack->top->next;
        dispose_frame(tmp->frame);
        free(tmp);
    }
}



int interpret_tac(tDLList *inst_tape){
    
    DLFirst(inst_tape);
    tInst * inst;
    while(DLActive(inst_tape)){
        
        DLCopy(inst_tape, (void **)&inst);
        
        inst->f(inst->op1, inst->op2, inst->result);
        d_print("%d \n", inst->result->i); //TODO
        DLSucc(inst_tape);  
    } 
    
    
   return 0; //TODO
}
