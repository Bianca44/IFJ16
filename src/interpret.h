#ifndef INTERPRET_H
#define INTERPRET_H
#include "list.h"
#include "symbol_table.h"

typedef struct tFrame{       
    tVar *ret_val;
    unsigned size;
    /* flexible array member */
    tVar local[];   
}tFrame;

typedef struct tFSElem {
    tFrame *frame;
    struct tFSElem * next;
} tFSElem;

typedef struct tFrameStack{
    tFSElem *top;
    tFrame *prepared;
} tFrameStack;
    
extern tFrameStack frame_stack;
extern tList * processed_tape;

int interpret_tac(tList *inst_tape);

void init_frame_stack(tFrameStack *stack);
tFrame * top_frame(tFrameStack *stack);
void pop_frame(tFrameStack *stack);
void push_frame(tFrameStack *stack, tFrame * frame);
tFrame * init_frame(unsigned size);
void dispose_frame_buffer();

#endif //INTERPRET
