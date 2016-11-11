#include <stdio.h>
#include <stdlib.h>
#include "interpret.h"
#include "instructions.h"
#include "debug.h"

tFrame * init_frame(unsigned size){
    
    tFrame * new_frame;
    if((new_frame = malloc(sizeof(tFrame) + size*sizeof(tVar *))) == NULL){
        //TODO calloc ?
    }

    tVar * result;
    if((result = malloc(sizeof(tVar))) == NULL){
        //TODO
    }

    new_frame->result = result;
    
    return new_frame;
}

void dispose_frame(tFrame *frame){
    free(frame->result);
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
    //adding new frame to stack top 
    ptr->next = stack->top;
    stack->top = ptr->next;
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



int interpret_tac(tDLList *inst_tape){
    
    DLFirst(inst_tape);
    tInst * inst;
    while(DLActive(inst_tape)){
        
        DLCopy(inst_tape, (void **)&inst);
        
        inst->f(inst->op1, inst->op2, inst->result);
        d_print("%d", inst->result->i); //TODO
        DLSucc(inst_tape);  
    } 
     
   return 0; //TODO
}
